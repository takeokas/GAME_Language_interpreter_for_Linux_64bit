/*
 gtypes.h
 take@takeoka.org
*/

#ifndef GTYPES_H_
#define GTYPES_H_

#define u_char unsigned char
#define u_short unsigned short
#define u_int unsigned int

#ifdef B64
#define Int long long
#define u_Int unsigned long long
#endif /* B64 */

#ifdef B32
#define Int long
#define u_Int unsigned long
#endif /* B32 */

#define Byte8 long long
#define Byte4 int
#define u_Byte4 u_int
#define Byte2 short
#define u_Byte2 u_short


#ifndef NULL
#define NULL ((void*)0)
#endif

#endif /* GTYPES_H_ */
