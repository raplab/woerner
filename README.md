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
| AI0   |  estop_pin |   |   |   |
| AI1   |  contactor_pin |   |   |   |
| AI2   |  vfd_state_pin |   |   |   |
| AI3   |  cp_on_pin |   |   |   |
| AI4   |  cp_off_pin |   |   |   |
| AI5   |  cp_cw_pin |   |   |   |
| AI6   |  cp_ccw_pin |   |   |   |
| AI7   |  cp_mode_pin |   |   |   |
| AI9   |  w_topswitch_pin |   |   |   |
| AI10  |  w_bottomswitch_pin |   |   |   |
| AI11  |  w_footswitch_pin |   |   |   |
| AI12  |  cp_speed_pin |   |   |   |
| DO0   |  vfd_run_pin |   |   |   |
| DO1   |  vfd_cw_pin |   |   |   |
| DO2   |  vfd_ccw_pin |   |   |   |
| DO3   |  cp_indicator_pin |   |   |   |
| AO0   |  vfd_speed_pin |   |   | Analog out from 0-10V to control motor speed  |

