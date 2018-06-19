#ifndef ATMEL_START_H_INCLUDED
#define ATMEL_START_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "driver_init.h"
#include "rtos_start.h"

#define WDT_FREQ (1000)
#define WDT_1_SEC_TIMEOUT  (16)
#define WDT_3_SEC_TIMEOUT  (64)
#define WDT_10_SEC_TIMEOUT (8192)

typedef enum
{
    WDT_TIMEOUT_FAST,
    WDT_TIMEOUT_NORMAL,
    WDT_TIMEOUT_DELAYED,
}WDT_TIMEOUT_TYPE;


typedef struct
{
    WDT_TIMEOUT_TYPE type;
    uint16_t timeOutValue;
}WdtDelay;

extern WdtDelay cfgSystemWdt[3];

/**
 * Initializes MCU, drivers and middleware in the project
**/
void atmel_start_init(void);
void configureWatchDogTimeOut(WDT_TIMEOUT_TYPE type);
int32_t kickWatchDog(void);
void enableWatchDogTimer(void);

#ifdef __cplusplus
}
#endif
#endif
