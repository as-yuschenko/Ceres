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

#ifndef CERES_EVENTS_H_INCLUDED
#define CERES_EVENTS_H_INCLUDED

#define CERES_ET_COMMON       0
#define CERES_ET_ALARM        1
#define CERES_ET_ARM          2
#define CERES_ET_TROUBLE      4
#define CERES_ET_RECOVERY     8
#define CERES_ET_RELAY        16
#define CERES_ET_ACCESS       32
#define CERES_ET_TECH         64

#define CERES_ET_IS_COMMON(event) ({char ret = 0; if (event == CERES_ET_COMMON)   ret = 1; ret;})
#define CERES_ET_IS_ALARM(event) ({char ret = 0; if (event == CERES_ET_ALARM)    ret = 1; ret;})
#define CERES_ET_IS_ARM(event) ({char ret = 0; if (event == CERES_ET_ARM)      ret = 1; ret;})
#define CERES_ET_IS_TROUBLE(event) ({char ret = 0; if (event == CERES_ET_TROUBLE)  ret = 1; ret;})
#define CERES_ET_IS_RECOVERY(event) ({char ret = 0; if (event == CERES_ET_RECOVERY) ret = 1; ret;})
#define CERES_ET_IS_RELAY(event) ({char ret = 0; if (event == CERES_ET_RELAY)    ret = 1; ret;})
#define CERES_ET_IS_ACCESS(event) ({char ret = 0; if (event == CERES_ET_ACCESS)   ret = 1; ret;})
#define CERES_ET_IS_TECH(event) ({char ret = 0; if (event == CERES_ET_TECH)     ret = 1; ret;})


extern const char* CERES_EVENT_DESC_ARR[256];
extern const char* CERES_EVENT_DESC_TRNSLT_ARR[256];
extern const unsigned char CERES_EVENT_TYPE_ARR[256];

#endif // CERES_EVENTS_H_INCLUDED
