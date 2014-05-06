#ifndef _TYPES__H_
#define _TYPES__H_


#define CASE    break; case


typedef             char    s8;
typedef unsigned    char    u8;
typedef             short   s16;
typedef unsigned    short   u16;
typedef             int     s32;
typedef unsigned    int     u32;
typedef             long    s64;
typedef unsigned    long    u64;


typedef
struct process
{
    u64             _pid;                                       // Expected start time
    u64             _curtm;                                     // Actual start time
    u32             _vas;                                       // Virtual Adddress Size
    u32             _cs;                                        // Code Size
    u32             _ds;                                        // Data Size
    u32             _fs;                                        // File Size
    u32             _ca;                                        // Code Address
    u32             _da;                                        // Data Address
    u32             _fa;                                        // File Address
    u64             _ct;                                        // Code Time
    u64             _dt;                                        // Data Time
    u64             _ft;                                        // File Time
    u64             _nt;                                        // Net Time
    u64             _ntf;                                       // First Net Time
    u64             _kt;                                        // kbd Time
    void            ( *_cp )( u32 m, u32 t, u32 *l, u32 *d );   // Code Profile
    void            ( *_dp )( u32 m, u32 t, u32 *l, u32 *d );   // Data Profile
    void            ( *_fp )( u32 m, u32 t, u32 *l, u32 *d );   // File Profile
    void            ( *_np )( u32 m, u32 t, u32 *l, u32 *d );   // Net Profile
    void            ( *_kp )( u32 m, u32 t, u32 *l, u32 *d );   // kbd Profile
    u16             _fpi;                                       // File Pointer Index
    u64             _time;
    u32             _priority;
    u16             _next;
    u16             _pti;                                       // Page Table Index
    u16             _sbt[1024];                                 // Swap Block Table
    u8              _arrid;                                     // Assiged at creation to know which process in the array
    u16             _fault;                                     // Find out how many times it page faulted
} process;


typedef union
{
    u8      _u8     [4096];
    s8      _s8     [4096];
    u16     _u16    [2048];
    s16     _s16    [2048];
    u32     _u32    [1024];
    s32     _s32    [1024];
    u64     _u64    [ 512];
    s64     _s64    [ 512];
    process _proc;
} page;


typedef enum
{
    Code,
    Data,
    File,
    Network,
    Keyboard,
    Timer
} dev_type;


#endif
