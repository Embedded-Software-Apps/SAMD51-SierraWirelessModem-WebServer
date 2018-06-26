/*
 * ModemDiagTask.h
 *
 * Created: 6/20/2018 12:56:23 AM
 *  Author: anilj
 */ 


#ifndef MODEMDIAGTASK_H_
#define MODEMDIAGTASK_H_

#include "Apps/Common/Common.h"
#include "Apps/Tasks/ModemTask/include/ModemAtCommandSet.h"

typedef enum
{
	MODEM_DIAG_TEST_AT,
	MODEM_DIAG_GET_IMEI,
	MODEM_DIAG_GET_SERIAL,
	MODEM_DIAG_GET_CARRIER
}MODEM_DIAG_STATES_T;

typedef struct
{
	MODEM_DIAG_STATES_T diagState;
	uint8_t* diagData;

}DIAG_RESPONSE_DATABASE;





void ModemDiagTask( void *ModemTaskParam);
void ModemDiagUpdateDataBase(CmdResponseType* cmdResponse);



#endif /* MODEMDIAGTASK_H_ */
