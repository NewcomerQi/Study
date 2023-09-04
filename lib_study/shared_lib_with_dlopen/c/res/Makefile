OBJS = my_math.c
CFLAGS = -shared -Wl,-soname,${SHAREDLIB} -o ${SHAREDLIB} -fPIC
#gcc -shared -Wl,-soname,libmy_math.so -o libmy_math.so *.c -fPIC
MV = mv
${SHAREDLIB}:${OBJS} ${LIBDIR}
	${CC} ${CFLAGS} $<
	${MV} $@ ${LIBDIR}
${LIBDIR}:
	${MKDIR} $@

