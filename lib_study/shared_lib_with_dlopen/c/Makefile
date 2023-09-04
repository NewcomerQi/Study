# 递归调用子目录中的Makefile
TARGET = main
OBJS = main.o
CFLAGS = -Iinc -Llib -lmy_math -ldl
export SHAREDLIB = libmy_math.so
export LIBDIR = ${shell pwd}/lib
export OBJSDIR = ${shell pwd}/objs
export MKDIR = mkdir -p

${TARGET}:${OBJSDIR} ${OBJS} ${SHAREDLIB}
	${CC} ${OBJSDIR}/${OBJS} -o $@ ${CFLAGS}

${OBJS}:%o:%c
	${CC} -c $< -o ${OBJSDIR}/$@ -Iinc

${SHAREDLIB}:
	${MAKE} -C res

${OBJSDIR}:
	${MKDIR} $@

clean:
	${RM} ${TARGET}
	${RM} -r ${OBJSDIR}
	${RM} -r ${LIBDIR}
