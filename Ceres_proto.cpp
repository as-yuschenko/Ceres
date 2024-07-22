#include "Ceres_proto.h"
#include <cstdlib>

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
