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

void            ceres_request_transform(unsigned char* frame, int* len, unsigned char* global_key)
{
    for (int i = 3; i < *len; i++) frame[i] = frame[i] ^ /*msg key*/frame[2] ^ /*dev key*/(*global_key);
    return;
}

void            ceres_reply_transform(unsigned char* frame, int* len, unsigned char cmd)
{
    for (int i = 3; i < *len; i++) frame[i] = frame[i] ^ (/*command*/cmd ^ /*msg key*/frame[2]);
    return;
};



unsigned char ceres_counter_calc(int iteration)
{
    return (ceres_counter_table[(iteration%16)]) ^ ((((iteration%256)/16))*16);
};
