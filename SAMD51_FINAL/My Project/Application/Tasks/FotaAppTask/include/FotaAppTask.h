/*
 * FotaAppTask.h
 *
 * Created: 8/7/2018 8:59:16 PM
 *  Author: anilj
 */ 


#ifndef FOTAAPPTASK_H_
#define FOTAAPPTASK_H_


void FotaAppTask( void *FotaTaskParam);
bool isFotaVerificationDone(void);


/* FOTA Main States */
typedef enum
{
	SET_APN_TO_C0NNECT_WITH_AIRVANTAGE,
	ACTIVATE_DEVICE_SERVICE_INDICATIONS,
	ACTIVATE_USER_AGREEMENTS_WITH_HOST,
	START_DM_SESSION_WITH_AIR_VANTAGE,
	SYSTEM_IS_IN_FIRMWARE_DOWNLOAD_MODE,
	FIRMWARE_UPDATE_COMPLETED_SUCCESSFULLY,
	ERROR_OCCURED_DURING_FIRMWARE_DOWNLOAD,
	REVERT_FIRMWARE_TO_PREVIOUS_VERSION
}FOTA_MAIN_STATES_T;

/* User Agreements sub states */
typedef enum
{
	ACTIVATE_USER_AGREEMENT_FOR_FW_DOWNLOAD,
	ACTIVATE_USER_AGREEMENT_FOR_FW_INSTALLATION
}USER_AGREEMENT_ACTIVATION_STATES_T;

typedef enum
{
	APN_SET_READY_FOR_DM_SESSION = 0,                  /* +WDSI: 0 */
	AIRVANTAGE_ORIGINATED_DM_SESSION_REQUEST = 1,      /* +WDSI: 1 */
	AIRVANTAGE_REQUESTS_FOR_FIRMWARE_DOWNLOAD = 2,     /* +WDSI: 2 */
	AIRVANTAGE_REQUESTS_FOR_FIRMWARE_INSTALLATION = 3, /* +WDSI: 3 */
	STARTING_UP_THE_DM_SESSION_WITH_AIR_VANTAGE = 4,   /* +WDSI: 4 */
	DM_SESSION_STARTED_TRANSACTIONS_OCCURED = 6,       /* +WDSI: 6 */
	DM_SESSION_WITH_AIRVANTAGE_IS_CLOSED = 8,          /* +WDSI: 8 */
	FIRMWARE_IS_AVAILABLE_FOR_DOWNLOAD = 9,            /* +WDSI: 9 */
	FIRMWARE_IS_DOWNLOADED_STORED_IN_FLASH = 10,       /* +WDSI: 10 */
	DOWNLOADED_PACKAGE_IS_VERIFIED_AS_CERTIFIED = 12,  /* +WDSI: 12 */
	STARTING_THE_FIRMWARE_UPDATE = 14,                 /* +WDSI: 14 */
	FAILED_TO_UPDATE_THE_FIRMWARE = 15,                /* +WDSI: 15 */
	FIRMWARE_UPDATED_SUCCESSFULLY = 16,                /* +WDSI: 16 */
	DOWNLOAD_IN_PROGRESS_IN_PERCENTAGE = 18            /* +WDSI: 18 */
}DEVICE_SERVICE_INDICATION_TYPE;


typedef enum
{
	FOTA_APP_OPERATIONAL_TX_MODE,
	FOTA_APP_OPERATIONAL_RX_MODE
}FOTA_APP_OPERATIONAL_STATE_T;
#endif /* FOTAAPPTASK_H_ */
