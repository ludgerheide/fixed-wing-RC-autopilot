/*! \file uart2.c \brief Dual UART driver with buffer support. */
//*****************************************************************************
//
// File Name	: 'uart2.c'
// Title		: Dual UART driver with buffer support
// Author		: Pascal Stang - Copyright (C) 2000-2004
// Created		: 11/20/2000
// Revised		: 07/04/2004
// Version		: 1.0
// Target MCU	: ATMEL AVR Series
// Editor Tabs	: 4
//
// Description	: This is a UART driver for AVR-series processors with two
//		hardware UARTs such as the mega161 and mega128 
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <assert.h>

#include "buffer.h"
#include "uart4.h"

// UART global variables
// flag variables
volatile u08   uartReadyTx[4];
volatile u08   uartBufferedTx[4];
// receive and transmit buffers
cBuffer uartRxBuffer[4];
cBuffer uartTxBuffer[4];
unsigned short uartRxOverflow[4];
#ifndef UART_BUFFER_EXTERNAL_RAM
	// using internal ram,
	// automatically allocate space in ram for each buffer
	static char uart0RxData[UART0_RX_BUFFER_SIZE];
	static char uart0TxData[UART0_TX_BUFFER_SIZE];
	static char uart1RxData[UART1_RX_BUFFER_SIZE];
    static char uart1TxData[UART1_TX_BUFFER_SIZE];
    static char uart2RxData[UART2_RX_BUFFER_SIZE];
    static char uart2TxData[UART2_TX_BUFFER_SIZE];
    static char uart3RxData[UART3_RX_BUFFER_SIZE];
    static char uart3TxData[UART3_TX_BUFFER_SIZE];
#endif

typedef void (*voidFuncPtru08)(unsigned char);
static volatile voidFuncPtru08 UartRxFunc[4];

void uartInit(u08 nUart) {
    assert(nUart < 4);
    switch (nUart) {
        case 0:
            uart0Init();
            break;
            
        case 1:
            uart1Init();
            break;
            
        case 2:
            uart2Init();
            break;
            
        case 3:
            uart3Init();
            break;
            
        default:
            break;
    }
}

void uart0Init(void) {
	// initialize the buffers
	uart0InitBuffers();
	// initialize user receive handlers
	UartRxFunc[0] = 0;
	// enable RxD/TxD and interrupts
	outb(UCSR0B, BV(RXCIE0)|BV(TXCIE0)|BV(RXEN0)|BV(TXEN0));
	// set default baud rate
	uartSetBaudRate(0, UART0_DEFAULT_BAUD_RATE); 
	// initialize states
	uartReadyTx[0] = TRUE;
	uartBufferedTx[0] = FALSE;
	// clear overflow count
	uartRxOverflow[0] = 0;
	// enable interrupts
	sei();
}

void uart1Init(void) {
	// initialize the buffers
	uart1InitBuffers();
	// initialize user receive handlers
	UartRxFunc[1] = 0;
	// enable RxD/TxD and interrupts
	outb(UCSR1B, BV(RXCIE1)|BV(TXCIE1)|BV(RXEN1)|BV(TXEN1));
	// set default baud rate
	uartSetBaudRate(1, UART1_DEFAULT_BAUD_RATE);
	// initialize states
	uartReadyTx[1] = TRUE;
	uartBufferedTx[1] = FALSE;
	// clear overflow count
	uartRxOverflow[1] = 0;
	// enable interrupts
	sei();
}

void uart2Init(void) {
    // initialize the buffers
    uart2InitBuffers();
    // initialize user receive handlers
    UartRxFunc[2] = 0;
    // enable RxD/TxD and interrupts
    outb(UCSR2B, BV(RXCIE2)|BV(TXCIE2)|BV(RXEN2)|BV(TXEN2));
    // set default baud rate
    uartSetBaudRate(2, UART2_DEFAULT_BAUD_RATE);
    // initialize states
    uartReadyTx[2] = TRUE;
    uartBufferedTx[2] = FALSE;
    // clear overflow count
    uartRxOverflow[2] = 0;
    // enable interrupts
    sei();
}

void uart3Init(void) {
    // initialize the buffers
    uart3InitBuffers();
    // initialize user receive handlers
    UartRxFunc[3] = 0;
    // enable RxD/TxD and interrupts
    outb(UCSR3B, BV(RXCIE3)|BV(TXCIE3)|BV(RXEN3)|BV(TXEN3));
    // set default baud rate
    uartSetBaudRate(3, UART3_DEFAULT_BAUD_RATE);
    // initialize states
    uartReadyTx[3] = TRUE;
    uartBufferedTx[3] = FALSE;
    // clear overflow count
    uartRxOverflow[3] = 0;
    // enable interrupts
    sei();
}

void uart0InitBuffers(void) {
	#ifndef UART_BUFFER_EXTERNAL_RAM
		// initialize the UART0 buffers
		bufferInit(&uartRxBuffer[0], (u08*)uart0RxData, UART0_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[0], (u08*)uart0TxData, UART0_TX_BUFFER_SIZE);
	#else
		// initialize the UART0 buffers
		bufferInit(&uartRxBuffer[0], (u08*) UART0_RX_BUFFER_ADDR, UART0_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[0], (u08*) UART0_TX_BUFFER_ADDR, UART0_TX_BUFFER_SIZE);
	#endif
}

void uart1InitBuffers(void) {
	#ifndef UART_BUFFER_EXTERNAL_RAM
		// initialize the UART1 buffers
		bufferInit(&uartRxBuffer[1], (u08*)uart1RxData, UART1_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[1], (u08*)uart1TxData, UART1_TX_BUFFER_SIZE);
	#else
		// initialize the UART1 buffers
		bufferInit(&uartRxBuffer[1], (u08*) UART1_RX_BUFFER_ADDR, UART1_RX_BUFFER_SIZE);
		bufferInit(&uartTxBuffer[1], (u08*) UART1_TX_BUFFER_ADDR, UART1_TX_BUFFER_SIZE);
	#endif
}

void uart2InitBuffers(void) {
#ifndef UART_BUFFER_EXTERNAL_RAM
    // initialize the UART1 buffers
    bufferInit(&uartRxBuffer[2], (u08*)uart2RxData, UART2_RX_BUFFER_SIZE);
    bufferInit(&uartTxBuffer[2], (u08*)uart2TxData, UART2_TX_BUFFER_SIZE);
#else
    // initialize the UART1 buffers
    bufferInit(&uartRxBuffer[2], (u08*) UART2_RX_BUFFER_ADDR, UART2_RX_BUFFER_SIZE);
    bufferInit(&uartTxBuffer[2], (u08*) UART2_TX_BUFFER_ADDR, UART2_TX_BUFFER_SIZE);
#endif
}

void uart3InitBuffers(void) {
#ifndef UART_BUFFER_EXTERNAL_RAM
    // initialize the UART1 buffers
    bufferInit(&uartRxBuffer[3], (u08*)uart3RxData, UART3_RX_BUFFER_SIZE);
    bufferInit(&uartTxBuffer[3], (u08*)uart3TxData, UART3_TX_BUFFER_SIZE);
#else
    // initialize the UART1 buffers
    bufferInit(&uartRxBuffer[3], (u08*) UART3_RX_BUFFER_ADDR, UART3_RX_BUFFER_SIZE);
    bufferInit(&uartTxBuffer[3], (u08*) UART3_TX_BUFFER_ADDR, UART3_TX_BUFFER_SIZE);
#endif
}

void uartSetRxHandler(u08 nUart, void (*rx_func)(unsigned char c)) {
    assert(nUart < 4);
	// make sure the uart number is within bounds (even without assertion as not to trample memory)
	if(nUart < 4)
	{
		// set the receive interrupt to run the supplied user function
		UartRxFunc[nUart] = rx_func;
	}
}

void uartSetBaudRate(u08 nUart, u32 baudrate) {
    assert(nUart < 4);
	// calculate division factor for requested baud rate, and set it
	u16 bauddiv = ((F_CPU+(baudrate*8L))/(baudrate*16L)-1);
    switch (nUart) {
        case 0:
            outb(UBRR0L, bauddiv);
            #ifdef UBRR0H
            outb(UBRR0H, bauddiv>>8);
            #endif
            break;
            
        case 1:
            outb(UBRR1L, bauddiv);
            #ifdef UBRR1H
            outb(UBRR1H, bauddiv>>8);
            #endif
            break;
            
        case 2:
            outb(UBRR2L, bauddiv);
            #ifdef UBRR2H
            outb(UBRR2H, bauddiv>>8);
            #endif
            break;
            
        case 3:
            outb(UBRR3L, bauddiv);
            #ifdef UBRR3H
            outb(UBRR3H, bauddiv>>8);
            #endif
            break;
            
        default:
            break;
    }
}

cBuffer* uartGetRxBuffer(u08 nUart) {
    assert(nUart < 4);
	// return rx buffer pointer
	return &uartRxBuffer[nUart];
}

cBuffer* uartGetTxBuffer(u08 nUart) {
    assert(nUart < 4);
	// return tx buffer pointer
	return &uartTxBuffer[nUart];
}

void uartSendByte(u08 nUart, u08 txData) {
    assert(nUart < 4);
    switch (nUart) {
        case 0:
            while(!(UCSR0A & (1<<UDRE0)));
            outb(UDR0, txData);
            break;
            
        case 1:
            while(!(UCSR1A & (1<<UDRE1)));
            outb(UDR1, txData);
            break;
            
        case 2:
            while(!(UCSR2A & (1<<UDRE2)));
            outb(UDR2, txData);
            break;
            
        case 3:
            while(!(UCSR3A & (1<<UDRE3)));
            outb(UDR3, txData);
            break;
            
        default:
            break;
    }
    
    // set ready state to FALSE
	uartReadyTx[nUart] = FALSE;
}

void uart0SendByte(u08 data) {
	// send byte on UART0
	uartSendByte(0, data);
}

void uart1SendByte(u08 data) {
	// send byte on UART1
	uartSendByte(1, data);
}

void uart2SendByte(u08 data) {
    // send byte on UART2
    uartSendByte(2, data);
}

void uart3SendByte(u08 data) {
    // send byte on UART3
    uartSendByte(3, data);
}

int uartGetByte(u08 nUart) {
    assert(nUart < 4);
    u08 c;
    
    // get single byte from receive buffer (if available)
    if(uartReceiveByte(nUart, &c))
        return c;
    else
        return -1;
}

int uart0GetByte(void) {
    return uartGetByte(0);
}

int uart1GetByte(void) {
    return uartGetByte(1);
}

int uart2GetByte(void) {
    return uartGetByte(2);
}

int uart3GetByte(void) {
    return uartGetByte(3);
}


u08 uartReceiveByte(u08 nUart, u08* rxData) {
    assert(nUart < 4);
	// make sure we have a receive buffer
	if(uartRxBuffer[nUart].size) {
		// make sure we have data
		if(uartRxBuffer[nUart].datalength) {
			// get byte from beginning of buffer
			*rxData = bufferGetFromFront(&uartRxBuffer[nUart]);
			return TRUE;
		}
		else
			return FALSE;			// no data
	}
	else
		return FALSE;				// no buffer
}

void uartClearReceiveBuffer(u08 nUart) {
    assert(nUart < 4);
	// flush all data from receive buffer
	bufferFlush(&uartRxBuffer[nUart]);
}

u08 uartReceiveBufferIsEmpty(u08 nUart) {
    assert(nUart < 4);
	return (uartRxBuffer[nUart].datalength == 0);
}

void uartAddToTxBuffer(u08 nUart, u08 data) {
    assert(nUart < 4);
	// add data byte to the end of the tx buffer
	bufferAddToEnd(&uartTxBuffer[nUart], data);
}

void uart0AddToTxBuffer(u08 data) {
	uartAddToTxBuffer(0,data);
}

void uart1AddToTxBuffer(u08 data) {
	uartAddToTxBuffer(1,data);
}

void uart2AddToTxBuffer(u08 data) {
  uartAddToTxBuffer(2,data);
}

void uart3AddToTxBuffer(u08 data) {
    uartAddToTxBuffer(3,data);
}

void uartSendTxBuffer(u08 nUart) {
    assert(nUart < 4);
	// turn on buffered transmit
	uartBufferedTx[nUart] = TRUE;
	// send the first byte to get things going by interrupts
	uartSendByte(nUart, bufferGetFromFront(&uartTxBuffer[nUart]));
}

u08 uartSendBuffer(u08 nUart, char *buffer, u16 nBytes) {
    assert(nUart < 4);
    
    //Assert the buffer is empty so we don't send garbage
    assert(uartTxBuffer[nUart].datalength == 0);
    
	register u08 first;
	register u16 i;

	// check if there's space (and that we have any bytes to send at all)
	if((uartTxBuffer[nUart].datalength + nBytes <= uartTxBuffer[nUart].size) && nBytes)
	{
		// grab first character and increment the buffer
		first = *buffer++;
		// copy user buffer to uart transmit buffer
		for(i = 0; i < nBytes-1; i++)
		{
			// put data bytes at end of buffer
			bufferAddToEnd(&uartTxBuffer[nUart], *buffer++);
		}

		// send the first byte to get things going by interrupts
		uartBufferedTx[nUart] = TRUE;
		uartSendByte(nUart, first);
		// return success
		return TRUE;
	}
	else
	{
		// return failure
		return FALSE;
	}
}

// UART Transmit Complete Interrupt Function
void uartTransmitService(u08 nUart) {
    assert(nUart < 4);
    
	// check if buffered tx is enabled
	if(uartBufferedTx[nUart]) {
		// check if there's data left in the buffer
		if(uartTxBuffer[nUart].datalength) {
			// send byte from top of buffer
            switch (nUart) {
                case 0:
                    outb(UDR0,  bufferGetFromFront(&uartTxBuffer[0]) );
                    break;
                    
                case 1:
                    outb(UDR1,  bufferGetFromFront(&uartTxBuffer[1]) );
                    break;
                    
                case 2:
                    outb(UDR2,  bufferGetFromFront(&uartTxBuffer[2]) );
                    break;
                    
                case 3:
                    outb(UDR3,  bufferGetFromFront(&uartTxBuffer[3]) );
                    break;
                    
                default:
                    break;
            }
		}
		else {
			// no data left
			uartBufferedTx[nUart] = FALSE;
			// return to ready state
			uartReadyTx[nUart] = TRUE;
		}
	}
	else {
		// we're using single-byte tx mode
		// indicate transmit complete, back to ready
		uartReadyTx[nUart] = TRUE;
	}
}

// UART Receive Complete Interrupt Function
void uartReceiveService(u08 nUart) {
    assert(nUart < 4);
    
	u08 c;
	// get received char
    switch (nUart) {
        case 0:
            c = inb(UDR0);
            break;
            
        case 1:
            c = inb(UDR1);
            break;
            
        case 2:
            c = inb(UDR2);
            break;
            
        case 3:
            c = inb(UDR3);
            break;
            
        default:
            return;
    }

	// if there's a user function to handle this receive event
	if(UartRxFunc[nUart]) {
		// call it and pass the received data
		UartRxFunc[nUart](c);
	}
    else {
		// otherwise do default processing
		// put received char in buffer
		// check if there's space
		if( !bufferAddToEnd(&uartRxBuffer[nUart], c) ) {
			// no space in buffer
			// count overflow
			uartRxOverflow[nUart]++;
		}
	}
}

UART_INTERRUPT_HANDLER(USART0_TX_vect) {
	// service UART0 transmit interrupt
	uartTransmitService(0);
}

UART_INTERRUPT_HANDLER(USART1_TX_vect) {
	// service UART1 transmit interrupt
	uartTransmitService(1);
}

UART_INTERRUPT_HANDLER(USART2_TX_vect) {
    // service UART2 transmit interrupt
    uartTransmitService(2);
}

UART_INTERRUPT_HANDLER(USART3_TX_vect) {
    // service UART3 transmit interrupt
    uartTransmitService(3);
}

UART_INTERRUPT_HANDLER(USART0_RX_vect) {
	// service UART0 receive interrupt
	uartReceiveService(0);
}

UART_INTERRUPT_HANDLER(USART1_RX_vect) {
	// service UART1 receive interrupt
	uartReceiveService(1);
}

UART_INTERRUPT_HANDLER(USART2_RX_vect) {
    // service UART2 receive interrupt
    uartReceiveService(2);
}

UART_INTERRUPT_HANDLER(USART3_RX_vect) {
    // service UART3 receive interrupt
    uartReceiveService(3);
}
