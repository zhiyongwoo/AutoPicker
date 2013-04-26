#include "ext_lib/mod_command.h"

/*  TODO:
/    a) Read MOD Address
/    b) Analyse MOD CMD to either provide response or not.
/    c) Analyse MOD CMD + Contents once finished receiving MOD FRAME
/    d) Redesing the flow on "receive MOD CMD -> Analyse -> Process -> Action"
/
/    Tested:
/    a) MODCMD_Ctl_Validation
*/

const uint8_t ID_k_TASK_1 = 'T';
uint8_t MODCMD_addr = MOD_ADDR_OFFSET;

void MODCMD_Read_ModAddress(void)
{
  /*  Read the ModBus Address from I/O pin  */
  //Read pin 1 & 2 to set new address
  MODCMD_addr |= 0x00 ;
}

uint8_t MODCMD_Ctl_Validation (uint8_t ctl)
{
  /*  Validate CTL  */
  uint8_t result;
  
  if ( ctl == MODCMD_addr )
  {
    result = 1u;
  }
  else
  {
    result = 0u;
  }
  return result;
}

uint8_t MODCMD_Cmd_Analyse (uint8_t cmd, uint8_t direct_resp)
{
  uint8_t resp = 0;
  
  if (direct_resp == 0)
  {
    /*  Indirect response  */
    
  }
  
  switch (cmd)
  {
  case CMD_CMD_TASK_COMMAND_1:
  case CMD_CMD_TASK_COMMAND_2:
  case CMD_CMD_TASK_COMMAND_3:
  case CMD_CMD_TASK_COMMAND_4:
  case CMD_CMD_TASK_COMMAND_5:
  case CMD_CMD_TASK_COMMAND_6: 
    break;
  case CMD_CMD_GET_TASKBUF_RESP:
    break;
  case CMD_CMD_GET_ID:
    break;
  case CMD_CMD_GET_FLAG_REG:
    break;
  case CMD_CMD_RESET_MODULE:
    break;
  case CMD_CMD_READ_INPUTS:
    break;
  case CMD_CMD_WRITE_OUTPUT:
    break;
  default:
    break;
  }
  
  return 0u;
}