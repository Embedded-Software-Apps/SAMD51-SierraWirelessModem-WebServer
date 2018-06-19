/*
 * ModemDiagTask.h
 *
 * Created: 6/20/2018 12:56:23 AM
 *  Author: anilj
 */ 


#ifndef MODEMDIAGTASK_H_
#define MODEMDIAGTASK_H_

typedef enum
{
	MODEM_DIAG_TEST_AT,
	MODEM_DIAG_GET_IMEI,
	MODEM_DIAG_GET_SERIAL,
	MODEM_DIAG_GET_CARRIER
}MODEM_DIAG_STATES_T;









void ModemDiagTask( void *ModemTaskParam);




#endif /* MODEMDIAGTASK_H_ */
