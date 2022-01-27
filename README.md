# Wörner - Conventional Drill 
drilling and tapping machine control-code for a custom made electrical cabinet as upgrade of an old drill/milling-machine.
The controllino maxi automation acts as an SPS controlling various inputs and states - e.g. drilling, tapping and e-stop-mode.

## Requirements
- Controllino MAXI Automation https://www.controllino.com/product/controllino-maxi-automation/
- Eaton DE-1 (vfd) for motor control

# Documentation

## PIN Out
| Controllino Pin | Variable Name |  Input or Output |Cabinet Pin | Notes |   
|---|---|---|---|---|
| AI0   |  estop_pin |  INPUT |  - | read e-stop state, HIGH=ESTOP |
| AI1   |  contactor_pin | INPUT  | -  | read contactor state to determin if VFD has power  |
| AI2   |  vfd_state_pin |  INPUT | -  | read VFD state |
| AI3   |  cp_on_pin | INPUT  | -  |  spindle on |
| AI4   |  cp_off_pin | INPUT  | -  |  spindle off |
| AI5   |  cp_cw_pin | INPUT  | -  |  spindle direction clockwise |
| AI6   |  cp_ccw_pin | INPUT  | -  | spindle direction counterclockwise  |
| AI7   |  cp_mode_pin |  INPUT | -  | select between drilling or tapping mode  |
| AI9   |  w_topswitch_pin | INPUT  | -  | top switch for tapping mode (reverse spindle direction on HIGH) |
| AI10  |  w_bottomswitch_pin | INPUT  | -  |  bottom switch for tapping mode (reverse spindle direction on HIGH) |
| AI11  |  w_footswitch_pin |  INPUT | -  | foot switch for tapping mode (reverse spindle direction on HIGH)  |
| AI12  |  cp_speed_pin | INPUT  | -  |  analog input from 4.7k pot (0,1025) mapped to (0,255) vfd_speed_pin |
| AI13  |  tap_speed_pin | INPUT  | -  |  analog input from 4.7k pot (0,1025) mapped to (0,255) tap_speed_pin |
| DO0   |  vfd_run_pin | OUTPUT  | -  | run spindle  |
| DO1   |  vfd_cw_pin | OUTPUT | -  |  turn spindle on in clockwise direction |
| DO2   |  vfd_ccw_pin | OUTPUT  | -  |  turn spindle on in counterclockwise direction  |
| DO3   |  cp_indicator_pin | OUTPUT   | -  |  led - high if spindle is on |
| AO0   |  vfd_speed_pin | ANALOG OUTPUT  |   | Analog out from 0-10V to control motor speed  |

