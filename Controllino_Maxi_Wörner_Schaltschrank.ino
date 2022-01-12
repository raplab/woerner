#include <SPI.h>
#include <Controllino.h>

//Emercency Stop - Input
#define estop_pin CONTROLLINO_AI0

//Contactor interupt - Input
#define contactor_pin CONTROLLINO_AI1

//Controlpanel - Input / Output
#define cp_on_pin CONTROLLINO_AI3
#define cp_off_pin CONTROLLINO_AI4
#define cp_cw_pin CONTROLLINO_AI5
#define cp_ccw_pin CONTROLLINO_AI6
#define cp_mode_pin CONTROLLINO_AI7 //mode selector -> drilling or tapping
#define cp_speed_pin CONTROLLINO_AI12 //analog input 4.7k pot
#define cp_indicator_pin CONTROLLINO_DO0 //output -> led

//Electrical Cabinet - Input / Output
#define ec_reset_led CONTROLLINO_DO3

//Wörner endstops for tapping - Input
#define w_topswitch_pin CONTROLLINO_AI9
#define w_bottomswitch_pin CONTROLLINO_AI10
#define w_footswitch_pin CONTROLLINO_AI11

//Eaton DE1 Control- Input / Output
#define vfd_state_pin CONTROLLINO_AI2 //input
#define vfd_run_pin CONTROLLINO_DO0
#define vfd_cw_pin CONTROLLINO_DO1
#define vfd_ccw_pin CONTROLLINO_DO2
#define vfd_speed_pin CONTROLLINO_AO0 // Analog out 0 - 0-10V

//Variables
int old_spindle_speed = 0;
int spindle_speed = 0;
int dir_change_delay = 10;
int fs_state = 0; //footswitch
int ts_state = 0; //topswitch
int bs_state = 0; //bootomswitch
int fs_last_state = 0; //footswitch
int ts_last_state = 0; //topswitch
int bs_last_state = 0; //bottomswich
String tspindle_dir = "CCW"; //CW or CCW

//STATES
enum m_mode {DRILL, TAP, ESTOP};
enum m_mode machine_mode;
enum m_state {ON, OFF};
enum m_state machine_state;
enum s_dir {CW, CCW, NONE};
enum s_dir spindle_direction;


void setup() {
  Serial.begin(19200);
  //e-stop, contactor, reset
  pinMode(estop_pin, INPUT);
  pinMode(contactor_pin, INPUT);
  pinMode(ec_reset_led, OUTPUT);

  //control panel
  pinMode(cp_on_pin, INPUT);
  pinMode(cp_off_pin, INPUT);
  pinMode(cp_cw_pin, INPUT);
  pinMode(cp_ccw_pin, INPUT);
  pinMode(cp_mode_pin, INPUT);
  pinMode(cp_speed_pin, INPUT);
  pinMode(cp_indicator_pin, OUTPUT);

  //wörner
  pinMode(w_topswitch_pin, INPUT);
  pinMode(w_bottomswitch_pin, INPUT);
  pinMode(w_footswitch_pin, INPUT);

  //eaton DE1
  pinMode(vfd_state_pin, INPUT);
  pinMode(vfd_run_pin, OUTPUT);
  pinMode(vfd_cw_pin, OUTPUT);
  pinMode(vfd_ccw_pin, OUTPUT);
  pinMode(vfd_speed_pin, OUTPUT);

  //Waiting for ESTOP, CONTACTOR and VFD to signal all good!
  Serial.println("INIT...");
  while (digitalRead(estop_pin) == LOW && digitalRead(contactor_pin) == LOW && digitalRead(vfd_state_pin) == LOW) {
    Serial.println("waiting for e_stop, contactor and or vfd to start operation!");
    delay(250);
  }
  Serial.println("MACHINE READY!");
  
  //make sure spindle is off
  machine_state = OFF;
}

void loop() {
  check_mode();
  
  switch (machine_mode) {
    case DRILL:
      drilling_mode();
      break;
    case TAP:
      tapping_mode();
      break;
    case ESTOP:
      estop_mode();
      break;
    default:
      break;
  }
}

void check_mode() {
  //check drill, tap switch and estop and reset state
  if (digitalRead(estop_pin) == HIGH) {
    machine_mode = ESTOP;
  }
  if (digitalRead(estop_pin) == LOW && digitalRead(cp_mode_pin) == HIGH) {
    machine_mode = TAP;
  }
  if (digitalRead(estop_pin) == LOW &&  digitalRead(cp_mode_pin) == LOW) {
    machine_mode = DRILL;
  }
}

void run_machine() {
  check_onoff();
  //turn machine on or off
  switch (machine_state) {
    case ON:
      set_spindle_speed();
      if(machine_mode == DRILL){
        get_spindle_dir();
        set_spindle_dir();
      }
      if(machine_mode == TAP){
        if(spindle_direction == NONE){
          spindle_direction = CW;
        }
        get_tapping_dir();
        set_spindle_dir();
      }
      digitalWrite(vfd_run_pin, HIGH);
      break;
    case OFF:
      spindle_direction = NONE;
      set_spindle_dir();
      digitalWrite(vfd_run_pin, LOW);
      break;
    default:
      break;
  }
}

void check_onoff() {
  //check input panel to see if the user turns the machine on or off
  if (digitalRead(cp_on_pin) == HIGH && digitalRead(cp_off_pin) == LOW) {
    machine_state = ON;
  }
  if (digitalRead(cp_on_pin) == LOW && digitalRead(cp_off_pin) == HIGH) {
    machine_state = OFF;
  }
}


void set_spindle_dir() {
  //set direction output pins
  switch (spindle_direction) {
    case CW:
      digitalWrite(vfd_ccw_pin, LOW);
      digitalWrite(vfd_cw_pin, HIGH);
      break;
    case CCW:
      digitalWrite(vfd_ccw_pin, HIGH);
      digitalWrite(vfd_cw_pin, LOW);
      break;
    case NONE:
      digitalWrite(vfd_cw_pin, LOW);
      digitalWrite(vfd_ccw_pin, LOW);
      break;
    default:
      break;
  }
}

void get_spindle_dir() {
  //get the current spindle direction
  if (digitalRead(cp_cw_pin) == LOW && digitalRead(cp_ccw_pin) == HIGH) {
    spindle_direction = CCW;
  }
  if (digitalRead(cp_cw_pin) == HIGH && digitalRead(cp_ccw_pin) == LOW) {
    spindle_direction = CW;
  }
  if (digitalRead(cp_cw_pin) == LOW && digitalRead(cp_ccw_pin) == LOW) {
    spindle_direction = NONE;
  }
}

void set_spindle_speed() {
  //set spindle speed acording to pot value
  spindle_speed = analogRead(cp_speed_pin);
  int speed_value = int(map(spindle_speed, 0, 1025, 30, 255));
  analogWrite(vfd_speed_pin, speed_value);
}

void get_tapping_dir() {
  //invert spindle direction when either top or bottom endstop are pressed
  bs_state = digitalRead(w_bottomswitch_pin);
  ts_state = digitalRead(w_topswitch_pin);
  fs_state = digitalRead(w_footswitch_pin);
  //check bottom switch
  if (bs_state != bs_last_state) {
    if (bs_state == HIGH) {
      spindle_direction = CCW;
    }
  }
  //check top switch
  if (ts_state != ts_last_state) {
    if (ts_state == HIGH) {
      spindle_direction = CW;
    }
  }
  //check foot switch
  if (fs_state != fs_last_state) {
    if (fs_state == HIGH && ts_state == LOW && bs_state == LOW) {
      spindle_direction = CCW;
    }
  }

  fs_last_state = fs_state;
  bs_last_state = bs_state;
  ts_last_state = ts_state;
}

void drilling_mode() {
  Serial.println("DRILLING MODE");
  machine_state = OFF;
  while (true) {
    check_mode();
    run_machine();
    if (machine_mode != DRILL) {
      break;
    }
  }
}

void tapping_mode() {
  Serial.println("TAPPING MODE");
  machine_state = OFF;
  while (true) {
    check_mode();
    run_machine();
    if (machine_mode != TAP) {
      break;
    }
  }
}

void estop_mode() {
  Serial.println("ESTOP MODE");
  digitalWrite(ec_reset_led, HIGH);

  while (true) { 
    if (digitalRead(estop_pin) == LOW && digitalRead(contactor_pin) == HIGH) {
      Serial.println("RESET PRESSED!");
      digitalWrite(ec_reset_led, LOW);
      machine_state = OFF;
      break;
    }
  }
}
