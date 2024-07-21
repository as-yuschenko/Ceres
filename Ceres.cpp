#include "Ceres.h"
#include <cstdlib>
#include <time.h>
#include <cstring>
#include <cstdio>
#include "Ceres_events.h"

void ceres_init ()
{
    srand(time(nullptr));
};

void ceres_free ()
{

};


/*--------CRC--------*/
unsigned char   ceres_crc_calc(unsigned char* frame, int* len)
{
    unsigned char crc = 0;
    for (int i = 0; i < *len; ++i) crc = ceres_crc_table[crc ^ frame[i]];
    return crc;
};

void            ceres_crc_add (unsigned char* frame, int* len)
{
    frame[*len] = ceres_crc_calc(frame, len);
    (*len)++;
    return;
};

char            ceres_crc_trim (unsigned char* frame, int* len)
{
    (*len)--;
    if (frame[(*len)] == ceres_crc_calc(frame, len)) return 0;
    else (*len)++;
    return -1;
};


/*--------CIPHER--------*/
unsigned char   ceres_msg_keygen()
{
    return ceres_crc_table[rand()%256];
};

void            ceres_base_transform(unsigned char* frame, int* len, unsigned char* global_key)
{
    for (int i = 3; i < *len; i++) frame[i] = frame[i] ^ /*msg key*/frame[2] ^ /*dev key*/(*global_key);
    return;
}

void            ceres_additional_transform(unsigned char* frame, int* len, unsigned char* global_key, unsigned char cmd)
{
    for (int i = 3; i < *len; i++) frame[i] = frame[i] ^ (/*command*/cmd ^ /*dev key*/(*global_key));
    return;
};

unsigned char   ceres_sniff_dev_key(unsigned char* frame, int* len, unsigned char addr)
{
    if (!ceres_crc_trim(frame, len))
    {
        if ((addr ^ 0x80) == frame[0])
        {
            if (frame [1] == 0x06)
            {
                for (unsigned char k = 0x00; k <= 0xff; k++)
                {
                    if  ((frame[2] ^ frame[3] ^ k) == 0x01)
                    {
                        if  ((frame[2] ^ frame[4] ^ k) == 0x00)
                        {
                            if  ((frame[2] ^ frame[5] ^ k) == 0xf7)
                            {
                                return k;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
};


/*--------POLLING--------*/

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

    ceres_base_transform(frame, len, global_key);

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

    ceres_base_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};




/*S2000-KDL*/
char             ceres_s2k_kdl_event_type(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, int* event_type_dest)
{
    *event_type_dest = 0;                                       //drop event type

    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_base_transform(frame, len, global_key);

    if ((frame[1] < 0x0A))                                      // online, event is not access
    {
        ceres_additional_transform(frame, len, global_key, 0x04);
        if ((frame[1] == 0x05) && (frame[4] == 0x06)) return 1; //no event
    }
    else if ((frame[1] == 0x0E))                                // online, event is access
    {
        ceres_additional_transform(frame, len, global_key, 0x0E);
    }
    else if ((frame[1] == 0x10))                                // from buffer, event is access
    {
        ceres_additional_transform(frame, len, global_key, 0x04);
    }

    else if ((frame[1] > 0x0A))                                 // from buffer event is not access
    {
        ceres_additional_transform(frame, len, global_key, 0x0C);
    }
    else                                                        //error
    {
        //err proc
        return -1;
    }

    *event_type_dest = CERES_EVENT_TYPE_ARR[(int)(frame[3])];

    return 0;
};

void             ceres_s2k_kdl_event_access(unsigned char* frame, unsigned char* event_dest, unsigned char code_dest[CERES_SIZE_CARD_CODE])
{
    *event_dest = frame[3];
    for (int i = 13; i > 3; i--) code_dest[13-i] = frame[i];
    return;
};

void             ceres_s2k_kdl_event_relay(unsigned char* frame, unsigned char* event_dest, unsigned char* relay_dest, unsigned char* program_dest)
{
    *event_dest = frame[3];
    *relay_dest = frame[4];
    *program_dest = frame[5];
    return;
};

void             ceres_s2k_kdl_event_common(unsigned char* frame, unsigned char* event_dest, unsigned char* zone_dest)
{
    *event_dest = frame[3];
    *zone_dest = frame[4];
    return;
};

void            ceres_s2k_kdl_q_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone)
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

    ceres_base_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
}

char            ceres_s2k_kdl_r_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, long long unsigned int* counter_dest)
{
    *counter_dest = 0;

    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_base_transform(frame, len, global_key);
    ceres_additional_transform(frame, len, global_key, 0x58);

    if (frame[4] != zone) return -1;

    for (int i = 12; i >= 7; i--)
    {
        (*counter_dest) |= frame[i];
        if (i > 7) (*counter_dest) <<= 8;
    }

    return 0;
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

    ceres_base_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;

};

char             ceres_r_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_base_transform(frame, len, global_key);
    ceres_additional_transform(frame, len, global_key, 0x16);

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

    ceres_base_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;

};

char             ceres_r_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_base_transform(frame, len, global_key);
    ceres_additional_transform(frame, len, global_key, 0x16);

    if ((frame[3] == relay) && (frame[4] == 0x02)) return 0;

    return -1;
};


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

    ceres_base_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

char             ceres_r_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_base_transform(frame, len, global_key);
    ceres_additional_transform(frame, len, global_key, 0x3A);

    frame[*len] = 0x00;

    return 0;
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

    ceres_base_transform(frame, len, global_key);

    ceres_crc_add(frame, len);
    return;
};

char             ceres_r_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key)
{
    if (frame[0] != *addr_s) return -1;                         //check addr
    if (ceres_crc_trim(frame, len)) return -1;                  //check crc

    ceres_base_transform(frame, len, global_key);
    ceres_additional_transform(frame, len, global_key, 0x58);

    frame[*len] = 0x00;

    return 0;
};

void             ceres_extract_adc(unsigned char* frame, int* len, double* dest)
{
    int l = 0;
    int factor = 1;
    char* pos = nullptr;
    for (int p = 3; p < *len; p++)
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
void             ceres_extract_adc(unsigned char* frame, int* len, long int* dest)
{
    int l = 0;
    int factor = 1;
    char* pos = nullptr;
    for (int p = 3; p < *len; p++)
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
























