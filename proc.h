#ifndef _PROC__H_


#define _PROC__H_


void blocked_enq( u16 index );

void blocked_deq( );

void ready_enq( u16 index );

u16 ready_deq( u16 priority );

void scheduler( );

void initialize_queues( );

void process_exec ( u16 index, u64 t );

u16 new_proc( u64 start_time, u32 run_time, u32 code_size, u32 data_size, u32 priority );

u32 new_code_addr( u32 addr, u32 limit );

u64 new_code_time( );

u32 new_data_addr( u32 addr, u32 base, u32 limit );

u64 new_data_time( );


#endif
