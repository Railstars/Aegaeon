#include <stdint.h>

extern uint32_t _millis_counter, _micros_counter;

void force_timer(int new_millis)
{
    _millis_counter = new_millis;
    _micros_counter = new_millis * 1000;
}

void force_timer_incr(int new_millis)
{
    _millis_counter += new_millis;
    _micros_counter += new_millis * 1000;
}
