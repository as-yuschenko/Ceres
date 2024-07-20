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

char            ceres_r_get_event_type_s2k_kdl(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, int* event_type_dest)
{
    *event_type_dest = 0; //drop event type

    if (frame[0] != *addr_s) return -1; //check addr
    if (ceres_crc_trim(frame, len)) return -1; //check crc

    ceres_base_transform(frame, len, global_key);

    if ((frame[1] < 0x0A)) // online, event not card
    {
        ceres_additional_transform(frame, len, global_key, 0x04);
        if ((frame[1] == 0x05) && (frame[4] == 0x06)) return 1; //no event
    }
    else if ((frame[1] == 0x0E)) // online, event card
    {
        ceres_additional_transform(frame, len, global_key, 0x0E);
    }
    else if ((frame[1] == 0x10)) // from buffer, event card
    {
        ceres_additional_transform(frame, len, global_key, 0x04);
    }

    else if ((frame[1] > 0x0A)) // from buffer event not card
    {
        ceres_additional_transform(frame, len, global_key, 0x0C);
    }
    else //error
    {
        //err proc
        return -1;
    }

    *event_type_dest = CERES_EVENT_TYPE_ARR[(int)(frame[3])];

    printf("\nEvent: %s\n\n", CERES_EVENT_DESC_ARR[(int)(frame[3])]);
    return 0;
};


