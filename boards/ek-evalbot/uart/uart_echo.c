//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the EK-LM3S9D92 Firmware Package.
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

/*
 * Added by Yong
*/
#include "ext_lib/MODBus.h"
#include "ext_lib/mod_command.h"
#include <stdint.h>           /*  Support MISRA standard define types*/
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>UART Echo (uart_echo)</h1>
//!
//! This example application utilizes the UART to echo text.  The first UART
//! (connected to the FTDI virtual serial port on the evaluation board) will be
//! configured in 115,200 baud, 8-n-1 mode.  All characters received on the
//! UART are transmitted back to the UART.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UARTIntHandler(void)
{
    unsigned long ulStatus;
    uint8_t uart_buff;
    
    //
    // Get the interrrupt status.
    //
    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ulStatus);

    /* Check new character in receive FIFO  */
    if (ROM_UARTCharsAvail(UART0_BASE))
    {
      /*  Get one character from the receive FIFO  */
      uart_buff = (uint8_t)ROM_UARTCharGetNonBlocking(UART0_BASE);
    }
    else
    { 
      /*  No new char in receive FIFO then exit */
      return;
    }
            
    /*  Is incoming char = ModBus first Frame*/
    if(uart_buff == STX)
    {
        /*  Initialise ModBus Connection*/
        MODBUS_Reset();
        return;                               /*  Exit  */
    }
    else
    {
      /*  Check ModBus Communication activated */
      if (MODBUS_CONN == 0u )
      {
        /*  ModBus comminication is not activated yet  */
        return; 
      }
      
      /*  Register new Frame as new ModBus Frame */
      MODBUS_NewFrame(uart_buff);
    }

    /*  Anything needed to send out */
    if (MODBUS_TX_REQ > 0u )
    {
      MODBUS_TX_REQ_CLR;
      /*  Send data out to UART's TX FIFO */
      uart_buff = MODBUS_OutFrame_Get();
      ROM_UARTCharPutNonBlocking(UART0_BASE, uart_buff);
    }
    
    /*  Is Incoming frame is the last frame or Burst Mode is activated*/
    if (MODBUS_CONN_END_REQ > 0u )
    {
      MODBUS_CONN_END_REQ_CLR;
      MODBUS_CONN_END;
    }
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
}

//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clocking to run directly from the crystal.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    //
    // Prompt for text to be entered.
    //
    //UARTSend((unsigned char *)"\033[2JEnter text: ", 16);
    UARTSend((unsigned char *)"v1.00", 5u);
    //
    // Loop forever echoing data through the UART.
    //
    while(1)
    {
      /*  New incoming frame  */
      if( MODBUS_INCOMING_FRAME > 0u )
      {
        MODBUS_INCOMING_FRAME_CLR;
        
        /*  Analyse the new frame */
        MODBUS_Decode();
        
        /*  Analyse valid CTL for immediate response */
        if (MODBUS_CTL_READY > 0u)
        {
          MODBUS_CTL_READY_CLR;
          /*  Is CTL invalid  */
          uint8_t res = MODBUS_Ctl_Get();
          res = MODCMD_Ctl_Validation(res);
          if ( res == 0u )  
          {
            /*  Terminate ModBus communication */
            MODBUS_CONN_END_REQ_SET;
            MODBUS_CONN_END;
            UARTSend((unsigned char *)"Invalid CTL\n", 13u);
          }
        }
        else
        {        
//          /*  Analyse new command for immediate response  */
//          if (MODBUS_CMD_READY > 0)
//          {
//            MODBUS_CMD_READY_CLR;
//            if (1)  /*  Is any immediate response  */
//            {
//              /*  Then update Slave buffer frame  */
//              
//            }
//          }
//          
//          /*  Process received data  */
//          if (MODBUS_DATA_READY > 0)
//          {
//            MODBUS_DATA_READY_CLR;
//            
//            /*  Echo the contents back to UART  */
//            
//          }
        }        
      }
      
      /*  Others function routine check */
      
    }
}
