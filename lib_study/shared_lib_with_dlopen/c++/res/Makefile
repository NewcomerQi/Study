OBJS = my_math.cpp
CFLAGS = -shared -Wl,-soname,${SHAREDLIB} -o ${SHAREDLIB} -fPIC
MV = mv
${SHAREDLIB}:${OBJS} ${LIBDIR}
	${CXX} ${CFLAGS} $<
	${MV} $@ ${LIBDIR}
${LIBDIR}:
	${MKDIR} $@

