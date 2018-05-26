/*
 * ModemConnection.h
 *
 * Created: 5/22/2018 12:51:22 PM
 *  Author: anilj
 */ 


#ifndef MODEMCONNECTION_H_
#define MODEMCONNECTION_H_

/* Main states for Modem Connection */
typedef enum
{
	MODEM_CONNECT_INIT,
	MODEM_CONNECT_STARTUP,
	MODEM_CONNECT_OPERATIONAL,
	MODEM_CONNECT_DEACTIVATE,
	MODEM_CONNECT_FAULTY,
	MODEM_CONNECT_SHUTDOWN	
}MODEM_CONNECTION_STATUS;








#endif /* MODEMCONNECTION_H_ */