#ifndef _MEM_MAN__H_

#define _MEM_MAN__H_


#include "types.h"
#include "proc.h"


typedef
struct
{
    u64 _used   :  1;
    u64 _dirty  :  1;
    u64 _pinned :  1;
    u64         : 13;
    u64 _l1i    : 10;   //index
    u64         :  6;
    u64 _l2i    : 10;   //index
    u64         :  6;
    u64 _proc   : 16;
} mem_manage;


void read_page( page *x, u16 y );

void write_page( page *x, u16 y );

void build_mem_avail( );

u16 page_alloc( u8 cleanMe );

void page_free( u16 x );

int vas_alloc( u16 v[], u32 size );

void vas_free( u16 v[], u32 size );

u16 virt_to_phys( u32 addr, u16 index, u8 type );

u16 walk_page_ring( u8 cleanMe );

#endif
