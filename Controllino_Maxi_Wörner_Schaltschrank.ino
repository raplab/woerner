#include <SPI.h>
#include <Controllino.h>

//Emercency Stop - Input 
#define estop_pin CONTROLLINO_AI0

//Contactor interupt - Input 
#define contactor_pin CONTROLLINO_AI1

//Electrical Cabinet
#define ec_reset_pin CONTROLLINO_DO3

//Controlpanel - Input / Output 
#define cp_on_pin CONTROLLINO_AI3
#define cp_off_pin CONTROLLINO_AI4
#define cp_cw_pin CONTROLLINO_AI5
#define cp_ccw_pin CONTROLLINO_AI6
#define cp_mode_pin CONTROLLINO_AI7 //mode selector -> drilling or tapping
#define cp_speed_pin CONTROLLINO_AI8 //analog input 10k pot
#define cp_indicator_pin CONTROLLINO_DO3 //output -> led

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
int spindle_speed = 0;
int dir_change_delay = 100;
int fs_state = 0; //footswitch
int ts_state = 0; //topswitch
int bs_state = 0; //bootomswitch
int fs_last_state = 0; //footswitch
int ts_last_state = 0; //topswitch
int bs_last_state = 0; //bottomswich
String spindle_dir = "CW"; //CW or CCW

void setup() {
  Serial.begin(9600);
  //e-stop, contactor, reset
  pinMode(estop_pin, INPUT);
  pinMode(contactor_pin, INPUT);
  pinMode(ec_reset_pin, OUTPUT);

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
  while(digitalRead(estop_pin) == LOW && digitalRead(contactor_pin) == LOW && digitalRead(vfd_state_pin) == LOW){
    Serial.println("waiting for e_stop, contactor and or vfd to start operation!");
    delay(250);
  }
  Serial.println("READY!");
}

void loop() {
  //see if estop is active and choose drill or tap mode
  if(estop_checker() == false && drill_or_tap() == false){
    drilling_mode();
  }
  if(estop_checker() == false && drill_or_tap() == true){
    tapping_mode();
  }
}

bool estop_checker(){
  //check estop pin
  if(digitalRead(estop_pin) == HIGH){
    digitalWrite(ec_reset_pin, HIGH);
    return true;
  }
  if(digitalRead(estop_pin) == LOW){
    digitalWrite(ec_reset_pin, LOW);
    return false;
  }
}

bool drill_or_tap() {
  //check drill or tap mode pin
  if(digitalRead(cp_mode_pin) == HIGH){
    return true;
  }else{
    return false;
  }
}

void run() {
  //run the spindle 
  if(digitalRead(cp_on_pin) == HIGH && digitalRead(cp_on_pin) == LOW){
    digitalWrite(vfd_run_pin, HIGH); //spindle on
  }
  if(digitalRead(cp_on_pin) == LOW && digitalRead(cp_on_pin) == HIGH){
    digitalWrite(vfd_run_pin, HIGH);
  }
}


void set_spindle_direction(String dir) {
  //change spindle direction depending on dir string
  if(dir == "CW"){
    digitalWrite(vfd_ccw_pin, LOW);
    delay(dir_change_delay);
    digitalWrite(vfd_cw_pin, HIGH);
  }
  if(dir == "CCW"){
    digitalWrite(vfd_cw_pin, LOW);
    delay(dir_change_delay);
    digitalWrite(vfd_ccw_pin, HIGH);
  }
}

String get_spindle_dir(){
  //get the current spindle direction
  if(digitalRead(cp_cw_pin) == HIGH && digitalRead(cp_ccw_pin) == LOW){
    return "CCW";
  }
  if(digitalRead(cp_cw_pin) == LOW && digitalRead(cp_ccw_pin) == HIGH){
    return "CW";
  }
}

void set_spindle_speed() {
  //set spindle speed acording to pot valie
  spindle_speed = analogRead(cp_speed_pin);
  analogWrite(vfd_speed_pin, spindle_speed);
}

void tap_spindle_direction(){
  //change spindle direction depending on the state of the inputs
  fs_state = digitalRead(w_footswitch_pin);
  bs_state = digitalRead(w_bottomswitch_pin);
  ts_state = digitalRead(w_topswitch_pin);

  //check bottom switch state
  if(bs_state != bs_last_state){
    if(bs_state == HIGH){
      if(get_spindle_dir() == "CW"){
        set_spindle_direction("CCW");
      }
      if(get_spindle_dir() == "CCW"){
        set_spindle_direction("CW");
      }
    }
  }

  //check foot switch state
  if(fs_state != fs_last_state){
    if(fs_state == HIGH){
      if(get_spindle_dir() == "CW"){
        set_spindle_direction("CCW");
      }
      if(get_spindle_dir() == "CCW"){
        set_spindle_direction("CW");
      }
    }
  }  

  //check top switch state
  if(ts_state != ts_last_state){
    if(ts_state == HIGH){
      if(get_spindle_dir() == "CW"){
        set_spindle_direction("CCW");
      }
      if(get_spindle_dir() == "CCW"){
        set_spindle_direction("CW");
      }
    }
  }

  fs_last_state = fs_state;
  bs_last_state = bs_state;
  ts_last_state = ts_state;

}

void drilling_mode(){
  tap_spindle_direction();
  set_spindle_speed();
  run();
}

void tapping_mode(){
    tap_spindle_direction();
    set_spindle_speed();
    run();
}
