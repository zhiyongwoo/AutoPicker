#include <stdint.h>           /*  Support MISRA standard define types*/

#ifndef __MOD_BUS_H__
#define __MOD_BUS_H__

/*  General */
extern uint16_t MODBUS_status;
extern uint8_t MODBUS_DirecResp [7];

/*  MODBUS Frame  */
#define STX         0x02    /*First incoming frame of MODBUS (Host -> Controller)*/
#define MA          (!STX)  /*First outgoing frame of MODBUS (Controller -> Host) */
#define ACK         0x06    /*Valid CHECKSUM*/
#define NAK         0x15    /*Invalid CHECKSUM*/
#define HOST_DUMMY  0x00    /*Dummy received from Host*/
#define SLAVE_DUMMY 0xAA    /*Dummy sent to Host*/
#define ESC         0x1B    /*ESC char*/
#define ESC_NOT     0xE4    /*Reverse of ESC char*/

#define DATA_MAX_LENGTH 14  /*Max data content length in both Host & Slave*/

#define MODBUS_CONN         (MODBUS_status & 0x01)      /* bit-0  0000 0001 */
#define MODBUS_CONN_START   (MODBUS_status |= 0x01)
#define MODBUS_CONN_END     (MODBUS_status &= 0xFE)  

#define MODBUS_CONN_END_REQ     (MODBUS_status & 0x02)  /* bit-1  0000 0010 */
#define MODBUS_CONN_END_REQ_SET (MODBUS_status |= 0x02)
#define MODBUS_CONN_END_REQ_CLR (MODBUS_status &= 0xFD)

#define MODBUS_INCOMING_FRAME       (MODBUS_status & 0x04)   /* bit-2 0000 0100 */
#define MODBUS_INCOMING_FRAME_SET   (MODBUS_status |= 0x04)
#define MODBUS_INCOMING_FRAME_CLR   (MODBUS_status &= 0xFB)

#define MODBUS_BUST_MODE      (MODBUS_status & 0x08)  /* bit-3 0000 1000 */
#define MODBUS_BUST_MODE_SET  (MODBUS_status |= 0x08)
#define MODBUS_BUST_MODE_CLR  (MODBUS_status &= 0xF7)

#define MODBUS_REV        (MODBUS_status & 0x10)        /* bit-4 0001 0000 */
#define MODBUS_REV_SET    (MODBUS_status |= 0x10)
#define MODBUS_REV_CLR    (MODBUS_status &= 0xEF)

#define MODBUS_TX_REQ       (MODBUS_status & 0x80)      /* bit-7  1000 0000 */
#define MODBUS_TX_REQ_SET   (MODBUS_status |= 0x80)
#define MODBUS_TX_REQ_CLR   (MODBUS_status &= 0x7F)

#define MODBUS_CTL_READY      (MODBUS_status & 0x0100)  /* bit-8  */
#define MODBUS_CTL_READY_SET  (MODBUS_status |= 0x0100)
#define MODBUS_CTL_READY_CLR  (MODBUS_status &= 0xFEFF) 

#define MODBUS_CMD_READY     (MODBUS_status & 0x0200)   /* bit-9  */
#define MODBUS_CMD_READY_SET (MODBUS_status |= 0x0200)
#define MODBUS_CMD_READY_CLR (MODBUS_status &= 0xFDFF)

#define MODBUS_DATA_READY     (MODBUS_status & 0x0400)  /* bit-10  */
#define MODBUS_DATA_READY_SET (MODBUS_status |= 0x0400)
#define MODBUS_DATA_READY_CLR (MODBUS_status &= 0xFBFF)


/*  Function Prototype  */
extern void MODBUS_Reset(void);
extern void MODBUS_NewFrame(uint8_t input);
extern uint8_t MODBUS_OutFrame_Get(void);
extern uint8_t MODBUS_Ctl_Get(void);
extern void MODBUS_Decode(void);

#endif  //__MOD_BUS_H__