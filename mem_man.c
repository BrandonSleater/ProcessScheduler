#include <stdio.h>
#include "types.h"
#include "mem_man.h"
#include "util.h"
#include "util.c"
#include "disk.c"
#include "sys_time.h"


#define PAGE_COUNT        65536
#define PAGE_MASK         ( PAGE_COUNT - 1 )
#define VAS_VEC_SIZE      ( 1 << 6 )            //64 100000
#define VAS_VEC_SIZE_MASK ( VAS_VEC_SIZE - 1 )  //63 011111


static  page        mem[PAGE_COUNT];                          // Big boy page holder
static  u16         page_avail            = 0;                // Count of available pages
static  mem_manage  mem_man[PAGE_COUNT]   = { 0 };            // Meta data of mem
static  u16         mem_offset            = 1;                // Not sure...
static  u64         vas_vec[VAS_VEC_SIZE] = { 0 };            // Array of size 64
static  u32         vas_offset            = 0;                // Offset usually into vas_vec array
static  u32         vas_count             = 4096;             // 64


/**
    This will read a page from memory by handing
    it a pointer to a page and its offset in memory
*/
void read_page( page *x, u16 y )
{
    u32 i;

    for ( i = 0; i < 512; ++i )
    {
        x->_u64[i] = mem[y]._u64[i];
    }
}


/**
    This will write a page to memory by handing
    it a pointer to a page and its offset in memory
*/
void write_page( page *x, u16 y )
{
    u32 i;

    for ( i = 0; i < 512; ++i )
    {
        mem[y]._u64[i] = x->_u64[i];
    }
}


/**
    This will populate our table with page entries
    The first u32 index points to the previous page
*/
void build_mem_avail( )
{
    u32 i;

    // Assign max amount of pages minus 1
    // to account for mem starting at index 0
    page_avail = PAGE_COUNT - 1;

    // Iterate through mem, linking each page
    for ( i = PAGE_COUNT - 1; i > 0; i-- )
    {
        // The first u32 of the entry points to the previous page
        mem[i]._u32[0] = i - 1;
    }
}


/**
    Delete a process once it has been completed and written to output
*/
u32 delete_count = 0;
void delete_proc( u16 index )
{
    u32 i;
    u32 j;

    printf("++delete_count:\t%llu\n", ++delete_count);

    // Determine the amount of blocks the process has to free
    u64 size   = mem[index]._proc._cs + mem[index]._proc._ds;
    u16 blocks = size >> 22;

    if ( size & ( ( 1 << 22 ) - 1 ) )
    {
        blocks++;
    }

    printf("before: vas_count:\t%llu\n", vas_count);

    // Free em up
    vas_free( mem[index]._proc._sbt, blocks );
    printf("after: vas_count:\t%llu\n", vas_count);

    // Lets go through the different levels freeing up u32's
    u16 level1 = mem[index]._proc._pti;

    for ( i = 0; i < 1024; i++ )
    {
        // There is an l2 page
        if ( mem[level1]._u32[i] )
        {
            u16 level2 = mem[level1]._u32[i];

            // Have to go through one more time for level 2
            for ( j = 0; j < 1024; j++ )
            {
                if ( mem[level2]._u32[j] )
                {
                    printf("1\n");
                    page_free( mem[level2]._u32[j] );
                }
            }
            printf("2\n");
            page_free( level2 );
        }
    }
    printf("3\n");
    page_free( level1 );
    printf("4\n");
    page_free( index );
}


/**
    This will return us an available page entry
*/
u16 page_alloc( u8 cleanMe )
{
    // Determine if we have a page or need to get one
    if ( page_avail )
    {
        printf("IN PAGE ALLOC\n");
        printf("page_avail:\t%llu\n", page_avail);
        // Save which page is available
        u16 temp   = page_avail;

        // Set page avail to the next page the index points to
        page_avail = mem[page_avail]._u32[0];

        // Return the available page
        return temp;
    }
    else
    {
        printf("IN else PAGE ALLOC\n");
        // Go get a page
        return walk_page_ring( cleanMe );
    }
}


/**
    This function takes in a page entry index,
    clears its "meta data" in mem_man, then sets the
    next free page to the page that was passed in
*/
void page_free( u16 index )
{
    // Set the soon to be freed pages pointer
    // to the next available page
    mem[index]._u32[0] = page_avail;

    mem[index]._proc._next = 0;

    printf("FREE: index:\t%llu\n", index);
    printf("FREE: page_avail:\t%llu\n", page_avail);

    // Reset page struct bits to 0
    mem_man[index]._used   = 0;
    mem_man[index]._pinned = 0;
    mem_man[index]._dirty  = 0;

    // Set the next page entry to be used, to the
    // entry we just freed
    page_avail = index;

    printf("FREE: page_avail:\t%llu\n", page_avail);
}


/**
    This will allocate virtual address spaces
    v[] is the array that the process will have that
    stores all the disk blocks the process has access to
*/
int vas_alloc( u16 v[], u32 size )
{
    u32 pos;            // Holder for lsb of that offset
    u32 counter = 0;    // Overall counter for amount of blocks

    // We don't have enough blocks to give out
    if ( vas_count < size )
    {
        return 0;
    }

    // For the amount of blocks that we need
    while ( counter < size )
    {
        // Bitwise NOT the bits, if there is a 0 block
        // then it becomes a 1 and we go into the if
        if ( ~( vas_vec[vas_offset] ) )
        {
            // Find the first open spot
            pos = lsb64( vas_vec[vas_offset] );

            // Shift 1 to the spot, then OR it
            // to allocate that spot (UL for overflow)
            vas_vec[vas_offset] |= 1UL << pos;

            // Encode the single u16 with the index into vas_vec
            // where the block is located and the position of the
            // bit in the 64 bit number that we gave the process
            v[counter] = (vas_offset << 6) | pos;

            // We got a block, increment for the while loop
            counter++;
        }
        else
        {
            // There are no more free blocks,
            // move to the next index
            vas_offset++;
        }
    }

    // Decrement the amount of address spaces left
    vas_count -= size;

    // Return success on allocation
    return 1;
}


/**
    This function is handed an array of addresses and the
    amount of entries to be freed up
*/
void vas_free( u16 v[], u32 size )
{
    u32 i;

    // For the amount of entries
    for ( i = 0; i < size; i++ )
    {
        // And address with the mask to get the shift amount
        // Bitwise NOT and Bitwise AND the offsets 1 bit to a 0
        vas_vec[v[i] >> 6] &= ~(1UL << (v[i] & VAS_VEC_SIZE_MASK));
    }

    // Increase number of virtual address spaces
    vas_count += size;
}


/**
    The name describes it perfectly
*/
u16 walk_page_ring( u8 cleanMe )
{
    int newPage = 0;

    while ( !newPage )
    {
        if ( mem_man[mem_offset]._pinned 
             && !( cleanMe 
                   && mem_man[mem_offset]._dirty ) )
        {
            // do nothing
        }
        else if ( mem_man[mem_offset]._used )
        {
            mem_man[mem_offset]._used = 0;
        }
        else
        {
            if ( mem_man[mem_offset]._dirty )
            {
                //@TODO - Figure out how to get disk u32 block. Think it has something to do with _sbt
                disk_write( 0, mem_offset );
            }

            mem_manage current = mem_man[mem_offset];

            // Need to get to the l3
            u16 level1 = mem[current._proc]._proc._pti;
            u16 level2 = mem[level1]._u32[current._l1i];

            // Set the page to clean
            mem[level2]._u32[current._l2i] = 0;

            // found a page, lets exit
            newPage = 1;
        }

        // Go to the next page
        mem_offset++;
    }

    // Send back the new page
    return mem_offset;
}

/**
    Convert a virtual address to physical.
    0 is a read, 1 is a write
*/
u16 virt_to_phys( u32 addr, u16 index, u8 type )
{
    u32 mask   = 0xFFC00000;                // Upper 10 bits, all 1's
    u32 x      = ( addr & mask ) >> 22;

    u16 level1 = mem[index]._proc._pti;
    u16 level2 = mem[level1]._u32[x];

    // If there is an level2
    if ( !level2 )
    {
        // Needs to be clean
        level2 = page_alloc( 1 );

        mem[level1]._u32[x]     = level2;
        mem_man[level2]._l1i    = level1;
        mem_man[level2]._used   = 1;
        mem_man[level2]._pinned = 1;
        mem_man[level2]._proc   = level2;
    }

    // Next middle 10 bits
    mask >>= 10;
    u32 y  = ( addr & mask ) >> 12;

    u16 level3 = mem[level2]._u32[y];

    // Either return a l3 or get one
    if ( level3 )
    {
        mem_man[level3]._used   = 1;
        mem_man[level3]._dirty |= ( type == 1 );  // Write

        return level3;
    }
    else
    {
        // Page fault
        level3 = page_alloc( 0 );

        mem[level2]._u32[y]    = level3;
        mem_man[level3]._l1i   = mem[index]._proc._pti;
        mem_man[level3]._l2i   = level2;
        mem_man[level3]._used  = 1;
        mem_man[level3]._proc  = level3;

        //@TODO - Figure out how to get disk u32 block. Think it has something to do with _sbt
        mem[index]._proc._time = disk_read( 0, level3 );
        
        return 0;
    }
}
