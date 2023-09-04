OBJS = my_math.o
CFLAGS = -rcs
MV = mv
${STATICLIB}:${LIBDIR} ${OBJS}
	${AR} ${CFLAGS} $@ ${OBJSDIR}/${OBJS}
	${MV} $@ ${LIBDIR}
${LIBDIR}:
	${MKDIR} $@
${OBJS}:%o:%cpp
	${CC} -c $< -o ${OBJSDIR}/$@

