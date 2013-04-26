#include "ext_lib/MODBus.h"

/*  TODO:
/    a) Terminate MODBUS communication once everything done 
/       - Some CMD required no response.
/    b) Burst Mode not supported
/
/    Tested:
/    a) Host frame lenght > Slave frame length
/    b) Host frame length = Slave frame length (Host has ESC)
/    c) ESC char 
/    d) Example of situation 1 (MODCOM.DOC rev.3)
/    e) Example of situation 2 (MODCOM.DOC rev.3)
*/

/*
 *           (Ref Pointer by MODBUS_in_ref - Not included ESC char )
 *           Index start @ 0
 *           |-------------------------------------------------------|
 *  | STX | CTL | CMD | NTX | TD0 | TD1 | TDn-1 | DUM | DUM | SUM | RXF |
 *              |        |------------------|
 *              |        (Ref Pointer by MODBUS_host_dataPtr - Not included 
 *              |                                                   Dummy char)
 *              |
 *              | ~STX | NRX | RD0 | RD1 | RDn-1 | RID | DUM | SUM | TXF |
 *                              |-------------------------------------|
 *                              (Ref Pointer by MODBUS_slave_dataPtr 
 *                                                    - Not included Dummy char)
*/

/*  Structure   */
typedef struct MODBus_Frame_Host {
  uint8_t ctl;
  uint8_t cmd;
  uint8_t length;     /*  Stored the max index of data[]  */
  uint8_t checksum;
  uint8_t data[DATA_MAX_LENGTH];
}Frame_Host;

typedef struct MODBus_Frame_Slave {
  uint8_t nrx;
  uint8_t length;     /*  Stored the max index of data[]  */
  uint8_t rid;
  uint8_t checksum;
  uint8_t data[DATA_MAX_LENGTH];  /*  The longest content is 7-byte (buffer 7-byte)  */
}Frame_Slave;

/*  Variable  */
Frame_Host MODBUS_frame_host;
Frame_Slave MODBUS_frame_slave;
uint16_t MODBUS_status;
uint8_t MODBUS_in_ref;
uint8_t MODBUS_host_dataPtr;    /*  Ref to index of MODBUS_frame_host.data[]  */
uint8_t MODBUS_slave_dataPtr;   /*  Ref to index of MODBUS_frame_slave frame position  */
uint8_t MODBUS_new_buf_frame;
uint8_t MODBUS_out_buf_frame;

void MODBUS_Reset(void)
{
  MODBUS_in_ref = 0;
  MODBUS_host_dataPtr = 0;
  MODBUS_slave_dataPtr = 0;
  MODBUS_status = 0;
  MODBUS_frame_host.checksum = 0;
  MODBUS_frame_slave.checksum = 0;
  MODBUS_CONN_START;
  
//  MODBUS_frame_slave.nrx = 0x22;
//  /*  length must represent the max index of data content  */
//  MODBUS_frame_slave.length = (0x0F & MODBUS_frame_slave.nrx) - 1;
//  MODBUS_frame_slave.data[0] = 0x2F;
//  MODBUS_frame_slave.data[1] = 0x00;
//  MODBUS_frame_slave.data[2] = 0x00;
//  MODBUS_frame_slave.data[3] = 0x00;
//  MODBUS_frame_slave.data[4] = 0x00;
//  MODBUS_frame_slave.data[5] = 0x00;
//  MODBUS_frame_slave.data[6] = 0x00;
//  MODBUS_frame_slave.data[7] = 0x00;
//  MODBUS_frame_slave.data[8] = 0x00;
//  MODBUS_frame_slave.data[9] = 0x00;
//  MODBUS_frame_slave.rid = 0x33;
}

void MODBUS_NewFrame(uint8_t input)
{  
  MODBUS_INCOMING_FRAME_SET;      /*  acknowledge new incoming frame  */
  MODBUS_new_buf_frame = input;
  
  /*  Calculate host's CHKSUM  */
  MODBUS_frame_host.checksum ^= input;
}

uint8_t MODBUS_OutFrame_Get(void)
{
  /*  Calculate slave's CHKSUM  */
  MODBUS_frame_slave.checksum ^= MODBUS_out_buf_frame;
  
  return MODBUS_out_buf_frame;
}

uint8_t MODBUS_Ctl_Get(void)
{
  return MODBUS_frame_host.ctl;
}

void MODBUS_Decode(void)
{
  uint8_t temp;
  
  /*  Check for ESC char  */
  if ((MODBUS_in_ref > 0) && (MODBUS_new_buf_frame == ESC))
  {
    /*  CMD or later frame & ESC char found
     *  Acknowledge to reverse next incoming frame  */
    MODBUS_REV_SET;
    
    /*  Setup new frame to send out  */
    MODBUS_out_buf_frame = ESC_NOT;   /*  Inverted ESC as response */
    MODBUS_TX_REQ_SET;
    
    return;
  }  
    
  /*  Check for restore frame operation  */
  if (MODBUS_REV > 0)
  {
    /*  Clear reverse acknowledge flag  */
    MODBUS_REV_CLR;
    MODBUS_new_buf_frame = ~MODBUS_new_buf_frame;
  }  
  
  /*  Extract/Decode new frame  */
  switch (MODBUS_in_ref)
  {
  case 0:   /*  CTL  */
    MODBUS_CTL_READY_SET;             /*  Acknowledge CTL is ready  */
    MODBUS_frame_host.ctl = MODBUS_new_buf_frame;
    temp = ~MODBUS_new_buf_frame; /*  Load ~CTL to out buffer frame */
    break;
    
  case 1:   /*  CMD  */
    MODBUS_CMD_READY_SET;           /*  Acknowledge CMD is ready  */
    MODBUS_frame_host.cmd = MODBUS_new_buf_frame;
    temp = MODBUS_frame_slave.nrx;  /*  Load NRX to out buffer frame  */
    break;
    
  case 2:   /*  NTX  + Burst Mode  */
    MODBUS_frame_host.length = MODBUS_new_buf_frame & 0x0F;
    MODBUS_frame_host.length = MODBUS_frame_host.length - 1; 
    /*  Check Burst Mode  */
    if ((MODBUS_new_buf_frame & 0x80) > 0)
    {
      /*  Host frame's length is not depends on slave frame's length */
      MODBUS_BUST_MODE_SET;        
    }
    else
    {
      /*  Host frame's length is depends on slave frame's length */
      MODBUS_BUST_MODE_CLR;
    }
    /*  Load RDx to out buffer frame  */
    MODBUS_host_dataPtr = 0;    /*  Init index of data[] for Host frame  */
    MODBUS_slave_dataPtr = 0;   /*  Init index of data[] for Slave frame*/
    temp = MODBUS_frame_slave.data[MODBUS_slave_dataPtr];
    MODBUS_slave_dataPtr++;
    break;
    
  default:  /*  Contents (or) CHKSUM (or) ACK */
    
    /*  Store Host contents (TDx)  */
    /*  Any contents after host frame length will be Dummy  */
    if (MODBUS_host_dataPtr < MODBUS_frame_host.length)
    {
      MODBUS_frame_host.data[MODBUS_host_dataPtr] = MODBUS_new_buf_frame;
      /*  host content index increament and 
       *  value stop increament when it reach host frame length  */
      MODBUS_host_dataPtr++;
    }
    
    /*  Send RDx  */
    if ( MODBUS_slave_dataPtr < MODBUS_frame_slave.length )
    {
      /*  Load RDx to out buffer frame  */
      temp = MODBUS_frame_slave.data[MODBUS_slave_dataPtr];
      MODBUS_slave_dataPtr++;
    }
    /*  Send RID byte  */
    else if ( MODBUS_slave_dataPtr == MODBUS_frame_slave.length )
    {
      /*  Index of data[] has reached content limit  */
      /*  Load RID to out buffer frame  */
      temp = MODBUS_frame_slave.rid;
      MODBUS_slave_dataPtr++;
    }
    /*  Send CheckSUM (or) Dummy byte  */
    else if ( MODBUS_slave_dataPtr == (MODBUS_frame_slave.length + 1) )
    {
      /*  Slave is finished sending contents & RID */
      if( MODBUS_host_dataPtr == MODBUS_frame_host.length)
      {
        /*  Host has finished sending contents       */
        /*  Load receive CHKSUM to out buffer frame  */
        temp = MODBUS_frame_slave.checksum;
        MODBUS_slave_dataPtr++;
      }
      else
      {
        /*  Host is not yet finish sending contents  */        
        /*  Load Dummy byte to out buffer frame  */
        temp = SLAVE_DUMMY;
      }
    }
    /*  Send Acknowledge byte  */
    else if (MODBUS_slave_dataPtr == (MODBUS_frame_slave.length + 2))
    {
      /* Validate Host checksum  */
      /*  CheckSUM always done when new frame is inserted therefore if 
          Host checkSUM match with local calucated host checkSUM then
          it always equal to zero  */
      if (MODBUS_frame_host.checksum == 0)
      {
        MODBUS_DATA_READY_SET;  /*  Acknowledge DATA are received completed  */
        temp = ACK;   /*  Good checksum  */
      }
      else
      {
        temp = NAK;   /*  Bad checksum  */
      }      
      /*  Request to terminate ModBus Communication  */
      MODBUS_CONN_END_REQ_SET;
      MODBUS_slave_dataPtr++;
    }
    /* MODBUS_slave_dataPtr == (MODBUS_frame_slave.length + 3) */
    else 
    {
      /*  Last frame received from Host after sending out slave's ACK to Host */
      if (MODBUS_new_buf_frame == NAK )
      {
        /*  Host received bad checksum  */
        /*  Do something  e.g. Log  */
      }      
      return;
    }
    break;
  }
  
  /*  Increate receive frame position for next incoming frame  */
  MODBUS_in_ref++;  
  
  /*  Setup new frame to send out  */
  MODBUS_out_buf_frame = temp;
  MODBUS_TX_REQ_SET;
  return;
}