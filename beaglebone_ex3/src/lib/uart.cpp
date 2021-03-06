/**
 * @brief 
 * 
 * @file uart.cpp
 * @author NKNguyen
 * @date 2018-09-17
 */

#include "uart.h"
#include <iostream>

using namespace std;
/**
 * @brief Open UART port
 * 
 * @param uart 
 * @return int 
 */
int uart_open(uart_properties *uart) {
	// Find the address
	char buf[30] = "/dev/ttyO";
	char port_nr[2];
	sprintf(port_nr, "%d", uart->uart_id);
	strcat(buf,port_nr);
	struct termios uart_port;

	uart->fd = open(buf, O_RDWR | O_NOCTTY);
	if(uart->fd < 0) printf("port failed to open\n");

	bzero(&uart_port,sizeof(uart_port));
	// Choose UART mode and baurate
	uart_port.c_cflag = uart->baudrate | CS8 | CLOCAL | CREAD;
	uart_port.c_iflag = IGNPAR | ICRNL;
	uart_port.c_oflag = 0;
	uart_port.c_lflag = 0;

	uart_port.c_cc[VTIME] = 0;
	uart_port.c_cc[VMIN]  = 1;

	//clean the line and set the attributes
	tcflush(uart->fd,TCIFLUSH);
	tcsetattr(uart->fd,TCSANOW,&uart_port);
	return 0;
}
/**
 * @brief Send string via UART
 * 
 * @param uart 
 * @param tx 
 * @param length 
 * @return int 
 */
int uart_send(uart_properties *uart, char *tx, int length) {
	if (write(uart->fd, tx, length) == -1) {
		syslog(LOG_ERR, "Could not write %s to uart %i", tx, uart->uart_id);
		return -1;
	}
	syslog(LOG_INFO, "Wrote %s to uart %i", tx, uart->uart_id);
	return 0;
}
/**
 * @brief Read char via UART
 * 
 * @param uart 
 * @param rx 
 * @param length 
 * @return int 
 */
int uart_read(uart_properties *uart,char *rx, int length) {
	int count;
	if( (count = read(uart->fd,(void*)rx,length)) > 0) {
		syslog(LOG_ERR, "Could not read from uart %i", uart->uart_id);
		return count;
	}
	syslog(LOG_INFO,"Read %s from uart %i", rx, uart->uart_id);
	return 0;
}
/**
 * @brief Close UART port
 * 
 * @param uart 
 * @return int 
 */
int uart_close(uart_properties *uart) {
	close(uart->fd);
	return 0;
}
