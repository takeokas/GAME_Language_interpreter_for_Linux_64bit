/*
	GAME Language interpriter 64bit under Linux
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <setjmp.h>
#include <string.h>

#define MODULO_OP

#define	MAX_STK	100

u_char text_buf[10000];

#define Int long long

extern void xgets(u_char*, int);
extern void xxgets(u_char*, int);
extern void xputs(u_char*);
extern void crlf();
extern Int pop();
extern Int push(Int);
extern u_char* makeLine(u_char*);
extern u_char* dispLine(u_char*);
extern Int operand();
extern Int term(int);
extern Int expr(int);
extern int skipBlank();
extern int skipAlpha();
extern void do_cmd();
extern void exqt();
extern u_char *dispLine(u_char *);
extern void dispList(u_char *);
extern void deleteLine(u_char *);
extern void addLine(int ,u_char *, u_char *);
extern void breakCheck();
extern void errMsg(u_char *);



/*****/
#define iSnum(c) ('0'<=(c) && (c)<='9')
#define iShex(c) (('0'<=(c) && (c)<='9')||('A'<=(c) && (c)<='F')||('a'<=(c) && (c)<='f'))

#define LINBUF0SIZ 256

u_char lin[LINBUF0SIZ];
u_char *pc;
int  sp;
Int  stack[MAX_STK];

jmp_buf toplvl;

Int lno;

/*	Var	*/
unsigned long long var[256-32];

#define VARA(v) var[(v)-0x20]

#define	TOPP	VARA('=')
#define	BTMP	VARA('&')
#define	RAND	VARA('\'')
#define	MOD	VARA('%')

Int
getNum(Int *f)
{
 Int c;
 Int n=0;
 *f=0;
 for(;c= *pc;){
	if(!iSnum(c)) break;
	n= n*10 + (c-'0');
	pc++;
	*f=1;
 }
 return n;
}

Int
getHex(Int *f)
{
 Int c;
 Int n=0;
 *f=0;
 for(;c= *pc;){
	if(!iShex(c)) break;
	n= n*16 + ((c<'A')?(c-'0'):((c<'a')?(c-'A') :(c-'a'))+10 );
	pc++;
	*f=1;
 }
 return n;
}



void
newText1()
{
  char *p;
	BTMP = TOPP;
	printf("TOPP=%llx:BTMP=%llx\r\n",(TOPP), (BTMP));
	p=(char*)(BTMP);
	*p = 0xFF;
}

void
newText()
{
	if( *((u_char*)BTMP) != 0xFF){
		xputs("\ntext is locked");
		longjmp(toplvl,1);
	}
	newText1();
}

extern u_char* searchLine(Int ,Int *);
extern u_char* makeLine(u_char *);

void
main()
{
 Int n,x;

	mach_init();

	TOPP =(unsigned long long)text_buf;
	/*printf("main1:var=%llx\r\n",var);
	printf("main1:&TOPP=%llx\r\n", &(TOPP));
	printf("main1:&BTMP=%llx\r\n", &(BTMP));
	printf("main1:text_buf=%llx,TOPP=%llx\r\n",text_buf,TOPP);*/
	newText1();

	crlf();xputs("  --- GAME Interpriter (95/Jan/01) ---\r\n");
	printf("sizeof(int)=%d,sizeof(long)=%d\r\n",sizeof(int),sizeof(long));
	for(;;){
		setjmp(toplvl);
		sp= -1;
		lno=0;
/*		printf("BTMP=%x\n",BTMP); /**/
		*lin='\0';
		//crlf();
 reenter:
		xputs("G>>");
		xxgets(lin,LINBUF0SIZ);
		crlf();
		*(lin+strlen(lin)+1) = 0x80; /* EOF on endOfLinebuf*/
		pc=lin;
		skipBlank();
		n=getNum(&x);
		if(x==0)exqt();
		else{
			if(*pc=='\\'){ /* edit the line */
				Int f;
				u_char *p;
				p=searchLine(n, &f);
				///printf(" searchLine:f=%lld\r\n",f);
				if(f==0) continue;
				p=makeLine(p);
				///printf(" makeLine:p=%s\r\n",p);
				strcpy(lin,p);
				goto reenter;
			}
			edit(n);
		}
	}
}

void
dmp(u_char *p, Int n)
{
 int i;
 for(i=0;i<n;i++)
	printf("%2x ",*p++);
}

u_char*
skipLine(u_char *p)
{
 for(;*p;)
	p++;
 return p+1;
}

u_char*
searchLine(Int n,Int *f)
{
 u_char* p;
 int l;

 for(p=(u_char*)TOPP;!(*p & 0x80);){
	l= (*p << 8) | *(p+1);
	if(n==l){ *f=1; return p;}
	if(n< l){ *f=0; return p;}
	p=skipLine(p+2);
 }
 *f=0;
 return p;
}

/* line edit routines */
edit(int n)
{
 u_char *p;
 Int f;
	if(n==0){ dispList((u_char*)TOPP); return 0; }

	p=searchLine(n, &f);
	if(*pc=='/'){ /* list */
		dispList(p);
	}else{ /*edit */
/*		printf("edit:(%d)%d=%s",f,n,pc); */
		if(*((u_char*)BTMP) != 0xFF){
			xputs("Text is locked\n");
			return 0;
		}
		if(f) deleteLine(p);
		if(*pc=='\0') return 0; /* delete line */
		addLine(n,p,pc);
	}
	return 0;
}

void
addLine(int n,u_char *p, u_char *new)
{
 Int l;
 l= 2+ strlen(new)+1;
 bcopy(p,p+l,(((u_char*)BTMP)-p)+1);
 *p= n>>8;
 *(p+1)= n;
 strcpy(p+2,new);
 BTMP += l;
/* *((u_char*)BTMP) = 0xFF; */
}

void
deleteLine(u_char *p)
{
 Int l;
 l= 2+ strlen(p+2)+1;
 bcopy(p+l,p,(((u_char*)BTMP)-p)-l+1);
 BTMP -= l;
}

/** listing display routine **/
u_char *
makeLine(u_char *p)
{
 Int l;
 static u_char buf[256];

 l= (*p << 8) | *(p+1); p+=2;
 sprintf(buf, "%lld", l);
 strcat(buf,p);
 return buf;
} 

u_char *
dispLine(u_char *p)
{
 Int l;
 u_char b[256];

 l= (*p << 8) | *(p+1); p+=2;
 sprintf(b, "%lld", l);
 xputs(b);
 for(;*p;){
	xputc(*p++);
 }
 crlf();
 return p+1;
}

void
dispList(u_char *p)
{
 for(;!(*p & 0x80);){
	breakCheck();
	p = dispLine(p);
 }
}


int
skipBlank()
{int x;
	for(;;){
		if((x= *pc) != ' ') return x;
		pc++;
	}
}

int
skipAlpha()
{int x;
	for(;;){
		x= *pc;
		if((x<'A')||('z'<x)||('Z'<x && x<'a')) return x;
		pc++;
	}
}

/** execute statement **/
void
exqt()
{int c;
	for(;;){
		c=skipBlank();
		do_cmd();
	}
}

void
topOfLine()
{
 int x,c;
more:
	x= *pc++;
	if(x & 0x80){
		//xputs("\nat EOF ");
		crlf();
		longjmp(toplvl,1);
	}
	lno = (x <<8)| *pc++;

	if(*pc != ' '){ /* Comment */
		pc=skipLine(pc);
		goto more;
	}
}

void
breakCheck()
{int c;
 if(xkeychk()){
	c=xgetkey();
	if(c == 0x03) longjmp(toplvl,1);
	if(c == 0x13) xgetkey(); /*pause*/
 }
}

void
do_pr()
{
 int x;
	for(;;){
		if('"' == (x= *pc++)) break;
		if(x== '\0'){ pc--;break;}
		xputc(x);
	}
}

void
do_prSpc(Int e)
{int i;
 for(i=0;i<e;i++)
	xputc(' ');
}

void
do_prChar(Int e)
{
 xputc(e);
}

void
do_prNum(int c1)
{
 u_char buf[256];
 Int e,digit;

 if(c1== '('){
	u_char form[256];
	pc++;
	digit=term(c1);
/*printf("prDigi=%c\n", *pc);/**/
	e=operand();
	sprintf(form,"%%%lldd",digit);
/*printf("form=%s",form);/**/
	sprintf(buf,form,e);
	xputs(buf);
	return ;
 }

 e=operand();
 switch(c1){
 case '?' :
#ifdef G64
	sprintf(buf,"%016llx",e);
#endif
#ifdef G32
	sprintf(buf,"%08llx",e &0xFFFFFFFF);
#endif
	break;
 case '@' : // Hex 16digit,64bit
	sprintf(buf,"%016llx",e);
	break;
 case '*' : // Hex 8digit,32bit
	sprintf(buf,"%08llx",e &0xFFFFFFFF);
	break;
 case '!' :  // Hex 4digit,16bit
	sprintf(buf,"%04llx",e & 0xFFFF);
	break;
 case '$' :
	sprintf(buf,"%02llx",e & 0xFF);
	break;
 case '=' :
	sprintf(buf,"%lld",e);
	break;
 default:
	xputs("unknown cmd\n");
	longjmp(toplvl,1);
 }
 xputs(buf);
}

void
do_until(Int e,int val)
{
/*printf("until:val=%02x,e=%d,ev=%d,pc=%x\n",val, e, stack[sp],stack[sp-1]);/**/
	VARA(val)=e;
	if(e>stack[sp]){
		sp-=2; /*pop pc,value*/
		return ;
	}
	/* repeat */
	pc=(u_char*)stack[sp-1]; /*pc*/
	return ;
}

void
do_do()
{
	push((Int)pc);
	push(0);
}

void
do_if(Int e)
{
 if(e==0){
	pc=skipLine(pc);
	topOfLine();
 }
}

void
do_goto(Int n)
{
 Int f;
 u_char *p;
	if(n==-1) longjmp(toplvl,1); /* Prog Stop */
	p=searchLine(n, &f);
	pc=p;
	topOfLine();
}

void
do_gosub(Int n)
{
 Int f;
 u_char *p;
	p=searchLine(n, &f);
	push((Int)pc);
	pc=p;
	topOfLine();
}


void
do_cmd()
{int c,c1,c2,vmode;
 Int e,off;
	breakCheck();
	c= *pc++;
	c1= *pc;
/* printf("%02x ",c); /**/
	switch(c){
	case '\0':
		topOfLine();
		return;
	case ']' :
		pc=(u_char*)pop(); return;
	case '"' :
		do_pr(); return;
	case '/' :
		crlf();return;
	case '@' :
		if(c1=='='){break; }
		do_do();return;

	case '?' :
		do_prNum(c1);return;
	/**/
	case '\\' :
		mach_fin();
	}

	if(c1=='='){
		switch(c){
		case '#' :
			e=operand();do_goto(e);return;
		case '!' :
			e=operand();do_gosub(e);return;
		case '$' :
			e=operand();do_prChar(e);return;
		case '.' :
			e=operand();do_prSpc(e);return;
		case ';' :
			e=operand();do_if(e);return;
		case '\'' : /*RAND seed */
			e=operand();srand(e);return;
		case '@' :
			c2= *(pc+1);e=operand();do_until(e,c2); return;
		case '&' :
			e=operand();
			if(e==0){
				newText();
			}
			return;
		default:
			/* Variable */
			break;
		}
	}
	vmode=skipAlpha();
/* printf("exp:%02x ",vmode); /**/
	if(vmode==':' || vmode=='[' || vmode=='{' || vmode=='(' ){
		pc++;
		off=expr(*pc++);
		if(*(pc-1) !=')'){
			errMsg("var ')' mismatch");
			longjmp(toplvl,1);
		}
		e=operand();
/*printf(" v=%c%c%d)=%d ",c,vmode,off,e); /**/
		switch(vmode){
		case ':' : *(((u_char*)VARA(c)+off))=e; return;
		case '[' : *(((u_short*)VARA(c)+off))=e; return;
		case '{' : *(((u_int*)VARA(c)+off))=e; return;
		case '(' : *(((Int*)VARA(c)+off))=e; return;
		}
		return;
	}
	e=operand();
	VARA(c)=e;
/* printf("exp:%02x ",*(pc-1)); /**/
	if(*(pc-1)== ','){ /* For */
			c= *pc++;
			e= expr(c);
/*printf("operandExpr=%d\n",e); /**/
			push((Int)pc);
			push(e);
	}
	return;
}



Int
pop()
{
	if(sp<0){
		xputs("Stack UnderFlow\n");
		longjmp(toplvl,1);
	}
	return stack[sp--];
}

Int
push(Int x)
{
	if(sp>=(MAX_STK-1)){
		xputs("Stack OverFlow\n");
		longjmp(toplvl,1);
	}
	return stack[++sp]=x;
}

Int
operand()
{
 Int x,e;
	for(;;){
		x= *pc++;
/*printf("operandC=%02x\n",x); /**/
		if(x == '=') break;
		if(!(x & 0xDF)){
			errMsg("No operand expression");
			longjmp(toplvl,1);
		}
	}
	x= *pc++;
	e= expr(x);
/*printf("operandExpr=%d\n",e); /**/
	return e;
}

Int
expr(int c)
{
 Int o,o1, op1,op2;
 Int e;

 e=term(c);

 for(;;){
	o= *pc++;
/*printf("exprC=%02x\n",o); /**/
	switch(o){
	case '\0' :
		pc--;
	case ' ' :
	case ')' :
	case ',' :
		return e;
	case '<' :
		o1= *pc++;
		switch(o1){
		case '>' :
			op2=term(*pc++);
			e= (e!=op2);
			continue;
		case '=' :
			op2=term(*pc++);
			e=(e<=op2);
			continue;
		default:
			op2=term(o1);
			e=(e<op2);
			continue;
		}
	case '>' :
		o1= *pc++;
		switch(o1){
		case '=' :
			op2=term(*pc++);
			e=(e>=op2);
			continue;
		default:
			op2=term(o1);
			e=(e>op2);
			continue;
		}
	case '+' : op2=term(*pc++);e= e+op2;break;
	case '-' : op2=term(*pc++);e= e-op2;break;
	case '*' : op2=term(*pc++);e= e*op2;break;
	case '/' : op2=term(*pc++); MOD=e%op2; e= e/op2;break;
#ifdef MODULO_OP
	case '%' : op2=term(*pc++); MOD=e%op2; e= e%op2;break;
#endif //MODULO_OP

	case '=' : op2=term(*pc++);e= (e==op2);break;

	case '&' : op2=term(*pc++);e= e&op2;break;
	case '|' : op2=term(*pc++);e= e|op2;break;
	case '^' : op2=term(*pc++);e= e^op2;break;
	default:
		xputc(o); errMsg(" unknown operator");
		longjmp(toplvl,1);
	}
 }
}

Int
term(int c)
{
 Int e,f=0, vmode;
/*printf("termC=%02x\n",c); /**/
 switch(c){
 case '$' :
	e= getHex(&f);
	if(f==0){ /* get Char */
		return xgetc();
	}
/*	printf("hexTerm=%x",e); /**/
	return e;
 case '(' : /*EXPR */
	e=expr(*pc++);
	if(*(pc-1) !=')'){
		errMsg("')' mismatch");
		longjmp(toplvl,1);
	}
	return e;
 case '+' : /*ABS */
	e= term(*pc++);
	return e<0? -e : e;
 case '-' : /* MINUS */
	return -(term(*pc++));
 case '#' : /* NOT */
	return !(term(*pc++));
 case '\'' : /*RAND */
	return rand()%term(*pc++);
#ifdef MODULO_FUNC
 case '%' : /* MOD not yet*/
	return 0;
#endif //MODULO_FUNC
 case '?' : /*input */
	{u_char *ppp, b[LINBUF0SIZ];
	        xgets(b,LINBUF0SIZ);
		ppp=pc;
		pc=b;
		e=expr(*pc++);
		pc=ppp;
		return e;
	}
 case '"' : /*Char const */
	e = *pc++;
	if(*pc++ != '"'){
		errMsg("\" mismatch");
		longjmp(toplvl,1);
	}
	return e;
 }
 if(iSnum(c)){
	pc--; e= getNum(&f);
/*	printf("term=%d",e); /**/
	return e;
 }
/*printf("valiable=%c\n",c); /**/
/* vmode= *pc;*/
 vmode= skipAlpha();
 if(vmode==':' || vmode=='[' || vmode=='{'|| vmode=='(' ){
	pc++;
	e=expr(*pc++);
	if(*(pc-1) !=')'){
		errMsg("var ')' mismatch");
		longjmp(toplvl,1);
	}
/* printf(" v=%c ",c); /**/
	switch(vmode){
	case ':' : return *(((u_char*)VARA(c)+e));
	case '[' : return *(((u_short*)VARA(c)+e));
	case '{' : return *(((u_int*)VARA(c)+e));
	case '(' : return *((((Int*)VARA(c))+e));
	}
 }
 return VARA(c);
}

void
errMsg(u_char *s)
{
 char b[10];
 xputs(s);
 if(lno !=0){
	xputs(" in ");
	sprintf(b,"%lld", lno);
	xputs(b);
 }
 crlf();
}

/* EOF */
