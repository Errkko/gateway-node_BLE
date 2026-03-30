#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

extern AlarmInfo alarmInfo;
void vAlarmReceiveTask(void* params);


// packad strukt
typedef struct __attribute__((packed))
{
    AlarmTrigger trigger;
    uint32_t time;
}AlarmInfo;


typedef enum : uint8_t
{
    NONE = 0,       // = Heartbeat
    WATER = 1,
    DOOR = 2,
    MOTION = 3,
    FIRE = 4
}AlarmTrigger;



#endif