#include <stdint.h>           /*  Support MISRA standard define types*/

#ifndef __MOD_CMD_H__
#define __MOD_CMD_H__

/*  General  */
#define MOD_ADDR_OFFSET   0x38u

/*  CTL  */
#define CMD_CTL_DEFAULT   0x38u

/*  CMD  */
#define CMD_CMD_TASK_COMMAND_1    0x30u
#define CMD_CMD_TASK_COMMAND_2    0x31u
#define CMD_CMD_TASK_COMMAND_3    0x32u
#define CMD_CMD_TASK_COMMAND_4    0x33u
#define CMD_CMD_TASK_COMMAND_5    0x34u
#define CMD_CMD_TASK_COMMAND_6    0x35u
#define CMD_CMD_GET_TASKBUF_RESP  0x36u
#define CMD_CMD_GET_ID            0x41u
#define CMD_CMD_GET_FLAG_REG      0x42u
#define CMD_CMD_RESET_MODULE      0x47u
#define CMD_CMD_INIT_SYNC         0x48u
#define CMD_CMD_ACTIVATE_SYNC     0x49u
#define CMD_CMD_READ_INPUTS       0x55u
#define CMD_CMD_WRITE_OUTPUT      0x56u
#define CMD_CMD_WRITE_EDS         0x70u
#define CMD_CMD_READ_EDS          0x71u

/*  TASK_CMD from data content  */
/*  General (Task 1)  */
#define TASK_CMD_SWITCH_BUZZER    0x05u
#define TASK_CMD_GET_BOARD_ADD    0x41u
#define TASK_CMD_GET_TASK_ID      0x49u
#define TASK_CMD_RESET_TASK       0x52u
#define TASK_CMD_GET_TASK_STATUS  0x53u

/*  Motor Specific (Task 2-5)  */
#define TASK_CMD_MOVE_LEFT            0x10u
#define TASK_CMD_MOVE_RIGHT           0x11u
#define TASK_CMD_SENSE_MOVE_LEFT      0x12u
#define TASK_CMD_SENSE_MOVE_RIGHT     0x13u
#define TASK_CMD_SENSE_NOT_MOVE_LEFT  0x14u
#define TASK_CMD_SENSE_NOT_MOVE_RIGHT 0x15u
#define TASK_CMD_INIT_LOCKLEVEL       0x20u
#define TASK_CMD_MOVE_LOCK            0x21u
#define TASK_CMD_MOVE_STOP            0x24u
#define TASK_CMD_GET_TOTAL_STEPS      0x2Du
#define TASK_CMD_GET_SENSE_STEPS      0x2Eu
#define TASK_CMD_GET_SENSOR_WIDTH     0x2Fu
#define TASK_CMD_SET_STEPMODE         0x32u
#define TASK_CMD_GET_IDENTITY         0x49u
#define TASK_CMD_RESET_TASK           0x52u
#define TASK_CMD_GET_TASK_STATUS      0x53u
#define TASK_CMD_UPDATE_TASK_STATUS   0x54u
#define TASK_CMD_INIT_CUSTOM_PATTERN  0x60u

/*  TASK ID  */
extern const uint8_t ID_k_TASK_1;
//const uint8_t ID_k_TASK_1 'T';

/*  Function Prototype  */
extern void MODCMD_Read_ModAddress(void);
extern uint8_t MODCMD_Ctl_Validation(uint8_t ctl);

#endif  //__MOD_CMD_H__