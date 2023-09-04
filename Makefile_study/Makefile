# Makefile 版本1，初始版本
# # 生成main，";" 左边为目标，右边为依赖项
# main:add_int.o sub_int.o mul_int.o div_int.o main.o
# 	gcc -o main add/add_int.o sub/sub_int.o \
# 	mul/mul_int.o div/div_int.o main.o

# # 生成add_int.o的规则，将add_int.c编译成目标文件add_int.o
# add_int.o:add/add_int.c add/add_int.h
# 	gcc -c -o add/add_int.o add/add_int.c

# # 生成sub_int.o的规则，将sub_int.c编译成目标文件sub_int.o
# sub_int.o:sub/sub_int.c sub/sub_int.h
# 	gcc -c -o sub/sub_int.o sub/sub_int.c

# # 生成mul_int.o的规则，将mul_int.c编译成目标文件mul_int.o
# mul_int.o:mul/mul_int.c mul/mul_int.h
# 	gcc -c -o mul/mul_int.o mul/mul_int.c

# # 生成div_int.o的规则，将div_int.c编译成目标文件div_int.o
# div_int.o:div/div_int.c div/div_int.h
# 	gcc -c -o div/div_int.o div/div_int.c

# # 生成main.o的规则
# main.o:main.c add/add_int.h sub/sub_int.h mul/mul_int.h div/div_int.h
# 	gcc -c -o main.o main.c -Iadd -Isub -Imul -Idiv

# # 清理规则
# clean:
# 	rm -f main add/*.o sub/*.o mul/*.o div/*.o *.o

# Makefile版本2，使用预定义变量
# OBJS = add/add_int.o sub/sub_int.o mul/mul_int.o div/div_int.o main.o
# CFLAGS = -Iadd -Isub -Imul -Idiv
# TARGET = main

# ${TARGET}:${OBJS}
# 	${CC} -o ${TARGET} ${OBJS} ${CFLAGS}

# clean:
# 	${RM} ${TARGET} ${OBJS}

# Makefile版本3，使用自动变量
# OBJS = add/add_int.o sub/sub_int.o mul/mul_int.o div/div_int.o main.o
# CFLAGS = -Iadd -Isub -Imul -Idiv
# TARGET = main

# ${TARGET}:${OBJS}
# 	${CC} -o $@ $^ ${CFLAGS}

# ${OBJS}:%o:%c
# 	${CC} -c $< ${CFLAGS} -o $@

# clean:
# 	${RM} ${TARGET} ${OBJS}

# Makefile版本4，使用搜索路径
# OBJS = add_int.o sub_int.o mul_int.o div_int.o main.o
# OBJSDIR = objs
# VPATH = add:sub:mul:div:.
# CFLAGS = -Iadd -Isub -Imul -Idiv
# TARGET = main

# ${TARGET}:${OBJSDIR} ${OBJS}
# 	${CC} -o $@ ${OBJSDIR}/*.o ${CFLAGS}

# ${OBJS}:%o:%c
# 	${CC} -c $< ${CFLAGS} -o ${OBJSDIR}/$@

# ${OBJSDIR}:
# 	mkdir -p ./$@

# clean:
# 	${RM} ${TARGET}
# 	${RM} -r ${OBJSDIR}

#Makefile版本5，使用自动推导
# OBJS = add_int.o sub_int.o mul_int.o div_int.o main.o
# VPATH = add:sub:mul:div:.
# CFLAGS = -Iadd -Isub -Imul -Idiv
# TARGET = main

# ${TARGET}:${OBJS}
# 	${CC} -o $@ $^ ${CFLAGS}

# clean:
# 	${RM} ${TARGET} ${OBJS}

# Makefile版本6，递归调用子目录中的Makefile
# CFLAGS = -Iadd -Isub -Imul -Idiv
# TARGET = main
# export OBJSDIR = ${shell pwd}/objs

# ${TARGET}:${OBJSDIR} main.o
# 	${MAKE} -C add
# 	${MAKE} -C sub
# 	${MAKE} -C mul
# 	${MAKE} -C div
# 	${CC} -o ${TARGET} ${OBJSDIR}/*.o

# main.o:%o:%c
# 	${CC} -c $< -o ${OBJSDIR}/$@ ${CFLAGS}

# ${OBJSDIR}:
# 	mkdir -p $@
# clean:
# 	${RM} ${TARGET}
# 	${RM} -r ${OBJSDIR}

# Makefile版本7，调用函数
CFLAGS = -Iadd -Isub -Imul -Idiv
TARGET = main
DIRS = add sub mul div .
FILES = ${foreach dir, ${DIRS}, ${wildcard ${dir}/*.c}}
OBJS = ${patsubst %.c,%.o,${FILES}}
${TARGET}:${OBJS}
	${CC} -o $@ ${OBJS}
clean:
	${RM} ${TARGET}
	${RM} ${OBJS}
