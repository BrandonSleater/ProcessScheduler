/**
 * Author: Brandon Sleater
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include 	"types.h"
#include 	"mem_man.c"
#include 	"proc.h"
#include 	<time.h>
#include 	"sys_time.c"

#define		MAX_PROCS	100

static u16 high_head;
static u16 high_tail;
static u16 med_head;
static u16 med_tail;
static u16 low_head;
static u16 low_tail;
static u16 blocked_list;

static u16 PROC_POINT = 0;

FILE *outFile;


typedef struct
{
	u64 _t;           // time to which process is allowed to run
    u32 *_code_addr;
    u32 *_code_time;
    u32 *_data_addr;
    u32 *_data_time;
} run_time;


struct
{
    u64 _start_time;
    u32 _run_time;
    u32 _code_size;
    u32 _data_size;
    u32 _priority;
} proc_init[100] =
{
    {               10ul,   5100000,        20000000,       30000000,       1   },
    {      10000000001ul,   6200000,        40000000,       20000000,       2   },
    {      10000000200ul,   7300000,        60000000,       10000000,       3   },
    {      80000000013ul,   8400000,        80000000,       90000000,       1   },
    {      80000000014ul,   9500000,        10000000,       80000000,       2   },
    {      80000000015ul,   5600000,        20000000,       70000000,       3   },
    {     200000000016ul,   6700000,        30000000,       60000000,       1   },
    {     200000000107ul,   7800000,        40000000,       50000000,       2   },
    {     200000000801ul,   8900000,        50000000,       40000000,       3   },
    {     200000009001ul,   9600000,        60000000,       30000000,       1   },
    {     800000000000ul,   5500000,        70000000,       20000000,       2   },
    {     800000000001ul,   6400000,        80000000,       10000000,       3   },
    {     800000000002ul,   7300000,        90000000,       50000000,       1   },
    {     800000000003ul,   8200000,        10000000,       40000000,       2   },
    {     800000000004ul,   9000000,        20000000,       30000000,       3   },
    {    1200000000025ul,   5400000,        30000000,       20000000,       1   },
    {    1200000000026ul,   6000000,        40000000,       10000000,       2   },
    {    1200000000027ul,   7666660,        50000000,       60000000,       3   },
    {    1200000000028ul,   8400000,        60000000,       30000000,       1   },
    {    1200000000029ul,   9200000,        70000000,       10000000,       2   },
    {    1200000000010ul,   5000000,        20000000,       30000000,       1   },
    {    2000000000011ul,   6100000,        40000000,       20000000,       2   },
    {    2000000000102ul,   7200000,        60000000,       10000000,       3   },
    {    2000000000301ul,   8300000,        80000000,       90000000,       1   },
    {    2000000000401ul,   9400000,        10000000,       80000000,       2   },
    {    2000000050001ul,   5500000,        20000000,       70000000,       3   },
    {    2000000060001ul,   6600000,        30000000,       60000000,       1   },
    {    2000000070001ul,   7700000,        40000000,       50000000,       2   },
    {    3000000000018ul,   8800000,        50000000,       40000000,       3   },
    {    3000000000019ul,   9900000,        60000000,       30000000,       1   },
    {    3000000000020ul,   5000000,        70000000,       20000000,       2   },
    {    3000000000021ul,   6900000,        80000000,       10000000,       3   },
    {    3000000000022ul,   7800000,        90000000,       50000000,       1   },
    {    3000000000023ul,   8700000,        10000000,       40000000,       2   },
    {    3000000000024ul,   9600000,        20000000,       30000000,       3   },
    {    3000000000025ul,   5500000,        30000000,       20000000,       1   },
    {    5000000001026ul,   6400000,        40000000,       10000000,       2   },
    {    5000000002027ul,   7300000,        50000000,       60000000,       3   },
    {    5000000003028ul,   8200000,        60000000,       30000000,       1   },
    {    5000000004029ul,   9100000,        70000000,       10000000,       2   },
    {    5000000005010ul,   5000000,        20000000,       30000000,       1   },
    {    5000000006011ul,   6600000,        40000000,       20000000,       2   },
    {    5000000007012ul,   7500000,        60000000,       10000000,       3   },
    {    5000000008013ul,   8400000,        80000000,       90000000,       1   },
    {    5000000009014ul,   9300000,        10000000,       80000000,       2   },
    {    7000000000005ul,   5200000,        20000000,       70000000,       3   },
    {    7000000000016ul,   6100000,        30000000,       60000000,       1   },
    {    7000000000118ul,   8000000,        50000000,       40000000,       3   },
    {    7000000001019ul,   9600000,        60000000,       30000000,       1   },
    {    7000000010020ul,   5500000,        70000000,       20000000,       2   },
    {    7000000020021ul,   6400000,        80000000,       10000000,       3   },
    {    7000000030022ul,   7300000,        90000000,       50000000,       1   },
    {    7000000040023ul,   8200000,        10000000,       40000000,       2   },
    {    7000000050017ul,   7100000,        40000000,       50000000,       2   },
    {    7000000060024ul,   9000000,        20000000,       30000000,       3   },
    {    9000000000025ul,   5600000,        30000000,       20000000,       1   },
    {    9000000000026ul,   6500000,        40000000,       10000000,       2   },
    {    9000000000027ul,   7400000,        50000000,       60000000,       3   },
    {    9000000000028ul,   8300000,        60000000,       30000000,       1   },
    {    9000000000029ul,   9200000,        70000000,       10000000,       2   },
    {    9000000000110ul,   5100000,        20000000,       30000000,       1   },
    {    9000000000111ul,   6000000,        40000000,       20000000,       2   },
    {    9000000000112ul,   7600000,        60000000,       10000000,       3   },
    {    9000000000113ul,   8500000,        80000000,       90000000,       1   },
    {    9000000000114ul,   9400000,        10000000,       80000000,       2   },
    {    9000000000115ul,   5300000,        20000000,       70000000,       3   },
    {   11000000001016ul,   6200000,        30000000,       60000000,       1   },
    {   11000000002017ul,   7100000,        40000000,       50000000,       2   },
    {   11000000003018ul,   8000000,        50000000,       40000000,       3   },
    {   11000000010019ul,   9300000,        60000000,       30000000,       1   },
    {   11000000020020ul,   5000000,        70000000,       20000000,       2   },
    {   11000000030021ul,   6600000,        80000000,       10000000,       3   },
    {   11000000040022ul,   7200000,        90000000,       50000000,       1   },
    {   11000000050023ul,   8400000,        10000000,       40000000,       2   },
    {   11000000060024ul,   9100000,        20000000,       30000000,       3   },
    {   11000000070025ul,   5000000,        30000000,       20000000,       1   },
    {   11000000080026ul,   6000000,        40000000,       10000000,       2   },
    {   11000000090027ul,   7000000,        50000000,       60000000,       3   },
    {   14000000003028ul,   8000000,        60000000,       30000000,       1   },
    {   14000000004029ul,   9900000,        70000000,       10000000,       2   },
    {   14000000005010ul,   5800000,        20000000,       30000000,       1   },
    {   14000000006011ul,   6600000,        40000000,       20000000,       2   },
    {   14000000010012ul,   7300000,        60000000,       10000000,       3   },
    {   14000000020013ul,   8000000,        80000000,       90000000,       1   },
    {   14000000030014ul,   9000000,        10000000,       80000000,       2   },
    {   14000000040015ul,   5000000,        20000000,       70000000,       3   },
    {   14000000050016ul,   6000000,        30000000,       60000000,       1   },
    {   14000000060017ul,   7600000,        40000000,       50000000,       2   },
    {   14000000070018ul,   8300000,        50000000,       40000000,       3   },
    {   14000000080019ul,   9300000,        60000000,       30000000,       1   },
    {   14000000090020ul,   5600000,        70000000,       20000000,       2   },
    {   15000000010021ul,   6300000,        80000000,       10000000,       3   },
    {   15000000020022ul,   7000000,        90000000,       50000000,       1   },
    {   15000000030023ul,   8000000,        10000000,       40000000,       2   },
    {   15000000040024ul,   9000000,        20000000,       30000000,       3   },
    {   15000000050025ul,   5100000,        30000000,       20000000,       1   },
    {   15000000060026ul,   6300000,        40000000,       10000000,       2   },
    {   15000000070027ul,   7500000,        50000000,       60000000,       3   },
    {   15000000080028ul,   8300000,        60000000,       30000000,       1   },
    {   15000000090029ul,   9400000,        70000000,       10000000,       2   }
};


void checkBlockList() {

    u16 temp = blocked_list;

    while (temp) {

        if (temp = mem[temp]._proc._next) {
            printf("CHECK: temp:\t%llu\n", temp);
            printf("CHECK: mem[temp]._proc._arrid:\t%llu\n", mem[temp]._proc._arrid);
            int x = 1 / 0;
        }

        temp = mem[temp]._proc._next;
    }

    printf("\n");
}


u32 new_code_addr( u32 addr, u32 limit )
{

    static u32 x[32] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    	2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 8, 16
    };

    u32 r = rand( );
    addr  = ( r & 64 ) ? addr + x[r & 31] : addr - x[r & 31];

    return ( addr > limit ) ? addr = r % limit : addr;
}


u64 new_code_time( )
{
    return 50 + ( rand( ) & 0xfff );
}


u32 new_data_addr( u32 addr, u32 base, u32 limit )
{

    static u32 x[32] = { 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4,
    	5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 11, 12, 16, 20, 28, 40
    };

    u32 r = rand( );
    addr  = ( r & 64 ) ? addr + x[r & 31] : addr - x[r & 31];

    return ( ( base < addr ) || ( addr > limit ) ) ? 
    	addr = base + ( r % ( limit - base ) ) : addr;
}


u64 new_data_time( )
{
    return 100 + ( rand( ) & 0x1fff );
}


/**
    Build and configure a new process
*/
u16 new_proc( u64 start_time, u32 run_time, u32 code_size, u32 data_size, u32 priority )
{

    // If the current time is less than the processes start time, don't make it
    if ( get_time() < start_time )
    {
        return 0;
    }

    // Figure out how many blocks it needs
    u64 size   = code_size + data_size;
    u16 blocks = size >> 22;

    if ( size & ( ( 1 << 22 ) - 1 ) )
    {
        blocks++;
    }

    // A page for the process to live in
    u16 proc_page = page_alloc( 1 );
    printf("NEW PROC: proc_page:\t%llu\n", proc_page);

    // Allocate space with disk chunks
    if ( vas_alloc( mem[proc_page]._proc._sbt, blocks ) )
    {
    }
    else
    {
        // There wasn't enough space, free up the pages and exit
        page_free( proc_page );
        return 0;
    }

    // A page for the l1
    u16 l1_page                    = page_alloc( 1 );
    printf("NEW PROC: l1_page:\t%llu\n", l1_page);

    // Setup the page time
    mem[proc_page]._proc._pid      = start_time;

    // Actual start time
    mem[proc_page]._proc._curtm    = get_time( );

    // Set id
    mem[proc_page]._proc._arrid    = PROC_POINT;

    // Setup page table index (l1)
    mem[proc_page]._proc._pti      = l1_page;

    // Set size of vas blocks
    mem[proc_page]._proc._vas      = blocks;

    // Setup code size
    mem[proc_page]._proc._cs       = code_size;

    // Setup data size
    mem[proc_page]._proc._ds       = data_size;

    // Setup net time
    mem[proc_page]._proc._nt       = run_time;

    // Original net time
    mem[proc_page]._proc._ntf      = run_time;

    // Set page fault count
    mem[proc_page]._proc._fault    = 0;

    // Setup proc priority
    mem[proc_page]._proc._priority = priority;

    // Set the mem_man properties for the process page
    mem_man[proc_page]._pinned     = 1;
    mem_man[proc_page]._l1i        = l1_page;

    // Set the mem_man properties for the l1 page
    mem_man[l1_page]._proc         = proc_page;
    mem_man[l1_page]._pinned       = 1;

    return proc_page;
}


/**
    Execute a process
*/
void process_exec ( u16 index, u64 t )
{
    printf("PROC EXEC: index:\t%llu\n", index);
    // If processes run time is complete, print out its stats and delete it
    if ( mem[index]._proc._nt <= 0 
    	 || mem[index]._proc._nt > mem[index]._proc._ntf )
    {
        u64 complete = get_time( );
        
        fprintf( outFile, "Process ID: %d \t Expected Start Time: %llu \t\t Actual Start Time: %llu \t\t Completed at: %llu \t\t Should have ran for: %llu \t Page Faults: %i\n", 
                 mem[index]._proc._arrid, mem[index]._proc._pid, mem[index]._proc._curtm, complete, mem[index]._proc._ntf, mem[index]._proc._fault );
        
        delete_proc( index );

        return;
    }

    u32 code_trans = virt_to_phys( mem[index]._proc._ca, index, 0 ); 	// 0 is read
    u32 data_trans = virt_to_phys( mem[index]._proc._da, index, 0 );

    // Page fault
    if ( !code_trans 
    	 && mem[index]._proc._ca )
    {
        // Increment page fault count
        mem[index]._proc._fault++;

        blocked_enq( index );
        checkBlockList();
        return;
    }

    // Page fault
    if ( !data_trans 
    	 && mem[index]._proc._da )
    {
        // Increment page fault count
        mem[index]._proc._fault++;
        
        blocked_enq( index );
                checkBlockList();
        
        return;
    }

    while ( 1 )
    {
        u32 t_t_t = t - get_time( ); //time_till_timer

        // If code time is less than data time
        if ( mem[index]._proc._ct < mem[index]._proc._dt )
        {
            if ( mem[index]._proc._ct > t_t_t )
            {
                mem[index]._proc._nt -= t_t_t;
                mem[index]._proc._ct -= t_t_t;
                mem[index]._proc._dt -= t_t_t;

                ready_enq( index );
                
                return;
            }

            set_time( get_time( ) + mem[index]._proc._ct );

            mem[index]._proc._nt -= mem[index]._proc._ct;
            mem[index]._proc._dt -= mem[index]._proc._ct;
            
            mem[index]._proc._ca  = new_code_addr( mem[index]._proc._ca, mem[index]._proc._cs );
            mem[index]._proc._ct  = new_code_time( );

            code_trans = virt_to_phys( mem[index]._proc._ca, index, 0 );

            if ( !code_trans )
            {
                // Increment page fault count
                mem[index]._proc._fault++;
                
                blocked_enq( index );
                checkBlockList();
                
                return;
            }
        }
        else if ( mem[index]._proc._dt > t_t_t )
        {
            mem[index]._proc._nt -= t_t_t;
            mem[index]._proc._ct -= t_t_t;
            mem[index]._proc._dt -= t_t_t;
            
            ready_enq( index );
            
            return;
        }
        else
        {
            set_time( get_time( ) + mem[index]._proc._dt );

            mem[index]._proc._nt -= mem[index]._proc._dt;
            mem[index]._proc._ct -= mem[index]._proc._dt;

            mem[index]._proc._da  = new_data_addr( mem[index]._proc._da, mem[index]._proc._cs, mem[index]._proc._ds );
            mem[index]._proc._dt  = new_data_time( );

            data_trans = virt_to_phys( mem[index]._proc._da, index, 1 ); // Write

            if ( !data_trans )
            {
                // Increment page fault count
                mem[index]._proc._fault++;

                blocked_enq( index );
                checkBlockList();
                return;
            }
        }
    }
}


/**
    Set the queues to 0 at beginning of program
*/
void initialize_queues( )
{
    blocked_list = 0;
    high_head    = 0;
    high_tail    = 0;
    med_head     = 0;
    med_tail     = 0;
    low_head     = 0;
    low_tail     = 0;
}


/**
    Putting a process on the blocked list
*/
void blocked_enq( u16 index )
{

    printf("mem[index]._proc._next:\t%llu\n", mem[index]._proc._next);

    // Temporary pointer to head of blocked list
    u16 temp = blocked_list;

    // Situation 1: The blocked list is empty
    if ( !blocked_list )
    {
        // Set head to the new process
        blocked_list = index;
    }
    else if ( mem[index]._proc._time < mem[blocked_list]._proc._time )
    {
        // Add new process to head of the list
        mem[index]._proc._next = blocked_list;
        blocked_list           = index;
    }
    else
    {
        // Trying to find where to place the process
        while ( mem[temp]._proc._next 
        	    && mem[index]._proc._time > mem[mem[temp]._proc._next]._proc._time )
        {
            temp = mem[temp]._proc._next;
        }

        u16 tempnext = mem[temp]._proc._next;

        // Then set the temps "next" to point to the new process
        mem[temp]._proc._next  = index;

        // Set the new processes "next" to the temps "next"
        mem[index]._proc._next = tempnext;

    }
}


/**
    Take process off the blocked list
*/
void blocked_deq( )
{
    // Get current time
    u64 CURR_TIME = get_time( );

    // Start at the head of the list
    u16 curr_head = blocked_list;

    // No other process to point to
    if ( curr_head 
         && !mem[curr_head]._proc._next 
         && mem[curr_head]._proc._time <= CURR_TIME)
    {
        ready_enq( curr_head );
        curr_head = 0;
    }
    else
    {
        while ( curr_head 
        	    && mem[curr_head]._proc._next 
        	    && mem[curr_head]._proc._time <= CURR_TIME )
        {
            u16 next  = mem[curr_head]._proc._next;

            mem[curr_head]._proc._next = 0;

            ready_enq( curr_head );

            // Move to the next item
            curr_head = next;
        }
    }

    blocked_list = curr_head;
}


/**
    Put the process on the ready queue
*/
void ready_enq( u16 index )
{

    mem[index]._proc._next = 0;

    // Determine its priority
    switch ( mem[index]._proc._priority )
    {
	    // High Priority
	    case 1 :
	        if ( high_tail )
	        {
	            mem[high_tail]._proc._next = index;
	        }

	        high_tail = index;

	        if ( !high_head )
	        {
	            high_head = high_tail;
	        }

	        break;

	    // Medium Priority
	    case 2 :
	        if ( med_tail )
	        {
	            mem[med_tail]._proc._next = index;
	        }

	        med_tail = index;

	        if ( !med_head )
	        {
	            med_head = med_tail;
	        }

	        break;

	    // Low Priority
	    case 3 :
	        if ( low_tail )
	        {
	            mem[low_tail]._proc._next = index;
	        }

	        low_tail = index;

	        if ( !low_head )
	        {
	            low_head = low_tail;
	        }

	        break;
    }
}


/**
    Take process off the ready queue
*/
u16 ready_deq( u16 priority )
{
    u16 temp_proc;

    switch ( priority )
    {
	    // High Priority
	    case 1:
	        temp_proc = high_head;

            mem[temp_proc]._proc._next = 0;

	        high_head = mem[temp_proc]._proc._next;

            // Process to be ran
	        return temp_proc;

	        break;

	    // Medium Priority
	    case 2:
	        temp_proc = med_head;

            mem[temp_proc]._proc._next = 0;

	        med_head  = mem[temp_proc]._proc._next;

	        return temp_proc;

	        break;

	    // Low Priority
	    case 3:
	        temp_proc = low_head;

            mem[temp_proc]._proc._next = 0;

	        low_head  = mem[temp_proc]._proc._next;

	        return temp_proc;

	        break;
    }
}


void printBlockedList() {

    u16 temp = blocked_list;

    printf("head of list = %llu\n", mem[temp]._proc._time);

    while (temp) {

        temp = mem[temp]._proc._next;
        printf("next in list = %i\t%llu\n", temp, mem[temp]._proc._time);

    }

    printf("\n");
}


/**
    Runs it all
*/
void scheduler( )
{
    u16 temp_proc;

    // While we have stuff on the queues or list
    while ( blocked_list 
    	    || high_head 
    	    || med_head 
    	    || low_head )
    {
        if ( blocked_list 
        	 && !high_head 
        	 && !med_head 
        	 && !low_head )
        {
            printf("******************\n");
            printf("CURRENT_TIME:\t%llu\n", get_time());
            printf("SET_TIME TO: mem[blocked_list]._proc._time:\t%llu\n", mem[blocked_list]._proc._time);
            printf("******************\n");

            set_time( mem[blocked_list]._proc._time );
            printf("before deq\n");
            
            printf("mem[blocked_list]._proc._arrid:\t%i\n", mem[blocked_list]._proc._arrid);
            printf("mem[mem[blocked_list]._proc._next]._proc._arrid:\t%i\n", mem[mem[blocked_list]._proc._next]._proc._arrid);
            printf("mem[mem[mem[blocked_list]._proc._next]._proc._next]._proc._arrid:\t%i\n", mem[mem[mem[blocked_list]._proc._next]._proc._next]._proc._arrid);

            printf("get_time():\t%llu\n", get_time());
            blocked_deq( );
            printf("after deq\n");
        }

        // High priority
        u8 count = 0;
        while ( high_head 
        	    && count < 4 )
        {
            printf("RUNNING HIGH\n");
            temp_proc = ready_deq( 1 );
            printf("mem[temp_proc]._proc._next:\t%llu\n", mem[temp_proc]._proc._next);
            
            if ( mem[temp_proc]._proc._next ) {
                int x = 1 / 0;
            }

            printf("starting execute process with: %i\n", temp_proc);
            process_exec( temp_proc, get_time( ) + 2500 );
            printf("starting blocked in high\n");
            blocked_deq( );
            printf("done with blocked in high\n");

            count++;
        }

        // Med priority
        count = 0;
        while ( med_head 
        	    && count < 2 )
        {
            printf("RUNNING MED\n");

            temp_proc = ready_deq( 2 );
            process_exec( temp_proc, get_time( ) + 2500 );
            blocked_deq( );

            count++;
        }

        // Low priority
        count = 0;
        while ( low_head 
        	    && count < 1 )
        {
            printf("RUNNING LOW\n");
            temp_proc = ready_deq( 3 );
            process_exec( temp_proc, get_time( ) + 2500 );
            blocked_deq( );

            count++;
        }

        // Try to make other processes with currents
        u16 result = 1;
        while ( result && PROC_POINT < MAX_PROCS )
        {
            printf("IN WHILE - get_time():\t%llu\n", get_time());
            printf("proc_init[PROC_POINT]._start_time:\t%llu\n", proc_init[PROC_POINT]._start_time);
            result = new_proc( proc_init[PROC_POINT]._start_time, proc_init[PROC_POINT]._run_time, proc_init[PROC_POINT]._code_size, proc_init[PROC_POINT]._data_size, proc_init[PROC_POINT]._priority );
            printf("result:\t%llu\n", result);
            //printf("get_time():\t%llu\n", get_time());
            //printf("proc_init[PROC_POINT]._start_time:\t%llu\n\n\n", proc_init[PROC_POINT]._start_time);

            // Proc is ready to start, we got something back, put it on the queue
            if ( result )
            {
                printf("----------------------------------------------\n");
                printf("WHILE - New Process Index #:\t%i\n", PROC_POINT);
                printf("proc_init[PROC_POINT]._start_time:\t%llu\n", proc_init[PROC_POINT]._start_time);
                printf("----------------------------------------------\n");
                PROC_POINT++;
                ready_enq( result );
            }
        }

        // Nothing on queues and list but we haven't ran all the processes
        if ( PROC_POINT < MAX_PROCS 
        	 && !blocked_list 
        	 && !high_head 
        	 && !med_head 
        	 && !low_head )
        {
            printf("----------------------------------------------\n");
            printf("CURRENT TIME BEFORE SET:\t%llu\n", get_time());

            set_time( proc_init[PROC_POINT]._start_time );
            temp_proc = new_proc( proc_init[PROC_POINT]._start_time, proc_init[PROC_POINT]._run_time, proc_init[PROC_POINT]._code_size, proc_init[PROC_POINT]._data_size, proc_init[PROC_POINT]._priority );
            
            printf("IF - New Process Index #:\t%i\n", PROC_POINT);
            printf("NEW PROCESSES START TIME:\t%llu\n", proc_init[PROC_POINT]._start_time);
            printf("CURRENT TIME AFTER SET:\t\t%llu\n", get_time());
            printf("----------------------------------------------\n\n\n");
            
            PROC_POINT++;

            printf("high_head:\t%i\n", high_head);
            printf("med_head:\t%i\n", med_head);
            printf("low_head:\t%i\n", low_head);
            printf("mem[high_head]._proc._next:\t%i\n", mem[high_head]._proc._next);
            printf("mem[med_head]._proc._next:\t%i\n", mem[med_head]._proc._next);
            printf("mem[low_head]._proc._next:\t%i\n", mem[low_head]._proc._next);

            ready_enq( temp_proc );

            printf("high_head:\t%i\n", high_head);
            printf("med_head:\t%i\n", med_head);
            printf("low_head:\t%i\n", low_head);
            printf("mem[high_head]._proc._next:\t%i\n", mem[high_head]._proc._next);
            printf("mem[med_head]._proc._next:\t%i\n", mem[med_head]._proc._next);
            printf("mem[low_head]._proc._next:\t%i\n", mem[low_head]._proc._next);
        }
    }
    printf("FINISHED EXECUTING\n");
}


/**
    Start and intialize it all
*/
int main( )
{
    u32 i;

    // Seed the random generator for unique non-repeating numbers
    //srand( time( NULL ) );

    // Setup the file we write our data too
    outFile = fopen( "output.txt", "w" );

    // Setup our queues
    initialize_queues( );

    // Setup the pages
    for ( i = 1; i < PAGE_COUNT; i++ )
    {
        page_free(i);
    }

    // Jump to the first processes time
    set_time( proc_init[PROC_POINT]._start_time );

    // Create the processes
    u16 proc1 = new_proc( proc_init[PROC_POINT]._start_time, proc_init[PROC_POINT]._run_time, proc_init[PROC_POINT]._code_size, proc_init[PROC_POINT]._data_size, proc_init[PROC_POINT]._priority );

    printf("MAIN - New Process Index #:\t%i\n\n", PROC_POINT);

    // Increment our pointer
    PROC_POINT++;

    // Put the new process on the queue
    ready_enq( proc1 );

    // Start it up
    scheduler( );
}
