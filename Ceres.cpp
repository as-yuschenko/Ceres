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

#include "Ceres.h"
#include <cstdlib>
#include <time.h>
#include <cstring>
#include <cstdio>

//unsigned char ceres_msg_key;

void ceres_init ()
{
    srand(time(nullptr));
};

void ceres_free ()
{

};


/*--DEVICE--*/
void                    ceres_q_dev_info(unsigned char* frame, int* len, unsigned char* addr_o)
{
    frame[0] = *addr_o;
    frame[1] = 0x06;
    frame[2] = 0x00;
    frame[3] = 0x0D;
    frame[4] = 0x00;
    frame[5] = 0x00;

    *len = frame[1] ;

    ceres_crc_add(frame, len);
    return;
};

char                    ceres_r_dev_info(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* type_dest, unsigned char* ver_dest)
{
    if (frame[0] != *addr_o) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    *type_dest = frame[3];
    *ver_dest = frame[4];

    return 0;
};



/*--GO SEC MODE--*/
void            ceres_q_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key)
{
    frame[0] = *addr_o;
    frame[1] = 0x06;
    frame[2] = 0x00;
    frame[3] = 0x11;
    frame[4] = *global_key;
    frame[5] = *global_key;

    *len = frame[1];

    ceres_crc_add(frame, len);
    return;
};

char            ceres_r_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key)
{
    if (frame[0] == *addr_o)
        if (!ceres_crc_trim(frame, len))
            if ((frame[3] == *global_key) && (frame[4] == *global_key))
                return 0;

    return -1;
};



/*--STATES--*/
//SIMPLE
void            ceres_q_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x19;
    frame[4] = zone;
    frame[5] = 0x00;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

char            ceres_r_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char zone, unsigned char* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR])
{
    memset(states_dest, 0x00, CERES_SIZE_STATES_ARR);
    *obtain_dest = 0;

    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x1A);

    if (frame[3] == zone)
    {
        states_dest[0] = frame[4];
        (*obtain_dest)++;

        if (frame[3] == 0x00)
        {
            states_dest[1] = frame[5];
            states_dest[2] = frame[6];
            (*obtain_dest) += 2;
        }
    }
    else return -1;

    return 0;
};


//EXTENDED
void            ceres_q_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone)
{
    frame[0] = *addr_s;
    frame[1] = 0x08;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x57;
    frame[4] = 0x02;
    frame[5] = 0x00;
    frame[6] = 0x01;
    frame[7] = zone;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

char            ceres_r_state_ext(unsigned char* frame, int* len, unsigned char* addr_s,  unsigned char zone, unsigned char* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR])
{
    memset(states_dest, 0x00, CERES_SIZE_STATES_ARR);
    *obtain_dest = 0;

    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x58);

    if (frame[4] != zone) return -1;

    if (frame[6] > CERES_SIZE_STATES_ARR) return -1;

    for (int i = 0; i < frame[6]; i++) states_dest[i] = frame[7 + i];

    *obtain_dest =frame[6];

    return 0;
};


/*--EVENTS--*/
void            ceres_q_read_event(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x01;
    frame[4] = 0x00;
    frame[5] = 0xf7;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

void            ceres_q_load_event(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x03;
    frame[4] = 0x00;
    frame[5] = 0x00;

    *len = frame[1];

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};




/*S2000-KDL*/
int              ceres_09_event_type(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char* event_num_dest)
{
    *event_num_dest = 0;                                        //drop event

    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    switch ((*global_key) ^ ceres_msg_key ^ frame[2])
    {
        case 0x02:  //no event
            return CERES_ET_NO_EVENT;
        break;

        case 0x04: // live event
        case 0x0E: // live event podbor coda
            ceres_reply_transform_2(frame, len, global_key);
            *event_num_dest = frame[3];
            return CERES_EVENT_TYPE_ARR[(int)(frame[3])];
        break;

        case 0x0C: // buffer event
            ceres_reply_transform_2(frame, len, global_key);
            *event_num_dest = frame[3];
            /*
                proc datetime
                    h  m  s  d  M   y CRC
                ... 0D 24 13 0A 0A 18 02
            */
            return CERES_EVENT_TYPE_ARR[(int)(frame[3])];
        break;

        default:
            return -1;
        break;
    }
   return -1;
};

void             ceres_09_event_access(unsigned char* frame, unsigned char* event_num_dest, unsigned char code_dest[CERES_SIZE_CARD_CODE])
{
    *event_num_dest = frame[3];
    for (int i = 13; i > 3; i--) code_dest[13-i] = frame[i];
    return;
};

void             ceres_09_event_relay(unsigned char* frame, unsigned char* event_num_dest, unsigned char* relay_dest, unsigned char* program_dest)
{
    *event_num_dest = frame[3];
    *relay_dest = frame[4];
    *program_dest = frame[5];
    return;
};

void             ceres_09_event_common(unsigned char* frame, unsigned char* event_num_dest, unsigned char* zone_dest)
{
    *event_num_dest = frame[3];
    *zone_dest = frame[4];
    return;
};


/*S2000-2*/
int              ceres_10_event_type(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char* event_num_dest)
{
    *event_num_dest = 0;                                        //drop event

    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    switch ((*global_key) ^ ceres_msg_key ^ frame[2])
    {
        case 0x02:  //no event
            return CERES_ET_NO_EVENT;
        break;

        case 0x04: // live event
        case 0x0E: // live event
            ceres_reply_transform_2(frame, len, global_key);
            *event_num_dest = frame[3];
            return CERES_EVENT_TYPE_ARR[(int)(frame[3])];
        break;

        case 0x0C: // buffer event
            ceres_reply_transform_2(frame, len, global_key);
            *event_num_dest = frame[3];
            /*
                proc datetime
                    h  m  s  d  M   y CRC
                ... 0D 24 13 0A 0A 18 02
            */
            return CERES_EVENT_TYPE_ARR[(int)(frame[3])];
        break;

        default:
            return -1;
        break;
    }
   return -1;
};

void             ceres_10_event_access(unsigned char* frame, unsigned char* event_num_dest, unsigned char* reader_num_dest, unsigned char code_dest[CERES_SIZE_CARD_CODE])
{
    *event_num_dest = frame[3];
    *reader_num_dest = frame[4];
    for (int i = 13; i > 5; i--) code_dest[13-i] = frame[i];
    return;
};

void             ceres_10_event_relay(unsigned char* frame, unsigned char* event_num_dest, unsigned char* relay_dest, unsigned char* program_dest)
{
    *event_num_dest = frame[3];
    *relay_dest = frame[4];
    *program_dest = frame[5];
    return;
};

void             ceres_10_event_common(unsigned char* frame, unsigned char* event_num_dest, unsigned char* zone_dest)
{
    *event_num_dest = frame[3];
    *zone_dest = frame[4];
    return;
};;


/*ALARMS*/
void            ceres_q_drop_alarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x17;
    frame[4] = 0x00;
    frame[5] = 0x01;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

char            ceres_r_drop_alarm(unsigned char* frame, int* len, unsigned char* addr_s)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x18);

    if ((frame[3] == 0x00) && (frame[4] == 0x01)) return 0;

    return -1;
};



/*--------COUNTERS-------*/
void            ceres_q_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone)
{
    frame[0] = *addr_s;
    frame[1] = 0x08;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x57;
    frame[4] = 0x07;
    frame[5] = 0x00;
    frame[6] = 0x00;
    frame[7] = zone;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
}

char            ceres_r_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char zone, long long unsigned int* counter_dest)
{
    *counter_dest = 0;

    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len,  0x58);

    if (frame[4] != zone) return -1;

    for (int i = 12; i >= 7; i--)
    {
        (*counter_dest) |= frame[i];
        if (i > 7) (*counter_dest) <<= 8;
    }

    return 0;
};



/*--------ZONES-------*/
void             ceres_q_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x13;
    frame[4] = zone;
    frame[5] = 0x02;                //program num

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;

};

char             ceres_r_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char zone)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x14);

    if ((frame[3] == zone) && (frame[4] == 0x02)) return 0;

    return -1;
};

void             ceres_q_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x13;
    frame[4] = zone;
    frame[5] = 0x00;                //program num

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;

};

char             ceres_r_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char zone)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x14);

    if ((frame[3] == zone) && (frame[4] == 0x00)) return 0;

    return -1;
};



/*--------RELAY-------*/
void             ceres_q_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x15;
    frame[4] = relay;
    frame[5] = 0x01;                //program num

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;

};

char             ceres_r_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char relay)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x16);

    if ((frame[3] == relay) && (frame[4] == 0x01)) return 0;

    return -1;
};

void             ceres_q_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x15;
    frame[4] = relay;
    frame[5] = 0x02;                //program num

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;

};

char             ceres_r_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char relay)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x16);

    if ((frame[3] == relay) && (frame[4] == 0x02)) return 0;

    return -1;
};


/*--------ACCESS MANAGEMENT--------*/
void ceres_q_access_unblock(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char reader_num)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x23;
    frame[4] = 0x07; //open lock
    frame[5] = reader_num;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

char ceres_r_access_unblock(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char reader_num)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x24);

    if ((frame[3] == 0x07) && (frame[4] == reader_num)) return 0;

    return -1;
}

void ceres_q_access_block(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char reader_num)
{

    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x23;
    frame[4] = 0x04; //block reader
    frame[5] = reader_num;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

char ceres_r_access_block(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char reader_num)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x24);

    if ((frame[3] == 0x04) && (frame[4] == reader_num)) return 0;

    return -1;
}

void ceres_q_access_reset(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char reader_num)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x23;
    frame[4] = 0x01; //reset
    frame[5] = reader_num;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

char ceres_r_access_reset(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char reader_num)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_reply_transform(frame, len, 0x24);

    if ((frame[3] == 0x01) && (frame[4] == reader_num)) return 0;

    return -1;
}

void                    ceres_q_access_test(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char reader_num, unsigned char cmd)
{

    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x23;
    frame[4] = cmd; //0x01; //reset 0x04; //block reader 0x07; //open lock
    frame[5] = reader_num;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
}

/*--------ADC-------*/
void             ceres_q_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone)
{
    frame[0] = *addr_s;
    frame[1] = 0x06;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x39;
    frame[4] = zone;
    frame[5] = 0x0A;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

unsigned char*   ceres_r_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s)
{
    if (frame[0] != *addr_s) return nullptr;                         //check addr
    if (ceres_crc_trim(frame, len)) return nullptr;                  //check crc

    ceres_reply_transform(frame, len, 0x3A);

    frame[*len] = 0x00;

    return frame + 4;
};

void             ceres_q_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone)
{
    frame[0] = *addr_s;
    frame[1] = 0x08;
    frame[2] = ceres_msg_keygen();
    frame[3] = 0x57;
    frame[4] = 0x04;
    frame[5] = 0x05;
    frame[6] = 0x00;
    frame[7] = zone;

    *len = frame[1] ;

    ceres_request_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

unsigned char*   ceres_r_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s)
{
    if (frame[0] != *addr_s) return nullptr;                         //check addr
    if (ceres_crc_trim(frame, len)) return nullptr;                  //check crc

    ceres_reply_transform(frame, len, 0x58);

    frame[*len] = 0x00;

    return frame + 7;
};

void             ceres_extract_adc(unsigned char* frame, double* dest)
{
    int l = 0;
    int factor = 1;
    char* pos = nullptr;
    for (unsigned int p = 3; p < strlen((const char*)(frame)); p++)
    {
        if (((frame[p] >= 0x30) && (frame[p] <= 0x39)) || (frame[p] == 0x2E) || (frame[p] == 0x2C))
        {
            if (l == 0)
            {
                pos = (char*)(frame + p);
                if (frame[p - 1] == 0x2D) factor = -1;
            }
            if ((frame[p] == 0x2C)) frame[p] = 0x2E;

            l++;
        }
    }
    if (pos) pos[l] = 0x00;

    *dest = strtod(pos, nullptr) * factor;
    return;
}
void             ceres_extract_adc(unsigned char* frame, long int* dest)
{
    int l = 0;
    int factor = 1;
    char* pos = nullptr;
    for (unsigned int p = 3; p < strlen((const char*)(frame)); p++)
    {
        if ((frame[p] >= 0x30) && (frame[p] <= 0x39))
        {
            if (l == 0)
            {
                pos = (char*)(frame + p);
                if (frame[p - 1] == 0x2D) factor = -1;
            }

            l++;
        }
    }

    if (pos) pos[l] = 0x00;

    *dest = strtol(pos, nullptr, 10) * factor;
    return;
}


const unsigned char CERES_EVENT_TYPE_ARR[256] =
{
    /*00,0,Не доступно*/                        1,
    /*01,1,Восстановление сети 220В*/       CERES_ET_RECOVERY,
    /*02,2,Авария сети 220В*/               CERES_ET_TROUBLE,
    /*03,3,Тревога проникновения*/          CERES_ET_ALARM,
    /*04,4,Помеха*/                         CERES_ET_TROUBLE,
    /*05,5,Отключение звука*/                   1,
    /*06,6,Помеха устранена*/                   1,
    /*07,7,Ручн. вкл исп-го. устр-ва*/          1,
    /*08,8,Ручн. выкл исп-го. устр-ва*/         1,
    /*09,9,Активация УДП*/                      1,
    /*0A,10,Восстановление УДП*/                1,
    /*0B,11,*/                                  1,
    /*0C,12,*/                                  1,
    /*0D,13,*/                                  1,
    /*0E,14,Подбор кода*/                   CERES_ET_ACCESS,
    /*0F,15,Дверь открыта*/                 CERES_ET_ACCESS,
    /*10,16,*/                                  1,
    /*11,17,Неудачное взятие*/                  1,
    /*12,18,Предъявлен код принуждения*/        1,
    /*13,19,Тест извещателя*/                   1,
    /*14,20,Вход в режим тестирования*/         1,
    /*15,21,Выход из реж. тестирования*/        1,
    /*16,22,Восстановление контроля*/           1,
    /*17,23,Задержка взятия*/               CERES_ET_ARM,
    /*18,24,Взятие входа на охрану*/        CERES_ET_ARM,
    /*19,25,Доступ закрыт*/                 CERES_ET_ACCESS,
    /*1A,26,Доступ отклонен*/               CERES_ET_ACCESS,
    /*1B,27,Дверь взломана*/                CERES_ET_ACCESS,
    /*1C,28,Доступ предоставлен*/           CERES_ET_ACCESS,
    /*1D,29,Запрет доступа*/                CERES_ET_ACCESS,
    /*1E,30,Восстановление доступа*/        CERES_ET_ACCESS,
    /*1F,31,Дверь закрыта*/                 CERES_ET_ACCESS,
    /*20,32,Проход*/                        CERES_ET_ACCESS,
    /*21,33,Дверь заблокирована*/           CERES_ET_ACCESS,
    /*22,34,Идентификация*/                 CERES_ET_ACCESS,
    /*23,35,Восст.  техн.  входа*/              1,
    /*24,36,Нарушение  техн. входа*/            1,
    /*25,37,Пожар*/                         CERES_ET_ALARM,
    /*26,38,Нарушение 2го техн. входа*/         1,
    /*27,39,Восст. нормы оборудования*/     CERES_ET_RECOVERY,
    /*28,40,Пожар 2*/                       CERES_ET_ALARM,
    /*29,41,Неиспр.  оборудования*/         CERES_ET_TROUBLE,
    /*2A,42,Неизвестное устройство*/        CERES_ET_TROUBLE,
    /*2B,43,Предупреждение*/                    1,
    /*2C,44,Внимание!*/                     CERES_ET_ALARM,
    /*2D,45,Обрыв  входа*/                  CERES_ET_TROUBLE,
    /*2E,46,Обрыв ДПЛС*/                    CERES_ET_TROUBLE,
    /*2F,47,Восстановление ДПЛС*/           CERES_ET_RECOVERY,
    /*30,48,Сработка 2-го датчика*/             1,
    /*31,49,*/                                  1,
    /*32,50,*/                                  1,
    /*33,51,*/                                  1,
    /*34,52,*/                                  1,
    /*35,53,*/                                  1,
    /*36,54,*/                                  1,
    /*37,55,*/                                  1,
    /*38,56,*/                                  1,
    /*39,57,*/                                  1,
    /*3A,58,Тихая тревога*/                 CERES_ET_ALARM,
    /*3B,59,*/                                  1,
    /*3C,60,*/                                  1,
    /*3D,61,Сброс конфигурации*/                1,
    /*3E,62,Изменение конфигурации*/            1,
    /*3F,63,*/                                  1,
    /*40,64,*/                                  1,
    /*41,65,*/                                  1,
    /*42,66,*/                                  1,
    /*43,67,Изменение даты*/                    1,
    /*44,68,*/                                  1,
    /*45,69,Журнал заполнен*/                   1,
    /*46,70,Журнал переполнен*/                 1,
    /*47,71,Понижение уровня*/              CERES_ET_TROUBLE,   //С2000-ВТ+
    /*48,72,Норма уровня*/                  CERES_ET_RECOVERY,  //С2000-ВТ+
    /*49,73,Изменение времени*/                                  1,
    /*4A,74,Повышение уровня*/              CERES_ET_TROUBLE,   //С2000-ВТ+
    /*4B,75,Аварийное повышение уровня*/    CERES_ET_TROUBLE,
    /*4C,76,Повышение температуры*/         CERES_ET_TROUBLE,   //С2000-ВТ+
    /*4D,77,Аварийное понижение уровня*/    CERES_ET_TROUBLE,
    /*4E,78,Температура в норме*/           CERES_ET_RECOVERY,  //С2000-ВТ+
    /*4F,79,Тревога затопления*/            CERES_ET_ALARM,
    /*50,80,Восст. датчика затопления*/     CERES_ET_RECOVERY,
    /*51,81,*/                                  1,
    /*52,82,Неисправность термометра*/      CERES_ET_TROUBLE,
    /*53,83,Восстановление термометра*/     CERES_ET_RECOVERY,
    /*54,84,Начало  лок. программ-ния*/         1,
    /*55,85,*/                                  1,
    /*56,86,*/                                  1,
    /*57,87,*/                                  1,
    /*58,88,*/                                  1,
    /*59,89,*/                                  1,
    /*5A,90,Неисправность канала связи*/    CERES_ET_TROUBLE,
    /*5B,91,Восст. канала связи*/           CERES_ET_RECOVERY,
    /*5C,92,*/                                  1,
    /*5D,93,*/                                  1,
    /*5E,94,*/                                  1,
    /*5F,95,*/                                  1,
    /*60,96,*/                                  1,
    /*61,97,*/                                  1,
    /*62,98,*/                                  1,
    /*63,99,*/                                  1,
    /*64,100,*/                                 1,
    /*65,101,*/                                 1,
    /*66,102,*/                                 1,
    /*67,103,*/                                 1,
    /*68,104,*/                                 1,
    /*69,105,*/                                 1,
    /*6A,106,*/                                 1,
    /*6B,107,*/                                 1,
    /*6C,108,*/                                 1,
    /*6D,109,Снятие  входа  с  охраны*/     CERES_ET_ARM,
    /*6E,110,Сброс  тревоги*/                   1,
    /*6F,111,Включение ШС*/                     1,
    /*70,112,Отключение ШС*/                    1,
    /*71,113,Включение выхода*/                 1,
    /*72,114,Отключение выхода*/                1,
    /*73,115,*/                                 1,
    /*74,116,*/                                 1,
    /*75,117,Восст. снятого входа*/         CERES_ET_TECH,
    /*76,118,Тревога входа*/                CERES_ET_ALARM,
    /*77,119,Нарушение снятого входа*/      CERES_ET_TECH,
    /*78,120,*/                                 1,
    /*79,121,Обрыв выхода*/                 CERES_ET_TROUBLE,
    /*7A,122,КЗ выхода*/                    CERES_ET_TROUBLE,
    /*7B,123,Восстановление выхода*/        CERES_ET_RECOVERY,
    /*7C,124,*/                                 1,
    /*7D,125,*/                                 1,
    /*7E,126,Потеря  связи  с  выходом*/    CERES_ET_TROUBLE,
    /*7F,127,Восст.  связи  с  выходом*/    CERES_ET_RECOVERY,
    /*80,128,Изменение состояния выхода*/   CERES_ET_RELAY,
    /*81,129,*/                                 1,
    /*82,130,Включение  насоса*/                1,
    /*83,131,Выключение  насоса*/               1,
    /*84,132,*/                                 1,
    /*85,133,*/                                 1,
    /*86,134,*/                                 1,
    /*87,135,Ошибка при авт. тестир-нии*/       1,
    /*88,136,*/                                 1,
    /*89,137,Пуск*/                             1,
    /*8A,138,Неудачный пуск*/                   1,
    /*8B,139,Неудачный пуск ОТВ*/               1,
    /*8C,140,Тест*/                             1,
    /*8D,141,Задержка  пуска  АУП*/             1,
    /*8E,142,Автоматика АУП выключена*/         1,
    /*8F,143,Отмена пуска АУП*/                 1,
    /*90,144,Тушение*/                          1,
    /*91,145,Аварийный  пуск  АУП*/             1,
    /*92,146,Пуск  АУП*/                        1,
    /*93,147,Блокировка  пуска  АУП*/           1,
    /*94,148,Автоматика  АУП включена*/         1,
    /*95,149,Взлом  корпуса  прибора*/      CERES_ET_TROUBLE,
    /*96,150,Пуск речевого оповещения*/         1,
    /*97,151,Отмена  пуска  РО*/                1,
    /*98,152,Восст. корпуса прибора*/       CERES_ET_RECOVERY,
    /*99,153,ИУ в рабочем состоянии*/           1,
    /*9A,154,ИУ в исходном состоянии*/          1,
    /*9B,155,Отказ ИУ*/                         1,
    /*9C,156,Ошибка ИУ*/                        1,
    /*9D,157,Сброс задержки*/                   1,
    /*9E,158,Восст. внутренней зоны*/           1,
    /*9F,159,Задержка пуска РО*/                1,
    /*A0,160,*/                                 1,
    /*A1,161,Останов задержки пуска АУП*/       1,
    /*A2,162,Увеличение задержки пуска*/        1,
    /*A3,163,*/                                 1,
    /*A4,164,Саботаж*/                          1,
    /*A5,165,Ошибка параметров входа*/          1,
    /*A6,166,*/                                 1,
    /*A7,167,*/                                 1,
    /*A8,168,*/                                 1,
    /*A9,169,*/                                 1,
    /*AA,170,*/                                 1,
    /*AB,171,*/                                 1,
    /*AC,172,*/                                 1,
    /*AD,173,*/                                 1,
    /*AE,174,*/                                 1,
    /*AF,175,*/                                 1,
    /*B0,176,*/                                 1,
    /*B1,177,*/                                 1,
    /*B2,178,*/                                 1,
    /*B3,179,*/                                 1,
    /*B4,180,*/                                 1,
    /*B5,181,*/                                 1,
    /*B6,182,*/                                 1,
    /*B7,183,*/                                 1,
    /*B8,184,*/                                 1,
    /*B9,185,*/                                 1,
    /*BA,186,Требуется замена батареи*/     CERES_ET_TROUBLE,
    /*BB,187,Потеря связи со входом*/       CERES_ET_TROUBLE,
    /*BC,188,Восст. связи со входом*/       CERES_ET_RECOVERY,
    /*BD,189,Потеря связи по ДПЛС1*/        CERES_ET_TROUBLE,
    /*BE,190,Потеря связи по ДПЛС2*/        CERES_ET_TROUBLE,
    /*BF,191,Восст. связи по ДПЛС1*/        CERES_ET_RECOVERY,
    /*C0,192,Откл. вых. напряжения*/        CERES_ET_TROUBLE,
    /*C1,193,Подкл.  вых.  напряжения*/     CERES_ET_RECOVERY,
    /*C2,194,Перегрузка ист. пит.*/         CERES_ET_TROUBLE,
    /*C3,195,Перегрузка ист. пит. устр.*/   CERES_ET_RECOVERY,
    /*C4,196,Неиспр. зарядного уст-ва*/     CERES_ET_TROUBLE,
    /*C5,197,Восст. зарядного уст-ва*/      CERES_ET_RECOVERY,
    /*C6,198,Неиспр. источника питания*/    CERES_ET_TROUBLE,
    /*C7,199,Восст.  источника  питания*/   CERES_ET_RECOVERY,
    /*C8,200,Восст. батареи*/               CERES_ET_RECOVERY,
    /*C9,201,Восст.  связи по ДПЛС2*/       CERES_ET_RECOVERY,
    /*CA,202,Неисправность батареи*/        CERES_ET_TROUBLE,
    /*CB,203,Перезапуск прибора*/           CERES_ET_TECH,
    /*CC,204,Требуется обслуживание*/       CERES_ET_TECH,
    /*CD,205,Ошибка теста АКБ*/             CERES_ET_TROUBLE,
    /*CE,206,Понижение температуры*/        CERES_ET_TROUBLE,   //С2000-ВТ+
    /*CF,207,*/                                 1,
    /*D0,208,*/                                 1,
    /*D1,209,*/                                 1,
    /*D2,210,*/                                 1,
    /*D3,211,Батарея разряжена*/            CERES_ET_TROUBLE,
    /*D4,212,Разряд резервной батареи*/     CERES_ET_TROUBLE,
    /*D5,213,Восст. резервной батареи*/     CERES_ET_RECOVERY,
    /*D6,214,Короткое замыкание входа*/     CERES_ET_TROUBLE,
    /*D7,215,Короткое замыкание ДПЛС*/      CERES_ET_TROUBLE,
    /*D8,216,Сработка  датчика*/                1,
    /*D9,217,Отключение ветви RS-485*/      CERES_ET_TROUBLE,
    /*DA,218,Восст. ветви RS-485*/          CERES_ET_RECOVERY,
    /*DB,219,Доступ открыт*/                CERES_ET_ACCESS,
    /*DC,220,Срабатывание СДУ*/                 1,
    /*DD,221,Отказ СДУ*/                    CERES_ET_TROUBLE,
    /*DE,222,Повышение напряжения ДПЛС*/    CERES_ET_TROUBLE,
    /*DF,223,Отметка наряда*/                   1,
    /*E0,224,Некорр. ответ устр. в ДПЛС*/   CERES_ET_TROUBLE,
    /*E1,225,Неустойч. ответ устр. в ДПЛС*/ CERES_ET_TROUBLE,
    /*E2,226,*/                                 1,
    /*E3,227,*/                                 1,
    /*E4,228,*/                                 1,
    /*E5,229,Задержка пуска*/                   1,
    /*E6,230,Останов задержки пуска*/           1,
    /*E7,231,Остановка*/                        1,
    /*E8,232,Увеличение задержки пуска*/        1,
    /*E9,233,Сброс задержки пуска*/             1,
    /*EA,234,*/                                 1,
    /*EB,235,*/                                 1,
    /*EC,236,*/                                 1,
    /*ED,237,Раздел снят по принужд.*/          1,
    /*EE,238,*/                                 1,
    /*EF,239,*/                                 1,
    /*F0,240,*/                                 1,
    /*F1,241,Раздел взят*/                  CERES_ET_ARM,
    /*F2,242,Раздел снят*/                  CERES_ET_ARM,
    /*F3,243,Удал. запрос на взятие*/       CERES_ET_ARM,
    /*F4,244,Удал. запрос на снятие*/       CERES_ET_ARM,
    /*F5,245,*/                                 1,
    /*F6,246,*/                                 1,
    /*F7,247,*/                                 1,
    /*F8,248,*/                                 1,
    /*F9,249,Оконч. лок. программирования*/     1,
    /*FA,250,Потеряна связь сприбором*/         1,
    /*FB,251,Восст. связь сприбором*/           1,
    /*FC,252,Подмена прибора*/                  1,
    /*FD,253,Включение пульта С2000М*/          1,
    /*FE,254,Отметка времени - сутки*/          1,
    /*FF,255,Отметка времени - час*/            1
};

const unsigned char CERES_EVENT_PRIORITY_ARR[256] =
{
    /*00,0,Не доступно*/                        0,
    /*01,1,Восстановление сети 220В*/           150,
    /*02,2,Авария сети 220В*/                   199,
    /*03,3,Тревога проникновения*/              238,
    /*04,4,Помеха*/                             222,
    /*05,5,Отключение звука*/                   132,
    /*06,6,Помеха устранена*/                   144,
    /*07,7,Ручн. вкл исп-го. устр-ва*/          101,
    /*08,8,Ручн. выкл исп-го. устр-ва*/         100,
    /*09,9,Активация УДП*/                      178,
    /*0A,10,Восстановление УДП*/                165,
    /*0B,11,*/                                  0,
    /*0C,12,*/                                  0,
    /*0D,13,*/                                  0,
    /*0E,14,Подбор кода*/                       102,
    /*0F,15,Дверь открыта*/                     175,
    /*10,16,*/                                  0,
    /*11,17,Неудачное взятие*/                  204,
    /*12,18,Предъявлен код принуждения*/        239,
    /*13,19,Тест извещателя*/                   0,
    /*14,20,Вход в режим тестирования*/         106,
    /*15,21,Выход из реж. тестирования*/        105,
    /*16,22,Восстановление контроля*/           137,
    /*17,23,Задержка взятия*/                   183,
    /*18,24,Взятие входа на охрану*/            174,
    /*19,25,Доступ закрыт*/                     154,
    /*1A,26,Доступ отклонен*/                   131,
    /*1B,27,Дверь взломана*/                    211,
    /*1C,28,Доступ предоставлен*/               129,
    /*1D,29,Запрет доступа*/                    130,
    /*1E,30,Восстановление доступа*/            151,
    /*1F,31,Дверь закрыта*/                     168,
    /*20,32,Проход*/                            128,
    /*21,33,Дверь заблокирована*/               210,
    /*22,34,Идентификация*/                     127,
    /*23,35,Восст.  техн.  входа*/              162,
    /*24,36,Нарушение  техн. входа*/            163,
    /*25,37,Пожар*/                             243,
    /*26,38,Нарушение 2го техн. входа*/         164,
    /*27,39,Восст. нормы оборудования*/         161,
    /*28,40,Пожар 2*/                           244,
    /*29,41,Неиспр.  оборудования*/             218,
    /*2A,42,Неизвестное устройство*/            103,
    /*2B,43,Предупреждение*/                    241,
    /*2C,44,Внимание!*/                         242,
    /*2D,45,Обрыв  входа*/                      221,
    /*2E,46,Обрыв ДПЛС*/                        205,
    /*2F,47,Восстановление ДПЛС*/               147,
    /*30,48,Сработка 2-го датчика*/             93,
    /*31,49,*/                                  0,
    /*32,50,*/                                  0,
    /*33,51,*/                                  0,
    /*34,52,*/                                  0,
    /*35,53,*/                                  0,
    /*36,54,*/                                  0,
    /*37,55,*/                                  0,
    /*38,56,*/                                  0,
    /*39,57,*/                                  0,
    /*3A,58,Тихая тревога*/                     240,
    /*3B,59,*/                                  0,
    /*3C,60,*/                                  0,
    /*3D,61,Сброс конфигурации*/                92,
    /*3E,62,Изменение конфигурации*/            91,
    /*3F,63,*/                                  0,
    /*40,64,*/                                  0,
    /*41,65,*/                                  0,
    /*42,66,*/                                  0,
    /*43,67,Изменение даты*/                    126,
    /*44,68,*/                                  0,
    /*45,69,Журнал заполнен*/                   125,
    /*46,70,Журнал переполнен*/                 124,
    /*47,71,Понижение уровня*/                  177,   //С2000-ВТ+
    /*48,72,Норма уровня*/                      170,  //С2000-ВТ+
    /*49,73,Изменение времени*/                 123,
    /*4A,74,Повышение уровня*/                  181,   //С2000-ВТ+
    /*4B,75,Аварийное повышение уровня*/        215,
    /*4C,76,Повышение температуры*/             180,   //С2000-ВТ+
    /*4D,77,Аварийное понижение уровня*/        214,
    /*4E,78,Температура в норме*/               169,  //С2000-ВТ+
    /*4F,79,Тревога затопления*/                236,
    /*50,80,Восст. датчика затопления*/         173,
    /*51,81,*/                                  0,
    /*52,82,Неисправность термометра*/          217,
    /*53,83,Восстановление термометра*/         159,
    /*54,84,Начало  лок. программ-ния*/         122,
    /*55,85,*/                                  0,
    /*56,86,*/                                  0,
    /*57,87,*/                                  0,
    /*58,88,*/                                  0,
    /*59,89,*/                                  0,
    /*5A,90,Неисправность канала связи*/        0,      //depricated in s2000pp ver 3.00
    /*5B,91,Восст. канала связи*/               0,      //depricated in s2000pp ver 3.00
    /*5C,92,*/                                  0,
    /*5D,93,*/                                  0,
    /*5E,94,*/                                  0,
    /*5F,95,*/                                  0,
    /*60,96,*/                                  0,
    /*61,97,*/                                  0,
    /*62,98,*/                                  0,
    /*63,99,*/                                  0,
    /*64,100,*/                                 0,
    /*65,101,*/                                 0,
    /*66,102,*/                                 0,
    /*67,103,*/                                 0,
    /*68,104,*/                                 0,
    /*69,105,*/                                 0,
    /*6A,106,*/                                 0,
    /*6B,107,*/                                 0,
    /*6C,108,*/                                 0,
    /*6D,109,Снятие  входа  с  охраны*/         185,
    /*6E,110,Сброс  тревоги*/                   121,
    /*6F,111,Включение ШС*/                     139,
    /*70,112,Отключение ШС*/                    134,
    /*71,113,Включение выхода*/                 138,
    /*72,114,Отключение выхода*/                133,
    /*73,115,*/                                 0,
    /*74,116,*/                                 0,
    /*75,117,Восст. снятого входа*/             184,
    /*76,118,Тревога входа*/                    237,
    /*77,119,Нарушение снятого входа*/          186,
    /*78,120,*/                                 0,
    /*79,121,Обрыв выхода*/                     213,
    /*7A,122,КЗ выхода*/                        212,
    /*7B,123,Восстановление выхода*/            158,
    /*7C,124,*/                                 0,
    /*7D,125,*/                                 0,
    /*7E,126,Потеря  связи  с  выходом*/        226,
    /*7F,127,Восст.  связи  с  выходом*/        142,
    /*80,128,Изменение состояния выхода*/       97,
    /*81,129,*/                                 0,
    /*82,130,Включение  насоса*/                182,
    /*83,131,Выключение  насоса*/               172,
    /*84,132,*/                                 0,
    /*85,133,*/                                 0,
    /*86,134,*/                                 0,
    /*87,135,Ошибка при авт. тестир-нии*/       208,
    /*88,136,*/                                 0,
    /*89,137,Пуск*/                             250,
    /*8A,138,Неудачный пуск*/                   231,
    /*8B,139,Неудачный пуск ОТВ*/               245,
    /*8C,140,Тест*/                             120,
    /*8D,141,Задержка  пуска  АУП*/             252,
    /*8E,142,Автоматика АУП выключена*/         189,
    /*8F,143,Отмена пуска АУП*/                 235,
    /*90,144,Тушение*/                          255,
    /*91,145,Аварийный  пуск  АУП*/             254,
    /*92,146,Пуск  АУП*/                        253,
    /*93,147,Блокировка  пуска  АУП*/           190,
    /*94,148,Автоматика  АУП включена*/         171,
    /*95,149,Взлом  корпуса  прибора*/          194,
    /*96,150,Пуск речевого оповещения*/         251,
    /*97,151,Отмена  пуска  РО*/                234,
    /*98,152,Восст. корпуса прибора*/           146,
    /*99,153,ИУ в рабочем состоянии*/           179,
    /*9A,154,ИУ в исходном состоянии*/          166,
    /*9B,155,Отказ ИУ*/                         229,
    /*9C,156,Ошибка ИУ*/                        223,
    /*9D,157,Сброс задержки*/                   98,
    /*9E,158,Восст. внутренней зоны*/           160,
    /*9F,159,Задержка пуска РО*/                249,
    /*A0,160,*/                                 0,
    /*A1,161,Останов задержки пуска АУП*/       247,
    /*A2,162,Увеличение задержки пуска*/        90,
    /*A3,163,*/                                 0,
    /*A4,164,Саботаж*/                          95,
    /*A5,165,Ошибка параметров входа*/          219,
    /*A6,166,*/                                 0,
    /*A7,167,*/                                 0,
    /*A8,168,*/                                 0,
    /*A9,169,*/                                 0,
    /*AA,170,*/                                 0,
    /*AB,171,*/                                 0,
    /*AC,172,*/                                 0,
    /*AD,173,*/                                 0,
    /*AE,174,*/                                 0,
    /*AF,175,*/                                 0,
    /*B0,176,*/                                 0,
    /*B1,177,*/                                 0,
    /*B2,178,*/                                 0,
    /*B3,179,*/                                 0,
    /*B4,180,*/                                 0,
    /*B5,181,*/                                 0,
    /*B6,182,*/                                 0,
    /*B7,183,*/                                 0,
    /*B8,184,*/                                 0,
    /*B9,185,*/                                 0,
    /*BA,186,Требуется замена батареи*/         193,
    /*BB,187,Потеря связи со входом*/           227,
    /*BC,188,Восст. связи со входом*/           143,
    /*BD,189,Потеря связи по ДПЛС1*/            192,
    /*BE,190,Потеря связи по ДПЛС2*/            191,
    /*BF,191,Восст. связи по ДПЛС1*/            136,
    /*C0,192,Откл. вых. напряжения*/            203,
    /*C1,193,Подкл.  вых.  напряжения*/         157,
    /*C2,194,Перегрузка ист. пит.*/             202,
    /*C3,195,Перегрузка ист. пит. устр.*/       156,
    /*C4,196,Неиспр. зарядного уст-ва*/         201,
    /*C5,197,Восст. зарядного уст-ва*/          155,
    /*C6,198,Неиспр. источника питания*/        200,
    /*C7,199,Восст.  источника  питания*/       152,
    /*C8,200,Восст. батареи*/                   149,
    /*C9,201,Восст.  связи по ДПЛС2*/           135,
    /*CA,202,Неисправность батареи*/            198,
    /*CB,203,Перезапуск прибора*/               117,
    /*CC,204,Требуется обслуживание*/           216,
    /*CD,205,Ошибка теста АКБ*/                 197,
    /*CE,206,Понижение температуры*/            176,   //С2000-ВТ+
    /*CF,207,*/                                 0,
    /*D0,208,*/                                 0,
    /*D1,209,*/                                 0,
    /*D2,210,*/                                 0,
    /*D3,211,Батарея разряжена*/                196,
    /*D4,212,Разряд резервной батареи*/         195,
    /*D5,213,Восст. резервной батареи*/         148,
    /*D6,214,Короткое замыкание входа*/         220,
    /*D7,215,Короткое замыкание ДПЛС*/          207,
    /*D8,216,Сработка  датчика*/                94,
    /*D9,217,Отключение ветви RS-485*/          145,
    /*DA,218,Восст. ветви RS-485*/              116,
    /*DB,219,Доступ открыт*/                    0,      //depricated in s2000pp ver 3.00
    /*DC,220,Срабатывание СДУ*/                 187,
    /*DD,221,Отказ СДУ*/                        232,
    /*DE,222,Повышение напряжения ДПЛС*/        206,
    /*DF,223,Отметка наряда*/                   104,
    /*E0,224,Некорр. ответ устр-ва в ДПЛС*/     225,
    /*E1,225,Неустойч. ответ устр-ва в ДПЛС*/   224,
    /*E2,226,*/                                 0,
    /*E3,227,*/                                 0,
    /*E4,228,*/                                 0,
    /*E5,229,Задержка пуска*/                   248,
    /*E6,230,Останов задержки пуска*/           246,
    /*E7,231,Остановка*/                        233,
    /*E8,232,Увеличение задержки пуска*/        88,
    /*E9,233,Сброс задержки пуска*/             89,
    /*EA,234,*/                                 0,
    /*EB,235,*/                                 0,
    /*EC,236,*/                                 0,
    /*ED,237,Раздел снят по принужд.*/          113,
    /*EE,238,*/                                 0,
    /*EF,239,*/                                 0,
    /*F0,240,*/                                 0,
    /*F1,241,Раздел взят*/                      115,
    /*F2,242,Раздел снят*/                      114,
    /*F3,243,Удал. запрос на взятие*/           0,
    /*F4,244,Удал. запрос на снятие*/           0,
    /*F5,245,*/                                 0,
    /*F6,246,*/                                 0,
    /*F7,247,*/                                 0,
    /*F8,248,*/                                 0,
    /*F9,249,Оконч. лок. программирования*/     96,
    /*FA,250,Потеряна связь сприбором*/         228,
    /*FB,251,Восст. связь сприбором*/           141,
    /*FC,252,Подмена прибора*/                  230,
    /*FD,253,Включение пульта С2000М*/          110,
    /*FE,254,Отметка времени - сутки*/          109,
    /*FF,255,Отметка времени - час*/            108
};

const unsigned char CERES_DEV_TYPE[CERES_SIZE_DEV_NAMES_ARR][3] =
{
    {1, 20, 0},
    {2, 20, 5},
    {3, 0, 4},
    {4, 4, 2},
    {7, 0, 0},
    {8, 0, 0},
    {9, 127, 0},
    {10, 0, 0},
    {11, 20, 5},
    {13, 0, 0},
    {14, 6, 0},
    {15, 2, 6},
    {16, 2, 2},
    {19, 0, 0},
    {20, 4, 1},
    {21, 26, 0},
    {22, 0, 0},
    {24, 4, 0},
    {25, 47, 0},
    {26, 20, 5},
    {28, 0, 0},
    {30, 0, 1},
    {31, 134, 0},
    {33, 5, 0},
    {34, 110, 4},
    {36, 0, 0},
    {38, 5, 0},
    {39, 5, 0},
    {41, 127, 0},
    {43, 0, 0},
    {44, 0, 0},
    {45, 0, 0},
    {46, 3, 1},
    {47, 200, 0},
    {48, 6, 0},
    {49, 5, 0},
    {53, 5, 0},
    {54, 6, 0},
    {55, 5, 0},
    {59, 2, 4},
    {61, 127, 0},
    {66, 5, 5},
    {67, 7, 1},
    {76, 0, 0},
    {78, 5, 0},
    {79, 6, 0},
    {80, 5, 0},
    {81, 127, 0},
    {82, 10, 4},
    {85, 22, 0},
    {87, 27, 0},
    {88, 6, 0},
    {89, 5, 0},
};




















