# Ceres
## Назначение
**Опрос и управление устройствами системы "Орион" компании Болид.**
## Реализованый в библиотеке функционал в настоящее время позволяет:

+ Получать информацию о событиях приборов;
+ По запросу получать общее или расширенное состояние шлейфов, выходов и приборов;
+ Управлять взятием и снятием зон с охраны;
+ Управлять включением и выключением реле;
+ Получать данные счетчиков;
+ Получать данные о температуре, влажности и т.д.;
+ Получать коды проксимити-карт при событиях СКУД;
+ Получать информацию о типах и версиях приборов.

Обмен данными с приборами компании Болид производится по "зашифрованному" протоколу "Орион".
Для работы библиотеки требуется буфер, который вы можете создать любым, удобным вам способом. Рекомендуемый размер буфера - 100 байт. 

Применяемые термины:

Открытый адрес - адрес устройства на шине "Орион", определяемый для устройств через UProg.

Закрытый адрес - адрес устройства используемый для работы по протоколу "Орион". Закрытый адрес формируется из открытого поднятием старшего бита открытого адреса. Т.к. максимальное количество адресов 127 (0x7F), просто выполняем XOR с 0x80. Например: о.а. = 2, тогда з.а. = 0x02^0x80 = 0x82, или о.а. = 78, тогда з.а. = 0x4E^0x80 = 0xCE.

Глобальный ключ - ключ, размерностью 1 байт, устанавливаемый для каждого устройства, для работы по протоколу "Орион". Глобальный ключ, при необходимости, можно менять в процессе опроса. 

Ниже приведено описание функций и констант библиотеки.
   
### Общее для всех функций.
Во всех функциях, использующих буфер:

```cpp 
	unsigned char* frame	// указатель на буфер
	int* len				// указатель на переменную, хранящую количество принятых или подготовленных для передачи, после формирования запроса, байт.
```
Для всех функций, формирующих запросы или производящих анализ ответов:

```cpp 
	addr_o						//указатель на открытый адрес
	addr_s						//указатель на закрытый адрес
	unsigned char* global_key	//указатель на глобаный ключ
```

Внимание! После запроса переход на зашифрованный протокол обмена "Орион", необходимо использовать закрытый адрес. 

Функции, начинающиеся с "ceres_q_", формируют последовательность байт запроса и помещают его в буфер [frame], при этом в переменную [len] сохраняется количество байт, подготовленных к передаче.

Функции, начинающиеся с "ceres_r_", проверяют достоверность полученных данных и, за некоторым исключением, сохраняют по переданным указателям запрошенные данные.  
  

### Получения типа и версии устройства.

```cpp
    /** Запрос для получения информации о устройстве.
    * @return void
    */
    void	ceres_q_dev_info(unsigned char* frame, int* len, unsigned char* addr_o);

	
    /** Анализ ответа на запрос для получения информации о устройстве.
    * @param unsigned char* type_dest 	- указатель на переменную для сохранения типа устройства.
    * @param unsigned char* ver_dest 	- указатель на переменную для сохранения версии устройства.
    * @return
    * 			0	- успешно.
    * 			-1 	- ошибка принятых данных.
    */
	char	ceres_r_dev_info(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* type_dest, unsigned char* ver_dest);
```

### Переход на зашифрованный протокол обмена "Орион".

```cpp
    /** Запрос на переход на зашифрованный протокол обмена.
    * @return void
    */
	void	ceres_q_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key);

	
    /** Анализ ответа на запрос на переход на зашифрованный протокол обмена.
    * @return
    * 			0	- успешно.
    * 			-1 	- ошибка принятых данных.
    */
	char	ceres_r_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key);
```

### Общее состояние шлейфа, выхода или прибора.

Для получения состояния(й) необходимо создать буфер размером 5 байт, или использовать значение, определенное в CERES_SIZE_STATES_ARR. Полуенные значения будут сохранены в этот буфер, а в переменную obtain_dest будет сохранено количество полученных состояний. Для получения состояния прибора необходимо передать zone равным нулю.

Внимание! В любом случае буфер states_dest и obtain_dest сначала заполняются нулями.  

```cpp
    /** Запрос на получение общего состояния шлейфа, выхода или прибора.
    * @param unsigned char zone		- номер шлейфа или выхода.
    * @return void
    */
	void ceres_q_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

	
    /** Анализ ответа на запрос на получение общего состояния шлейфа, выхода или прибора.
    * @param unsigned char zone		- номер шлейфа или выхода.
    * @param int* obtain_dest		- указатель на переменную для сохранения количества полученных состояний.
    * @param unsigned char states_dest[CERES_SIZE_STATES_ARR]	- указатель на буфер для сохранения состояний.
    * @return
    * 			0	- успешно.
    * 			-1 	- ошибка принятых данных.
    */
	char ceres_r_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, int* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR]);
```

### Расширенное состояние шлейфа, выхода или прибора.

Для получения состояния(й) необходимо создать (использовать созданный ранее) буфер размером 5 байт, или использовать значение, определенное в CERES_SIZE_STATES_ARR. Полуенные значения будут сохранены в этот буфер, а в переменную obtain_dest будет сохранено количество полученных состояний. Для получения состояния прибора необходимо передать zone равным нулю.

Внимание! В любом случае буфер states_dest и obtain_dest сначала заполняются нулями.  

```cpp
    /** Запрос на получение расширенного состояния шлейфа, выхода или прибора.
    * @param unsigned char zone		- номер шлейфа или выхода.
    * @return void
    */
	void ceres_q_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

	
    /** Анализ ответа на запрос на получение расширенного состояния шлейфа, выхода или прибора.
    * @param unsigned char zone		- номер шлейфа или выхода.
    * @param int* obtain_dest		- указатель на переменную для сохранения количества полученных состояний.
    * @param unsigned char states_dest[CERES_SIZE_STATES_ARR]	- указатель на буфер для сохранения состояний.
    * @return
    * 			0	- успешно.
    * 			-1 	- ошибка принятых данных.
    */
	char ceres_r_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, int* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR]);
```

### Получение события прибора.

Для получения события прибора необходимо 3 этапа, в отличие от других запросов.

+ Сформировать запрос; 
+ Получив ответ, с помощью функции ceres_xxx_event_type(...), где ххх - тип прибора, получить тип события и код события для дальнейшей обработки;
+ Обработать ответ в зависимости от типа события и получить полезные данные.  

```cpp
    /** Запрос на получение события прибора.
    * @return void
    */
	void ceres_q_read_event(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);

	
    /** Анализ ответа на запрос на получение события прибора.
    * @param unsigned char zone		- номер шлейфа или выхода.
    * @param int* obtain_dest		- указатель на переменную для сохранения количества полученных состояний.
    * @param unsigned char states_dest[CERES_SIZE_STATES_ARR]	- указатель на буфер для сохранения состояний.
    * @return
    * 			0	- успешно.
    * 			-1 	- ошибка принятых данных.
    */
	char ceres_r_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, int* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR]);
```

### Взятие шлейфа под охрану.

```cpp
    /** Запрос на взятие шлейфа под охрану.
    * @param unsigned char zone		- номер шлейфа.
    * @return void
    */
	void	ceres_q_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

	
    /** Анализ ответа на запрос на взятие шлейфа под охрану.
    * @param unsigned char zone		- номер шлейфа или выхода.
    * @return
    * 			0	- успешно.
    * 			-1 	- ошибка принятых данных.
    */
	char ceres_r_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);
```

### Снятие шлейфа с охраны.

```cpp
    /** Запрос на снятие шлейфа с охраны.
    * @param unsigned char zone		- номер шлейфа.
    * @return void
    */
	void	ceres_q_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

	
    /** Анализ ответа на запрос на снятие шлейфа с охраны.
    * @param unsigned char zone		- номер шлейфа.
    * @return
    * 			0	- успешно.
    * 			-1 	- ошибка принятых данных.
    */
	char ceres_r_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);
```

### Включить реле (активировать выход).

```cpp
    /** Запрос на включение реле.
    * @param unsigned char relay		- номер реле (выхода).
    * @return void
    */
	void ceres_q_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);

	
    /** Анализ ответа на запрос на включение реле.
    * @param unsigned char relay		- номер реле (выхода).
    * @return
    * 			0	- успешно.
    * 			-1 	- ошибка принятых данных.
    */
	char ceres_r_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);
```

### Выключить реле (деактивировать выход).

```cpp
    /** Запрос на выключение реле.
    * @param unsigned char relay		- номер реле (выхода).
    * @return void
    */
	void ceres_q_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);

	
    /** Анализ ответа на запрос на выключение реле.
    * @param unsigned char relay		- номер реле (выхода).
    * @return
    * 			0	- успешно.
    * 			-1 	- ошибка принятых данных.
    */
	char ceres_r_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);
```

### Запрос информации о шлейфе в Unicode строку.

Данные запросы применяется для получения информации:

+ о температуре и влажности получаемых от С2000-ВТ,  
+ о напряжении, токе и т.д. от РИП12-RS,
+ и т.д.
  
```cpp

	/*Версия 1*/
	
    /** Запрос информации о шлейфе в Unicode строку.
    * @param unsigned char zone		- номер шлейфа.
    * @return void
    */
	void ceres_q_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

	
    /** Анализ ответа на запрос информации о шлейфе в Unicode строку.
    * @return
    * 			Указатель на строку данных, в случае успеха.
    * 			null 	- ошибка принятых данных.
    */
	unsigned char* ceres_r_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);
	
	
	/*Версия 2*/
	
	/** Запрос информации о шлейфе в Unicode строку.
    * @param unsigned char zone		- номер шлейфа.
    * @return void
    */
	void ceres_q_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

	
    /** Анализ ответа на запрос информации о шлейфе в Unicode строку.
    * @return
    * 			Указатель на строку данных, в случае успеха.
    * 			null 	- ошибка принятых данных.
    */
	unsigned char* ceres_r_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);
```

Для удобства реализованы 2 функции для извлечения значений типа long int и double из полученной Unicode строки.

```cpp

	/** Извлекает знаковое значение типа long int из Unicode строки.
    * @param long int* dest		- указатель на переменную для сохранения значения.
    * @return void
    */
	void ceres_extract_adc(unsigned char* frame, int* len, long int* dest);
	
	/** Извлекает значение типа double из Unicode строки.
    * @param double* dest		- указатель на переменную для сохранения значения.
    * @return void
    */
	void ceres_extract_adc(unsigned char* frame, int* len, double* dest);
```






















































