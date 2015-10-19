#include "common.h"
#include "varapi.h"
#include "Variable.h"

#include "AgentService.h" 
#include "MalfMonitor.h" 
#include "MeterUploader.h"
#include "Metering.h"
#include "CommonUtil.h"
#include "SystemUtil.h"
#include "Utility.h"
#include "MemoryDebug.h"
#include "DebugServer.h"
#include "DebugUtil.h"
#include "revision.h"

#include "TCPMultiplexServer.h"

#include "rep/VariableHelper.h"

#define DEFAULT_COORDINATOR_INDEX        0

extern  BOOL m_bEnableCommandLog;
extern  int    m_nDebugLevel;

BOOL    m_bGprsPerfTestMode         = FALSE;
BOOL    m_bPingTestMode             = FALSE;

int     m_nSystemType               = MCU_TYPE_OUTDOOR;
UINT    m_nSystemID                 = 0;

int     m_nEthernetType             = ETHER_TYPE_PPP;
int     m_nMobileType               = MOBILE_TYPE_GSM;
int     m_nMobileMode               = MOBILE_MODE_CSD;
UINT    m_nMobileVendor             = MOBILE_VENDOR_MC55;
char    m_szMobileApn[65]           = "";

int     m_nResetReason              = 0;
char    m_szFepServer[256]          = "0.0.0.0";
int     m_nFepPort                  = IF4_DEFAULT_PORT;
int     m_nFepAlarmPort             = IF4_DEFAULT_ALARM_PORT;
int     m_nLocalPort                = IF4_DEFAULT_PORT;

int     m_nCircuitMode              = FALSE;
int     m_nSensorLimit              = -1;
int     m_nJoinNodeCount            = 0;
//sungyeung
BYTE    m_nOpMode                   = OP_MODE_TEST;
//BYTE    m_nOpMode                   = OP_MODE_NORMAL;


BOOL    m_bEnableAutoMetering       = TRUE;
BOOL    m_bEnableKeepaliveEvent     = FALSE;
BOOL    m_bEnableSecurity           = FALSE;
BOOL    m_bEnableAutoRegister       = TRUE;
BOOL    m_bEnableAutoReset          = TRUE;
BOOL    m_bEnableGpsTimesync        = TRUE;
BOOL    m_bEnableRecovery           = TRUE;
BOOL    m_bEnableAutoUpload         = TRUE;
BOOL    m_bEnableMemoryCheck        = TRUE;
BOOL    m_bEnableFlashCheck         = TRUE;
BOOL    m_bEnableTimeBroadcast      = TRUE;
BOOL    m_bEnableStatusMonitor      = FALSE;
BOOL    m_bEnableCommLog            = TRUE;
BOOL    m_bEnableMobileLog          = TRUE;
BOOL    m_bEnableAutoTimesync       = TRUE;

BOOL    m_bEnableLowBattery         = TRUE;
BOOL    m_bEnableReadMeterEvent     = TRUE;
BOOL    m_bEnableMeterTimesync      = FALSE;
BOOL    m_bEnableMeterCheck         = FALSE;

BOOL    m_bEnableHighRAM            = FALSE;        // Issue #1965: Default LOW RAM Coordinator
BOOL    m_bEnableGarbageCleaning    = FALSE;        // Issue #1964
BOOL    m_bEnableDebugLog           = FALSE;        // Issue #1426: 시작할때는 항상 Off
int     m_nDebugServerPort          = 9981;         // Issue #2426: DebugServer Port

UINT    m_nMeteringInterval         = 60;           // varMeterReadingInterval(2.3.61에서 사용)
BYTE    m_nAllUploadHour            = 4; 

BOOL    m_bActiveRetry              = FALSE;
int     m_nGroupInterval            = 1;
int     m_nRetryStartSecond         = 23;
UINT    m_nDataSaveDay              = 14;

UINT    m_nMeterDayMask             = 0x7FFFFFFF;
UINT    m_nMeterHourMask            = 0x00000002;                       // 01시에 읽기 시작한다.
BYTE    m_nMeterStartMin            = 5;                                // 05분에 시작한다.
UINT    m_nMeteringRetry            = 3;
UINT    m_nMeteringPeriod           = 80;                               // 80분을 읽는다

/** Issue #10 : Scheduler Type 지원 */
BYTE    m_nSchedulerType            = SCHEDULER_TYPE_MASK;

UINT    m_nEventReadDayMask         = 0x7FFFFFFF;                        // 매일
UINT    m_nEventReadHourMask        = 0x00FFFFFE;                        // Metering/Recovery시 항상 읽기 시도

UINT    m_nTimesyncDayMask          = 0x7FFFFFFF;
UINT    m_nTimesyncHourMask         = 0x00000020;

int     m_nTimesyncThresholdLow    = 0;                                // 0 : Disable, n : sec
int     m_nTimesyncThresholdHigh   = 0;                                // 0 : Disable, n : sec

UINT    m_nRecoveryDayMask          = 0x7FFFFFFF;
UINT    m_nRecoveryHourMask         = 0x00FFFFFC;                       // 03시 부터 계속 읽기 시작한다.
BYTE    m_nRecoveryStartMin         = 5;
UINT    m_nRecoveryRetry            = 3;
UINT    m_nRecoveryPeriod           = 35;                               // 35분을 읽는다

int     m_nUploadType               = UPLOAD_TYPE_HOURLY;
int     m_nUploadCycle              = 0x00FFFFFF;
int     m_nUploadStartHour          = 6;
int     m_nUploadStartMin           = 46;
int     m_nUploadRetry              = 3;
UINT    m_nUploadTryTime            = 10;

UINT    m_nCheckDayMask             = 0x7FFFFFFF;
UINT    m_nCheckHourMask            = 0x00FFFFFF;
BYTE    m_nCheckStartMin            = 40;
UINT    m_nCheckRetry               = 3;
UINT    m_nCheckPeriod              = 10;

int     m_nAutoResetInterval        = 1;
TIMESTAMP   m_tmAutoResetTime       = { 0, 0, 0, 0, 10, 0 };
TIMESTAMP   m_tmMeterIssueDate      = { 0, 0, 0, 0, 0, 0 };

int     m_nTimeBroadcastInterval    = 60 * 15;                 // 15분
int     m_nKeepaliveInvterval       = 60;
int     m_nMobileLiveCheckInterval  = 10;                    // 10분
int     m_nStatusMonitorMin         = 35;
int     m_nMobileLiveCheckMethod    = 0;

int     m_nEventLogSize             = 100;
int     m_nAlarmLogSize             = 100;
int     m_nCmdLogSize               = 100;
int     m_nCommLogSize              = 100;
int     m_nMobileLogSize            = 100;
int     m_nMeterLogSize             = 200;
int     m_nUpgradeLogSize           = 100;
int     m_nUploadLogSize            = 100;
int     m_nTimesyncLogSize          = 100;

int     m_nAlarmSaveDays            = 7;
int     m_nCmdHistSaveDay           = 7;
int     m_nEventLogSaveDay          = 7;
int     m_nCommLogSaveDay           = 7;
int     m_nMobileLogSaveDay         = 7;
int     m_nMeterLogSaveDay          = 7;
int     m_nUpgradeLogSaveDay        = 7;
int     m_nUploadLogSaveDay         = 7;
int     m_nTimesyncLogSaveDay       = 7;

int     m_nPowerOffDelay            = 5;
int     m_nTempMonInterval          = 5;
int     m_nAutoTimeSyncInterval     = 12 * 60 * 60;                // 12시간
int     m_nMeteringThreadCount      = 1;
int     m_nSinkPollingInterval      = 10;
int     m_nSinkAckWaitTime          = 5;
int     m_nSinkResetInterval        = 60;
int     m_nSinkResetDelay           = 3;
int     m_nSinkLedTurnOffInterval   = 0;

int     m_nTempRangeMin             = -100;                        // -10도 이하에서 이벤트 발생
int     m_nTempRangeMax             = 0;                        // 0도 이상에서 이벤트 발생

BOOL    m_bEnableHeater             = TRUE;
int     m_nHeaterOnTemp             = -100;                        // -10도에서 히터 가동
int     m_nHeaterOffTemp            = 0;                        // 0도에서 히터 OFF

BYTE    m_nEventAlertLevel          = EVTCLASS_MAJOR;
UINT    m_nEventSendDelay           = 120;                        // 디폴트 120초

BYTE    m_nMeteringStrategy         = METERING_STRATEGY_SERIAL; // 가능한 빨리 Metering
BYTE    m_nScanningStrategy         = SCANNING_STRATEGY_LAZY;   // 최대한 늦게 Inventory Scanning 수행

BOOL    m_bEnableMeterRecovery      = TRUE;                     // Meter Error가 있을 때 자동 복구를 시도

BYTE    m_nDefaultMeteringOption  = 0;                        // 기본적으로 0번을 사용한다
BYTE    m_nSensorCleaningThreshold  = 7;                      // Issue #1964: 기본으로 7일 이상 통신이 안되는 센서는 삭제한다

/*-- Issue #2020 : TimeSync 조합이 많아지기 때문에 이제 STRATEGY를 정의해서 쓰도록 한다. --*/
BYTE    m_nTimeSyncStrategy         = TIMESYNC_STRATEGY_HIBRID_MESH;

BYTE    m_nTransactionSaveDay       = 7;                        // Issue #1502 : Transaction 저장 기간

BOOL    m_bSuperParent              = FALSE;
BYTE    m_nCoordinatorFwVersion     = 0x00;
BYTE    m_nCoordinatorFwBuild       = 0x00;

UINT    m_nNzcPowerType             = NZC_POWER_TYPE_LINE | NZC_POWER_TYPE_BATTERY;

short   m_nTimeZone                 = 0;                        // TimeZone (min)

int     m_nMobileCSQ                = 0;
BYTE    m_nHwVersion                = HW_VERSION;

//  [9/13/2010 DHKim]
#if !defined(__DISABLE_SSL__)
extern  BYTE    m_nSecurityState;
#else
BYTE    m_nSecurityState = 0;
#endif
BOOL		m_bEnableJoinControl = FALSE;
//////////////////////////////////////////////////////////////////////////

extern  BYTE ge_negotiate[];
extern  BYTE ge_logon[];
extern  BYTE ge_master_security[];

#if     defined(__PATCH_4_1647__)
extern  BOOL    *m_pbPatch_4_1647;
#endif

VAROBJECT    m_Root_node[] =
{
    {   1,    "smi", { {1,0,0}, VARSMI_OID, 3 },                    VARATTR_RW,   m_smi_node, NULL, NULL },
    {   2,    "mcu", { {2,0,0}, VARSMI_OID, 3 },                    VARATTR_READ, m_mcu_node, NULL, NULL },
    {   3,    "sink", { {3,0,0}, VARSMI_OID, 3 },                   VARATTR_READ, m_sink_node, NULL, NULL },
    {   4,    "sensor", { {4,0,0}, VARSMI_OID, 3 },                 VARATTR_READ, m_sensor_node, NULL, NULL },
    {   5,    "network", { {5,0,0}, VARSMI_OID, 3 },                VARATTR_READ, m_network_node, NULL, NULL },
    {   6,    "transaction", { {6,0,0}, VARSMI_OID, 3 },            VARATTR_READ, m_transaction_node, NULL, NULL },
	{   7,    "group", { {7,0,0}, VARSMI_OID, 3 },          VARATTR_READ, m_group_node, NULL, NULL },		//  [11/10/2010 DHKim]
    {  10,    "meter", { {10,0,0}, VARSMI_OID, 3 },                 VARATTR_READ, m_meter_node, NULL, NULL },
    { 200,    "eventMcu", { {200,0,0}, VARSMI_OID, 3 },             VARATTR_READ, m_eventMcu_node, NULL, NULL },
    { 201,    "eventSink", { {201,0,0}, VARSMI_OID, 3 },            VARATTR_READ, m_eventSink_node, NULL, NULL },
    { 202,    "eventMobile", { {202,0,0}, VARSMI_OID, 3 },          VARATTR_READ, m_eventMobile_node, NULL, NULL },
    { 203,    "eventSensor", { {203,0,0}, VARSMI_OID, 3 },          VARATTR_READ, m_eventSensor_node, NULL, NULL },
    { 204,    "eventMalf", { {204,0,0}, VARSMI_OID, 3 },            VARATTR_READ, m_eventMalf_node, NULL, NULL },
    { 205,    "eventComm", { {205,0,0}, VARSMI_OID, 3 },            VARATTR_READ, m_eventComm_node, NULL, NULL },
    { 206,    "Alarm", { {206,0,0}, VARSMI_OID, 3 },                VARATTR_READ, m_Alarm_node, NULL, NULL },
    { 212,    "eventMeter", { {212,0,0}, VARSMI_OID, 3 },           VARATTR_READ, m_eventMeter_node, NULL, NULL },
	{ 214,	"eventOta", { {214,0,0}, VARSMI_OID, 0 },               VARATTR_READ, m_eventOta_node, NULL, NULL },
    { 215,    "eventTransaction", { {215,0,0}, VARSMI_OID, 3 },     VARATTR_READ, m_eventTransaction_node, NULL, NULL },
    { 255,    "eventManagement", { {255,0,0}, VARSMI_OID, 3 },      VARATTR_READ, m_eventManagement_node, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_smi_node[] =
{
    {   1,    "smiEntry", { {1,1,0}, VARSMI_OID, 3 },             VARATTR_RW, NULL, NULL, NULL },
    {   2,    "unknownEntry", { {1,2,0}, VARSMI_BYTE, 0 },        VARATTR_RW, NULL, NULL, NULL },
    {   3,    "boolEntry", { {1,3,0}, VARSMI_BOOL, 1 },           VARATTR_RW, NULL, NULL, NULL },
    {   4,    "byteEntry", { {1,4,0}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, NULL },
    {   5,    "wordEntry", { {1,5,0}, VARSMI_WORD, 2 },           VARATTR_RW, NULL, NULL, NULL },
    {   6,    "uintEntry", { {1,6,0}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
    {   7,    "charEntry", { {1,7,0}, VARSMI_CHAR, 1 },           VARATTR_RW, NULL, NULL, NULL },
    {   8,    "shortEntry", { {1,8,0}, VARSMI_SHORT, 2 },         VARATTR_RW, NULL, NULL, NULL },
    {   9,    "intEntry", { {1,9,0}, VARSMI_INT, 4 },             VARATTR_RW, NULL, NULL, NULL },
    {  10,    "oidEntry", { {1,10,0}, VARSMI_OID, 3 },            VARATTR_RW, m_oidEntry_node, NULL, oidEntryQuery },
    {  11,    "stringEntry", { {1,11,0}, VARSMI_STRING, 0 },      VARATTR_RW, NULL, NULL, NULL },
    {  12,    "streamEntry", { {1,12,0}, VARSMI_STREAM, 0 },      VARATTR_RW, NULL, NULL, NULL },
    {  13,    "opaqueEntry", { {1,13,0}, VARSMI_OPAQUE, 0 },      VARATTR_RW, NULL, NULL, NULL },
    {  14,    "eui64Entry", { {1,14,0}, VARSMI_EUI64, 8 },        VARATTR_RW, m_eui64Entry_node, NULL, eui64EntryQuery },
    {  15,    "ipEntry", { {1,15,0}, VARSMI_IPADDR, 4 },          VARATTR_RW, m_ipEntry_node, NULL, ipEntryQuery },
    {  16,    "timeEntry", { {1,16,0}, VARSMI_TIMESTAMP, 7 },     VARATTR_RW, m_timeEntry_node, NULL, timeEntryQuery },
    {  17,    "dayEntry", { {1,17,0}, VARSMI_TIMEDATE, 4 },       VARATTR_RW, m_dayEntry_node, NULL, dayEntryQuery },
    {  18,    "verEntry", { {1,18,0}, VARSMI_STRING, 16 },        VARATTR_RW, m_verEntry_node, NULL, verEntryQuery },
    {  19,    "smivarEntry", { {1,19,0}, VARSMI_STREAM, 0 },      VARATTR_RW, m_smivarEntry_node, NULL, smivarEntryQuery },
    {  20,    "pageEntry", { {1,20,0}, VARSMI_STREAM, 0 },        VARATTR_RW, m_pageEntry_node, NULL, pageEntryQuery },
    {  21,    "gmtEntry", { {1,21,0}, VARSMI_STREAM, 11 },        VARATTR_RW, m_gmtEntry_node, NULL, gmtEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_oidEntry_node[] =
{
    {   1,    "oidDot1", { {1,10,1}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, oidEntryQuery },
    {   2,    "oidDot2", { {1,10,2}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, oidEntryQuery },
    {   3,    "oidDot3", { {1,10,3}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, oidEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_eui64Entry_node[] =
{
    {   1,    "eui64High", { {1,14,1}, VARSMI_UINT, 4 },          VARATTR_RW, NULL, NULL, eui64EntryQuery },
    {   2,    "eui64Low", { {1,14,2}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, eui64EntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_ipEntry_node[] =
{
    {   1,    "ipDot1", { {1,15,1}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, ipEntryQuery },
    {   2,    "ipDot2", { {1,15,2}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, ipEntryQuery },
    {   3,    "ipDot3", { {1,15,3}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, ipEntryQuery },
    {   4,    "ipDot4", { {1,15,4}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, ipEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_timeEntry_node[] =
{
    {   1,    "timeYear", { {1,16,1}, VARSMI_WORD, 2 },           VARATTR_RW, NULL, NULL, timeEntryQuery },
    {   2,    "timeMon", { {1,16,2}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, timeEntryQuery },
    {   3,    "timeDay", { {1,16,3}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, timeEntryQuery },
    {   4,    "timeHour", { {1,16,4}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, timeEntryQuery },
    {   5,    "timeMin", { {1,16,5}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, timeEntryQuery },
    {   6,    "timeSec", { {1,16,6}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, timeEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_dayEntry_node[] =
{
    {   1,    "dayYear", { {1,17,1}, VARSMI_WORD, 2 },            VARATTR_RW, NULL, NULL, dayEntryQuery },
    {   2,    "dayMon", { {1,17,2}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, dayEntryQuery },
    {   3,    "dayDay", { {1,17,3}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, dayEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_verEntry_node[] =
{
    {   1,    "verPrefix", { {1,18,1}, VARSMI_STRING, 3 },        VARATTR_RW, NULL, NULL, verEntryQuery },
    {   2,    "verFixed1", { {1,18,2}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, verEntryQuery },
    {   3,    "verHwVersion", { {1,18,3}, VARSMI_STRING, 2 },     VARATTR_RW, NULL, NULL, verEntryQuery },
    {   4,    "verFixed2", { {1,18,4}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, verEntryQuery },
    {   5,    "verSwVersion", { {1,18,5}, VARSMI_STRING, 4 },     VARATTR_RW, NULL, NULL, verEntryQuery },
    {   6,    "verFixed3", { {1,18,6}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, verEntryQuery },
    {   7,    "verService", { {1,18,7}, VARSMI_STRING, 4 },       VARATTR_RW, NULL, NULL, verEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_smivarEntry_node[] =
{
    {   1,    "smivarOid", { {1,19,1}, VARSMI_OID, 3 },           VARATTR_RW, NULL, NULL, smivarEntryQuery },
    {   2,    "smivarLength", { {1,19,2}, VARSMI_WORD, 2 },       VARATTR_RW, NULL, NULL, smivarEntryQuery },
    {   3,    "smivarData", { {1,19,3}, VARSMI_STREAM, 0 },       VARATTR_RW, NULL, NULL, smivarEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_pageEntry_node[] =
{
    {   1,    "pageNumber", { {1,20,1}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, pageEntryQuery },
    {   2,    "pageLength", { {1,20,2}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, pageEntryQuery },
    {   3,    "pageData", { {1,20,3}, VARSMI_STREAM, 0 },         VARATTR_RW, NULL, NULL, pageEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_gmtEntry_node[] =
{
    {   1,    "gmtTimezone", { {1,21,1}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, gmtEntryQuery },
    {   2,    "gmtDstValue", { {1,21,2}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, gmtEntryQuery },
    {   3,    "gmtYear", { {1,21,3}, VARSMI_WORD, 2 },            VARATTR_RW, NULL, NULL, gmtEntryQuery },
    {   4,    "gmtMon", { {1,21,4}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, gmtEntryQuery },
    {   5,    "gmtDay", { {1,21,5}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, gmtEntryQuery },
    {   6,    "gmtHour", { {1,21,6}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, gmtEntryQuery },
    {   7,    "gmtMin", { {1,21,7}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, gmtEntryQuery },
    {   8,    "gmtSec", { {1,21,8}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, gmtEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_mcu_node[] =
{
    {   1,    "sysEntry",         { {2,1,0}, VARSMI_OPAQUE, sizeof(SYSENTRY) },
                                VARATTR_RW, m_sysEntry_node, NULL, sysEntryQuery },
    {   2,    "varEntry",         { {2,2,0}, VARSMI_OPAQUE, sizeof(VARENTRY) },
                                VARATTR_RW, m_varEntry_node, NULL, varEntryQuery },
    {   3,    "varValueEntry",     { {2,3,0}, VARSMI_OPAQUE, sizeof(VARVALUEENTRY) },
                                VARATTR_RW, m_varValueEntry_node, NULL, varValueEntryQuery },
    {   4,    "varSubValueEntry", { {2,4,0}, VARSMI_OPAQUE, sizeof(VARSUBVALUEENTRY) },
                                VARATTR_RW, m_varSubValueEntry_node, NULL, varSubValueEntryQuery },
    {   5,    "varGeEntry",         { {2,5,0}, VARSMI_OPAQUE, sizeof(VARGEENTRY) },
                                VARATTR_RW, m_varGeEntry_node, NULL, varGeEntryQuery },
    {   6,    "varEvtCfgEntry",     { {2,6,0}, VARSMI_OPAQUE, sizeof(EVTCFGENTRY) },
                                VARATTR_RW, m_varEvtCfgEntry_node, NULL, NULL },
    {  10,    "usrEntry",         { {2,10,0}, VARSMI_OPAQUE, sizeof(USRENTRY) },
                                VARATTR_RW | VARATTR_LIST, m_usrEntry_node, NULL, usrEntryQuery },
    {  11,    "gpsEntry",         { {2,11,0}, VARSMI_OPAQUE, sizeof(GPSENTRY) },
                                VARATTR_RW | VARATTR_LIST, m_gpsEntry_node, NULL, gpsEntryQuery },
    { 100,    "memEntry",         { {2,100,0}, VARSMI_OPAQUE, sizeof(MEMENTRY) },
                                VARATTR_READ, m_memEntry_node, NULL, memEntryQuery },
    { 101,    "flashEntry",         { {2,101,0}, VARSMI_OPAQUE, sizeof(FLASHENTRY) },
                                VARATTR_READ, m_flashEntry_node, NULL, flashEntryQuery },
    { 102,    "ethEntry",         { {2,102,0}, VARSMI_OPAQUE, sizeof(ETHENTRY) },
                                VARATTR_READ, m_ethEntry_node, NULL, ethEntryQuery },
    { 103,    "gpioEntry",         { {2,103,0}, VARSMI_OPAQUE, sizeof(GPIOENTRY) },
                                VARATTR_READ, m_gpioEntry_node, NULL, gpioEntryQuery },
    { 104,    "mobileEntry",         { {2,104,0}, VARSMI_OPAQUE, sizeof(MOBILEENTRY) },
                                VARATTR_READ, m_mobileEntry_node, NULL, mobileEntryQuery },
    { 105,    "pluginEntry",         { {2,105,0}, VARSMI_OPAQUE, sizeof(PLUGINENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_pluginEntry_node, NULL, pluginEntryQuery },
    { 106,    "procEntry",         { {2,106,0}, VARSMI_OPAQUE, sizeof(PROCENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_procEntry_node, NULL, procEntryQuery },
    { 107,    "pppEntry",         { {2,107,0}, VARSMI_OPAQUE, sizeof(PPPENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_pppEntry_node, NULL, pppEntryQuery },
    { 108,    "envEntry",         { {2,108,0}, VARSMI_OPAQUE, sizeof(ENVENTRY) },
                                VARATTR_READ, m_envEntry_node, NULL, envEntryQuery },
    { 109,    "gpioCtrlEntry",     { {2,109,0}, VARSMI_OPAQUE, sizeof(GPIOCTRLENTRY) },
                                VARATTR_READ, m_gpioCtrlEntry_node, NULL, gpioCtrlEntryQuery },
    { 110,    "patchEntry",         { {2,110,0}, VARSMI_OPAQUE, sizeof(PATCHENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_patchEntry_node, NULL, NULL },
    { 111,    "fwEntry",          { {2,111,0}, VARSMI_OPAQUE, sizeof(FWENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_fwEntry_node, NULL, NULL },
    { 200,    "commLogEntry",     { {2,200,0}, VARSMI_OPAQUE, sizeof(COMMLOGENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_commLogEntry_node, NULL, commLogEntryQuery },
    { 201,    "commLogStruct",     { {2,201,0}, VARSMI_OPAQUE, sizeof(COMMLOGSTRUCT) },
                                VARATTR_READ, m_commLogStruct_node, NULL, commLogStructQuery },
    { 202,    "cmdHistEntry",     { {2,202,0}, VARSMI_OPAQUE, sizeof(CMDHISTENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_cmdHistEntry_node, NULL, cmdHistEntryQuery },
    { 203,    "cmdHistStruct",     { {2,203,0}, VARSMI_OPAQUE, sizeof(CMDHISTSTRUCT) },
                                VARATTR_READ, m_cmdHistStruct_node, NULL, cmdHistStructQuery },
    { 204,    "mcuEventEntry",     { {2,204,0}, VARSMI_OPAQUE, sizeof(MCUEVENTENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_mcuEventEntry_node, NULL, mcuEventEntryQuery },
    { 205,    "mcuEventStruct",     { {2,205,0}, VARSMI_OPAQUE, sizeof(MCUEVENTSTRUCT) },
                                VARATTR_READ, m_mcuEventStruct_node, NULL, mcuEventStructQuery },
    { 206,    "meterLogEntry",     { {2,206,0}, VARSMI_OPAQUE, sizeof(METERLOGENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_meterLogEntry_node, NULL, meterLogEntryQuery },
    { 207,    "meterLogStruct",     { {2,207,0}, VARSMI_OPAQUE, sizeof(METERLOGSTRUCT) },
                                VARATTR_READ, m_meterLogStruct_node, NULL, meterLogStructQuery },
    { 208,    "mobileLogEntry",     { {2,208,0}, VARSMI_OPAQUE, sizeof(MOBILELOGENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_mobileLogEntry_node, NULL, mobileLogEntryQuery },
    { 209,    "mobileLogStruct",     { {2,209,0}, VARSMI_OPAQUE, sizeof(MOBILELOGSTRUCT) },
                                VARATTR_READ, m_mobileLogStruct_node, NULL, mobileLogStructQuery },
    {   0,    NULL }
};

VAROBJECT    m_sysEntry_node[] =
{
    {   1,    "sysMask", { {2,1,1}, VARSMI_UINT, 4 },                 VARATTR_RW, NULL, NULL, sysEntryQuery },
    {   2,    "sysID", { {2,1,2}, VARSMI_UINT, 4 },                   VARATTR_RW, NULL, NULL, sysEntryQuery },
    {   3,    "sysType", { {2,1,3}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, sysEntryQuery },
    {   4,    "sysName", { {2,1,4}, VARSMI_STRING, 64 },              VARATTR_RW, NULL, NULL, sysEntryQuery },
    {   5,    "sysDescr", { {2,1,5}, VARSMI_STRING, 64 },             VARATTR_RW, NULL, NULL, sysEntryQuery },
    {   6,    "sysLocation", { {2,1,6}, VARSMI_STRING, 64 },          VARATTR_RW, NULL, NULL, sysEntryQuery },
    {   7,    "sysContact", { {2,1,7}, VARSMI_STRING, 64 },           VARATTR_RW, NULL, NULL, sysEntryQuery },
    {   8,    "sysHwVersion", { {2,1,8}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, sysEntryQuery },
    {   9,    "sysSwVersion", { {2,1,9}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  10,    "sysPhoneNumber", { {2,1,10}, VARSMI_STRING, 16 },      VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  11,    "sysEtherType", { {2,1,11}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  12,    "sysMobileType", { {2,1,12}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  13,    "sysMobileMode", { {2,1,13}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  14,    "sysMobileAPN", { {2,1,14}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  15,    "sysUpTime", { {2,1,15}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  16,    "sysTime", { {2,1,16}, VARSMI_TIMESTAMP, 7 },           VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  17,    "sysCurTemp", { {2,1,17}, VARSMI_INT, 4 },              VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  18,    "sysMinTemp", { {2,1,18}, VARSMI_INT, 4 },              VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  19,    "sysMaxTemp", { {2,1,19}, VARSMI_INT, 4 },              VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  20,    "sysServer", { {2,1,20}, VARSMI_STRING, 256 },          VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  21,    "sysServerPort", { {2,1,21}, VARSMI_UINT, 4 },          VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  22,    "sysServerAlarmPort", { {2,1,22}, VARSMI_UINT, 4 },     VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  23,    "sysLocalPort", { {2,1,23}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  24,    "sysState", { {2,1,24}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  25,    "sysVendor", { {2,1,25}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  26,    "sysModel", { {2,1,26}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  27,    "sysCircuitMode", { {2,1,27}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  28,    "sysMobileVendor", { {2,1,28}, VARSMI_UINT, 4 },        VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  29,    "sysMobileAccessPointName", { {2,1,29}, VARSMI_STRING, 64 },VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  30,    "sysSwRevision", { {2,1,30}, VARSMI_STRING, 32 },       VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  31,    "sysResetReason", { {2,1,31}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  32,    "sysJoinNodeCount", { {2,1,32}, VARSMI_UINT, 4 },       VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  33,    "sysTimeZone", { {2,1,33}, VARSMI_SHORT, 2 },           VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  34,    "sysOpMode", { {2,1,34}, VARSMI_BYTE, 1 },              VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  35,    "sysPowerType", { {2,1,35}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, sysEntryQuery },
    {  36,    "sysStateMask", { {2,1,36}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, sysEntryQuery },
    {  37,    "sysSecurityState", { {2,1,37}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, sysEntryQuery },            //  [9/13/2010 DHKim]
    {   0,    NULL }
};

VAROBJECT    m_varEntry_node[] =
{
    {   1,    "varEnableMask", { {2,2,1}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, varEntryQuery },
    {   2,    "varEnableRecovery", { {2,2,2}, VARSMI_BOOL, 1 },       VARATTR_RW, NULL, NULL, varEntryQuery },
    {   3,    "varEnableCmdHistLog", { {2,2,3}, VARSMI_BOOL, 1 },     VARATTR_RW, NULL, NULL, varEntryQuery },
    {   4,    "varEnableCommLog", { {2,2,4}, VARSMI_BOOL, 1 },        VARATTR_RW, NULL, NULL, varEntryQuery },
    {   5,    "varEanbleAutoRegister", { {2,2,5}, VARSMI_BOOL, 1 },   VARATTR_RW, NULL, NULL, varEntryQuery },
    {   6,    "varEnableAutoReset", { {2,2,6}, VARSMI_BOOL, 1 },      VARATTR_RW, NULL, NULL, varEntryQuery },
    {   7,    "varEnableAutoTimeSync", { {2,2,7}, VARSMI_BOOL, 1 },   VARATTR_RW, NULL, NULL, varEntryQuery },
    {   8,    "varEnableAutoSinkReset", { {2,2,8}, VARSMI_BOOL, 1 },  VARATTR_RW, NULL, NULL, varEntryQuery },
    {   9,    "varEnableSubNetwork", { {2,2,9}, VARSMI_BOOL, 1 },     VARATTR_RW, NULL, NULL, varEntryQuery },
    {  10,    "varEnableMobileStaticLog", { {2,2,10}, VARSMI_BOOL, 1 },VARATTR_RW, NULL, NULL, varEntryQuery },
    {  11,    "varEnableAutoUpload", { {2,2,11}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, varEntryQuery },
    {  12,    "varEnableSecurity", { {2,2,12}, VARSMI_BOOL, 1 },      VARATTR_RW, NULL, NULL, varEntryQuery },
    {  13,    "varEnableMonitoring", { {2,2,13}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, varEntryQuery },
    {  14,    "varEnableKeepalive", { {2,2,14}, VARSMI_BOOL, 1 },     VARATTR_RW, NULL, NULL, varEntryQuery },
    {  15,    "varEnableGpsTimesync", { {2,2,15}, VARSMI_BOOL, 1 },   VARATTR_RW, NULL, NULL, varEntryQuery },
    {  16,    "varEnableTimeBroadcast", { {2,2,16}, VARSMI_BOOL, 1 }, VARATTR_RW, NULL, NULL, varEntryQuery },
    {  17,    "varEnableStatusMonitoring", { {2,2,17}, VARSMI_BOOL, 1 },VARATTR_RW, NULL, NULL, varEntryQuery },
    {  18,    "varEnableMemoryCheck", { {2,2,18}, VARSMI_BOOL, 1 },   VARATTR_RW, NULL, NULL, varEntryQuery },
    {  19,    "varEnableFlashCheck", { {2,2,19}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, varEntryQuery },
    {  20,    "varEnableMeterTimesync", { {2,2,20}, VARSMI_BOOL, 1 }, VARATTR_RW, NULL, NULL, varEntryQuery },
    {  21,    "varEnableMeterCheck", { {2,2,21}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, varEntryQuery },
    {  22,    "varEnableHeater", { {2,2,22}, VARSMI_BOOL, 1 },        VARATTR_RW, NULL, NULL, varEntryQuery },
    {  23,    "varEnableReadMeterEvent", { {2,2,23}, VARSMI_BOOL, 1 },VARATTR_RW, NULL, NULL, varEntryQuery },
    {  24,    "varEnableLowBattery", { {2,2,24}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, varEntryQuery },
    {  25,    "varEnableTestMode", { {2,2,25}, VARSMI_BOOL, 1 },      VARATTR_RW, NULL, NULL, varEntryQuery },
    {  26,    "varEnableDebugLog", { {2,2,26}, VARSMI_BOOL, 1 },      VARATTR_RW, NULL, NULL, varEntryQuery },
    {  27,    "varEnableMeterRecovery", { {2,2,27}, VARSMI_BOOL, 1 }, VARATTR_RW, NULL, NULL, varEntryQuery },
    {  28,    "varEnableHighRAM", { {2,2,28}, VARSMI_BOOL, 1 },       VARATTR_RW, NULL, NULL, varEntryQuery },
    {  29,    "varEnableGarbageCleaning",{ {2,2,29}, VARSMI_BOOL, 1 },VARATTR_RW, NULL, NULL, varEntryQuery },
	{  30,    "varEnableJoinControl",{ {2,2,30}, VARSMI_BOOL, 1 },VARATTR_RW, NULL, NULL, varEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_varValueEntry_node[] =
{
    {   1,    "varValueMask", { {2,3,1}, VARSMI_UINT, 4 },                    VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {   2,    "varAutoResetCheckInterval", { {2,3,2}, VARSMI_UINT, 4 },       VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {   3,    "varSysPowerOffDelay", { {2,3,3}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {   4,    "varSysTempMonInterval", { {2,3,4}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {   5,    "varAutoTimeSyncInterval", { {2,3,5}, VARSMI_UINT, 4 },         VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {   6,    "varSysMeteringThreadCount", { {2,3,6}, VARSMI_BYTE, 1 },       VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {   7,    "varSinkPollingInterval", { {2,3,7}, VARSMI_UINT, 4 },          VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {   8,    "varSinkResetInterval", { {2,3,8}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {   9,    "varSinkResetDelay", { {2,3,9}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  10,    "varSinkLedTurnOffInterval", { {2,3,10}, VARSMI_UINT, 4 },      VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  11,    "varSinkAckWaitTime", { {2,3,11}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  12,    "varSensorTimeout", { {2,3,12}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  13,    "varSensorKeepalive", { {2,3,13}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  14,    "varSensorMeterSaveCount", { {2,3,14}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  15,    "varMeterDayMask", { {2,3,15}, VARSMI_UINT, 4 },                VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  16,    "varMeterHourMask", { {2,3,16}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  17,    "varMeterStartMin", { {2,3,17}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  18,    "varRecoveryDayMask", { {2,3,18}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  19,    "varRecoveryHourMask", { {2,3,19}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  20,    "varRecoveryStartMin", { {2,3,20}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  21,    "varCmdHistSaveDay", { {2,3,21}, VARSMI_BYTE, 1 },              VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  22,    "varEventLogSaveDay", { {2,3,22}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  23,    "varCommLogSaveDay", { {2,3,23}, VARSMI_BYTE, 1 },              VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  24,    "varMeterLogSaveDay", { {2,3,24}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  25,    "varAutoResetTime", { {2,3,25}, VARSMI_TIMESTAMP, 7 },          VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  26,    "varMeterUploadCycleType", { {2,3,26}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  27,    "varMeterUploadCycle", { {2,3,27}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  28,    "varMeterUploadStartHour", { {2,3,28}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  29,    "varMeterUploadStartMin", { {2,3,29}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  30,    "varMeterUploadRetry", { {2,3,30}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  31,    "varMeterIssueDate", { {2,3,31}, VARSMI_TIMESTAMP, 7 },         VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  32,    "varMemoryCriticalRate", { {2,3,32}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  33,    "varFlashCriticalRate", { {2,3,33}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  34,    "varNapcGroupInterval", { {2,3,34}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  35,    "varNapcRetry", { {2,3,35}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  36,    "varNapcRetryHour", { {2,3,36}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  37,    "varNapcRetryStartSecond", { {2,3,37}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  38,    "varNapcRetryClear", { {2,3,38}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  39,    "varMaxEventLogSize", { {2,3,39}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  40,    "varMaxAlarmLogSize", { {2,3,40}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  41,    "varMaxCmdLogSize", { {2,3,41}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  42,    "varMaxCommLogSize", { {2,3,42}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  43,    "varMaxMobileLogSize", { {2,3,43}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  44,    "varKeepaliveInterval", { {2,3,44}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  45,    "varAlarmLogSaveDay", { {2,3,45}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  46,    "varTimeBroadcastInterval", { {2,3,46}, VARSMI_UINT, 4 },       VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  47,    "varStatusMonitorTime", { {2,3,47}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  48,    "varUploadTryTime", { {2,3,48}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  49,    "varDataSaveDay", { {2,3,49}, VARSMI_UINT, 4 },                 VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  50,    "varMeteringPeriod", { {2,3,50}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  51,    "varRecoveryPeriod", { {2,3,51}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  52,    "varMeteringRetry", { {2,3,52}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  53,    "varRecoveryRetry", { {2,3,53}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  54,    "varCheckDayMask", { {2,3,54}, VARSMI_UINT, 4 },                VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  55,    "varCheckHourMask", { {2,3,55}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  56,    "varCheckStartMin", { {2,3,56}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  57,    "varCheckPeriod", { {2,3,57}, VARSMI_UINT, 4 },                 VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  58,    "varCheckRetry", { {2,3,58}, VARSMI_UINT, 4 },                  VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  59,    "varMeterTimesyncDayMask", { {2,3,59}, VARSMI_UINT, 4 },        VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  60,    "varMeterTimesyncHourMask", { {2,3,60}, VARSMI_UINT, 4 },       VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  61,    "varMeterReadingInterval", { {2,3,61}, VARSMI_UINT, 4 },        VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  62,    "varHeaterOnTemp", { {2,3,62}, VARSMI_INT, 4 },                 VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  63,    "varHeaterOffTemp", { {2,3,63}, VARSMI_INT, 4 },                VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  64,    "varMobileLiveCheckInterval", { {2,3,64}, VARSMI_INT, 4 },      VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  65,    "varEventReadDayMask", { {2,3,65}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  66,    "varEventReadHourMask", { {2,3,66}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  67,    "varEventSendDelay", { {2,3,67}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  68,    "varEventAlertLevel", { {2,3,68}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  69,    "varSensorLimit", { {2,3,69}, VARSMI_INT, 4 },                  VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  70,    "varMeteringStrategy", { {2,3,70}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  71,    "varTimesyncThresholdLow", { {2,3,71}, VARSMI_INT, 4 },         VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  72,    "varMobileLiveCheckMethod", { {2,3,72}, VARSMI_BYTE, 1 },       VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  73,    "varScanningStrategy", { {2,3,73}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  74,    "varMobileLogSaveDay", { {2,3,74}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  75,    "varUpgradeLogSaveDay", { {2,3,75}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  76,    "varUploadLogSaveDay", { {2,3,76}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  77,    "varTimesyncLogSaveDay", { {2,3,77}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  78,    "varMaxMeterLogSize", { {2,3,78}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  79,    "varMaxUpgradeLogSize", { {2,3,79}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  80,    "varMaxUploadLogSize", { {2,3,80}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  81,    "varMaxTimesyncLogSize", { {2,3,81}, VARSMI_UINT, 4 },          VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  82,    "varDefaultMeteringOption", { {2,3,82}, VARSMI_BYTE, 1 },       VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  83,    "varSensorCleaningThreshold", { {2,3,83}, VARSMI_BYTE, 1 },     VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  84,    "varTimeSyncStrategy", { {2,3,84}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  85,    "varTransactionSaveDay", { {2,3,85}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  86,    "varSchedulerType", { {2,3,86}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {  87,    "varTimesyncThresholdHigh", { {2,3,87}, VARSMI_INT, 4 },        VARATTR_RW, NULL, NULL, varValueEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_varSubValueEntry_node[] =
{
    {   1,    "varSubValueMask", { {2,4,1}, VARSMI_UINT, 4 },                 VARATTR_RW, NULL, NULL, varSubValueEntryQuery },
    {   2,    "varSubChannel", { {2,4,2}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, varSubValueEntryQuery },
    {   3,    "varSubPanID", { {2,4,3}, VARSMI_WORD, 2 },                     VARATTR_RW, NULL, NULL, varSubValueEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_varGeEntry_node[] =
{
    {   1,    "varGeMeteringInterval", { {2,5,1}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, varGeEntryQuery },
    {   2,    "varGeUser", { {2,5,2}, VARSMI_STREAM, 10 },                    VARATTR_RW, NULL, NULL, varGeEntryQuery },
    {   3,    "varGeMeterSecurity", { {2,5,3}, VARSMI_STREAM, 20 },           VARATTR_RW, NULL, NULL, varGeEntryQuery },
    {   4,    "varGeAllUploadHour", { {2,5,4}, VARSMI_BYTE, 1 },              VARATTR_RW, NULL, NULL, varGeEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_varEvtCfgEntry_node[] =
{
    {   1,    "varEvtCfgOid", { {2,6,1}, VARSMI_STREAM, 16 },                 VARATTR_READ, NULL, NULL, NULL },
    {   2,    "varEvtCfgEvent", { {2,6,2}, VARSMI_STREAM, 64 },               VARATTR_READ, NULL, NULL, NULL },
    {   3,    "varEvtCfgSeverity", { {2,6,3}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, NULL },
    {   4,    "varEvtCfgDefaultSeverity", { {2,6,4}, VARSMI_BYTE, 1 },        VARATTR_READ, NULL, NULL, NULL },
    {   5,    "varEvtCfgNotify", { {2,6,4}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_usrEntry_node[] =
{
    {   1,    "usrMask", { {2,10,1}, VARSMI_BYTE, 1 },                        VARATTR_RW, NULL, NULL, usrEntryQuery },
    {   2,    "usrAccount", { {2,10,2}, VARSMI_STRING, 16 },                  VARATTR_RW, NULL, NULL, usrEntryQuery },
    {   3,    "usrPassword", { {2,10,3}, VARSMI_STRING, 16 },                 VARATTR_RW, NULL, NULL, usrEntryQuery },
    {   4,    "usrGroup", { {2,10,4}, VARSMI_BYTE, 1 },                       VARATTR_RW, NULL, NULL, usrEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_gpsEntry_node[] =
{
    {   1,    "gpsFixPosition", { {2,11,1}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, gpsEntryQuery },
    {   2,    "gpsCount", { {2,11,2}, VARSMI_BYTE, 1 },                       VARATTR_RW, NULL, NULL, gpsEntryQuery },
    {   3,    "gpsTime", { {2,11,3}, VARSMI_TIMESTAMP, 7 },                   VARATTR_RW, NULL, NULL, gpsEntryQuery },
    {   4,    "gpsPos", { {2,11,4}, VARSMI_OPAQUE, 32 },                      VARATTR_RW, NULL, NULL, gpsEntryQuery },
    {   5,    "gpsTimezone", { {2,11,5}, VARSMI_INT, 4 },                     VARATTR_RW, NULL, NULL, gpsEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_memEntry_node[] =
{
    {   1,    "memTotalSize", { {2,100,1}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, memEntryQuery },
    {   2,    "memFreeSize", { {2,100,2}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, memEntryQuery },
    {   3,    "memUseSize", { {2,100,3}, VARSMI_UINT, 4 },                VARATTR_READ, NULL, NULL, memEntryQuery },
    {   4,    "memCacheSize", { {2,100,4}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, memEntryQuery },
    {   5,    "memBufferSize", { {2,100,5}, VARSMI_UINT, 4 },             VARATTR_READ, NULL, NULL, memEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_flashEntry_node[] =
{
    {   1,    "flashTotalSize", { {2,101,1}, VARSMI_UINT, 4 },            VARATTR_READ, NULL, NULL, flashEntryQuery },
    {   2,    "flashFreeSize", { {2,101,2}, VARSMI_UINT, 4 },             VARATTR_READ, NULL, NULL, flashEntryQuery },
    {   3,    "flashUseSize", { {2,101,3}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, flashEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_ethEntry_node[] =
{
    {   1,    "ethName", { {2,102,1}, VARSMI_STRING, 16 },                VARATTR_READ, NULL, NULL, ethEntryQuery },
    {   2,    "ethPhyAddr", { {2,102,2}, VARSMI_STREAM, 6 },              VARATTR_READ, NULL, NULL, ethEntryQuery },
    {   3,    "ethIpAddr", { {2,102,3}, VARSMI_IPADDR, 4 },               VARATTR_RW, NULL, NULL, ethEntryQuery },
    {   4,    "ethSubnetMask", { {2,102,4}, VARSMI_IPADDR, 4 },           VARATTR_RW, NULL, NULL, ethEntryQuery },
    {   5,    "ethGateway", { {2,102,5}, VARSMI_IPADDR, 4 },              VARATTR_RW, NULL, NULL, ethEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_gpioEntry_node[] =
{
    {   1,    "gpioMask", { {2,103,1}, VARSMI_UINT, 4 },                  VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   2,    "gpioPowerFail", { {2,103,2}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   3,    "gpioLowBattery", { {2,103,3}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   4,    "gpioBUSY", { {2,103,4}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   5,    "gpioSTBY", { {2,103,5}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   6,    "gpioDCD", { {2,103,6}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   7,    "gpioRI", { {2,103,7}, VARSMI_BYTE, 1 },                    VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   8,    "gpioSink1State", { {2,103,8}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   9,    "gpioSink2State", { {2,103,9}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  10,    "gpioHeaterState", { {2,103,10}, VARSMI_BYTE, 1 },          VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  11,    "gpioCoverState", { {2,103,11}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  12,    "gpioCharginState", { {2,103,12}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_gpioCtrlEntry_node[] =
{
    {   1,    "gpioPowerCtrl", { {2,109,1}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   2,    "gpioReset",      { {2,109,2}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   3,    "gpioMobilePower", { {2,109,3}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   4,    "gpioMobileReset", { {2,109,4}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   5,    "gpioMobileDtr", { {2,109,5}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   6,    "gpioSink1Power", { {2,109,6}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   7,    "gpioSink2Power", { {2,109,7}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   8,    "gpioExternalPower", { {2,109,8}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   9,    "gpioLedSink1", { {2,109,9}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  10,    "gpioLedSink2", { {2,109,10}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  11,    "gpioLedReady", { {2,109,11}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  12,    "gpioLedDebug1", { {2,109,12}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  13,    "gpioLedDebug2", { {2,109,13}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  14,    "gpioLedDebug3", { {2,109,14}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  15,    "gpioLedDebug4", { {2,109,15}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  16,    "gpioEmergencyOff", { {2,109,16}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {  17,    "gpioCoordinatorReset", { {2,109,17}, VARSMI_BYTE, 1 },     VARATTR_READ, NULL, NULL, gpioEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_mobileEntry_node[] =
{
    {   1,    "mobileID", { {2,104,1}, VARSMI_STRING, 16 },               VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {   2,    "mobileIpaddr", { {2,104,2}, VARSMI_IPADDR, 4 },            VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {   3,    "mobileRxDbm", { {2,104,3}, VARSMI_INT, 4 },                VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {   4,    "mobileTxDbm", { {2,104,4}, VARSMI_INT, 4 },                VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {   5,    "mobilePacketLiveTime", { {2,104,5}, VARSMI_UINT, 4 },      VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {   6,    "mobileSendBytes", { {2,104,6}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {   7,    "mobileRecvBytes", { {2,104,7}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {   8,    "mobileLastConnectTime", { {2,104,8}, VARSMI_TIMESTAMP, 7 },VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {   9,    "mobileSimCardId", { {2,104,9}, VARSMI_STRING, 7 },         VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {  10,    "mobileVolt", { {2,104,10}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, mobileEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_pluginEntry_node[] =
{
    {   1,    "pluginName", { {2,105,1}, VARSMI_STRING, 16 },             VARATTR_READ, NULL, NULL, pluginEntryQuery },
    {   2,    "pluginFileName", { {2,105,2}, VARSMI_STRING, 32 },         VARATTR_READ, NULL, NULL, pluginEntryQuery },
    {   3,    "pluginDescr", { {2,105,3}, VARSMI_STRING, 64 },            VARATTR_READ, NULL, NULL, pluginEntryQuery },
    {   4,    "pluginType", { {2,105,4}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, pluginEntryQuery },
    {   5,    "pluginState", { {2,105,5}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, pluginEntryQuery },
    {   6,    "pluginVersion", { {2,105,6}, VARSMI_STRING, 8 },           VARATTR_READ, NULL, NULL, pluginEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_procEntry_node[] =
{
    {   1,    "procID", { {2,106,1}, VARSMI_UINT, 4 },                    VARATTR_READ, NULL, NULL, procEntryQuery },
    {   2,    "procTTY", { {2,106,2}, VARSMI_STRING, 8 },                 VARATTR_READ, NULL, NULL, procEntryQuery },
    {   3,    "procSize", { {2,106,3}, VARSMI_UINT, 4 },                  VARATTR_READ, NULL, NULL, procEntryQuery },
    {   4,    "procState", { {2,106,4}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, procEntryQuery },
    {   5,    "procCmd", { {2,106,5}, VARSMI_STRING, 50 },                VARATTR_READ, NULL, NULL, procEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_pppEntry_node[] =
{
    {   1,    "pppProvider", { {2,107,1}, VARSMI_STRING, 64 },            VARATTR_READ, NULL, NULL, pppEntryQuery },
    {   2,    "pppUser", { {2,107,2}, VARSMI_STRING, 32 },                VARATTR_RW, NULL, NULL, pppEntryQuery },
    {   3,    "pppPassword", { {2,107,3}, VARSMI_STRING, 32 },            VARATTR_RW, NULL, NULL, pppEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_envEntry_node[] =
{
    {   1,    "envKernelVersion", { {2,108,1}, VARSMI_STRING, 64 },       VARATTR_READ, NULL, NULL, envEntryQuery },
    {   2,    "envGccVersion", { {2,108,2}, VARSMI_STRING, 64 },          VARATTR_READ, NULL, NULL, envEntryQuery },
    {   3,    "envCpuName", { {2,108,3}, VARSMI_STRING, 64 },             VARATTR_READ, NULL, NULL, envEntryQuery },
    {   4,    "envCpuMips", { {2,108,4}, VARSMI_UINT, 4 },                VARATTR_READ, NULL, NULL, envEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_patchEntry_node[] =
{
    {   1,    "patchID", { {2,110,1}, VARSMI_UINT, 4 },                   VARATTR_READ, NULL, NULL, NULL },
    {   2,    "patchIssueNumber", { {2,110,2}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, NULL },
    {   3,    "patchState", { {2,110,3}, VARSMI_BOOL, 1 },                VARATTR_RW,   NULL, NULL, NULL },
    {   4,    "patchDescr", { {2,110,4}, VARSMI_STRING, 128 },            VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_fwEntry_node[] =
{
    {   1,    "fwType", { {2,111,1}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
    {   2,    "fwCore", { {2,111,2}, VARSMI_STRING, 64 },               VARATTR_READ, NULL, NULL, NULL },
    {   3,    "fwHardware", { {2,111,3}, VARSMI_WORD, 2 },              VARATTR_READ, NULL, NULL, NULL },
    {   4,    "fwSoftware", { {2,111,4}, VARSMI_WORD, 2 },              VARATTR_READ, NULL, NULL, NULL },
    {   5,    "fwBuild", { {2,111,5}, VARSMI_WORD, 2 },                 VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_commLogEntry_node[] =
{
    {   1,    "commLogDate", { {2,200,1}, VARSMI_TIMESTAMP, 7 },          VARATTR_READ, NULL, NULL, commLogEntryQuery },
    {   2,    "commLogFileName", { {2,200,2}, VARSMI_STRING, 32 },        VARATTR_READ, NULL, NULL, commLogEntryQuery },
    {   3,    "commLogFileSize", { {2,200,3}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, commLogEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_commLogStruct_node[] =
{
    {   1,    "commLogLength", { {2,201,1}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, commLogStructQuery },
    {   2,    "commLogMin", { {2,201,2}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, commLogStructQuery },
    {   3,    "commLogSec", { {2,201,3}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, commLogStructQuery },
    {   4,    "commLogData", { {2,201,4}, VARSMI_STREAM, 0 },             VARATTR_READ, NULL, NULL, commLogStructQuery },
    {   0,    NULL }
};

VAROBJECT    m_cmdHistEntry_node[] =
{
    {   1,    "cmdHistDate", { {2,202,1}, VARSMI_TIMESTAMP, 7 },          VARATTR_READ, NULL, NULL, cmdHistEntryQuery },
    {   2,    "cmdHistFileName", { {2,202,2}, VARSMI_STRING, 32 },        VARATTR_READ, NULL, NULL, cmdHistEntryQuery },
    {   3,    "cmdHistFileSize", { {2,202,3}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, cmdHistEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_cmdHistStruct_node[] =
{
    {   1,    "cmdHistLength", { {2,203,1}, VARSMI_WORD, 2 },             VARATTR_READ, NULL, NULL, cmdHistStructQuery },
    {   2,    "cmdHistTimeHour", { {2,203,2}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, cmdHistStructQuery },
    {   3,    "cmdHistTimeMin", { {2,203,3}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, cmdHistStructQuery },
    {   4,    "cmdHistTimeSec", { {2,203,4}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, cmdHistStructQuery },
    {   5,    "cmdHistOwner", { {2,203,5}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, cmdHistStructQuery },
    {   6,    "cmdHistUser", { {2,203,6}, VARSMI_STRING, 16 },            VARATTR_READ, NULL, NULL, cmdHistStructQuery },
    {   7,    "cmdHistData", { {2,203,7}, VARSMI_STREAM, 0 },             VARATTR_READ, NULL, NULL, cmdHistStructQuery },
    {   0,    NULL }
};

VAROBJECT    m_mcuEventEntry_node[] =
{
    {   1,    "mcuEvnetDate", { {2,204,1}, VARSMI_TIMESTAMP, 7 },         VARATTR_READ, NULL, NULL, mcuEventEntryQuery },
    {   2,    "mcuEventFileName", { {2,204,2}, VARSMI_STRING, 32 },       VARATTR_READ, NULL, NULL, mcuEventEntryQuery },
    {   3,    "mcuEventFileSize", { {2,204,3}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, mcuEventEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_mcuEventStruct_node[] =
{
    {   1,    "mcuEventLength", { {2,205,1}, VARSMI_WORD, 2 },            VARATTR_READ, NULL, NULL, mcuEventStructQuery },
    {   2,    "mcuEventData", { {2,205,2}, VARSMI_STREAM, 0 },            VARATTR_READ, NULL, NULL, mcuEventStructQuery },
    {   0,    NULL }
};

VAROBJECT    m_meterLogEntry_node[] =
{
    {   1,    "meterLogDate", { {2,206,1}, VARSMI_TIMESTAMP, 7 },         VARATTR_READ, NULL, NULL, meterLogEntryQuery },
    {   2,    "meterLogFileName", { {2,206,2}, VARSMI_STRING, 32 },       VARATTR_READ, NULL, NULL, meterLogEntryQuery },
    {   3,    "meterLogFileSize", { {2,206,3}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, meterLogEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_meterLogStruct_node[] =
{
    {   1,    "meterLogID", { {2,207,1}, VARSMI_EUI64, 8 },               VARATTR_READ, NULL, NULL, meterLogStructQuery },
    {   2,    "meterLogType", { {2,207,2}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, meterLogStructQuery },
    {   3,    "meterLogTime", { {2,207,3}, VARSMI_TIMESTAMP, 7 },         VARATTR_READ, NULL, NULL, meterLogStructQuery },
    {   4,    "meterLogStartTime", { {2,207,4}, VARSMI_TIMESTAMP, 7 },    VARATTR_READ, NULL, NULL, meterLogStructQuery },
    {   5,    "meterLogEndTime", { {2,207,5}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, meterLogStructQuery },
    {   6,    "meterLogResult", { {2,207,6}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, meterLogStructQuery },
    {   7,    "meterLogElapse", { {2,207,7}, VARSMI_UINT, 4 },            VARATTR_READ, NULL, NULL, meterLogStructQuery },
    {   8,    "meterLogTryCount", { {2,207,8}, VARSMI_BYTE, 1 },          VARATTR_READ, NULL, NULL, meterLogStructQuery },
    {   0,    NULL }
};

VAROBJECT    m_mobileLogEntry_node[] =
{
    {   1,    "mobileLogDate", { {2,208,1}, VARSMI_TIMEDATE, 4 },         VARATTR_READ, NULL, NULL, mobileLogEntryQuery },
    {   2,    "mobileLogFileName", { {2,208,2}, VARSMI_STRING, 32 },      VARATTR_READ, NULL, NULL, mobileLogEntryQuery },
    {   3,    "mobileLogFileSize", { {2,208,3}, VARSMI_UINT, 4 },         VARATTR_READ, NULL, NULL, mobileLogEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_mobileLogStruct_node[] =
{
    {   0,    NULL }
};

VAROBJECT    m_sink_node[] =
{
    {   1,    "sinkEntry", { {3,1,0}, VARSMI_OPAQUE, 63 }, 
                        VARATTR_RW | VARATTR_LIST, m_sinkEntry_node, NULL, sinkEntryQuery },
    {   2,    "sinkStaticEntry", { {3,2,0}, VARSMI_OPAQUE, 39 },          
                        VARATTR_READ, m_sinkStaticEntry_node, NULL, sinkStaticEntryQuery },
    {   3,  "codiEntry", { {3,3,0}, VARSMI_OPAQUE, sizeof(CODIENTRY) }, 
                        VARATTR_RW | VARATTR_LIST, m_codiEntry_node, NULL, codiEntryQuery },
    {   4,  "codiDeviceEntry", { {3,4,0}, VARSMI_OPAQUE, sizeof(CODIDEVICEENTRY) }, 
                        VARATTR_RW | VARATTR_LIST, m_codiDeviceEntry_node, NULL, codiDeviceEntryQuery },
    {   5,  "codiBindingEntry", { {3,5,0}, VARSMI_OPAQUE, sizeof(CODIBINDINGENTRY) },         
                        VARATTR_READ | VARATTR_LIST, m_codiBindingEntry_node, NULL, codiBindingEntryQuery },
    {   6,  "codiNeighborEntry", { {3,6,0}, VARSMI_OPAQUE, sizeof(CODINEIGHBORENTRY) },        
                        VARATTR_READ | VARATTR_LIST, m_codiNeighborEntry_node, NULL, codiNeighborEntryQuery },
    {   7,  "codiMemoryEntry", { {3,7,0}, VARSMI_OPAQUE, sizeof(CODIMEMORYENTRY) },           
                        VARATTR_RW | VARATTR_LIST, m_codiMemoryEntry_node, NULL, codiMemoryEntryQuery },
    {   8,  "codiRegisterEntry", { {3,8,0}, VARSMI_OPAQUE, sizeof(CODIREGENTRY) },           
                        VARATTR_RW | VARATTR_LIST, m_codiRegEntry_node, NULL, codiRegisterEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_sinkEntry_node[] =
{
    {   1,    "sinkMask", { {3,1,1}, VARSMI_BYTE, 1 },                    VARATTR_RW, NULL, NULL, sinkEntryQuery },
    {   2,    "sinkIndex", { {3,1,2}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, sinkEntryQuery },
    {   3,    "sinkID", { {3,1,3}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {   4,    "sinkDevice", { {3,1,4}, VARSMI_STRING, 16 },               VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {   5,    "sinkType", { {3,1,5}, VARSMI_BYTE, 1 },                    VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {   6,    "sinkMaxBind", { {3,1,6}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {   7,    "sinkChannel", { {3,1,7}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, sinkEntryQuery },
    {   8,    "sinkPanID", { {3,1,8}, VARSMI_WORD, 2 },                   VARATTR_RW, NULL, NULL, sinkEntryQuery },
    {   9,    "sinkNeighborNode", { {3,1,9}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {  10,    "sinkRfPower", { {3,1,10}, VARSMI_BYTE, 1 },                VARATTR_RW, NULL, NULL, sinkEntryQuery },
    {  11,    "sinkBaudrate", { {3,1,11}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {  12,    "sinkState", { {3,1,12}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {  13,    "sinkMaxActiveCount", { {3,1,13}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {  14,    "sinkResetKind", { {3,1,14}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {  15,    "sinkPC", { {3,1,15}, VARSMI_WORD, 2 },                     VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {  16,    "sinkLastConnect", { {3,1,16}, VARSMI_TIMESTAMP, 7 },       VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {  17,    "sinkLastDisconnect", { {3,1,17}, VARSMI_TIMESTAMP, 7 },    VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {  18,    "sinkLastReset", { {3,1,18}, VARSMI_TIMESTAMP, 7 },         VARATTR_READ, NULL, NULL, sinkEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_sinkStaticEntry_node[] =
{
    {   1,    "sinkStaticIndex", { {3,2,1}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, sinkStaticEntryQuery },
    {   2,    "sinkRecvBytes", { {3,2,2}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {   3,    "sinkSendBytes", { {3,2,3}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {   4,    "sinkErrorBytes", { {3,2,4}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {   5,    "sinkRecvPacket", { {3,2,5}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {   6,    "sinkSendPacket", { {3,2,6}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {   7,    "sinkTimeoutPacket", { {3,2,7}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {   8,    "sinkSeqErrorPacket", { {3,2,8}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {   9,    "sinkCrcErrorPacket", { {3,2,9}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {  10,    "sinkSendDropPacket", { {3,2,10}, VARSMI_UINT, 4 },         VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {  11,    "sinkResetCount", { {3,2,11}, VARSMI_WORD, 2 },             VARATTR_READ, NULL, NULL, sinkStaticEntryQuery },
    {   0,    NULL }
};

VAROBJECT   m_codiEntry_node[] =
{
    {   1,  "codiMask", { {3,3,1}, VARSMI_WORD, 2 },                    VARATTR_RW, NULL, NULL, codiEntryQuery },
    {   2,  "codiIndex", { {3,3,2}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, codiEntryQuery },
    {   3,  "codiID", { {3,3,3}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, codiEntryQuery },
    {   4,  "codiType", { {3,3,4}, VARSMI_BYTE, 1 },                    VARATTR_READ, NULL, NULL, codiEntryQuery },
    {   5,  "codiShortID", { {3,3,5}, VARSMI_WORD, 2 },                 VARATTR_READ, NULL, NULL, codiEntryQuery },
    {   6,  "codiFwVer", { {3,3,6}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, codiEntryQuery },
    {   7,  "codiHwVer", { {3,3,7}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, codiEntryQuery },
    {   8,  "codiZAIfVer", { {3,3,8}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, codiEntryQuery },
    {   9,  "codiZZIfVer", { {3,3,9}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, codiEntryQuery },
    {  10,  "codiFwBuild", { {3,3,10}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, codiEntryQuery },
    {  11,  "codiResetKind", { {3,3,11}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, codiEntryQuery },
    {  12,  "codiAutoSetting", { {3,3,12}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  13,  "codiChannel", { {3,3,13}, VARSMI_BYTE, 1 },                VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  14,  "codiPanID", { {3,3,14}, VARSMI_WORD, 2 },                  VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  15,  "codiExtPanID", { {3,3,15}, VARSMI_STREAM, 8 },             VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  16,  "codiRfPower", { {3,3,16}, VARSMI_CHAR, 1 },                VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  17,  "codiTxPowerMode", { {3,3,17}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  18,  "codiPermit", { {3,3,18}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  19,  "codiEnableEncrypt", { {3,3,19}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  20,  "codiLineKey", { {3,3,20}, VARSMI_STREAM, 16 },             VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  21,  "codiNetworkKey", { {3,3,21}, VARSMI_STREAM, 16 },          VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  22,  "codiRouteDiscovery", { {3,3,22}, VARSMI_BOOL, 1 },         VARATTR_RW, NULL, NULL, codiEntryQuery },
    {  23,  "codiMulticastHops", { {3,3,23}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, codiEntryQuery },
    {   0,  NULL }
};

VAROBJECT   m_codiDeviceEntry_node[] =
{
    {   1,  "codiIndex", { {3,4,1}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, codiDeviceEntryQuery },
    {   2,  "codiID", { {3,4,2}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, codiDeviceEntryQuery },
    {   3,  "codiDevice", { {3,4,3}, VARSMI_STRING, 16 },               VARATTR_READ, NULL, NULL, codiDeviceEntryQuery },
    {   4,  "codiBaudRate", { {3,4,4}, VARSMI_BYTE, 1 },                VARATTR_RW, NULL, NULL, codiDeviceEntryQuery },
    {   5,  "codiParityBit", { {3,4,5}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, codiDeviceEntryQuery },
    {   6,  "codiDataBit", { {3,4,6}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, codiDeviceEntryQuery },
    {   7,  "codiStopBit", { {3,4,7}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, codiDeviceEntryQuery },
    {   8,  "codiRtsCts", { {3,4,8}, VARSMI_BYTE, 1 },                  VARATTR_RW, NULL, NULL, codiDeviceEntryQuery },
    {   0,  NULL }
};

VAROBJECT   m_codiBindingEntry_node[] =
{
    {   1,  "codiBindIndex", { {3,5,1}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, codiBindingEntryQuery },
    {   2,  "codiBindType", { {3,5,2}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, codiBindingEntryQuery },
    {   3,  "codiBindLocal", { {3,5,3}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, codiBindingEntryQuery },
    {   4,  "codiBindRemote", { {3,5,4}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, codiBindingEntryQuery },
    {   5,  "codiBindID", { {3,5,5}, VARSMI_EUI64, 8 },                 VARATTR_READ, NULL, NULL, codiBindingEntryQuery },
    {   6,  "codiLastHeard", { {3,5,6}, VARSMI_WORD, 2 },               VARATTR_READ, NULL, NULL, codiBindingEntryQuery },
    {   0,  NULL }
};

VAROBJECT   m_codiNeighborEntry_node[] =
{
    {   1,  "codiNeighborIndex", { {3,6,1}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, codiNeighborEntryQuery },
    {   2,  "codiNeighborShortID", { {3,6,2}, VARSMI_WORD, 2 },         VARATTR_READ, NULL, NULL, codiNeighborEntryQuery },
    {   3,  "codiNeighborLqi", { {3,6,3}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, codiNeighborEntryQuery },
    {   4,  "codiNeighborInCost", { {3,6,4}, VARSMI_BYTE, 1 },          VARATTR_READ, NULL, NULL, codiNeighborEntryQuery },
    {   5,  "codiNeighborOutCost", { {3,6,5}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, codiNeighborEntryQuery },
    {   6,  "codiNeighborAge", { {3,6,6}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, codiNeighborEntryQuery },
    {   7,  "codiNeighborID", { {3,6,7}, VARSMI_EUI64, 8 },             VARATTR_READ, NULL, NULL, codiNeighborEntryQuery },
    {   0,  NULL }
};

VAROBJECT   m_codiMemoryEntry_node[] =
{
    {   1,  "codiIndex", { {3,7,1}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, codiMemoryEntryQuery },
    {   2,  "codiID", { {3,7,2}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {   3,  "codiAddressTableSize", { {3,7,3}, VARSMI_BYTE, 1 },        VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {   4,  "codiWholeAddressTableSize", { {3,7,4}, VARSMI_BYTE, 1 },   VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {   5,  "codiNeighborTableSize", { {3,7,5}, VARSMI_BYTE, 1 },       VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {   6,  "codiSourceRouteTableSize", { {3,7,6}, VARSMI_BYTE, 1 },    VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {   7,  "codiRouteTableSize", { {3,7,7}, VARSMI_BYTE, 1 },          VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {   8,  "codiMaxHops", { {3,7,8}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {   9,  "codiPacketBufferCount", { {3,7,9}, VARSMI_BYTE, 1 },       VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {  10,  "codiSoftwareVersion", { {3,7,10}, VARSMI_WORD, 2 },       VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {  11,  "codiKeyTableSize", { {3,7,11}, VARSMI_BYTE, 1 },       VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {  12,  "codiMaxChildren", { {3,7,12}, VARSMI_BYTE, 1 },       VARATTR_READ, NULL, NULL, codiMemoryEntryQuery },
    {   0,  NULL }
};

VAROBJECT   m_codiRegEntry_node[] =
{
    {   1,  "codiRegIndex", { {3,8,1}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, codiRegisterEntryQuery },
    {   2,  "codiRegStats", { {3,8,2}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, codiRegisterEntryQuery },
    {   3,  "codiID", { {3,8,3}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, codiRegisterEntryQuery },
    {   0,  NULL }
};


VAROBJECT    m_sensor_node[] =
{
    {   1,    "sensorConnectEntry", { {4,1,0}, VARSMI_OPAQUE, 54 },       
                            VARATTR_READ | VARATTR_LIST, m_sensorConnectEntry_node, NULL, sensorConnectEntryQuery },
    {   2,    "sensorInfoEntry", { {4,2,0}, VARSMI_OPAQUE, 42 },          
                            VARATTR_READ | VARATTR_LIST, m_sensorInfoEntry_node, NULL, NULL },
    {   3,    "sensorInfoNewEntry", { {4,3,0}, VARSMI_OPAQUE, sizeof(SENSORINFOENTRYNEW) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorInfoNewEntry_node, NULL, NULL },
    {   4,    "sensorPathEntry", { {4,4,0}, VARSMI_OPAQUE, sizeof(SENSORPATHENTRY) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorPathEntry_node, NULL, NULL },
    {   6,    "sensorEventEntry", { {4,6,0}, VARSMI_OPAQUE, sizeof(SENSOREVENTENTRY) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorEventEntry_node, NULL, NULL },
    {   7,    "sensorBatteryEntry", { {4,7,0}, VARSMI_OPAQUE, sizeof(SENSORBATTERYENTRY) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorBatteryEntry_node, NULL, NULL },
    {   8,    "sensorConstantEntry", { {4,8,0}, VARSMI_OPAQUE, sizeof(SENSORCONSTANTENTRY) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorConstantEntry_node, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_sensorConnectEntry_node[] =
{
    {   1,    "sensorID", { {4,1,1}, VARSMI_EUI64, 8 },                   VARATTR_READ, NULL, NULL, sensorConnectEntryQuery },
    {   2,    "sensorType", { {4,1,2}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, sensorConnectEntryQuery },
    {   3,    "sensorLastConnect", { {4,1,3}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, sensorConnectEntryQuery },
    {   4,    "sensorLastDisconnect", { {4,1,4}, VARSMI_TIMESTAMP, 7 },   VARATTR_READ, NULL, NULL, sensorConnectEntryQuery },
    {   5,    "sensorInstallDate", { {4,1,5}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, sensorConnectEntryQuery },
    {   6,    "sensorPageCount", { {4,1,6}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, sensorConnectEntryQuery },
    {   7,    "sensorState", { {4,1,7}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, sensorConnectEntryQuery },
    {   8,    "sensorFlag", { {4,1,8}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, sensorConnectEntryQuery },
    {   9,    "sensorVendor", { {4,1,9}, VARSMI_BYTE, 1 },                VARATTR_RW, NULL, NULL, sensorConnectEntryQuery },
    {  10,    "sensorSerialNumber", { {4,1,10}, VARSMI_STRING, 20 },      VARATTR_RW, NULL, NULL, sensorConnectEntryQuery },
    {  11,    "sensorServiceType", { {4,1,11}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_sensorInfoEntry_node[] =
{
    {   1,    "sensorGroup", { {4,2,1}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
    {   2,    "sensorMember", { {4,2,2}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, NULL },
    {   3,    "sensorID", { {4,2,3}, VARSMI_EUI64, 8 },                   VARATTR_READ, NULL, NULL, NULL },
    {   4,    "sensorSerialNumber", { {4,2,4}, VARSMI_STRING, 18 },       VARATTR_READ, NULL, NULL, NULL },
    {   5,    "sensorLastConnect", { {4,2,5}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
    {   6,    "sensorInstallDate", { {4,2,6}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
    {   7,    "sensorAttr", { {4,2,7}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_sensorInfoNewEntry_node[] =
{
    {   1,    "sensorID", { {4,3,1}, VARSMI_EUI64, 8 },                   VARATTR_READ, NULL, NULL, NULL },
    {   2,    "sensorSerialNumber", { {4,3,2}, VARSMI_STREAM, 20 },       VARATTR_READ, NULL, NULL, NULL },
    {   3,    "sensorLastConnect", { {4,3,3}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
    {   4,    "sensorInstallDate", { {4,3,4}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
    {   5,    "sensorAttr", { {4,3,5}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
    {   6,    "sensorState", { {4,3,6}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
    {   7,    "sensorModel", { {4,3,7}, VARSMI_STREAM, 18 },              VARATTR_READ, NULL, NULL, NULL },
    {   8,    "sensorHwVersion", { {4,3,8}, VARSMI_WORD, 2 },             VARATTR_READ, NULL, NULL, NULL },
    {   9,    "sensorFwVersion", { {4,3,9}, VARSMI_WORD, 2 },             VARATTR_READ, NULL, NULL, NULL },
    {  10,    "sensorOTAState", { {4,3,10}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
    {  11,    "sensorLastOTATime", { {4,3,11}, VARSMI_TIMESTAMP, 7 },     VARATTR_READ, NULL, NULL, NULL },
    {  12,    "sensorFwBuild", { {4,3,12}, VARSMI_WORD, 2 },              VARATTR_READ, NULL, NULL, NULL },
    {  13,    "sensorParser", { {4,3,13}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_sensorPathEntry_node[] =
{
    {   1,    "sensorPathID",      { {4,4,1}, VARSMI_EUI64,      8 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "sensorPathSerial",  { {4,4,2}, VARSMI_STREAM,    20 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "sensorPathModel",   { {4,4,3}, VARSMI_STREAM,    18 },     VARATTR_READ, NULL, NULL, NULL },
    {   4,    "sensorPathShortID", { {4,4,4}, VARSMI_WORD,       2 },     VARATTR_READ, NULL, NULL, NULL },
    {   5,    "sensorPathHops",    { {4,4,5}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   6,    "sensorPathNode",    { {4,4,6}, VARSMI_STREAM,    60 },     VARATTR_READ, NULL, NULL, NULL },
    {   7,    "sensorPathTime",    { {4,4,7}, VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
    {   8,    "sensorPathLQI",     { {4,4,8}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   9,    "sensorPathRSSI",    { {4,4,9}, VARSMI_CHAR,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_sensorEventEntry_node[] =
{
    {   1,    "sevtTime",           { {4,6,1}, VARSMI_TIMESTAMP, 7 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "sevtType",           { {4,6,2}, VARSMI_BYTE,      1 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "sevtStatus",         { {4,6,3}, VARSMI_STREAM,    4 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_sensorBatteryEntry_node[] =
{
    {   1,    "battVolt",           { {4,7,1}, VARSMI_WORD,     2 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "battCurrent",        { {4,7,2}, VARSMI_WORD,     2 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "battOffset",         { {4,7,3}, VARSMI_BYTE,     1 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_sensorConstantEntry_node[] =
{
    {   1,    "powerVoltageScaleInt", { {4,8,1}, VARSMI_INT,    4 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "powerVoltageScaleReal",{ {4,8,2}, VARSMI_UINT,   4 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "powerCurrentScaleInt", { {4,8,3}, VARSMI_INT,    4 },     VARATTR_READ, NULL, NULL, NULL },
    {   4,    "powerCurrentScaleReal",{ {4,8,4}, VARSMI_UINT,   4 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_network_node[] =
{
    {   1,    "ffdEntry", { {5,1,0}, VARSMI_OPAQUE, 62 },       
                            VARATTR_READ | VARATTR_LIST, m_ffdEntry_node, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_ffdEntry_node[] =
{
    {   1,    "ffdID",             { {5,1,1},  VARSMI_EUI64,     8 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "ffdModel",          { {5,1,2},  VARSMI_STREAM,   18 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "ffdLastPoll",       { {5,1,3},  VARSMI_TIMESTAMP, 7 },     VARATTR_READ, NULL, NULL, NULL },
    {   4,    "ffdLastPush",       { {5,1,4},  VARSMI_TIMESTAMP, 7 },     VARATTR_READ, NULL, NULL, NULL },
    {   5,    "ffdHwVersion",      { {5,1,5},  VARSMI_WORD,      3 },     VARATTR_READ, NULL, NULL, NULL },
    {   6,    "ffdFwVersion",      { {5,1,6},  VARSMI_WORD,      3 },     VARATTR_READ, NULL, NULL, NULL },
    {   7,    "ffdFwBuild",        { {5,1,7},  VARSMI_WORD,      3 },     VARATTR_READ, NULL, NULL, NULL },
    {   8,    "ffdPowerType",      { {5,1,8},  VARSMI_BYTE,      1 },     VARATTR_READ, NULL, NULL, NULL },
    {   9,    "ffdPermitMode",     { {5,1,9},  VARSMI_BYTE,      1 },     VARATTR_READ, NULL, NULL, NULL },
    {  10,    "ffdPermitState",    { {5,1,10}, VARSMI_BYTE,      1 },     VARATTR_READ, NULL, NULL, NULL },
    {  11,    "ffdMaxChildTbl",    { {5,1,11}, VARSMI_BYTE,      1 },     VARATTR_READ, NULL, NULL, NULL },
    {  12,    "ffdCurrentChild",   { {5,1,12}, VARSMI_BYTE,      1 },     VARATTR_READ, NULL, NULL, NULL },
    {  13,    "ffdAlarmMask",      { {5,1,13}, VARSMI_WORD,      2 },     VARATTR_READ, NULL, NULL, NULL },
    {  14,    "ffdAlarmFlag",      { {5,1,14}, VARSMI_BYTE,      1 },     VARATTR_READ, NULL, NULL, NULL },
    {  15,    "ffdTestFlag",       { {5,1,15}, VARSMI_BYTE,      1 },     VARATTR_READ, NULL, NULL, NULL },
    {  16,    "ffdSolarAdVolt",    { {5,1,16}, VARSMI_WORD,      2 },     VARATTR_READ, NULL, NULL, NULL },
    {  17,    "ffdSolarChgBattVolt",{ {5,1,17}, VARSMI_WORD,     2 },     VARATTR_READ, NULL, NULL, NULL },
    {  18,    "ffdSolarBoardVolt", { {5,1,18}, VARSMI_WORD,      2 },     VARATTR_READ, NULL, NULL, NULL },
    {  19,    "ffdSolarBattChgState",{ {5,1,19}, VARSMI_BYTE,    1 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_transaction_node[] =
{
    {   1,    "trInfoEntry", { {6,1,0}, VARSMI_OPAQUE, 39 },       
                            VARATTR_READ | VARATTR_LIST, m_trInfoEntry_node, NULL, NULL },
    {   2,    "trHistoryEntry", { {6,2,0}, VARSMI_OPAQUE, 12 },       
                            VARATTR_READ | VARATTR_LIST, m_trHistoryEntry_node, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_trInfoEntry_node[] =
{
    {   1,    "triTargetID",          { {6,1,1},  VARSMI_EUI64,      8 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "triCommand",           { {6,1,2},  VARSMI_OID,        3 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "triID",                { {6,1,3},  VARSMI_WORD,       2 },     VARATTR_READ, NULL, NULL, NULL },
    {   4,    "triOption",            { {6,1,4},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   5,    "triDay",               { {6,1,5},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   6,    "triInitNice",          { {6,1,6},  VARSMI_CHAR,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   7,    "triCurNice",           { {6,1,7},  VARSMI_CHAR,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   8,    "triInitTry",           { {6,1,8},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   9,    "triCurTry",            { {6,1,9},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {  10,    "triQueue",             { {6,1,10}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {  11,    "triCreateTime",        { {6,1,11}, VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
    {  12,    "triLastTime",          { {6,1,12}, VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
    {  13,    "triState",             { {6,1,13}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {  14,    "triError",             { {6,1,14}, VARSMI_INT,        4 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_trHistoryEntry_node[] =
{
    {   1,    "trhTime",              { {6,2,1},  VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "trhState",             { {6,2,2},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "trhError",             { {6,2,3},  VARSMI_INT,        4 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_group_node[] =
{
	{   1,    "groupInfoEntry", { {7,1,0}, VARSMI_OPAQUE, sizeof(GROUPINFO) },       
								VARATTR_READ | VARATTR_LIST, m_groupInfoEntry_node, NULL, NULL },
	{   2,    "commandInfoEntry", { {7,2,0}, VARSMI_OPAQUE, sizeof(GROUPCOMMANDINFO) },       
								VARATTR_READ | VARATTR_LIST, m_commandInfoEntry_node, NULL, NULL },
	{   0,    NULL }
};

VAROBJECT    m_groupInfoEntry_node[] =
{
	{   1,    "groupTotalCnt",          { {7,1,1},  VARSMI_INT,      4 },     VARATTR_READ, NULL, NULL, NULL },
	{   2,    "groupKey",				{ {7,1,2},  VARSMI_INT,      4 },     VARATTR_READ, NULL, NULL, NULL },
	{   3,    "groupName",				{ {7,1,3},  VARSMI_STRING,      256 },     VARATTR_READ, NULL, NULL, NULL },
	{   4,    "memberCnt",				{ {7,1,4},  VARSMI_INT,        4 },     VARATTR_READ, NULL, NULL, NULL },
	{   5,    "memberInfo",             { {7,1,5},  VARSMI_STREAM,       0 },     VARATTR_READ, NULL, NULL, NULL },
	{   0,    NULL }
};

VAROBJECT	m_commandInfoEntry_node[] =
{
	{   1,    "commandKey",			{ {7,2,1},  VARSMI_INT,      4 },     VARATTR_READ, NULL, NULL, NULL },
	{   2,    "groupKey",				{ {7,2,2},  VARSMI_INT,      4 },     VARATTR_READ, NULL, NULL, NULL },
	{   3,    "memberID",				{ {7,2,3},  VARSMI_EUI64,      8 },     VARATTR_READ, NULL, NULL, NULL },
	{   4,    "groupName",				{ {7,2,4},  VARSMI_STRING,      256 },     VARATTR_READ, NULL, NULL, NULL },
	{   5,    "commandOID",			{ {7,2,5},  VARSMI_OID,      3 },     VARATTR_READ, NULL, NULL, NULL },
	{   6,    "commandState",          { {7,2,6},  VARSMI_INT,      4 },     VARATTR_READ, NULL, NULL, NULL },
	{   7,    "commandCreateTime",	{ {7,2,7},  VARSMI_TIMESTAMP,      7 },     VARATTR_READ, NULL, NULL, NULL },
	{   8,    "commandExecTime",		{ {7,2,8},  VARSMI_TIMESTAMP,      7 },     VARATTR_READ, NULL, NULL, NULL },
	{   9,    "commandResult",			{ {7,2,9},  VARSMI_INT,      4 },     VARATTR_READ, NULL, NULL, NULL },
	{   0,    NULL }
};

VAROBJECT    m_meter_node[] =
{
    {   1,    "meterDataEntry", { {10,1,0}, VARSMI_OPAQUE, 268 },         
                        VARATTR_READ | VARATTR_LIST, m_meterDataEntry_node, NULL, meterDataEntryQuery },
    {   2,    "meterEntry", { {10,2,0}, VARSMI_OPAQUE, sizeof(METERENTRY) },      
                        VARATTR_READ | VARATTR_LIST, m_meterEntry_node, NULL, meterEntryQuery },
    {   3,    "meterLPEntry", { {10,3,0}, VARSMI_OPAQUE, sizeof(METERLPENTRY) },      
                        VARATTR_READ | VARATTR_LIST, m_meterLPEntry_node, NULL, meterLPEntryQuery },
    {   4,    "meterEntryMbus", { {10,4,0}, VARSMI_OPAQUE, sizeof(METERENTRYMBUS) }, 
                        VARATTR_READ | VARATTR_LIST, m_meterEntry_mbus, NULL, meterMBusEntryQuery },
    {   0,    NULL }
};

VAROBJECT    m_meterDataEntry_node[] =
{
    {    1,    "mdID", { {10,1,1}, VARSMI_EUI64, 8 },                      VARATTR_READ, NULL, NULL, NULL },
    {    2,    "mdType", { {10,1,2}, VARSMI_BYTE, 1 },                     VARATTR_READ, NULL, NULL, NULL },
    {    3,    "mdVendor", { {10,1,3}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, NULL },
    {    4,    "mdServiceType", { {10,1,4}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, NULL },
    {    5,    "mdTime", { {10,1,5}, VARSMI_TIMESTAMP, 7 },                VARATTR_READ, NULL, NULL, NULL },
    {    6,    "mdLength", { {10,1,6}, VARSMI_WORD, 2 },                   VARATTR_READ, NULL, NULL, NULL },
    {    7,    "mdData", { {10,1,7}, VARSMI_STREAM, 0 },                   VARATTR_READ, NULL, NULL, NULL },
    {    0,    NULL }
};

VAROBJECT    m_meterEntry_node[] =
{
    {    1,    "mtrGroup", { {10,2,1}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, NULL },
    {    2,    "mtrMember", { {10,2,2}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
    {    3,    "mtrID", { {10,2,3}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, NULL },
    {    4,    "mtrSerial", { {10,2,4}, VARSMI_STREAM, 18 },               VARATTR_READ, NULL, NULL, NULL },
    {    5,    "mtrTime", { {10,2,5}, VARSMI_TIMESTAMP, 7 },               VARATTR_READ, NULL, NULL, NULL },
    {    6,    "mtrLength", { {10,2,6}, VARSMI_WORD, 2 },                  VARATTR_READ, NULL, NULL, NULL },
    {    7,    "mtrData", { {10,2,7}, VARSMI_STREAM, 0 },                  VARATTR_READ, NULL, NULL, NULL },
    {    0,    NULL }
};

VAROBJECT    m_meterLPEntry_node[] =
{
    {    1,    "mlpId", { {10,3,1}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, NULL },
    {    2,    "mlpMid", { {10,3,2}, VARSMI_STREAM, 20 },                  VARATTR_READ, NULL, NULL, NULL },
    {    3,    "mlpType", { {10,3,3}, VARSMI_BYTE, 1 },                    VARATTR_READ, NULL, NULL, NULL },
    {    4,    "mlpServiceType", { {10,3,4}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
    {    5,    "mlpVendor", { {10,3,5}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
    {    6,    "mlpDataCount", { {10,3,6}, VARSMI_WORD, 2 },               VARATTR_READ, NULL, NULL, NULL },
    {    7,    "mlpLength", { {10,3,7}, VARSMI_WORD, 2 },                  VARATTR_READ, NULL, NULL, NULL },
    {    8,    "mlpTime", { {10,3,8}, VARSMI_TIMESTAMP, 7 },               VARATTR_READ, NULL, NULL, NULL },
    {    9,    "mlpData", { {10,3,9}, VARSMI_STREAM, 0 },                  VARATTR_READ, NULL, NULL, NULL },
    {    0,    NULL }
};

VAROBJECT   m_meterEntry_mbus[] =
{
    {   1,  "mmtrID", { {10,4,1}, VARSMI_EUI64, 8 }, VARATTR_READ, NULL, NULL, NULL },
    {   2,  "mmtrMasterBank", { {10,4,2}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   3,  "mmtrSerialNumber", { {10,4,3}, VARSMI_STREAM, 22 },VARATTR_READ, NULL, NULL, NULL },
    {   4,  "mmtrMenufact", { {10,4,4}, VARSMI_STREAM, 22 }, VARATTR_READ, NULL, NULL, NULL },
    {   5,  "mmtrGener", { {10,4,5}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   6,  "mmtrMedia", { {10,4,6}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   7,  "mmtrCntAccess", { {10,4,7}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   8,  "mmtrStatus", { {10,4,8}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   9,  "mmtrLastMetering", { {10,4,9}, VARSMI_TIMESTAMP, 7 },VARATTR_READ, NULL, NULL, NULL },
    {   0,  NULL } 
}; 

VAROBJECT    m_eventMcu_node[] =
{
    {   1,    "eventMcuInstall", { {200,1,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
    {   2,    "eventMcuUninstall", { {200,2,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {   3,    "eventMcuStartup", { {200,3,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
    {   4,    "eventMcuShutdown", { {200,4,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
    {  10,    "eventMcuChange", { {200,10,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
    {  11,    "eventMcuOamLogin", { {200,11,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {  12,    "eventMcuOamLogout", { {200,12,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {  13,    "eventMcuClockChange", { {200,13,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    {  14,    "eventMcuPowerFail", { {200,14,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {  15,    "eventMcuPowerRestore", { {200,15,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
    {  16,    "eventMcuLowBattery", { {200,16,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
    {  17,    "eventMcuLowBatteryRestore", { {200,17,0}, VARSMI_OID, 0 }, VARATTR_READ, NULL, NULL, NULL },
    {  18,    "eventMcuTempRange", { {200,18,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {  19,    "eventMcuTempRangeResetore", { {200,19,0}, VARSMI_OID, 0 }, VARATTR_READ, NULL, NULL, NULL },
    {  20,    "eventMcuReset", { {200,20,0}, VARSMI_OID, 0 },             VARATTR_READ, NULL, NULL, NULL },
    {  21,    "eventMcuFactoryDefault", { {200,21,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
    {  22,    "eventMcuFirmwareUpdate", { {200,22,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
    {  23,    "eventMcuBatteryChargingStart", { {200,23,0}, VARSMI_OID, 0 },VARATTR_READ, NULL, NULL, NULL },
    {  24,    "eventMcuBatteryCharginEnd", { {200,24,0}, VARSMI_OID, 0 }, VARATTR_READ, NULL, NULL, NULL },
    {  25,    "eventMcuCoverOpen", { {200,25,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {  26,    "eventMcuCoverClose", { {200,26,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
    {  27,    "eventMcuHeaterOn", { {200,27,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {  28,    "eventMcuHeaterOff", { {200,28,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {  29,    "eventMcuAlert", { {200,29,0}, VARSMI_OID, 0 },             VARATTR_READ, NULL, NULL, NULL },
    { 100,    "eventMcuMeteringStart", { {200,100,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
    { 101,    "eventMcuMeteringEnd", { {200,101,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
    { 102,    "eventMcuRecoveryStart", { {200,102,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
    { 103,    "eventMcuRecoveryEnd", { {200,103,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
    { 104,    "eventMcuMeteringFail", { {200,104,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
    { 105,    "eventMcuDiagnosis", { {200,105,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_eventSink_node[] =
{
    {   1,    "eventSinkInstall", { {201,1,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
    {   2,    "eventSinkUninstall", { {201,2,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {   3,    "eventSinkConnect", { {201,3,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
    {   4,    "eventSinkDisconnect", { {201,4,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
    {   5,    "eventSinkChange", { {201,5,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
    {   6,    "eventSinkReset", { {201,6,0}, VARSMI_OID, 0 },             VARATTR_READ, NULL, NULL, NULL },
    {   7,    "eventSinkError", { {201,7,0}, VARSMI_OID, 0 },             VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_eventMobile_node[] =
{
    {   1,    "eventMobileInstall", { {202,1,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {   2,    "eventMobileChange", { {202,2,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {   3,    "eventMobileReset", { {202,3,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
    {   4,    "eventMobileConnect", { {202,4,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {   5,    "eventMobileDisconnect", { {202,5,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
    {   6,    "eventMobileKeepalive", { {202,6,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_eventSensor_node[] =
{
    {   1,    "eventSensorJoinNetwork", { {203,1,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "eventSensorLeaveNetwork", { {203,2,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
    {   3,    "eventSensorChange", { {203,3,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {   4,    "eventSensorConnect", { {203,4,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {   5,    "eventSensorDisconnect", { {203,5,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
    {   6,    "eventSensorTimeout", { {203,6,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {   7,    "eventSensorBreaking", { {203,7,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
    {   8,    "eventSensorUnknown", { {203,8,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {   9,    "eventSensorReset", { {203,9,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
    {  10,    "eventSensorAlarm", { {203,10,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {  11,    "eventSensorPageChange", { {203,11,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
    {  12,    "eventSensorPageRead", { {203,12,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    {  13,    "eventSensorPageReadAll", { {203,13,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
    {  14,    "eventSensorRecovery", { {203,14,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    { 100,    "eventSensorLogPage", { {203,100,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    { 105,    "eventSensorInstall", { {203,105,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    { 106,    "eventSensorUninstall", { {203,106,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
    { 107,    "eventSensorPowerOutage", { {203,107,0}, VARSMI_OID, 0 },   VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_eventMalf_node[] =
{
    {   1,    "eventMalfCommandError", { {204,1,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
    {   2,    "eventMalfMeterReadError", { {204,2,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
    {   3,    "eventMalfUploadError", { {204,3,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    {   4,    "eventMalfDiskError", { {204,4,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {   5,    "eventMalfDiskErrorRestore", { {204,5,0}, VARSMI_OID, 0 },  VARATTR_READ, NULL, NULL, NULL },
    {   6,    "eventMalfMemoryError", { {204,6,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    {   7,    "eventMalfMemoryErrorRestore", { {204,7,0}, VARSMI_OID, 0 },VARATTR_READ, NULL, NULL, NULL },
    {   8,    "eventMalfWatchdogReset", { {204,8,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
    {   9,    "eventMalfGarbageCleaning", { {204,9,0}, VARSMI_OID, 0 },   VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_eventComm_node[] =
{
    {   1,    "eventCommZRUError", { {205,1,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {   2,    "eventCommZMUError", { {205,2,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {   3,    "eventCommZEUError", { {205,3,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_Alarm_node[] =
{
    {   1,    "alarmMessage", { {206,1,0}, VARSMI_OID, 0 },               VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_eventMeter_node[] =
{
    {   1,    "eventMeterError", { {212,1,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT	m_eventOta_node[] =
{
	{   1,	"eventOtaDownload", { {214,1,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   2,	"eventOtaStart", 	{ {214,2,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   3,	"eventOtaEnd", 		{ {214,3,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   4,	"eventOtaResult", 	{ {214,4,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT    m_eventTransaction_node[] =
{
    {   1,    "eventTransaction", { {215,1,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT   m_eventManagement_node[] =
{
    {    1,    "eventManEcho", { {255,1,0}, VARSMI_OID, 0 },               VARATTR_READ, NULL, NULL, NULL },
    {    0,    NULL }
};

int oidEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int eui64EntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int ipEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int timeEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int dayEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int verEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int smivarEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int pageEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int gmtEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int sysEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // sysEntry(2.1.0) OPAQUE(329) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 SYSENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (SYSENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(SYSENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->sysMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->sysID, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->sysType, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->sysName, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[4], (void *)&pEntry->sysDescr, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[5], (void *)&pEntry->sysLocation, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[6], (void *)&pEntry->sysContact, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[7], (void *)&pEntry->sysHwVersion, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[8], (void *)&pEntry->sysSwVersion, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[9], (void *)&pEntry->sysPhoneNumber, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[10], (void *)&pEntry->sysEtherType, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[11], (void *)&pEntry->sysMobileType, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[12], (void *)&pEntry->sysMobileMode, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[13], (void *)&pEntry->sysMobileAPN, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[14], (void *)&pEntry->sysUpTime, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[15], (void *)&pEntry->sysTime, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[16], (void *)&pEntry->sysCurTemp, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[17], (void *)&pEntry->sysMinTemp, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[18], (void *)&pEntry->sysMaxTemp, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[19], (void *)&pEntry->sysServer, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[20], (void *)&pEntry->sysServerPort, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[21], (void *)&pEntry->sysServerAlarmPort, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[22], (void *)&pEntry->sysLocalPort, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[23], (void *)&pEntry->sysState, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[24], (void *)&pEntry->sysVendor, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[25], (void *)&pEntry->sysModel, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[26], (void *)&pEntry->sysCircuitMode, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[27], (void *)&pEntry->sysMobileVendor, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[28], (void *)&pEntry->sysMobileAccessPointName, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[29], (void *)&pEntry->sysSwRevision, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[30], (void *)&pEntry->sysResetReason, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[31], (void *)&pEntry->sysJoinNodeCount, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[32], (void *)&pEntry->sysTimeZone, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[33], (void *)&pEntry->sysOpMode, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[34], (void *)&pEntry->sysPowerType, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[35], (void *)&pEntry->sysStateMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[36], (void *)&pEntry->sysSecurityState, pParam);            //  [9/13/2010 DHKim]  SSL Security State
                 }
           }
           return 1;
      case 1 :
           // sysMask(2.1.1) UINT(4) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
               VARAPI_ReplaceValue(pObject, pValue);
           return 1;
      case 2 :
           // sysID(2.1.2) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
           {
               GetFileValue("/app/conf/system.id", (int *)&m_nSystemID);
               pObject->var.stream.u32 = m_nSystemID;
               m_pVariableHelper->Update("sysID", m_nSystemID); // DB Update
           }
           else if (nMethod == VARMETHOD_LOAD)
           {
               GetFileValue("/app/conf/system.id", (int *)&m_nSystemID);
               pObject->var.stream.u32 = m_nSystemID;
               m_pVariableHelper->Update("sysID", m_nSystemID); // DB Update
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u32 = m_nSystemID;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nSystemID = pValue->stream.u32;
               IF4API_SetID(pValue->stream.u32);
               VARAPI_ReplaceValue(pObject, pValue);
               SetFileValue("/app/conf/system.id", pValue->stream.u32);
               VARAPI_SetModify();
               m_pVariableHelper->Update("sysID", m_nSystemID); // DB Update
           }
           return 1;
      case 3 :
           // sysType(2.1.3) BYTE(1) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nSystemType;
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u8 = m_nSystemType;
           }
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = m_nSystemType;
           return 1;
      case 4 :
           // sysName(2.1.4) STRING(64) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 5 :
           // sysDescr(2.1.5) STRING(64) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 6 :
           // sysLocation(2.1.6) STRING(64) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 7 :
           // sysContact(2.1.7) STRING(64) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 8 :
           // sysHwVersion(2.1.8) BYTE(1) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = HW_VERSION;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = HW_VERSION;
           return 1;
      case 9 :
           // sysSwVersion(2.1.9) BYTE(1) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = SW_VERSION;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = SW_VERSION;
           return 1;
      case 10 :
           // sysPhoneNumber(2.1.10) STRING(16) Read/Write
           if (nMethod == VARMETHOD_GET)
		   {
               VARAPI_CopyValue(pObject, pValue);
			}
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 11 :
           // sysEtherType(2.1.11) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nEthernetType;
           else if (nMethod == VARMETHOD_LOAD)
           {
               GetFileValue("/app/conf/ethernet.type", &m_nEthernetType);
               pObject->var.stream.u8 = m_nEthernetType;
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u8 = m_nEthernetType;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nEthernetType = pValue->stream.u8;
               SetFileValue("/app/conf/ethernet.type", pValue->stream.u8);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 12 :
           // sysMobileType(2.1.12) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nMobileType;
           else if (nMethod == VARMETHOD_LOAD)
           {
               GetFileValue("/app/conf/mobile.type", &m_nMobileType);
               pObject->var.stream.u8 = m_nMobileType;
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u8 = m_nMobileType;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMobileType = pValue->stream.u8;
               SetFileValue("/app/conf/mobile.type", pValue->stream.u8);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 13 :
           // sysMobileMode(2.1.13) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nMobileMode;
           else if (nMethod == VARMETHOD_LOAD)
           {
               GetFileValue("/app/conf/mobile.mode", &m_nMobileMode);
               pObject->var.stream.u8 = m_nMobileMode;
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u8 = m_nMobileMode;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMobileMode = pValue->stream.u8;
               SetFileValue("/app/conf/mobile.mode", pValue->stream.u8);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 14 :
           // sysMobileAPN(2.1.14) BYTE(1) Read/Write
           // Norway이후 부터 지원되지 않는 기능
           return 0;    
      case 15 :
           // sysUpTime(2.1.15) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u32 = m_pService->GetUpTime();
           return 1;
      case 16 :
           // sysTime(2.1.16) TIMESTAMP(7) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
               m_pRealTimeClock->SetTime((TIMESTAMP *)pValue->stream.p);
           else if (nMethod == VARMETHOD_MOVE)
               GetTimestamp((TIMESTAMP *)pValue->stream.p, NULL);
           return 1;
      case 17 :
           // sysCurTemp(2.1.17) INT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.s32 = (int)((double)(m_pTempManager->GetValue()*(double)10));
           return 1;
      case 18 :
           // sysMinTemp(2.1.18) INT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s32 = m_nTempRangeMin;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTempRangeMin = pObject->var.stream.s32;
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTempRangeMin = pValue->stream.s32;
               m_pTempManager->SetRange(m_nTempRangeMin/10,m_nTempRangeMax/10);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 19 :
           // sysMaxTemp(2.1.19) INT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s32 = m_nTempRangeMax;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTempRangeMax = pObject->var.stream.s32;
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTempRangeMax = pValue->stream.s32;
               m_pTempManager->SetRange(m_nTempRangeMin/10,m_nTempRangeMax/10);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 20 :
           // sysServer(2.1.20) STRING(256) Read/Write
           if ((nMethod == VARMETHOD_DEFAULT) || (nMethod == VARMETHOD_LOAD))
           {
               char      szAddress[256] = "";

               GetFileString("/app/conf/server.addr", szAddress, sizeof(szAddress));
               if (!*szAddress) strcpy(szAddress, "0.0.0.0");
               setFepServerAddress(szAddress);
               strcpy(pObject->var.stream.p, szAddress);

               m_pVariableHelper->Update("sysServer", szAddress); // DB Update
           }
           else if (nMethod == VARMETHOD_GET)
           {
               char      szAddress[256] = "";

               GetFileString("/app/conf/server.addr", szAddress, sizeof(szAddress));
               if (!*szAddress) strcpy(szAddress, "0.0.0.0");
               VARAPI_CopyValue(pObject, pValue);
               strcpy(pObject->var.stream.p, szAddress);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               char  szAddress[256];

               memset(szAddress,0,sizeof(szAddress));
               strncpy(szAddress, pValue->stream.p, pValue->len > (int) sizeof(szAddress)-1 ? (int) sizeof(szAddress) - 1 : pValue->len);
    
               SetFileString("/app/conf/server.addr", szAddress);
               setFepServerAddress(szAddress);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();

               m_pVariableHelper->Update("sysServer", szAddress); // DB Update
           }
           return 1;
      case 21 :
           // sysServerPort(2.1.21) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
           {
               pObject->var.stream.u32 = m_nFepPort;
               
               m_pVariableHelper->Update("sysServerPort", m_nFepPort);  // DB Update
           }else if (nMethod == VARMETHOD_LOAD) {
               setFepServerPort(pObject->var.stream.u32);

               m_pVariableHelper->Update("sysServerPort", pObject->var.stream.u32); // DB Update
           }else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               setFepServerPort(pValue->stream.u32);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();

               m_pVariableHelper->Update("sysServerPort", pObject->var.stream.u32); // DB Update
           }
           return 1;
      case 22 :
           // sysServerAlarmPort(2.1.22) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nFepAlarmPort;
           else if (nMethod == VARMETHOD_LOAD)
               setFepServerAlarmPort(pObject->var.stream.u32);
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               setFepServerAlarmPort(pValue->stream.u32);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 23 :
           // sysLocalPort(2.1.23) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nLocalPort;
           else if (nMethod == VARMETHOD_LOAD)
               m_nLocalPort = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nLocalPort = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 24 :
           // sysState(2.1.24) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = m_pService->GetState();
           return 1;
      case 25 :
           // sysVendor(2.1.25) BYTE(1) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = VENDOR_NURITELECOM;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = VENDOR_NURITELECOM;
           return 1;
      case 26 :
           // sysModel(2.1.26) BYTE(1) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = MODEL_TYPE_HTAMM;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = MODEL_TYPE_HTAMM;
           return 1;
      case 27 :
           // sysCircuitMode(2.1.27) BYTE(1) Read/Write
           // Not use (SWAMM)
           return 0;
      case 28 :
           // sysMobileVendor(2.1.28) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nMobileVendor;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMobileVendor = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u32 = m_nMobileVendor;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMobileVendor = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 29 :
           // sysMobileAccessPointName(2.1.29) STRING(64) Read/Write
           if (nMethod == VARMETHOD_LOAD)
                strcpy(m_szMobileApn, pObject->var.stream.p); 
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               strcpy(m_szMobileApn, pValue->stream.p);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
            
           }
           return 1;
      case 30 :
           // sysSwRevision(2.1.30) STRING(32) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               strcpy(pValue->stream.p, SW_REVISION_FIX);
           }
           return 1;
      case 31 :
           // sysResetReason(2.1.31) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nResetReason;
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = m_pService->GetResetState();
           else if (nMethod == VARMETHOD_LOAD)
           {
               GetFileValue("/app/conf/reset.mcu", &m_nResetReason);
               pObject->var.stream.u8 = m_nResetReason;
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u8 = m_nResetReason;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nResetReason = pValue->stream.u8;
               SetFileValue("/app/conf/reset.mcu", pValue->stream.u8);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 32 :
           // sysJoinNodeCount(2.1.32) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u32 = m_nJoinNodeCount;
           }
           return 1;
      case 33 :
           // sysTimeZone(2.1.33) SHORT(2) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
#ifdef  __USE_BSD
               struct tm * stm;
               time_t tmp;

               time(&tmp);
               stm = localtime(&tmp);
#endif

               VARAPI_CopyValue(pObject, pValue);
                
#ifdef  __USE_BSD
               pValue->stream.u16 = stm ? (short) (stm->tm_gmtoff/60) : m_nTimeZone;
#endif
           }
           else if (nMethod == VARMETHOD_LOAD)
           {
               m_nTimeZone = pObject->var.stream.s16;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               short tzvalue;
               char buffer[256];

               VARAPI_ReplaceValue(pObject, pValue);

               m_nTimeZone = pValue->stream.s16;
               tzvalue =(short) (m_nTimeZone / 60);

               sprintf(buffer,"cp -f /app/sw/zoneinfo/GMT%d /app/conf/localtime 2> /dev/null", tzvalue);
               system(buffer);
               
               VARAPI_SetModify();

               m_pService->SetResetState(RST_COMMAND);
               m_pService->PostMessage(GWMSG_RESET, NULL);
           }
           return 1;
      case 34 :
           // sysOpMode(2.1.34) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_LOAD) 
           {
               m_nOpMode = (BYTE) pObject->var.stream.u8;
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nOpMode = (BYTE) pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
            
           }
           return 1;
      case 35 :
           // sysPowerType(2.1.35) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
           {
               pObject->var.stream.u32 = m_nNzcPowerType;
           }
           else if (nMethod == VARMETHOD_LOAD)
           {
               GetFileValue("/app/conf/powertype.mcu", (int *)&m_nNzcPowerType);
               pObject->var.stream.u32 = m_nNzcPowerType;
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nNzcPowerType = (UINT) pValue->stream.u32;
               SetFileValue("/app/conf/powertype.mcu", (int)m_nNzcPowerType);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();

               m_pService->SetResetState(RST_COMMAND);
               m_pService->PostMessage(GWMSG_RESET, NULL);
           }
           return 1;
      case 36 :
           // sysStateMask(2.1.36) UINT(4) Read
           if (nMethod == VARMETHOD_DEFAULT)
           {
               pObject->var.stream.u32 = m_pStatusMonitor->GetErrorMask();
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u32 = m_pStatusMonitor->GetErrorMask();
           }
           return 1;
      //////////////////////////////////////////////////////////////////////////
      //  [9/13/2010 DHKim]
      //  SecurityStae 추가
      //////////////////////////////////////////////////////////////////////////
      case 37 :
           // sysSecurityState(2.1.37) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_LOAD)
           {
               
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
              
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      //////////////////////////////////////////////////////////////////////////

    }
    return 0;
}

int varEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // varEntry(2.2.0) OPAQUE(15) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 VARENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (VARENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(VARENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->varEnableMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->varEnableRecovery, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->varEnableCmdHistLog, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->varEnableCommLog, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[4], (void *)&pEntry->varEanbleAutoRegister, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[5], (void *)&pEntry->varEnableAutoReset, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[6], (void *)&pEntry->varEnableAutoTimeSync, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[7], (void *)&pEntry->varEnableAutoSinkReset, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[8], (void *)&pEntry->varEnableSubNetwork, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[9], (void *)&pEntry->varEnableMobileStaticLog, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[10], (void *)&pEntry->varEnableAutoUpload, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[11], (void *)&pEntry->varEnableSecurity, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[12], (void *)&pEntry->varEnableMonitoring, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[13], (void *)&pEntry->varEnableKeepalive, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[14], (void *)&pEntry->varEnableGpsTimesync, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[15], (void *)&pEntry->varEnableTimeBroadcast, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[16], (void *)&pEntry->varEnableStatusMonitoring, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[17], (void *)&pEntry->varEnableMemoryCheck, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[18], (void *)&pEntry->varEnableFlashCheck, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[19], (void *)&pEntry->varEnableMeterTimesync, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[20], (void *)&pEntry->varEnableMeterCheck, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[21], (void *)&pEntry->varEnableHeater, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[22], (void *)&pEntry->varEnableReadMeterEvent, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[23], (void *)&pEntry->varEnableLowBattery, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[24], (void *)&pEntry->varEnableTestMode, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[25], (void *)&pEntry->varEnableDebugLog, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[26], (void *)&pEntry->varEnableMeterRecovery, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[27], (void *)&pEntry->varEnableHighRAM, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[28], (void *)&pEntry->varEnableGarbageCleaning, pParam);
					  VARAPI_GetIndexValue(&pObject->pChild[29], (void *)&pEntry->varEnableJoinControl, pParam);
                 }
           }
           return 1;
      case 1 :
           // varEnableMask(2.2.1) UINT(4) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
               VARAPI_ReplaceValue(pObject, pValue);
           return 1;
      case 2 :
           // varEnableRecovery(2.2.2) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableRecovery;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableRecovery = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableRecovery = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 3 :
           // varEnableCmdHistLog(2.2.3) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableCommandLog;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableCommandLog = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableCommandLog = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 4 :
           // varEnableCommLog(2.2.4) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableCommLog;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableCommLog = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableCommLog = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 5 :
           // varEanbleAutoRegister(2.2.5) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableAutoRegister;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableAutoRegister = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableAutoRegister = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 6 :
           // varEnableAutoReset(2.2.6) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableAutoReset;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableAutoReset = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableAutoReset = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 7 :
           // varEnableAutoTimeSync(2.2.7) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableAutoTimesync;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableAutoTimesync = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableAutoTimesync = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 8 :
           // varEnableAutoSinkReset(2.2.8) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = TRUE;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 9 :
           // varEnableSubNetwork(2.2.9) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = FALSE;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 10 :
           // varEnableMobileStaticLog(2.2.10) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableMobileLog;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableMobileLog = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableMobileLog = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 11 :
           // varEnableAutoUpload(2.2.11) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableAutoUpload;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableAutoUpload = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableAutoUpload = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 12 :
           // varEnableSecurity(2.2.12) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableSecurity;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableSecurity = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableSecurity = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 13 :
           // varEnableMonitoring(2.2.13) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nDebugLevel ? 0 : 1;
           else if (nMethod == VARMETHOD_GET)
           {
               pObject->var.stream.u8 = m_nDebugLevel ? 0 : 1;
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nDebugLevel = pValue->stream.u8 ? 0 : 1;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 14 :
           // varEnableKeepalive(2.2.14) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableKeepaliveEvent;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableKeepaliveEvent = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableKeepaliveEvent = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 15 :
           // varEnableGpsTimesync(2.2.15) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableGpsTimesync;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableGpsTimesync = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableGpsTimesync = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 16 :
           // varEnableTimeBroadcast(2.2.16) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableTimeBroadcast;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableTimeBroadcast = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableTimeBroadcast = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 17 :
           // varEnableStatusMonitoring(2.2.17) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableStatusMonitor;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableStatusMonitor = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableStatusMonitor = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 18 :
           // varEnableMemoryCheck(2.2.18) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableMemoryCheck;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableMemoryCheck = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableMemoryCheck = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 19 :
           // varEnableFlashCheck(2.2.19) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableFlashCheck;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableFlashCheck = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableFlashCheck = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 20 :
           // varEnableMeterTimesync(2.2.20) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableMeterTimesync;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableMeterTimesync = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableMeterTimesync = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 21 :
           // varEnableMeterCheck(2.2.21) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableMeterCheck;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableMeterCheck = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableMeterCheck = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 22 :
           // varEnableHeater(2.2.22) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableHeater;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableHeater = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableHeater = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 23 :
           // varEnableReadMeterEvent(2.2.23) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableReadMeterEvent;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableReadMeterEvent = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableReadMeterEvent = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 24 :
           // varEnableLowBattery(2.2.24) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableLowBattery;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableLowBattery = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableLowBattery = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 25 :
           // varEnableTestMode(2.2.25) BOOL(1) Read/Write
           // Issue #1477 : sysOpMode(2.1.34) 로 대체 (Deprecated)
           return 0;
      case 26 :
           // varEnableDebugLog(2.2.26) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = FALSE;
           else if (nMethod == VARMETHOD_GET)
           {
               pObject->var.stream.u8 = m_bEnableDebugLog;
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableDebugLog = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();

               if(m_bEnableDebugLog)
               {
                    if(m_pDebugServer == NULL)
                    {
                        m_pDebugServer = new CDebugServer();
                        m_pDebugServer->Startup(m_nDebugServerPort, 3);
                    }
               }
               else
               {
                    if(m_pDebugServer != NULL)
                    {
                        m_pDebugServer->Shutdown();
                        m_pDebugServer = NULL;
                        delete m_pDebugServer;
                    }
               }
           }
           return 1;
      case 27 :
           // varEnableMeterRecovery(2.2.27) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableMeterRecovery;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableMeterRecovery = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableMeterRecovery = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 28 : // Issue #1965 : HIGH/LOW RAM Coordinator 사용 여부
           // varEnableHighRAM(2.2.28) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableHighRAM;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableHighRAM = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableHighRAM = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 29 : // Issue #1964 : 자동 정리 기능 사용 여부
           // varEnableGarbageCleaning(2.2.29) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_bEnableGarbageCleaning;
           else if (nMethod == VARMETHOD_LOAD)
               m_bEnableGarbageCleaning = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_bEnableGarbageCleaning = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
	  case 30 : // HEMS JoinControl
		  // varEnableJoinCtrl(2.2.30) BOOL(1) Read/Write
		  if (nMethod == VARMETHOD_DEFAULT)
			  pObject->var.stream.u8 = m_bEnableJoinControl;
		  else if (nMethod == VARMETHOD_LOAD)
			  m_bEnableJoinControl = pObject->var.stream.u8;
		  else if (nMethod == VARMETHOD_GET)
			  VARAPI_CopyValue(pObject, pValue);
		  else if (nMethod == VARMETHOD_SET)
		  {
			  m_bEnableJoinControl = pValue->stream.u8;
			  VARAPI_ReplaceValue(pObject, pValue);
			  VARAPI_SetModify();
		  }
		  return 1;
    }
    return 0;
}

int varValueEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // varValueEntry(2.3.0) OPAQUE(88) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 VARVALUEENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (VARVALUEENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(VARVALUEENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->varValueMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->varAutoResetCheckInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->varSysPowerOffDelay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->varSysTempMonInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[4], (void *)&pEntry->varAutoTimeSyncInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[5], (void *)&pEntry->varSysMeteringThreadCount, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[6], (void *)&pEntry->varSinkPollingInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[7], (void *)&pEntry->varSinkResetInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[8], (void *)&pEntry->varSinkResetDelay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[9], (void *)&pEntry->varSinkLedTurnOffInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[10], (void *)&pEntry->varSinkAckWaitTime, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[11], (void *)&pEntry->varSensorTimeout, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[12], (void *)&pEntry->varSensorKeepalive, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[13], (void *)&pEntry->varSensorMeterSaveCount, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[14], (void *)&pEntry->varMeterDayMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[15], (void *)&pEntry->varMeterHourMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[16], (void *)&pEntry->varMeterStartMin, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[17], (void *)&pEntry->varRecoveryDayMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[18], (void *)&pEntry->varRecoveryHourMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[19], (void *)&pEntry->varRecoveryStartMin, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[20], (void *)&pEntry->varCmdHistSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[21], (void *)&pEntry->varEventLogSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[22], (void *)&pEntry->varCommLogSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[23], (void *)&pEntry->varMeterLogSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[24], (void *)&pEntry->varAutoResetTime, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[25], (void *)&pEntry->varMeterUploadCycleType, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[26], (void *)&pEntry->varMeterUploadCycle, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[27], (void *)&pEntry->varMeterUploadStartHour, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[28], (void *)&pEntry->varMeterUploadStartMin, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[29], (void *)&pEntry->varMeterUploadRetry, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[30], (void *)&pEntry->varMeterIssueDate, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[31], (void *)&pEntry->varMemoryCriticalRate, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[32], (void *)&pEntry->varFlashCriticalRate, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[33], (void *)&pEntry->varNapcGroupInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[34], (void *)&pEntry->varNapcRetry, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[35], (void *)&pEntry->varNapcRetryHour, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[36], (void *)&pEntry->varNapcRetryStartSecond, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[37], (void *)&pEntry->varNapcRetryClear, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[38], (void *)&pEntry->varMaxEventLogSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[39], (void *)&pEntry->varMaxAlarmLogSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[40], (void *)&pEntry->varMaxCmdLogSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[41], (void *)&pEntry->varMaxCommLogSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[42], (void *)&pEntry->varMaxMobileLogSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[43], (void *)&pEntry->varKeepaliveInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[44], (void *)&pEntry->varAlarmLogSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[45], (void *)&pEntry->varTimeBroadcastInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[46], (void *)&pEntry->varStatusMonitorTime, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[47], (void *)&pEntry->varUploadTryTime, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[48], (void *)&pEntry->varDataSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[49], (void *)&pEntry->varMeteringPeriod, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[50], (void *)&pEntry->varRecoveryPeriod, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[51], (void *)&pEntry->varMeteringRetry, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[52], (void *)&pEntry->varRecoveryRetry, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[53], (void *)&pEntry->varCheckDayMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[54], (void *)&pEntry->varCheckHourMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[55], (void *)&pEntry->varCheckStartMin, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[56], (void *)&pEntry->varCheckPeriod, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[57], (void *)&pEntry->varCheckRetry, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[58], (void *)&pEntry->varMeterTimesyncDayMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[59], (void *)&pEntry->varMeterTimesyncHourMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[60], (void *)&pEntry->varMeterReadingInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[61], (void *)&pEntry->varHeaterOnTemp, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[62], (void *)&pEntry->varHeaterOffTemp, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[63], (void *)&pEntry->varMobileLiveCheckInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[64], (void *)&pEntry->varEventReadDayMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[65], (void *)&pEntry->varEventReadHourMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[66], (void *)&pEntry->varEventSendDelay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[67], (void *)&pEntry->varEventAlertLevel, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[68], (void *)&pEntry->varSensorLimit, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[69], (void *)&pEntry->varMeteringStrategy, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[70], (void *)&pEntry->varTimesyncThresholdLow, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[71], (void *)&pEntry->varMobileLiveCheckMethod, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[72], (void *)&pEntry->varScanningStrategy, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[73], (void *)&pEntry->varMobileLogSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[74], (void *)&pEntry->varUpgradeLogSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[75], (void *)&pEntry->varUploadLogSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[76], (void *)&pEntry->varTimesyncLogSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[77], (void *)&pEntry->varMaxMeterLogSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[78], (void *)&pEntry->varMaxUpgradeLogSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[79], (void *)&pEntry->varMaxUploadLogSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[80], (void *)&pEntry->varMaxTimesyncLogSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[81], (void *)&pEntry->varDefaultMeteringOption, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[82], (void *)&pEntry->varSensorCleaningThreshold, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[83], (void *)&pEntry->varTimeSyncStrategy, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[84], (void *)&pEntry->varTransactionSaveDay, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[85], (void *)&pEntry->varSchedulerType, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[86], (void *)&pEntry->varTimesyncThresholdHigh, pParam);
                 }
           }
           return 1;
      case 1 :
           // varValueMask(2.3.1) UINT(4) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
               VARAPI_ReplaceValue(pObject, pValue);
           return 1;
      case 2 :
           // varAutoResetCheckInterval(2.3.2) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nAutoResetInterval;
           else if (nMethod == VARMETHOD_LOAD)
               m_nAutoResetInterval = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nAutoResetInterval = pValue->stream.u32; 
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 3 :
           // varSysPowerOffDelay(2.3.3) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nPowerOffDelay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nPowerOffDelay = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nPowerOffDelay = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 4 :
           // varSysTempMonInterval(2.3.4) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nTempMonInterval;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTempMonInterval = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTempMonInterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 5 :
           // varAutoTimeSyncInterval(2.3.5) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nAutoTimeSyncInterval;
           else if (nMethod == VARMETHOD_LOAD)
               m_nAutoTimeSyncInterval = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nAutoTimeSyncInterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 6 :
           // varSysMeteringThreadCount(2.3.6) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nMeteringThreadCount;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeteringThreadCount = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeteringThreadCount = MIN(pValue->stream.u8, MAX_ZIGBEE_WORKER);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 7 :
           // varSinkPollingInterval(2.3.7) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nSinkPollingInterval;
           else if (nMethod == VARMETHOD_LOAD)
               m_nSinkPollingInterval = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nSinkPollingInterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 8 :
           // varSinkResetInterval(2.3.8) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nSinkResetInterval;
           else if (nMethod == VARMETHOD_LOAD)
               m_nSinkResetInterval = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nSinkResetInterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 9 :
           // varSinkResetDelay(2.3.9) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nSinkResetDelay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nSinkResetDelay = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nSinkResetDelay = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 10 :
           // varSinkLedTurnOffInterval(2.3.10) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nSinkLedTurnOffInterval;
           else if (nMethod == VARMETHOD_LOAD)
               m_nSinkLedTurnOffInterval = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nSinkLedTurnOffInterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 11 :
           // varSinkAckWaitTime(2.3.11) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nSinkAckWaitTime;
           else if (nMethod == VARMETHOD_LOAD)
               m_nSinkAckWaitTime = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nSinkAckWaitTime = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 12 :
           // varSensorTimeout(2.3.12) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = 15;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 13 :
           // varSensorKeepalive(2.3.13) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = 65;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 14 :
           // varSensorMeterSaveCount(2.3.14) WORD(2) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u16 = 24*40;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 15 :
           // varMeterDayMask(2.3.15) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nMeterDayMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeterDayMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeterDayMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 16 :
           // varMeterHourMask(2.3.16) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nMeterHourMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeterHourMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeterHourMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 17 :
           // varMeterStartMin(2.3.17) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nMeterStartMin;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeterStartMin = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeterStartMin = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 18 :
           // varRecoveryDayMask(2.3.18) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nRecoveryDayMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nRecoveryDayMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nRecoveryDayMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 19 :
           // varRecoveryHourMask(2.3.19) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nRecoveryHourMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nRecoveryHourMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nRecoveryHourMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 20 :
           // varRecoveryStartMin(2.3.20) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nRecoveryStartMin;
           else if (nMethod == VARMETHOD_LOAD)
               m_nRecoveryStartMin = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nRecoveryStartMin = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 21 :
           // varCmdHistSaveDay(2.3.21) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nCmdHistSaveDay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nCmdHistSaveDay = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nCmdHistSaveDay = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 22 :
           // varEventLogSaveDay(2.3.22) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nEventLogSaveDay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nEventLogSaveDay = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nEventLogSaveDay = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 23 :
           // varCommLogSaveDay(2.3.23) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nCommLogSaveDay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nCommLogSaveDay = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nCommLogSaveDay = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 24 :
           // varMeterLogSaveDay(2.3.24) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nMeterLogSaveDay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeterLogSaveDay = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeterLogSaveDay = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 25 :
           // varAutoResetTime(2.3.25) TIMESTAMP(7) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
           {
               TIMESTAMP    *pTime;

               pTime = (TIMESTAMP *)pObject->var.stream.p;
               pTime->hour = m_tmAutoResetTime.hour;
               pTime->min  = m_tmAutoResetTime.min;
               pTime->sec  = m_tmAutoResetTime.sec;    
           }
           else if (nMethod == VARMETHOD_LOAD)
               memcpy(&m_tmAutoResetTime, pObject->var.stream.p, sizeof(TIMESTAMP));
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               memcpy(&m_tmAutoResetTime, pValue->stream.p, sizeof(TIMESTAMP));
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 26 :
           // varMeterUploadCycleType(2.3.26) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nUploadType;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUploadType = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUploadType = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 27 :
           // varMeterUploadCycle(2.3.27) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nUploadCycle;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUploadCycle = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUploadCycle = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 28 :
           // varMeterUploadStartHour(2.3.28) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nUploadStartHour;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUploadStartHour = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUploadStartHour = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 29 :
           // varMeterUploadStartMin(2.3.29) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nUploadStartMin;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUploadStartMin = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUploadStartMin = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 30 :
           // varMeterUploadRetry(2.3.30) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nUploadRetry;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUploadRetry = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUploadRetry = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 31 :
           // varMeterIssueDate(2.3.31) TIMESTAMP(7) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               memset(&m_tmMeterIssueDate, 0, sizeof(TIMESTAMP));
           else if (nMethod == VARMETHOD_LOAD)
               memcpy(&m_tmMeterIssueDate, pObject->var.stream.p, sizeof(TIMESTAMP));
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               memcpy(&m_tmMeterIssueDate, pValue->stream.p, sizeof(TIMESTAMP));
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 32 :
           // varMemoryCriticalRate(2.3.32) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = 90;
           else if (nMethod == VARMETHOD_LOAD)
               m_pMalfMonitor->SetMemoryRatio(pObject->var.stream.u8);
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               m_pMalfMonitor->SetMemoryRatio(pValue->stream.u8);
               VARAPI_SetModify();
           }
           return 1;
      case 33 :
           // varFlashCriticalRate(2.3.33) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = 90;
           else if (nMethod == VARMETHOD_LOAD)
               m_pMalfMonitor->SetDiskRatio(pObject->var.stream.u8);
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               m_pMalfMonitor->SetDiskRatio(pValue->stream.u8);
               VARAPI_SetModify();
           }
           return 1;
      case 34 :
           // varNapcGroupInterval(2.3.34) BYTE(1) Read/Write
           return 1;
      case 35 :
           // varNapcRetry(2.3.35) BYTE(1) Read/Write
           return 1;
      case 36 :
           // varNapcRetryHour(2.3.36) BYTE(1) Read/Write
           return 1;
      case 37 :
           // varNapcRetryStartSecond(2.3.37) BYTE(1) Read/Write
           return 1;
      case 38 :
           // varNapcRetryClear(2.3.38) UINT(4) Read/Write
           return 1;
      case 39 :
           // varMaxEventLogSize(2.3.39) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nEventLogSize;
           else if (nMethod == VARMETHOD_LOAD)
               m_nEventLogSize = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nEventLogSize = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 40 :
           // varMaxAlarmLogSize(2.3.40) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nAlarmLogSize;
           else if (nMethod == VARMETHOD_LOAD)
               m_nAlarmLogSize = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nAlarmLogSize = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 41 :
           // varMaxCmdLogSize(2.3.41) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
           {
               pObject->var.stream.u32 = m_nCmdLogSize;
               IF4API_SetMaxLogSize(m_nCmdLogSize);
           }
           else if (nMethod == VARMETHOD_LOAD)
           {
               m_nCmdLogSize = pObject->var.stream.u32;
               IF4API_SetMaxLogSize(m_nCmdLogSize);
           }
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nCmdLogSize = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
               IF4API_SetMaxLogSize(m_nCmdLogSize);
           }
           return 1;
      case 42 :
           // varMaxCommLogSize(2.3.42) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nCommLogSize;
           else if (nMethod == VARMETHOD_LOAD)
               m_nCommLogSize = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nCommLogSize = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 43 :
           // varMaxMobileLogSize(2.3.43) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nMobileLogSize;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMobileLogSize = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMobileLogSize = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 44 :
           // varKeepaliveInterval(2.3.44) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nKeepaliveInvterval;
           else if (nMethod == VARMETHOD_LOAD)
               m_nKeepaliveInvterval = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nKeepaliveInvterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 45 :
           // varAlarmLogSaveDay(2.3.45) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nAlarmSaveDays;
           else if (nMethod == VARMETHOD_LOAD)
               m_nAlarmSaveDays = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nAlarmSaveDays = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 46 :
           // varTimeBroadcastInterval(2.3.46) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nTimeBroadcastInterval;
           else if (nMethod == VARMETHOD_GET)
               pObject->var.stream.u32 = m_nTimeBroadcastInterval;
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTimeBroadcastInterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 47 :
           // varStatusMonitorTime(2.3.47) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nStatusMonitorMin;
           else if (nMethod == VARMETHOD_LOAD)
               m_nStatusMonitorMin = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nStatusMonitorMin = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 48 :
           // varUploadTryTime(2.3.48) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nUploadTryTime;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUploadTryTime = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUploadTryTime = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 49 :
           // varDataSaveDay(2.3.49) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
           {
               pObject->var.stream.u32 = m_nDataSaveDay;
               m_pVariableHelper->Update("dataSaveDay", m_nDataSaveDay); // DB Update
           }
           else if (nMethod == VARMETHOD_LOAD)
           {
               m_nDataSaveDay = pObject->var.stream.u32;
               m_pVariableHelper->Update("dataSaveDay", m_nDataSaveDay); // DB Update
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nDataSaveDay = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
               m_pVariableHelper->Update("dataSaveDay", m_nDataSaveDay); // DB Update
           }
           return 1;
      case 50 :
           // varMeteringPeriod(2.3.50) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nMeteringPeriod;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeteringPeriod = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeteringPeriod = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 51 :
           // varRecoveryPeriod(2.3.51) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nRecoveryPeriod;
           else if (nMethod == VARMETHOD_LOAD)
               m_nRecoveryPeriod = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nRecoveryPeriod = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 52 :
           // varMeteringRetry(2.3.52) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nMeteringRetry;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeteringRetry = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeteringRetry = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 53 :
           // varRecoveryRetry(2.3.53) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nRecoveryRetry;
           else if (nMethod == VARMETHOD_LOAD)
               m_nRecoveryRetry = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nRecoveryRetry = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 54 :
           // varCheckDayMask(2.3.54) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nCheckDayMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nCheckDayMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nCheckDayMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 55 :
           // varCheckHourMask(2.3.55) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nCheckHourMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nCheckHourMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nCheckHourMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 56 :
           // varCheckStartMin(2.3.56) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nCheckStartMin;
           else if (nMethod == VARMETHOD_LOAD)
               m_nCheckStartMin = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nCheckStartMin = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 57 :
           // varCheckPeriod(2.3.57) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nCheckPeriod;
           else if (nMethod == VARMETHOD_LOAD)
               m_nCheckPeriod = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nCheckPeriod = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 58 :
           // varCheckRetry(2.3.58) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nCheckRetry;
           else if (nMethod == VARMETHOD_LOAD)
               m_nCheckRetry = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nCheckRetry = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 59 :
           // varMeterTimesyncDayMask(2.3.59) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nTimesyncDayMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTimesyncDayMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTimesyncDayMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 60 :
           // varMeterTimesyncHourMask(2.3.60) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nTimesyncHourMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTimesyncHourMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTimesyncHourMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 61 :
           // varMeterReadingInterval(2.3.61) UINT(4) Read/Write
           // Norway 이후 사용 안함 (초기 RURAL 외부 시리얼 사용시 적용
           /** Issue #10 Interval Scheduler를 위해 다시 기능을 복구 한다.  
             */
           if (nMethod == VARMETHOD_DEFAULT)
           {
               m_nMeteringInterval = GetValidIntervalSchedulerValue(m_nMeteringInterval);
               pObject->var.stream.u32 = m_nMeteringInterval;
           }
           else if (nMethod == VARMETHOD_LOAD)
           {
               m_nMeteringInterval = GetValidIntervalSchedulerValue(pObject->var.stream.u32);
           }
           else if (nMethod == VARMETHOD_GET)
           {
               m_nMeteringInterval = GetValidIntervalSchedulerValue(m_nMeteringInterval);
               VARAPI_CopyValue(pObject, pValue);
               pObject->var.stream.u32 = m_nMeteringInterval;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               pValue->stream.u32 = GetValidIntervalSchedulerValue(pValue->stream.u32);
               m_nMeteringInterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 62 :
           // varHeaterOnTemp(2.3.62) int(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s32 = m_nHeaterOnTemp;
           else if (nMethod == VARMETHOD_LOAD)
               m_nHeaterOnTemp = pObject->var.stream.s32;
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.s32 = m_nHeaterOnTemp;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nHeaterOnTemp = pValue->stream.s32;
               /* NAZC Heater 설정 명령 부분 추가 필요 */
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 63 :
           // varHeaterOffTemp(2.3.63) int(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s32 = m_nHeaterOffTemp;
           else if (nMethod == VARMETHOD_LOAD)
               m_nHeaterOffTemp = pObject->var.stream.s32;
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.s32 = m_nHeaterOffTemp;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nHeaterOffTemp = pValue->stream.u32;
               /* NAZC Heater 설정 명령 부분 추가 필요 */
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 64 :
           // varMobileLiveCheckInterval(2.3.64) int(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nMobileLiveCheckInterval;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMobileLiveCheckInterval = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u32 = m_nMobileLiveCheckInterval;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMobileLiveCheckInterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 65 :
           // varEventReadDayMask(2.3.65) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nEventReadDayMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nEventReadDayMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nEventReadDayMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 66 :
           // varEventReadHourMask(2.3.66) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nEventReadHourMask;
           else if (nMethod == VARMETHOD_LOAD)
               m_nEventReadHourMask = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nEventReadHourMask = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 67 :
           // varEventSendDelay(2.3.67) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nEventSendDelay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nEventSendDelay = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nEventSendDelay = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 68 :
           // varEventAlertLevel(2.3.68) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nEventAlertLevel;
           else if (nMethod == VARMETHOD_LOAD)
               m_nEventAlertLevel = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nEventAlertLevel = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 69 :
           // varSensorLimit(2.3.69) int(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s32 = m_nSensorLimit;
           else if (nMethod == VARMETHOD_LOAD)
               m_nSensorLimit = pObject->var.stream.s32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nSensorLimit = pValue->stream.s32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 70 :
           // varMeteringStrategy(2.3.70) byte(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s8 = m_nMeteringStrategy;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeteringStrategy = pObject->var.stream.s8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeteringStrategy = pValue->stream.s8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 71 :
           // varTimesyncThresholdLow(2.3.71) int(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s32 = m_nTimesyncThresholdLow;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTimesyncThresholdLow = pObject->var.stream.s32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTimesyncThresholdLow = pValue->stream.s32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 72 :
           // varMobileLiveCheckMethod(2.3.72) byte(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s8 = m_nMobileLiveCheckMethod;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMobileLiveCheckMethod = pObject->var.stream.s8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMobileLiveCheckMethod = pValue->stream.s8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 73 :
           // varScanningStrategy(2.3.73) byte(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s8 = m_nScanningStrategy;
           else if (nMethod == VARMETHOD_LOAD)
               m_nScanningStrategy = pObject->var.stream.s8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nScanningStrategy = pValue->stream.s8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
               /** Issue #1648 : Scanning Strategy를 변경할 때 마다 Discovery Option을 자동으로 조정한다
                */
#if     defined(__PATCH_4_1647__)
               // Direct Comm 이거나 Immediately 이면 On 시킨다
               RouteDiscoveryControl((*m_pbPatch_4_1647) | (m_nScanningStrategy == SCANNING_STRATEGY_IMMEDIATELY));
#else
               RouteDiscoveryControl(m_nScanningStrategy == SCANNING_STRATEGY_LAZY ? FALSE : TRUE);
#endif
           }
           return 1;
      case 74 :
           // varMobileLogSaveDay(2.3.74) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nMobileLogSaveDay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMobileLogSaveDay = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMobileLogSaveDay = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 75 :
           // varUpgradeLogSaveDay(2.3.75) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nUpgradeLogSaveDay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUpgradeLogSaveDay = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUpgradeLogSaveDay = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 76 :
           // varUploadLogSaveDay(2.3.76) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nUploadLogSaveDay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUploadLogSaveDay = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUploadLogSaveDay = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 77 :
           // varTimesyncLogSaveDay(2.3.77) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nTimesyncLogSaveDay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTimesyncLogSaveDay = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTimesyncLogSaveDay = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 78 :
           // varMaxMeterLogSize(2.3.78) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nMeterLogSize;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeterLogSize = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeterLogSize = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 79 :
           // varMaxUpgradeLogSize(2.3.79) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nUpgradeLogSize;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUpgradeLogSize = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUpgradeLogSize = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 80 :
           // varMaxUploadLogSize(2.3.80) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nUploadLogSize;
           else if (nMethod == VARMETHOD_LOAD)
               m_nUploadLogSize = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nUploadLogSize = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 81 :
           // varMaxTimesyncLogSize(2.3.81) UINT(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nTimesyncLogSize;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTimesyncLogSize = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTimesyncLogSize = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 82 :
           // varDefaultMeteringOption(2.3.82) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nDefaultMeteringOption;
           else if (nMethod == VARMETHOD_LOAD)
               m_nDefaultMeteringOption = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nDefaultMeteringOption = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 83 :
           // varSensorCleaningThreshold(2.3.83) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nSensorCleaningThreshold;
           else if (nMethod == VARMETHOD_LOAD)
               m_nSensorCleaningThreshold = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nSensorCleaningThreshold = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 84 :
           // varTimeSyncStrategy(2.3.84) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nTimeSyncStrategy;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTimeSyncStrategy = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTimeSyncStrategy = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 85 :
           // varTransactionSaveDay(2.3.85) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nTransactionSaveDay;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTransactionSaveDay = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTransactionSaveDay = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 86 :
           // varSchedulerType(2.3.86) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
           {
               pObject->var.stream.u8 = m_nSchedulerType;
           }
           else if (nMethod == VARMETHOD_LOAD)
           {
               m_nSchedulerType = pObject->var.stream.u8;
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nSchedulerType = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();

               /** Issue #10 
                *
                * Scheduler Type에 영향을 받는 동작들을 조정해 줘야 한다.
                *  - Metering
                *  - Recovery
                *
                *  Metering.SetSchedulerType();
                *  Recovery.SetSchedulerType();
                */
               m_pMetering->SetSchedulerType(m_nSchedulerType);
               m_pRecovery->SetSchedulerType(m_nSchedulerType);

               /** Issue #10
                *
                * Interval Scheduler의 경우 Coordinator Discovery mode를 자동으로 On 시킨다.
                * Mask Scheduler의 경우 Coordinator Discovery mode를 변경하지 않는다.
                */
#if     defined(__PATCH_4_1647__)
               // Direct Comm 이거나 Interval Scheduler 이면 On 시킨다
               if((*m_pbPatch_4_1647) || m_nSchedulerType == SCHEDULER_TYPE_INTERVAL)
#else
               if(m_nSchedulerType == SCHEDULER_TYPE_INTERVAL)
#endif
               {
                    RouteDiscoveryControl(TRUE);
               }
           }
           return 1;
      case 87 :
           // varTimesyncThresholdHigh(2.3.87) int(4) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s32 = m_nTimesyncThresholdHigh;
           else if (nMethod == VARMETHOD_LOAD)
               m_nTimesyncThresholdHigh = pObject->var.stream.s32;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nTimesyncThresholdHigh = pValue->stream.s32;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
    }
    return 0;
}

int varSubValueEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // varSubValueEntry(2.4.0) OPAQUE(7) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 VARSUBVALUEENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (VARSUBVALUEENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(VARSUBVALUEENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->varSubValueMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->varSubChannel, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->varSubPanID, pParam);
                 }
           }
           return 1;
      case 1 :
           // varSubValueMask(2.4.1) UINT(4) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
               VARAPI_ReplaceValue(pObject, pValue);
           return 1;
      case 2 :
           // varSubChannel(2.4.2) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = 14;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 3 :
           // varSubPanID(2.4.3) WORD(2) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u16 = 6001;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
    }
    return 0;
}
    
int varGeEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // varGeEntry(2.5.0) OPAQUE(7) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 VARGEENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (VARGEENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(VARGEENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->varGeMeteringInterval, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->varGeUser, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->varGeMeterSecurity, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->varGeAllUploadHour, pParam);
                 }
           }
           return 1;
      case 1 :
           // varGeMeteringInterval(2.5.1) UINT(4) Read/Write
#if 0
           /** m_nMeteringInterval 을 varMeterReadingInterval에서 사용하기 위해
             * varGeMeteringInterval을 deprecate 시킨다
             */
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u32 = m_nMeteringInterval;
           else if (nMethod == VARMETHOD_LOAD)
               m_nMeteringInterval = pObject->var.stream.u32;
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pObject->var.stream.u32 = m_nMeteringInterval;
           }
           else if (nMethod == VARMETHOD_SET)
           {
               m_nMeteringInterval = pValue->stream.u32;
               VARAPI_ReplaceValue(pObject, pValue);
           }
           return 1;
#else
           return 0;
#endif
      case 2 :
           // varGeUser(2.5.2) STREAM(8) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
           {
               memset(pObject->var.stream.p, 0, 10);
               strncpy(pObject->var.stream.p, (char *)ge_logon, 10);
           }
           else if (nMethod == VARMETHOD_LOAD)
               memcpy(ge_logon, pObject->var.stream.p, 10);
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               memcpy(ge_logon, pValue->stream.p, 10);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 3 :
           // varGeMeterSecurity(2.5.3) STREAM(8) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
           {
               memset(pObject->var.stream.p, 0, 20);
               memcpy(pObject->var.stream.p, (char *)ge_master_security, 20);
           }
           else if (nMethod == VARMETHOD_LOAD)
               memcpy(ge_master_security, pObject->var.stream.p, 20);
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               memcpy(pObject->var.stream.p, ge_master_security, 20);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               memcpy(ge_master_security, pValue->stream.p, 20);
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 4 :
           // varGeAllUploadHour(2.5.4) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.u8 = m_nAllUploadHour;
           else if (nMethod == VARMETHOD_LOAD)
               m_nAllUploadHour = pObject->var.stream.u8;
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               m_nAllUploadHour = pValue->stream.u8;
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
    }
    return 0;
}

int usrEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // usrEntry(2.10.0) OPAQUE(34) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 USRENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (USRENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(USRENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->usrMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->usrAccount, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->usrPassword, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->usrGroup, pParam);
                 }
           }
           return 1;
      case 1 :
           // usrMask(2.10.1) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 2 :
           // usrAccount(2.10.2) STRING(16) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 3 :
           // usrPassword(2.10.3) STRING(16) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 4 :
           // usrGroup(2.10.4) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
    }
    return 0;
}

int gpsEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // gpsEntry(2.11.0) OPAQUE(45) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 GPSENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (GPSENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(GPSENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->gpsFixPosition, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->gpsCount, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->gpsTime, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->gpsPos, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[4], (void *)&pEntry->gpsTimezone, pParam);
                 }
           }
           return 1;
      case 1 :
           // gpsFixPosition(2.11.1) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               //pValue->stream.u8 = m_pGpsMonitor->getFixPosition();
           }
           return 1;
      case 2 :
           // gpsCount(2.11.2) BYTE(1) Read
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               //pValue->stream.u8 = m_pGpsMonitor->getSatCount();
           }
           return 1;
      case 3 :
           // gpsTime(2.11.3) TIMESTAMP(7) Read
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               //memcpy(pValue->stream.p, m_pGpsMonitor->getTime(), sizeof(TIMESTAMP)); 
           }
           return 1;
      case 4 :
           // gpsPos(2.11.4) BYTE(1) Read
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               //strcpy(pValue->stream.p, m_pGpsMonitor->getPosition());
           }
           return 1;
      case 5 :
           // gpsTimezone(2.11.5) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_DEFAULT)
               pObject->var.stream.s8 = 10;
           else if (nMethod == VARMETHOD_LOAD)
           {
               //m_pGpsMonitor->setTimezone(pObject->var.stream.s8);
           }
           else if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               //pValue->stream.s8 = m_pGpsMonitor->getTimezone();
           }
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               //m_pGpsMonitor->setTimezone(pValue->stream.s8);
               VARAPI_SetModify();
           }
           return 1;
    }
    return 0;
}

int memEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // memEntry(2.100.0) OPAQUE(20) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 MEMENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (MEMENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(MEMENTRY));
                   GetMemoryInfo(&pEntry->memTotalSize, &pEntry->memUseSize, &pEntry->memFreeSize,
                                 &pEntry->memCacheSize, &pEntry->memBufferSize);
                 }
           }
           return 1;
      case 1 :
           // memTotalSize(2.100.1) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 MEMENTRY    mem;
               GetMemoryInfo(&pValue->stream.u32, &mem.memUseSize, &mem.memFreeSize,
                         &mem.memCacheSize, &mem.memBufferSize);
           }
           return 1;
      case 2 :
           // memFreeSize(2.100.2) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 MEMENTRY    mem;
               GetMemoryInfo(&mem.memTotalSize, &pValue->stream.u32, &mem.memFreeSize,
                         &mem.memCacheSize, &mem.memBufferSize);
           }
           return 1;
      case 3 :
           // memUseSize(2.100.3) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 MEMENTRY    mem;
               GetMemoryInfo(&mem.memTotalSize, &mem.memUseSize, &pValue->stream.u32,
                         &mem.memCacheSize, &mem.memBufferSize);
                 // User Operation
           }
           return 1;
      case 4 :
           // memCacheSize(2.100.4) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 MEMENTRY    mem;
               GetMemoryInfo(&mem.memTotalSize, &mem.memUseSize, &mem.memFreeSize,
                         &pValue->stream.u32, &mem.memBufferSize);
           }
           return 1;
      case 5 :
           // memBufferSize(2.100.5) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 MEMENTRY    mem;
               GetMemoryInfo(&mem.memTotalSize, &mem.memUseSize, &mem.memFreeSize,
                         &mem.memCacheSize, &pValue->stream.u32);
           }
           return 1;
    }
    return 0;
}

int flashEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // flashEntry(2.101.0) OPAQUE(12) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 FLASHENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (FLASHENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(FLASHENTRY));
                   GetFlashInfo(&pEntry->flashTotalSize, &pEntry->flashUseSize, &pEntry->flashFreeSize);
                 }
           }
           return 1;
      case 1 :
           // flashTotalSize(2.101.1) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 FLASHENTRY    flash;
               GetFlashInfo(&pValue->stream.u32, &flash.flashUseSize, &flash.flashFreeSize);
           }
           return 1;
      case 2 :
           // flashFreeSize(2.101.2) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 FLASHENTRY    flash;
               GetFlashInfo(&flash.flashTotalSize, &flash.flashUseSize, &pValue->stream.u32);
           }
           return 1;
      case 3 :
           // flashUseSize(2.101.3) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 FLASHENTRY    flash;
               GetFlashInfo(&flash.flashTotalSize, &pValue->stream.u32, &flash.flashFreeSize);
           }
           return 1;
    }
    return 0;
}

int ethEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // ethEntry(2.102.0) OPAQUE(36) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 ETHENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (ETHENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(ETHENTRY));
                   GetEthernetInfo(pEntry->ethName,
                                &pEntry->ethIpAddr,
                                &pEntry->ethSubnetMask,
                                &pEntry->ethGateway,
                                (BYTE *)&pEntry->ethPhyAddr);
                 }
           }
           return 1;
      case 1 :
           // ethName(2.102.1) STRING(16) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 ETHENTRY    eth;
               GetEthernetInfo(pValue->stream.p, &eth.ethIpAddr, &eth.ethSubnetMask,
                                &eth.ethGateway, (BYTE *)&eth.ethPhyAddr);
           }
           return 1;
      case 2 :
           // ethPhyAddr(2.102.2) STREAM(8) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 ETHENTRY    eth;
               GetEthernetInfo(eth.ethName, &eth.ethIpAddr, &eth.ethSubnetMask,
                                &eth.ethGateway, (BYTE *)pValue->stream.p);
           }
           return 1;
      case 3 :
           // ethIpAddr(2.102.3) IPADDR(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               PutFileAddress("/app/conf/ip0", (BYTE *)pValue->stream.p);
           }
           else if (nMethod == VARMETHOD_MOVE)
           {
                 ETHENTRY    eth;
               GetEthernetInfo(eth.ethName, (UINT *)pValue->stream.p, &eth.ethSubnetMask,
                                &eth.ethGateway, (BYTE *)&eth.ethPhyAddr);
           }
           return 1;
      case 4 :
           // ethSubnetMask(2.102.4) IPADDR(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               PutFileAddress("/app/conf/sbnet0", (BYTE *)pValue->stream.p);
           }
           else if (nMethod == VARMETHOD_MOVE)
           {
                 ETHENTRY    eth;
               GetEthernetInfo(eth.ethName, &eth.ethIpAddr, (UINT *)pValue->stream.p,
                                &eth.ethGateway, (BYTE *)&eth.ethPhyAddr);
           }
           return 1;
      case 5 :
           // ethGateway(2.102.5) IPADDR(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               PutFileAddress("/app/conf/gateway", (BYTE *)pValue->stream.p);
           }
           else if (nMethod == VARMETHOD_MOVE)
           {
                 ETHENTRY    eth;
               GetEthernetInfo(eth.ethName, &eth.ethIpAddr, &eth.ethSubnetMask,
                                (UINT *)pValue->stream.p, (BYTE *)&eth.ethPhyAddr);
           }
           return 1;
    }
    return 0;
}

int gpioEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
#if     defined(__INCLUDE_GPIO__)
    switch(pObject->var.oid.id3) {
      case 0 :
           // gpioEntry(2.103.0) OPAQUE(12) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 GPIOENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (GPIOENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                   memset(pEntry, 0, sizeof(GPIOENTRY));
                   pEntry->gpioPowerFail    = GPIOAPI_ReadGpio(GP_PWR_FAIL_INPUT);
                   pEntry->gpioLowBattery   = GPIOAPI_ReadGpio(GP_LOW_BATT_INPUT);
                   pEntry->gpioBUSY         = 0;                                        // Not support
                   pEntry->gpioSTBY         = GPIOAPI_ReadGpio(GP_GSM_SYNC_INPUT);
                   pEntry->gpioSink1State   = 0;                                        // Not support
                   pEntry->gpioSink2State   = 0;                                        // Not support
                   pEntry->gpioCharginState = GPIOAPI_ReadGpio(GP_BATT_CHG_STATUS_INPUT);
                 }
           }
           return 1;
      case 1 :
           // gpioMask(2.103.1) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 2 :
           // gpioPowerFail(2.103.2) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
                  pValue->stream.u8 = GPIOAPI_ReadGpio(GP_PWR_FAIL_INPUT);
           return 1;
      case 3 :
           // gpioLowBattery(2.103.3) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = GPIOAPI_ReadGpio(GP_LOW_BATT_INPUT);
           return 1;
      case 4 :
           // gpioBUSY(2.103.4) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
               pValue->stream.u8 = 0;
           }
           return 1;
      case 5 :
           // gpioSTBY(2.103.5) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = GPIOAPI_ReadGpio(GP_GSM_SYNC_INPUT);
           return 1;
      case 6 :
           // gpioDCD(2.103.6) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = 0;
           return 1;
      case 7 :
           // gpioRI(2.103.7) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = 0;
           return 1;
      case 8 :
           // gpioSink1State(2.103.8) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
                  pValue->stream.u8 = 0;
           return 1;
      case 9 :
           // gpioSink2State(2.103.9) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = 0;
           return 1;
      case 10 :
           // gpioHeaterState(2.103.10) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = 0;
           return 1;
      case 11 :
           // gpioCoverState(2.103.11) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = 0;
           return 1;
      case 12 :
           // gpioCharginState(2.103.12) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u8 = GPIOAPI_ReadGpio(GP_BATT_CHG_STATUS_INPUT);
           return 1;
    }
#endif
    return 0;
}

int gpioCtrlEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
#if     defined(__INCLUDE_GPIO__)
    switch(pObject->var.oid.id3) {
      case 0 :
           // gpioEntry(2.103.0) OPAQUE(12) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
           }
           return 1;
      case 1 :
           // gpioPowerCtrl(2.109.1) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
                  GPIOAPI_WriteGpio(GP_BATT_SW_OUT, pValue->stream.u8);
           return 1;
      case 2 :
           // gpioReset(2.109.2) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
                  GPIOAPI_WriteGpio(GP_SW_RST_OUT, pValue->stream.u8);
           return 1;
      case 3 :
           // gpioMobilePower(2.109.3) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
                  GPIOAPI_WriteGpio(GP_GSM_PCTL_OUT, pValue->stream.u8);
           return 1;
      case 4 :
           // gpioMobileReset(2.109.4) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 5 :
           // gpioMobileDtr(2.109.5) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 6 :
           // gpioSink1Power(2.109.6) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
#ifdef  __SUPPORT_NZC1__
           else if (nMethod == VARMETHOD_SET)
                  GPIOAPI_WriteGpio(GP_CODI_PCTL_OUT, pValue->stream.u8);
#endif
           return 1;
      case 7 :
           // gpioSink2Power(2.109.7) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               // Not support
                  //GPIOAPI_WriteGpio(GPIOPORT_SINK_PWR, pValue->stream.u8);
           }
           return 1;
      case 8 :
           // gpioExternalPower(2.109.8) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               // Not support
                  // GPIOAPI_WriteGpio(GPIOPORT_ADSL_PWR, pValue->stream.u8);
           }
           return 1;
      case 9 :
           // gpioLedSink1(2.109.9) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               // Not support
               // GPIOAPI_WriteGpio(GPIOPORT_LED_SINK1, pValue->stream.u8);
           }
           return 1;
      case 10 :
           // gpioLedSink2(2.109.10) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               // Not support
               // GPIOAPI_WriteGpio(GPIOPORT_LED_SINK2, pValue->stream.u8);
           }
           return 1;
      case 11 :
           // gpioLedReady(2.109.11) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           
           return 1;
      case 12 :
           // gpioLedDebug1(2.109.12) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 13 :
           // gpioLedDebug(2.109.13) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
                  GPIOAPI_WriteGpio(GP_DEBUG_LED1_OUT, pValue->stream.u8);
           return 1;
      case 14 :
           // gpioLedDebug3(2.109.14) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
                  GPIOAPI_WriteGpio(GP_DEBUG_LED2_OUT, pValue->stream.u8);
           return 1;
      case 15 :
           // gpioLedDebug4(2.109.15) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
                  GPIOAPI_WriteGpio(GP_DEBUG_LED3_OUT, pValue->stream.u8);
           return 1;
      case 16 :
           // gpioEmergencyOff(2.109.16) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 17 :
           // gpioLedCoordinatorReset(2.109.17) BYTE(1) Write Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
                  GPIOAPI_WriteGpio(GP_CODI_RST_OUT, pValue->stream.u8);
           return 1;
    }
#endif
    return 0;
}

int mobileEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // mobileEntry(2.104.0) OPAQUE(47) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 MOBILEENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (MOBILEENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(MOBILEENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->mobileID, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->mobileIpaddr, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->mobileRxDbm, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->mobileTxDbm, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[4], (void *)&pEntry->mobilePacketLiveTime, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[5], (void *)&pEntry->mobileSendBytes, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[6], (void *)&pEntry->mobileRecvBytes, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[7], (void *)&pEntry->mobileLastConnectTime, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[8], (void *)&pEntry->mobileSimCardId, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[9], (void *)&pEntry->mobileVolt, pParam);
                 }
           }
           return 1;
      case 1 :
           // mobileID(2.104.1) STRING(16) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
               sprintf(pValue->stream.p, "%08d", IF4API_GetID());
           }
           return 1;
      case 2 :
           // mobileIpaddr(2.104.2) IPADDR(4) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
           {
                 // Default Value
           }
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 3 :
           // mobileRxDbm(2.104.3) INT(4) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
           {
                 // Default Value
           }
           else if (nMethod == VARMETHOD_GET)
           {
               char        *pszBuffer, *p;
               FILE        *fp;
               int        n=0;

               pszBuffer = (char *)MALLOC(1024);
               fp = fopen("/app/sw/mobileinfo", "rb");
               if (fp != NULL)
               {
                   memset(pszBuffer, 0, 1024);
                   fread(pszBuffer, 1, 1023, fp);
                   p = strstr(pszBuffer, "+CSQ: ");
                   if (p != NULL)
                   {
                       n = atoi(p+6);
                       if (n >= 99)
                            n = 0;
                       else n = -(113 - (n*2));
                   }
                   fclose(fp);
               } 
               FREE(pszBuffer);
               pObject->var.stream.s32 = n;
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 4 :
           // mobileTxDbm(2.104.4) INT(4) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
           {
                 // Default Value
           }
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 5 :
           // mobilePacketLiveTime(2.104.5) UINT(4) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
           {
                 // Default Value
           }
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 6 :
           // mobileSendBytes(2.104.6) UINT(4) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
           {
                 // Default Value
           }
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 7 :
           // mobileRecvBytes(2.104.7) UINT(4) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
           {
                 // Default Value
           }
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 8 :
           // mobileLastConnectTime(2.104.8) TIMESTAMP(7) Read Only
           if (nMethod == VARMETHOD_DEFAULT)
           {
                 // Default Value
           }
           else if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 9 :
           // mobileSimCardId(2.104.9) STRING(20) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_GET)
           {
               /* NAZC 모바일 정보 수집부 추가 예정 */
               strcpy(pValue->stream.p, "");
           }
           else if (nMethod == VARMETHOD_MOVE)
               strcpy(pValue->stream.p, "");
           return 1;
      case 10 :
           // mobileVolt(2.104.10) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_GET)
           {
               /* NAZC 모바일 정보 수집부 추가 예정 */
               pValue->stream.u32 = 0;
           }
           else if (nMethod == VARMETHOD_MOVE)
               pValue->stream.u32 = 0;
           return 1;
    }
    return 0;
}

int pluginEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // pluginEntry(2.105.0) OPAQUE(122) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 PLUGINENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (PLUGINENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                      memset(pEntry, 0, sizeof(PLUGINENTRY));
           }
           return 1;
      case 1 :
           // pluginName(2.105.1) STRING(16) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 2 :
           // pluginFileName(2.105.2) STRING(32) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 3 :
           // pluginDescr(2.105.3) STRING(64) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 4 :
           // pluginType(2.105.4) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 5 :
           // pluginState(2.105.5) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 6 :
           // pluginVersion(2.105.6) STRING(8) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
    }
    return 0;
}

int procEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // procEntry(2.106.0) OPAQUE(67) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 PROCENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (PROCENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(PROCENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->procID, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->procTTY, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->procSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->procState, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[4], (void *)&pEntry->procCmd, pParam);
                 }
           }
           return 1;
      case 1 :
           // procID(2.106.1) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 2 :
           // procTTY(2.106.2) STRING(8) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 3 :
           // procSize(2.106.3) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 4 :
           // procState(2.106.4) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
      case 5 :
           // procCmd(2.106.5) STRING(50) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 // User Operation
           }
           return 1;
    }
    return 0;
}

int pppEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // pppEntry(2.107.0) OPAQUE(48) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 PPPENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (PPPENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(PPPENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->pppProvider, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->pppUser, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->pppPassword, pParam);
                 }
           }
           return 1;
      case 1 :
           // pppProvider(2.107.1) STRING(64) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               GetPppInfo_Apn(pObject->var.stream.p);
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               SetPppInfo_Apn(pObject->var.stream.p);
           }
           return 1;
      case 2 :
           // pppUser(2.107.2) STRING(16) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               GetPppInfo_User(pObject->var.stream.p);
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               SetPppInfo_User(pValue->stream.p);
           }
           return 1;
      case 3 :
           // pppPassword(2.107.3) STRING(16) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               GetPppInfo_Password(pObject->var.stream.p);
               VARAPI_CopyValue(pObject, pValue);
           }
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               SetPppInfo_Password(pValue->stream.p);
           }
           return 1;
    }
    return 0;
}

int envEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    switch(pObject->var.oid.id3) {
      case 0 :
           // envEntry(2.108.0) OPAQUE(196) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 ENVENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (ENVENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(ENVENTRY));
                      GetEnvInfo(pEntry->envKernelVersion, pEntry->envGccVersion,
                              pEntry->envCpuName, &pEntry->envCpuMips);
                 }
           }
           return 1;
      case 1 :
           // envKernelVersion(2.108.1) STRING(64) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 2 :
           // envGccVersion(2.108.2) STRING(64) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 3 :
           // envCpuName(2.108.3) STRING(64) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
      case 4 :
           // envCpuMips(2.108.4) UINT(4) Read Only
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           return 1;
    }
    return 0;
}

int commLogEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int commLogStructQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int cmdHistEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int cmdHistStructQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int mcuEventEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int mcuEventStructQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int meterLogEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int meterLogStructQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int mobileLogEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int mobileLogStructQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int sinkEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int sinkStaticEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int codiEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    CODI_NETWORK_PAYLOAD    network;
    CODI_PERMIT_PAYLOAD        permit;
    CODI_SECURITY_PAYLOAD    security;
    CODI_MULTICASTHOP_PAYLOAD    hops;
    COORDINATOR        *pCoordinator;
    int                nLength, nError;

    switch(pObject->var.oid.id3) {
      case 0 :
           // codiEntry(3.3.0) OPAQUE(67) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 CODIENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (CODIENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(CODIENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->codiMask, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->codiIndex, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->codiID, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->codiType, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[4], (void *)&pEntry->codiShortID, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[5], (void *)&pEntry->codiFwVer, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[6], (void *)&pEntry->codiHwVer, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[7], (void *)&pEntry->codiZAIfVer, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[8], (void *)&pEntry->codiZZIfVer, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[9], (void *)&pEntry->codiFwBuild, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[10], (void *)&pEntry->codiResetKind, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[11], (void *)&pEntry->codiAutoSetting, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[12], (void *)&pEntry->codiChannel, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[13], (void *)&pEntry->codiPanID, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[14], (void *)&pEntry->codiExtPanID, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[15], (void *)&pEntry->codiRfPower, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[16], (void *)&pEntry->codiTxPowerMode, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[17], (void *)&pEntry->codiPermit, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[18], (void *)&pEntry->codiEnableEncrypt, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[19], (void *)&pEntry->codiLineKey, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[20], (void *)&pEntry->codiNetworkKey, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[21], (void *)&pEntry->codiRouteDiscovery, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[22], (void *)&pEntry->codiMulticastHops, pParam);
                 }
           }
           return 1;
      case 1 :
           // codiMask(3.3.1) WORD(2) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_SET)
           {
               VARAPI_ReplaceValue(pObject, pValue);
               VARAPI_SetModify();
           }
           return 1;
      case 2 :
           // codiIndex(3.3.2) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u8 = DEFAULT_COORDINATOR_INDEX;
           }
           else if (nMethod == VARMETHOD_SET)
               VARAPI_ReplaceValue(pObject, pValue);
           return 1;
      case 3 :
           // codiID(3.3.3) EUI64(8) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) memcpy(pValue->stream.p, &pCoordinator->eui64, sizeof(EUI64)); 
           }
           return 1;
      case 4 :
           // codiType(3.3.4) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) 
               {
                    if(pCoordinator->fw >> 4 & 0x01) 
                    {
                        pValue->stream.u8 = CODI_TYPE_ZIGBEE_STACK_30X;
                    }
                    else if(pCoordinator->fw >> 4 & 0x02) 
                    {
                        pValue->stream.u8 = CODI_TYPE_ZIGBEE_STACK_31X;
                    }
               }
           }
           return 1;
      case 5 :
           // codiShortID(3.3.5) WORD(2) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u16 = pCoordinator->shortid; 
           }
           return 1;
      case 6 :
           // codiFwVer(3.3.6) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->fw; 
           }
           return 1;
      case 7 :
           // codiHwVer(3.3.7) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->hw; 
           }
           return 1;
      case 8 :
           // codiZAIfVer(3.3.8) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->zaif; 
           }
           return 1;
      case 9 :
           // codiZZIfVer(3.3.9) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->zzif; 
           }
           return 1;
      case 10 :
           // codiFwBuild(3.3.10) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->build; 
           }
           return 1;
      case 11 :
           // codiResetKind(3.3.11) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->rk; 
           }
           return 1;
      case 12 :
           // codiAutoSetting(3.3.12) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->autoset; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               if ((pValue->stream.u8 != 0) && (pValue->stream.u8 != 1))
                    return -1;

               nError = codiGetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   network.autoset = pValue->stream.u8;
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                       return 1;
               }
               return -2;
           }
           return 1;
      case 13 :
           // codiChannel(3.3.13) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->channel; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               nError = codiGetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   network.channel = pValue->stream.u8;
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                       return 1;
               }
               return -2;
           }
           return 1;
      case 14 :
           // codiPanID(3.3.14) WORD(2) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u16 = pCoordinator->panid; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               nError = codiGetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   network.panid = ntoshort(pValue->stream.u16);
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                       return 1;
               }
               return -2;
           }
           return 1;
      case 15 :
           // codiExtPanID(3.3.15) STREAM(8) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) memcpy(pValue->stream.p, pCoordinator->extpanid, 8); 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               if (pValue->len != 8)
                    return -1;

               nError = codiGetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   memcpy(network.extpanid, pValue->stream.p, 8);
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                       return 1;
               }
               return -2;
           }
           return 1;
      case 16 :
           // codiRfPower(3.3.16) CHAR(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.s8 = pCoordinator->power; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               nError = codiGetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   network.power = pValue->stream.s8;
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                       return 1;
               }
               return -2;
           }
           return 1;
      case 17 :
           // codTxPowerMode(3.3.17) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->txpowermode; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               nError = codiGetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   network.txpowermode = pValue->stream.u8;
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_NETWORK_PARAM, (BYTE *)&network, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                       return 1;
               }
               return -2;
           }
           return 1;
      case 18 :
           // codiPermit(3.3.18) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->permit_time; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               nError = codiGetProperty(GetCoordinator(), CODI_CMD_PERMIT, (BYTE *)&permit, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   permit.permit_time = pValue->stream.u8;
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_PERMIT, (BYTE *)&permit, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                   {
                          nError = codiGetProperty(GetCoordinator(), CODI_CMD_PERMIT, (BYTE *)&permit, &nLength, 3000);
                          if (nError == CODIERR_NOERROR)
                       {
                              pCoordinator = (COORDINATOR *)GetCoordinator();
                              if (pCoordinator != NULL) pCoordinator->permit_time = permit.permit_time; 
                           if(permit.permit_time) m_nJoinNodeCount = 0;
                       }
                          VARAPI_ReplaceValue(pObject, pValue);
                       return 1;
                   }
               }
               return -2;
           }
           return 1;
      case 19 :
           // codiEnableEncrypt(3.3.19) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->security; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               nError = codiGetProperty(GetCoordinator(), CODI_CMD_SECURITY_PARAM, (BYTE *)&security, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   security.security = pValue->stream.u8;
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_SECURITY_PARAM, (BYTE *)&security, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                       return 1;
               }
               return -2;
           }
           return 1;
      case 20 :
           // codiLineKey(3.3.20) STREAM(16) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) memcpy(pValue->stream.p, pCoordinator->link_key, 16); 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               if (pValue->len != 16)
                   return -1;

               nError = codiGetProperty(GetCoordinator(), CODI_CMD_SECURITY_PARAM, (BYTE *)&security, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   memcpy(security.link_key, pValue->stream.p, 16);
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_SECURITY_PARAM, (BYTE *)&security, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                       return 1;
               }
               return -2;
           }
           return 1;
      case 21 :
           // codiNetworkKey(3.3.21) STREAM(16) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) memcpy(pValue->stream.p, &pCoordinator->network_key, 16); 
           }
           else if (nMethod == VARMETHOD_SET)
            {
               if (pValue->len != 16)
                   return -1;

               nError = codiGetProperty(GetCoordinator(), CODI_CMD_SECURITY_PARAM, (BYTE *)&security, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   memcpy(security.network_key, pValue->stream.p, 16);
                   nError = codiSetProperty(GetCoordinator(), CODI_CMD_SECURITY_PARAM, (BYTE *)&security, nLength, 3000);
                   if (nError == CODIERR_NOERROR)
                       return 1;
               }
               return -2;
           }
           return 1;
      case 22 :
           // codRouteDiscovery(3.3.22) BOOL(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->discovery; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               nError = RouteDiscoveryControl(pValue->stream.u8 ? TRUE : FALSE);
               if (nError != CODIERR_NOERROR) return -2;
           }
           return 1;
      case 23 :
           // codMulticastHops(3.3.23) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->mhops; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               pCoordinator = (COORDINATOR *)GetCoordinator();
               nError = codiGetProperty(pCoordinator, CODI_CMD_MULTICAST_HOP, (BYTE *)&hops, &nLength, 3000);
               if (nError == CODIERR_NOERROR)
               {
                   hops.hop = pValue->stream.u8;
                   if(hops.hop > 0 && hops.hop <= pCoordinator->max_hops) {
                       nError = codiSetProperty(pCoordinator, CODI_CMD_MULTICAST_HOP, (BYTE *)&hops, nLength, 3000);
                       if (nError == CODIERR_NOERROR)
                           return 1;
                   }
               }
               return -2;
           }
           return 1;
    }
    return 0;
}

int codiDeviceEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    COORDINATOR        *pCoordinator;

    switch(pObject->var.oid.id3) {
      case 0 :
           // codiDeviceEntry(3.4.0) OPAQUE(21) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 CODIDEVICEENTRY    *pEntry;
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (CODIDEVICEENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(CODIDEVICEENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->codiIndex, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->codiID, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->codiDevice, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->codiBaudRate, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[4], (void *)&pEntry->codiParityBit, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[5], (void *)&pEntry->codiDataBit, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[6], (void *)&pEntry->codiStopBit, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[7], (void *)&pEntry->codiRtsCts, pParam);
                 }
           }
           return 1;
      case 1 :
           // codiIndex(3.4.1) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u8 = DEFAULT_COORDINATOR_INDEX;
           }
           else if (nMethod == VARMETHOD_SET)
               VARAPI_ReplaceValue(pObject, pValue);
           return 1;
      case 2 :
           // codiID(3.4.2) EUI64(8) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) memcpy(pValue->stream.p, &pCoordinator->eui64, sizeof(EUI64)); 
           }
           return 1;
      case 3 :
           // codiDevice(3.4.3) STRING(16) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) memcpy(pValue->stream.p, pCoordinator->device.szDevice, 16); 
           }
           return 1;
      case 4 :
           // codiBaudRate(3.4.4) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->baudrate; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               // Not support : Do not change
               VARAPI_ReplaceValue(pObject, pValue);
           }
           return 1;
      case 5 :
           // codiParityBit(3.4.5) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->parity; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               // Not support : Do not change
               VARAPI_ReplaceValue(pObject, pValue);
           }
           return 1;
      case 6 :
           // codiDataBit(3.4.6) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->databit; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               // Not support : Do not change
               VARAPI_ReplaceValue(pObject, pValue);
           }
           return 1;
      case 7 :
           // codiStopBit(3.4.7) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->stopbit; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               // Not support : Do not change
               VARAPI_ReplaceValue(pObject, pValue);
           }
           return 1;
      case 8 :
           // codiRtsCts(3.4.8) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->rtscts; 
           }
           else if (nMethod == VARMETHOD_SET)
           {
               // Not support : Do not change
               VARAPI_ReplaceValue(pObject, pValue);
           }
           return 1;
    }
    return 0;
}

int codiBindingEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int codiNeighborEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int codiMemoryEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    COORDINATOR        *pCoordinator;

    switch(pObject->var.oid.id3) {
      case 0 :
           // codiMemoryEntry(3.7.0) OPAQUE(7) Read/Write
           if (nMethod == VARMETHOD_GET)
               VARAPI_CopyValue(pObject, pValue);
           else if (nMethod == VARMETHOD_MOVE)
           {
                 CODIMEMORYENTRY    *pEntry;

                 codiSync(GetCoordinator(), 3);
                 VARAPI_CopyValue(pObject, pValue);
                 pEntry = (CODIMEMORYENTRY *)pValue->stream.p;
                 if (pEntry != NULL)
                 {
                      memset(pEntry, 0, sizeof(CODIMEMORYENTRY));
                      VARAPI_GetIndexValue(&pObject->pChild[0], (void *)&pEntry->codiIndex, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[1], (void *)&pEntry->codiID, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[2], (void *)&pEntry->codiAddressTableSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[3], (void *)&pEntry->codiWholeAddressTableSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[4], (void *)&pEntry->codiNeighborTableSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[5], (void *)&pEntry->codiSourceRouteTableSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[6], (void *)&pEntry->codiRouteTableSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[7], (void *)&pEntry->codiMaxHops, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[8], (void *)&pEntry->codiPacketBufferCount, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[9], (void *)&pEntry->codiSoftwareVersion, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[10], (void *)&pEntry->codiKeyTableSize, pParam);
                      VARAPI_GetIndexValue(&pObject->pChild[11], (void *)&pEntry->codiMaxChildren, pParam);
                 }
           }
           return 1;
      case 1 :
           // codiIndex(3.7.1) BYTE(1) Read/Write
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pValue->stream.u8 = DEFAULT_COORDINATOR_INDEX;
           }
           else if (nMethod == VARMETHOD_SET)
               VARAPI_ReplaceValue(pObject, pValue);
           return 1;
      case 2 :
           // codiID(3.7.2) EUI64(8) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) memcpy(pValue->stream.p, &pCoordinator->eui64, sizeof(EUI64)); 
           }
           return 1;
      case 3 :
           // codiAddressTableSize(3.7.3) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->address_table_size; 
           }
           return 1;
      case 4 :
           // codiWholeAddressTableSize(3.7.4) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->whole_address_table_size; 
           }
           return 1;
      case 5 :
           // codiNeighborTableSize(3.7.5) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->neighbor_table_size; 
           }
           return 1;
      case 6 :
           // codiSourceRouteTableSize(3.7.6) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->source_route_table_size; 
           }
           return 1;
      case 7 :
           // codiRouteTableSize(3.7.7) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->route_table_size; 
           }
           return 1;
      case 8 :
           // codiMaxHops(3.7.8) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->max_hops; 
           }
           return 1;
      case 9 :
           // codiPacketBufferCount(3.7.9) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->packet_buffer_count; 
           }
           return 1;
      case 10 :
           // codiSotfwareVersion(3.7.10) WORD(2) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u16 = pCoordinator->swVersion; 
           }
           return 1;
      case 11 :
           // codiKeyTableSize(3.7.11) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->key_table_size; 
           }
           return 1;
      case 12 :
           // codiMaxChildren(3.7.12) BYTE(1) Read Only
           if (nMethod == VARMETHOD_GET)
           {
               codiSync(GetCoordinator(), 3);
               VARAPI_CopyValue(pObject, pValue);
               pCoordinator = (COORDINATOR *)GetCoordinator();
               if (pCoordinator != NULL) pValue->stream.u8 = pCoordinator->max_children; 
           }
           return 1;
    }
    return 0;
}

int codiRegisterEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int sensorConnectEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    return 0;
}

int meterDataEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    if (nMethod == VARMETHOD_GET)
        VARAPI_CopyValue(pObject, pValue);
    return 1;
}

int meterEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    if (nMethod == VARMETHOD_GET)
        VARAPI_CopyValue(pObject, pValue);
    return 1;
}

int meterLPEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    if (nMethod == VARMETHOD_GET)
        VARAPI_CopyValue(pObject, pValue);
    return 1;
}

int meterMBusEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)
{
    if (nMethod == VARMETHOD_GET)
        VARAPI_CopyValue(pObject, pValue);
    return 1;
}

