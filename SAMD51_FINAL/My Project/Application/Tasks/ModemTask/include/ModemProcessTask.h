/*
 * ModemProcessTask.h
 *
 * Created: 6/17/2018 8:31:40 AM
 *  Author: anilj
 */ 


#ifndef MODEMPROCESSTASK_H_
#define MODEMPROCESSTASK_H_


void ModemProcessTask(void *ModemTaskParam);
void clearPacketTransmitTimerExpiryFlag(void);
bool isPacketTransmitTimerExpired(void);

#endif /* MODEMPROCESSTASK_H_ */
