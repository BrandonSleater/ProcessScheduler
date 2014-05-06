#include <stdio.h>
#include "types.h"


u32 lsb8( u8 x )
{

    u32 pos = 0;

    if ( !( ~x & 15 ) )
    {
        pos += 4;
        x  >>= 4;
    }

    if ( !( ~x & 3 ) )
    {
        pos += 2;
        x  >>= 2;
    }

    if ( !( ~x & 1 ) )
    {
        pos += 1;
        x  >>= 1;
    }

    return pos;
}


u32 lsb16( u16 x )
{

    u32 pos = 0;

    if ( !( ~x & 255 ) )
    {
        pos += 8;
        x  >>= 8;
    }
    
    if ( !( ~x & 15 ) )
    {
        pos += 4;
        x  >>= 4;
    }
    
    if ( !( ~x & 3 ) )
    {
        pos += 2;
        x  >>= 2;
    }
    
    if ( !( ~x & 1 ) )
    {
        pos += 1;
        x  >>= 1;
    }

    return pos;
}


u32 lsb32(u32 x)
{

    u32 pos = 0;

    if ( !( ~x & 0xFFFF ) )
    {
        pos += 16;
        x  >>= 16;
    }

    if ( !( ~x & 0x00FF ) )
    {
        pos += 8;
        x  >>= 8;
    }
    
    if ( !( ~x & 0x000F ) )
    {
        pos += 4;
        x  >>= 4;
    }
    
    if ( !( ~x & 0x0003 ) )
    {
        pos += 2;
        x  >>= 2;
    }
    
    if ( !( ~x & 0x0001 ) )
    {
        pos += 1;
        x  >>= 1;
    }

    return pos;
}


u32 lsb64(u64 x)
{

    u32 pos = 0;

    if ( !( ~x & 0xFFFFFFFF ) )
    {
        pos += 32;
        x  >>= 32;
    }
    
    if ( !( ~x & 0x0000FFFF ) )
    {
        pos += 16;
        x  >>= 16;
    }
    
    if ( !( ~x & 0x000000FF ) )
    {
        pos +=  8;
        x  >>=  8;
    }
    
    if ( !( ~x & 0x0000000F ) )
    {
        pos +=  4;
        x  >>=  4;
    }
    
    if ( !( ~x & 0x00000003 ) )
    {
        pos +=  2;
        x  >>=  2;
    }
    
    if ( !( ~x & 0x00000001 ) )
    {
        pos +=  1;
        x  >>=  1;
    }

    return pos;
}


#ifdef _UTL_TEST_

int main( )
{
    int i, x, y, z;

    for ( i = 0; i < 256; ++i )
    {
        x = lsb8( i );
        printf( "lsb8(i) = %02x\n", x );
    }
}

#endif
