OBJS = div_int.o
all:${OBJS}
${OBJS}:%o:%c
	${CC} -c $< -o ${OBJSDIR}/$@

clean:
	${RM} ${OBJS}