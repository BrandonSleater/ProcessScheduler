#include "sys_time.h"

static u64 sys_time = 0;


u64 get_time( )
{
    return sys_time;
}

void set_time( u64 t )
{
    sys_time = t;
}

void increment_time( u64 t )
{
    sys_time += t;
}