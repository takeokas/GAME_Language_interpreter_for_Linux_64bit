#ifndef PROTO64_H_
#ifndef __P
#define __P( x )  x
#endif


#include "gtypes64.h"

/* #include "ba.h" */
#include "lined64.h"

extern u_char* files[];

extern void xputs __P((u_char *));
extern void xputc __P((int));
extern int xkeychk __P( (void) );
extern int xgeykey __P( (void) );
extern int xgetc __P( (void) );
extern void xgets __P( (u_char*, int) );
extern void xxgets __P( (u_char*, int) );


extern void mach_init __P(());
extern void mach_fin __P(());

extern void crlf();

#ifndef PRHEX_FUNC
#define do_prHex2(e) xputs(ItoH(e,2))
#define do_prHex4(e) xputs(ItoH(e,4))
#define do_prHex8(e) xputs(ItoH(e,8))
#endif

#define PROTO64_H_
#endif /*PROTO64_H_*/
