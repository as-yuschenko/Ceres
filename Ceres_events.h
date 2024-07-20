#ifndef CERES_EVENTS_H_INCLUDED
#define CERES_EVENTS_H_INCLUDED

#define CERES_ET_ALARM        1
#define CERES_ET_ARM          2
#define CERES_ET_TROUBLE      4
#define CERES_ET_RELAY        8
#define CERES_ET_ACCESS       16



extern const char* CERES_EVENT_DESC_ARR[256];
extern const char* CERES_EVENT_DESC_TRNSLT_ARR[256];
extern const unsigned char CERES_EVENT_TYPE_ARR[256];
#endif // CERES_EVENTS_H_INCLUDED

