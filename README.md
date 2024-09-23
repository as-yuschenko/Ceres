# Ceres ![](./Ceres.png?raw=true)
# Опрос и управление устройствами системы "Орион" компании Болид.

## Реализованный в библиотеке функционал в настоящее время позволяет:

+ Получать информацию о событиях приборов;
+ По запросу получать общее или расширенное состояние шлейфов, выходов и приборов;
+ Управлять взятием и снятием зон с охраны;
+ Управлять включением и выключением реле;
+ Получать данные счетчиков;
+ Получать данные о температуре, влажности и т.д.;
+ Получать коды проксимити-карт при событиях СКУД;
+ Получать информацию о типах и версиях приборов.

Обмен данными с приборами компании Болид производится по "зашифрованному" протоколу "Орион". 

## Для связи:
https://t.me/vsyst_ru <br />
mailto:a.s.yuschenko@gmail.com <br />

## MAN
Для работы библиотеки требуется буфер, который вы можете создать любым, удобным вам способом. Рекомендуемый размер буфера - 100 байт. 

Библиотека разделена на две части: функциональную и текстовую. 

Протокол обмена реализован в файлах Ceres.h, Ceres.cpp, Ceres_proto.h, Ceres_proto.cpp

Текстовая часть, содержащая имена событий на Кириллице и в транслите, а так же названия приборов, реализована в файлах Ceres_names.h и Ceres_names.php  

Применяемые термины:

**Открытый адрес** - адрес устройства на шине "Орион", определяемый для устройств через UProg.

**Закрытый адрес** - адрес устройства используемый для работы по протоколу "Орион". Закрытый адрес формируется из открытого поднятием старшего бита открытого адреса. Т.к. максимальное количество адресов 127 (0x7F), просто выполняем XOR с 0x80. Например: о.а. = 2, тогда з.а. = 0x02^0x80 = 0x82, или о.а. = 78, тогда з.а. = 0x4E^0x80 = 0xCE.

**Глобальный ключ** - ключ, размерностью 1 байт, устанавливаемый для каждого устройства, для работы по протоколу "Орион". Глобальный ключ, при необходимости, можно менять в процессе опроса. 

Ниже приведено описание функций и констант библиотеки.

## Константы.

### Размеры буферов.

**CERES_SIZE_CARD_CODE** определяет размер буфера для хранения кода проксимити-карты.
**CERES_SIZE_STATES_ARR** определяет размер буфера для хранения кодов состояния шлейфов.

### Приборы.

**CERES_SIZE_DEV_NAMES_ARR** определяет размер буфера для хранения имён и свойств приборов.
**CERES_DEV_TYPE[CERES_SIZE_DEV_NAMES_ARR][3]** массив содержащий свойства приборов согласно С2000-М вер. 4.14
+ [0] Тип прибора;
+ [1] Количество шлейфов;
+ [2] Количество реле.
**CERES_DEV_NAMES[CERES_SIZE_DEV_NAMES_ARR]** массив содержащий названия приборов согласно С2000-М вер. 4.14

### События.

**CERES_EVENT_DESC_ARR[256]** массив содержащий имена событий на кириллице.
**CERES_EVENT_DESC_TRNSLT_ARR[256]** массив содержащий транслит имен событий на латинице.
**CERES_EVENT_TYPE_ARR[256]** массив содержащий привязку события к определенным константами **CERES_ET_...** типам.

### Типы событий.

**CERES_ET_COMMON** общее событие

**CERES_ET_ALARM** событие тревога

**CERES_ET_ARM** событие взятие / снятие

**CERES_ET_TROUBLE** событие неисправность

**CERES_ET_RECOVERY** событие восстановление

**CERES_ET_RELAY** событие реле (выхода)

**CERES_ET_ACCESS** событие СКУД

**CERES_ET_TECH** событие техлоногическое


 
### Общее для всех функций.
Во всех функциях, использующих буфер:

```cpp 
unsigned char* frame // указатель на буфер
int* len // указатель на переменную, хранящую количество принятых или подготовленных для передачи, после формирования запроса, байт.
```
Для всех функций, формирующих запросы или производящих анализ ответов:

```cpp 
unsigned char* addr_o //указатель на открытый адрес
unsigned char* addr_s //указатель на закрытый адрес
unsigned char* global_key //указатель на глобаный ключ
```

**Внимание!** После запроса переход на зашифрованный протокол обмена "Орион", необходимо использовать закрытый адрес. 

Функции, начинающиеся с **"ceres_q_"**, формируют последовательность байт запроса и помещают его в буфер [frame], при этом в переменную [len] сохраняется количество байт, подготовленных к передаче.

Функции, начинающиеся с **"ceres_r_"**, проверяют достоверность полученных данных и, за некоторым исключением, сохраняют по переданным указателям запрошенные данные.  
  

## Получения типа и версии устройства.

```cpp
/** Запрос для получения информации о устройстве.
* @return void
*/
void ceres_q_dev_info(unsigned char* frame, int* len, unsigned char* addr_o);


/** Анализ ответа на запрос для получения информации о устройстве.
* @param unsigned char* type_dest - указатель на переменную для сохранения типа устройства.
* @param unsigned char* ver_dest - указатель на переменную для сохранения версии устройства.
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_dev_info(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* type_dest, unsigned char* ver_dest);
```

## Переход на зашифрованный протокол обмена "Орион".

```cpp
/** Запрос на переход на зашифрованный протокол обмена.
* @return void
*/
void ceres_q_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key);


/** Анализ ответа на запрос на переход на зашифрованный протокол обмена.
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key);
```

## Общее состояние шлейфа, выхода или прибора.

Для получения состояния(й) необходимо создать буфер размером 5 байт, или использовать значение, определенное в **CERES_SIZE_STATES_ARR**. Полученные значения будут сохранены в этот буфер, а в переменную obtain_dest будет сохранено количество полученных состояний. Для получения состояния прибора необходимо передать zone равным нулю.

**Внимание!** В любом случае буфер states_dest и obtain_dest сначала заполняются нулями.  

```cpp
/** Запрос на получение общего состояния шлейфа, выхода или прибора.
* @param unsigned char zone	- номер шлейфа или выхода.
* @return void
*/
void ceres_q_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);


/** Анализ ответа на запрос на получение общего состояния шлейфа, выхода или прибора.
* @param unsigned char zone	- номер шлейфа или выхода.
* @param int* obtain_dest - указатель на переменную для сохранения количества полученных состояний.
* @param unsigned char states_dest[CERES_SIZE_STATES_ARR] - указатель на буфер для сохранения состояний.
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, int* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR]);
```

## Расширенное состояние шлейфа, выхода или прибора.

Для получения состояния(й) необходимо создать (использовать созданный ранее) буфер размером 5 байт, или использовать значение, определенное в **CERES_SIZE_STATES_ARR**. Полученные значения будут сохранены в этот буфер, а в переменную obtain_dest будет сохранено количество полученных состояний. Для получения состояния прибора необходимо передать zone равным нулю.

**Внимание!** В любом случае буфер states_dest и obtain_dest сначала заполняются нулями.

```cpp
/** Запрос на получение расширенного состояния шлейфа, выхода или прибора.
* @param unsigned char zone - номер шлейфа или выхода.
* @return void
*/
void ceres_q_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);


/** Анализ ответа на запрос на получение расширенного состояния шлейфа, выхода или прибора.
* @param unsigned char zone - номер шлейфа или выхода.
* @param int* obtain_dest - указатель на переменную для сохранения количества полученных состояний.
* @param unsigned char states_dest[CERES_SIZE_STATES_ARR] - указатель на буфер для сохранения состояний.
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, int* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR]);
```

## Получение события прибора.

Для получения события прибора необходимо более 2-х шагов, в отличие от других запросов.

1. Сформировать запрос на чтение события с помощью функции ceres_q_read_event(...) и отправить его прибору; 

2. Получив ответ, с помощью функции ceres_xxx_event_type(...), где ххх - тип прибора, получить тип события, определяемый константами CERES_ET_..., и код события для дальнейшей обработки или значение, говорящее что событий нет;

3.1. В случае если событие есть - обработать ответ в зависимости от типа события и типа прибра, получив полезные данные. Сформировать запрос на загрузку следующего события с помощью функции ceres_q_load_event(...) и отправить его прибору. Внимание! Ответа от прибора не будет. Если ведется опрос одного прибора рекомендуется небольшой тайм-аут.

3.2. Если события нет, переходим к пункту 1.


**Чтение события прибора.**

```cpp
/** Запрос на чтение события прибора.
* @return void
*/
void ceres_q_read_event(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);
```


**Загрузка следующего события прибора.**

```cpp
/** Запрос на загрузку следующего события прибора.
* @return void
*/
void ceres_q_load_event(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);
```


### Обработка событий для С2000-КДЛ:

**Получение типа и кода события.**

```cpp
/** Анализ ответа на запрос на чтение события прибора, получение типа и кода события.
* @param int* event_type_dest - указатель на переменную для сохранения типа события.
* @param unsigned char* event_dest - указатель на переменную для сохранения кода события.
* @return
* 0 - данные обработаны, есть событие.
* 1 - данные обработаны, событий нет.
* -1 - ошибка принятых данных.
*/
char ceres_09_event_type(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, int* event_type_dest, unsigned char* event_dest);
```

**Получение данных о событии типа CERES_ET_ACCESS.**

Для получения кода проксимити-карты необходимо создать (использовать созданный ранее) буфер размером 10 байт, или использовать значение, определенное в **CERES_SIZE_CARD_CODE**. Полученный код проксимити-карты будет сохранен в этот буфер.

```cpp
/** Получение данных о событии типа CERES_ET_ACCESS.
* @param unsigned char code_dest[CERES_SIZE_CARD_CODE] - указатель на буфер для сохранения кода проксимити-карты.
* @param unsigned char* event_dest - указатель на переменную для сохранения кода события.
* @return
* void
*/
void ceres_09_event_access(unsigned char* frame, unsigned char* event_dest, unsigned char code_dest[CERES_SIZE_CARD_CODE]);
```

**Получение данных о событии типа CERES_ET_RELAY.**

```cpp
/** Получение данных о событии типа CERES_ET_RELAY.
* @param unsigned char* relay_dest - указатель на переменную для сохранения номера реле (выхода).
* @param unsigned char* program_dest - указатель на переменную для сохранения номера программы управления.
* @param unsigned char* event_dest - указатель на переменную для сохранения кода события.
* @return
* void
*/
void ceres_09_event_relay(unsigned char* frame, unsigned char* event_dest, unsigned char* relay_dest, unsigned char* program_dest);
```

**Получение данных о простых событиях.**

```cpp
/** Получение данных о простых событиях.
* @param unsigned char* zone_dest - указатель на переменную для сохранения номера зоны.
* @param unsigned char* event_dest - указатель на переменную для сохранения кода события.
* @return
* void
*/
void ceres_09_event_common(unsigned char* frame, unsigned char* event_dest, unsigned char* zone_dest);
```

## Сброс тревог.

```cpp
/** Запрос сброса тревоги.
* @return void
*/
void ceres_q_drop_alarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);


/** Анализ ответа на запрос сброса тревоги.
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_drop_alarm(unsigned char* frame, int* len, unsigned char* addr_s);
```

## Взятие шлейфа под охрану.

```cpp
/** Запрос на взятие шлейфа под охрану.
* @param unsigned char zone - номер шлейфа.
* @return void
*/
void ceres_q_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);


/** Анализ ответа на запрос на взятие шлейфа под охрану.
* @param unsigned char zone - номер шлейфа или выхода.
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);
```

## Снятие шлейфа с охраны.

```cpp
/** Запрос на снятие шлейфа с охраны.
* @param unsigned char zone - номер шлейфа.
* @return void
*/
void ceres_q_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);


/** Анализ ответа на запрос на снятие шлейфа с охраны.
* @param unsigned char zone - номер шлейфа.
* @return
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);
```

## Включить реле (активировать выход).

```cpp
/** Запрос на включение реле.
* @param unsigned char relay - номер реле (выхода).
* @return void
*/
void ceres_q_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);


/** Анализ ответа на запрос на включение реле.
* @param unsigned char relay- номер реле (выхода).
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);
```

## Выключить реле (деактивировать выход).

```cpp
/** Запрос на выключение реле.
* @param unsigned char relay - номер реле (выхода).
* @return void
*/
void ceres_q_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);


/** Анализ ответа на запрос на выключение реле.
* @param unsigned char relay - номер реле (выхода).
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);
```

## Запрос информации о шлейфе в Unicode строку.

Данные запросы применяется для получения информации:

+ о температуре и влажности получаемых от С2000-ВТ,  
+ о напряжении, токе и т.д. от РИП12-RS,
+ и т.д.
  
```cpp

/*Версия 1*/

/** Запрос информации о шлейфе в Unicode строку.
* @param unsigned char zone - номер шлейфа.
* @return void
*/
void ceres_q_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);


/** Анализ ответа на запрос информации о шлейфе в Unicode строку.
* @return
* Указатель на строку данных, в случае успеха.
* null - ошибка принятых данных.
*/
unsigned char* ceres_r_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s);


/*Версия 2*/

/** Запрос информации о шлейфе в Unicode строку.
* @param unsigned char zone - номер шлейфа.
* @return void
*/
void ceres_q_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);


/** Анализ ответа на запрос информации о шлейфе в Unicode строку.
* @return
* Указатель на строку данных, в случае успеха.
* null - ошибка принятых данных.
*/
unsigned char* ceres_r_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s);
```

Для удобства реализованы 2 функции для извлечения значений типа long int и double из полученной Unicode строки.

```cpp
/** Извлекает знаковое значение типа long int из Unicode строки.
* @param unsigned char* frame - Нуль-терминированная С-строка, после обработки ответа функциями ceres_r_adc_v1(_v2).
* @param long int* dest - указатель на переменную для сохранения значения.
* @return void
*/
void ceres_extract_adc(unsigned char* frame, long int* dest);

/** Извлекает значение типа double из Unicode строки.
* @param unsigned char* frame - Нуль-терминированная С-строка, после обработки ответа функциями ceres_r_adc_v1(_v2).
* @param double* dest - указатель на переменную для сохранения значения.
* @return void
*/
void ceres_extract_adc(unsigned char* frame, double* dest);
```

## Запрос данных счетчика.

```cpp
/** Запрос данных счетчика.
* @param unsigned char zone - адрес счетчика (номер зоны).
* @return void
*/
void ceres_q_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);


/** Анализ ответа на запрос данных счетчика.
* @param unsigned char zone - адрес счетчика (номер зоны).
* @param long long unsigned int* - указатель на переменную для сохранения значения счетчика.
* @return
* 0 - успешно.
* -1 - ошибка принятых данных.
*/
char ceres_r_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, long long unsigned int* counter_dest);
```

## Текстовые значения.

Внимание! Необходимо подключить заголовочный файл "Ceres_names.h"

```cpp
/** Получение строки с названием прибора. Кириллица.
* @param unsigned char event - тип прибора.
* @return 
* Указатель на строку данных.
*/
const char* ceres_name_dev(unsigned char type);

/** Получение строки с именем события. Кириллица.
* @param unsigned char event - код события.
* @return 
* Указатель на строку данных.
*/
const char* ceres_name_event(unsigned char event);

/** Получение строки с именем события. Латиница. Транслит.
* @param unsigned char event - код события.
* @return 
* Указатель на строку данных.
*/
const char* ceres_name_event_trnslt(unsigned char event);
```
