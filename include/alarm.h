#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H
#define ENTRY_TIMER_COUNTDOWN 30000
#include <stdint.h> 
// hämtar aktuella millisekunder sedan start
#define GET_SYS_TIME_MS() (pdTICKS_TO_MS(xTaskGetTickCount()))
#define systemTime (pdTICKS_TO_MS(xTaskGetTickCount()))

// ======= SYSTEM STATUS =======
typedef enum
{
    WAKING_UP,
    RUNNING
}RunStatus;

// ======= CONNECTION STATUS =======
typedef struct {
    bool wifiIsActive;
    bool bleIsActive;
    int bleConnHandle;          // "Hus" -> to use for SEND & RECEIVE (BLE)
    int bleRemoteWriteHandle;   // "Dörr" -> to use for SENDING alarm state (BLE)
    bool mqttIsActive;
}ConnectionStatus;

// ======= ALARM INFO::TRIGGERS =======
typedef enum : uint8_t
{
    NONE = 0,       // = Heartbeat
    WATER = 1,
    DOOR = 2,
    MOTION = 3,
    FIRE = 4
}AlarmTrigger;



typedef struct {
    int16_t inTemp;
    uint16_t inHum;
    int16_t apiTemp; // outdoor temp, from API
    uint16_t apiHum; // outdoor humidity, from API
} Climate;



// ======= ALARM STATE =======
typedef enum : uint8_t
{
    STATE_DISARMED = 0,
    STATE_ARMED_HOME = 1,
    STATE_ARMED_AWAY = 2
}AlarmState; 


// ======= ALARM INFO (Rx) =======
// packad strukt, ta emot larm. [Från Arduino]
typedef struct __attribute__((packed)) // 15-byte packat
{
    AlarmState alarmMode;    // State:   0=Disarmed, 1=ArmedHome, 2=ArmedAway                           [1 byte]
    AlarmTrigger trigger;   // Trigger: 0=None, 1=Water, 2=Door, 3=Motion, 4=Fire, 5=NodeMissing (ESP)  [1 byte]
    uint32_t time;          // Unixtime: Update for state AND trigger                                   [4 byte]
    uint8_t remoteActivete; // Remote activate alarm - from Thingsboard                                 [1 byte]
    Climate climate;        // Indoor & Outdoor (API): temp / humidity                                  [8 byte]
}AlarmInfo;

extern AlarmInfo alarmInfo;


typedef struct
{
    bool ALARM_INTRUSION;     // Larmar, inbrott
    bool ALARM_FIRE;        // Larmar, brand
    bool ALARM_WATER;
}AlarmingStatus;


// ======= WETHER API =======
typedef struct{
    float API_temp;
    float API_humid;
    char API_description[100]; 
}OpenMeteo;


// ======= SENSOR DATA =======
typedef struct
{
    float indoorTemp;
    float indoorHumidity;
    OpenMeteo wether;
}SensorData;


// =======================
// ==== STATE MACHINE ==== 
// =======================
typedef struct
{
    RunStatus runStatus;                // WAKING_UP | RUNNING --> (används ej än)
    ConnectionStatus connectionStatus;  // WiFi Active? | BLE Active? | MQTT Active?
    AlarmState systemState;             // STATE_DISARMED | STATE_ARMED_HOME | STATE_ARMED_AWAY
    AlarmingStatus alarmStatus;         // FAIL | IDLE | PENDING | ALARMING
    SensorData sensorData;              // API 'OpenWether' & DHT11
    volatile unsigned long sysTime;     // System-tiden
    bool buzzer;                        // Is buzzer: On or Off
}System;

// deklarera variabel för systemet
extern System node;


void vReceiveDataTask(void* params);
void vTransmitDataTask(void* params);
void vAlarmManageerTask(void* params);
void vAlarmManagerTask(void* params);
void vEntryDelayCallback(TimerHandle_t xTimer);
void manageAlarm();
void setAlarm();
void checkIfReset();
void checkHeartbeat();
void setAlarmState(AlarmState state);
void resetAlarm();
extern SemaphoreHandle_t xAlarmSemaphore;
extern QueueHandle_t alarmQueue;
extern QueueHandle_t stateQueue;
extern QueueHandle_t doorQueue;
extern TimerHandle_t xEntryDelayTimer;


#endif