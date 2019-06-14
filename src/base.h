#ifndef BASE_H
#define BASE_H

#define Kilobytes(value)    ((value)*1024)
#define Megabytes(value)    (Kilobytes(value)*1024)
#define Gigabytes(value)    (Megabytes(value)*1024)

#define ArrayCount(array) (sizeof(array)/sizeof((array)[0]))
#define Assert(Expression) if(!(Expression)) {*(int*)0 = 0;}

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

#define global_variable static
#define internal        static
#define local_persist   static

typedef bool bool32;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#endif  // BASE_H
