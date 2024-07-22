#ifndef CERES_H_INCLUDED
#define CERES_H_INCLUDED


#define CERES_SIZE_CARD_CODE                10
#define CERES_SIZE_STATES_ARR               5

void ceres_begin ();
void ceres_free ();


/*--DEVICE--*/
void                    ceres_q_dev_info(unsigned char* frame, int* len, unsigned char* addr_o);

char                    ceres_r_dev_info(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* type_dest, unsigned char* ver_dest);


/*--GO SEC MODE--*/
void                    ceres_q_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key);

char                    ceres_r_sec_begin(unsigned char* frame, int* len, unsigned char* addr_o, unsigned char* global_key);


/*--STATES--*/
//SIMPLE
void                    ceres_q_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_state_simp(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, int* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR]);

//EXTENDED
void                    ceres_q_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_state_ext(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, int* obtain_dest, unsigned char states_dest[CERES_SIZE_STATES_ARR]);



/*--EVENTS--*/
void                    ceres_q_read_event(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);

void                    ceres_q_load_event(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);



/*S2000-KDL*/
/** Store event type if event exist into [event_type_dest], if no event or error, stores zero.
    @return
    [-1]    error
    [0]     event
    [1]     no event

*/
char                    ceres_s2k_kdl_event_type(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, int* event_type_dest, unsigned char* event_dest);

/** Access event.
    Store event num into [event_dest] and card code into [code_dest].
    @return void
*/
void                    ceres_s2k_kdl_event_access(unsigned char* frame, unsigned char* event_dest, unsigned char code_dest[CERES_SIZE_CARD_CODE]);

/** Relay event.
    Store event num into [event_dest], relay num into [relay_dest] and relay program into [program_dest].
    @return void
*/
void                    ceres_s2k_kdl_event_relay(unsigned char* frame, unsigned char* event_dest, unsigned char* relay_dest, unsigned char* program_dest);

/** Common event.
    Store event num into [event_dest] and zone num into [zone_dest].
    @return void
*/
void                    ceres_s2k_kdl_event_common(unsigned char* frame, unsigned char* event_dest, unsigned char* zone_dest);

void                    ceres_s2k_kdl_q_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_s2k_kdl_r_counter(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone, long long unsigned int* counter_dest);



/*--------ZONES-------*/
void                    ceres_q_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_zone_arm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

void                    ceres_q_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_zone_disarm(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);



/*--------RELAY-------*/
void                    ceres_q_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);

char                    ceres_r_relay_on(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);

void                    ceres_q_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);

char                    ceres_r_relay_off(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char relay);



/*--------ADC-------*/
void                    ceres_q_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_adc_v1(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);

void                    ceres_q_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key, unsigned char zone);

char                    ceres_r_adc_v2(unsigned char* frame, int* len, unsigned char* addr_s, unsigned char* global_key);

void                    ceres_extract_adc(unsigned char* frame, int* len, double* dest);
void                    ceres_extract_adc(unsigned char* frame, int* len, long int* dest);
#endif // CERES_H_INCLUDED





















