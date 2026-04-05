#

#CFLAGS =  -DEMACS -DCOMPAT_43
CFLAGS = -g -DEMACS -DUNIX

all: gi

#
#	GAME interpriter
#
OBJS =	ggii.o lined64.o linux.o
SRCS =	ggii.c lined64.c linux.c
##OBJS =	ggii.o lined.o bsd.o
##SRCS =	ggii.c lined.c bsd.c
#OBJS =	ggii.o  bsd.o
#SRCS =	ggii.c  bsd.c

gi: ${OBJS}
	${CC} ${CFLAGS} -o gi ${OBJS}

ggii.o:	ggii.c
	${CC} ${CFLAGS} -c ggii.c

lined64: lined64.c
	${CC} ${CFLAGS} -DTEST -o lined64 lined64.c linux.o

# tar
DATE=`date +"%Y%m%d-%H%M%S"`
TAR_DIR=game-int64
tar:
	(cd ..; tar cvJf ${TAR_DIR}-${DATE}.txz ${TAR_DIR})



#
#	install
#
install:

#
#	make shar
#
shar:
	shar	readme.j Makefile $(SRCS) > gi.shar


#
#	clean
#
clean:
	rm -f *.o gi

