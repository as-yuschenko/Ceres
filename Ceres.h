/*MIT License

---Ceres lib---

Copyright (c) 2024 Aleksandr Yuschenko

For Contact:
https://github.com/as-yuschenko/Ceres
https://t.me/Alex_Yuschenko
mailto:a.s.yuschenko@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CERES_H_INCLUDED
#define CERES_H_INCLUDED

#define CERES_SIZE_CARD_CODE                8
#define CERES_SIZE_STATES_ARR               5
#define CERES_SIZE_DEV_NAMES_ARR            53

#define CERES_ET_NO_EVENT                   0
#define CERES_ET_NO_COMMON                  1
#define CERES_ET_ALARM                      2
#define CERES_ET_ARM                        4
#define CERES_ET_TROUBLE                    8
#define CERES_ET_RECOVERY                   16
#define CERES_ET_RELAY                      32
#define CERES_ET_ACCESS                     64
#define CERES_ET_TECH                       128

#include "Ceres_proto.h"


extern const unsigned char CERES_EVENT_TYPE_ARR[256];
extern const unsigned char CERES_EVENT_PRIORITY_ARR[256];
extern const unsigned char CERES_DEV_TYPE[CERES_SIZE_DEV_NAMES_ARR][3];


void ceres_init ();
void ceres_free ();


/*--------DEVICE--------*/
void                    ceres_q_dev_info(unsigned char* frame, int* len, unsigned char* addr_o);

char                    ceres_r_dev_info(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* type_dest, unsigned char* ver_dest);


/*--------GO SEC MODE--------*/
void                    ceres_q_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key);

char                    ceres_r_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key);


/*--------STATES--------*/
//SIMPLE
void                    ceres_q_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char zone, unsigned char* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR]);

//EXTENDED
void                    ceres_q_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char zone, unsigned char* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR]);



/*--------EVENTS--------*/
void                    ceres_q_read_event(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);

void                    ceres_q_load_event(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);



/*S2000-KDL*/
int                     ceres_09_event_type(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char* event_num_dest);

void                    ceres_09_event_access(unsigned char* frame, unsigned char* event_num_dest, unsigned char code_dest[CERES_SIZE_CARD_CODE]);

void                    ceres_09_event_relay(unsigned char* frame, unsigned char* event_num_dest, unsigned char* relay_dest, unsigned char* program_dest);

void                    ceres_09_event_common(unsigned char* frame, unsigned char* event_num_dest, unsigned char* zone_dest);

/*S2000-2*/
int                     ceres_10_event_type(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char* event_num_dest);

void                    ceres_10_event_access(unsigned char* frame, unsigned char* event_num_dest, unsigned char* reader_num_dest, unsigned char code_dest[CERES_SIZE_CARD_CODE]);

void                    ceres_10_event_relay(unsigned char* frame, unsigned char* event_num_dest, unsigned char* relay_dest, unsigned char* program_dest);

void                    ceres_10_event_common(unsigned char* frame, unsigned char* event_num_dest, unsigned char* zone_dest);



/*--------ALARMS--------*/
void                    ceres_q_drop_alarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);

char                    ceres_r_drop_alarm(unsigned char* frame, int* len, unsigned char* addr_s);


/*--------COUNTERS-------*/
void                    ceres_q_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char zone, long long unsigned int* counter_dest);



/*--------ZONES-------*/
void                    ceres_q_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char zone);

void                    ceres_q_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char zone);



/*--------RELAY-------*/
void                    ceres_q_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);

char                    ceres_r_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char relay);

void                    ceres_q_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);

char                    ceres_r_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char relay);

/*--------ACCESS MANAGEMENT--------*/
void                    ceres_q_access_unblock(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char reader_num);

char                    ceres_r_access_unblock(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char reader_num);

void                    ceres_q_access_block(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char reader_num);

char                    ceres_r_access_block(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char reader_num);

void                    ceres_q_access_reset(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char reader_num);

char                    ceres_r_access_reset(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char reader_num);

void                    ceres_q_access_test(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char reader_num, unsigned char cmd);

/*--------ADC-------*/
void                    ceres_q_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

unsigned char*          ceres_r_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s);

void                    ceres_q_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

unsigned char*          ceres_r_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s);

void                    ceres_extract_adc(unsigned char* frame, double* dest);
void                    ceres_extract_adc(unsigned char* frame, long int* dest);



#endif // CERES_H_INCLUDED





















