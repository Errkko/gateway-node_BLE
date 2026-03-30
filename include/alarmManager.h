#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H
typedef enum : uint8_t
{
    NONE = 0,       // = Heartbeat
    WATER = 1,
    DOOR = 2,
    MOTION = 3,
    FIRE = 4
}AlarmTrigger;

// packad strukt
typedef struct __attribute__((packed))
{
    AlarmTrigger trigger;
    uint32_t time;
}AlarmInfo;


extern AlarmInfo alarmInfo;
void vAlarmReceiveTask(void* params);


// ======= ALARM STATE =======
typedef enum
{
    STATE_DISARMED,
    STATE_ARMED_HOME,
    STATE_ARMED_AWAY
}alarm_state_t;


// ======= ALARM STATUS =======
typedef enum
{
    FAIL,       // Felläge
    IDLE,       // Normalläge
    PENDING,    // Ex. dörr öppnad - på väg att larma av (pip-ljud för uppmärksamhet) - efter 30s går larm.
    ALARMING    // Larmar!
}AlarmStatus;

#endif