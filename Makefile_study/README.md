# Makefile
使用gcc命令行进行程序编译在单个文件下是比较方便的，当工程中的文件逐渐增多，甚至变得十分庞大的时候，使用gcc命令编译就会变得力不从心。Linux中的make工具提供了一种管理工程的功能，可以方便地进行程序地编译，对更新地文件进行重新编译。

## 基本原则
1. 若想生成目标，检查规则中的依赖条件是否存在，如果不存在，则寻找是否有规则用来生成该依赖文件。
2. 检查规则中的目标是否需要更新，必须先检查它的所有依赖，依赖中有任一个被更新，则目标必须更新。
    - 分析各个目标和依赖之间的关系
    - 根据依赖关系自底向上执行命令
    - 根据修改时间比目标新，确定更新
    - 如果目标不依赖任何条件，则执行时对应命令，以示更新

## Makefile的规则
Makefile的框架是由规则构成的。make命令执行时先在Makefile文件中查找各种规则，对各种规则进行解析后运行规则。规则的基本格式为:
```
TARGET:DEPENDEDS
    COMMAND
    ...
    ...
```
- TARGET: 规则所定义的目标。通常规则是最后生成的可执行文件的文件名或者为了生成可执行文件而依赖的目标文件的文件名，也可以是一个动作，称之为“伪目标”。
- DEPENDEDS: 执行此规则所必须的依赖条件，例如生成可执行文件的目标文件。DEPENDEDS 也可以是某个TARGET，这样就形成了TARGET之间的嵌套。
- COMMAND: 规则所执行的命令，及规则的动作，例如编译文件、生成库文件、进入目录等。动作可以是多个，每个命令占一行。

### 首先看一个多文件的工程示例
```
./
├── add
│   ├── add_int.c
│   └── add_int.h
├── div
│   ├── div_int.c
│   └── div_int.h
├── main.c
├── Makefile
├── mul
│   ├── mul_int.c
│   └── mul_int.h
└── sub
    ├── sub_int.c
    └── sub_int.h
```

1. main.c
```
#include <stdio.h>
#include "add_int.h"
#include "sub_int.h"
#include "mul_int.h"
#include "div_int.h"

int main()
{
	int a = 10, b = 5;
	// 计算两个变量之间的加减乘除
	printf("%d + %d = %d\n", a, b, add_int(a, b));
	printf("%d - %d = %d\n", a, b, sub_int(a, b));
	printf("%d x %d = %d\n", a, b, mul_int(a, b));
	printf("%d / %d = %d\n", a, b, div_int(a, b));
	return 0;
}
```

2. 加操作

add/add.h
```
#ifndef ADD_INT_H_
#define ADD_INT_H_

int add_int(int a, int b);

#endif
```
add/add.c
```
#include "add_int.h"

int add_int(int a, int b)
{
	return a + b;
}
```
3.减操作

sub/sub.h
```
#ifndef SUB_INT_H_
#define SUB_INT_H_

int sub_int(int a, int b);

#endif
```
sub/sub.c
```
#include "sub_int.h"

int sub_int(int a, int b)
{
	return a - b;
}
```

4. 乘操作

mul/mul.h
```
#ifndef MUL_INT_H_
#define MUL_INT_H_

int mul_int(int a, int b);

#endif
```

mul/mul.c
```
#include "mul_int.h"

int mul_int(int a, int b)
{
	return a * b;
}
```

5. 除操作

div/div.h
```
#ifndef DIV_INT_H_
#define DIV_INT_H_

int div_int(int a, int b);

#endif
```

div/div.c
```
#include "div_int.h"

int div_int(int a, int b)
{
	return a / b;
}
```

6. 使用gcc命令行编译所有文件
```
gcc main.c -o main add/add_int.c sub/sub_int.c mul/mul_int.c div/div_int.c  -Iadd -Isub -Imul -Idiv
```
利用上面一条命令生成可执行文件的方法是比较容易的。但是当频繁修改源文件或者当项目中的文件比较多、关系比较复杂的时候，用gcc直接进行编译就会变得十分困难。

使用make进行项目管理，需要一个Makefile文件，make在进行编译的时候，从Makefile文件中读取设置情况，进行解析后运行相关的规则。make程序查找当前目录下的文件Makefile或者makefile，按照其规则运行。例如，建立一个如下规则的Makefile文件。
```
# Makefile版本1
# 生成main，";" 左边为目标，右边为依赖项
main:add_int.o sub_int.o mul_int.o div_int.o main.o
	gcc -o main add/add_int.o sub/sub_int.o \
	mul/mul_int.o div/div_int.o main.o

# 生成add_int.o的规则，将add_int.c编译成目标文件add_int.o
add_int.o:add/add_int.c add/add_int.h
	gcc -c -o add/add_int.o add/add_int.c

# 生成sub_int.o的规则，将sub_int.c编译成目标文件sub_int.o
sub_int.o:sub/sub_int.c sub/sub_int.h
	gcc -c -o sub/sub_int.o sub/sub_int.c

# 生成mul_int.o的规则，将mul_int.c编译成目标文件mul_int.o
mul_int.o:mul/mul_int.c mul/mul_int.h
	gcc -c -o mul/mul_int.o mul/mul_int.c

# 生成div_int.o的规则，将div_int.c编译成目标文件div_int.o
div_int.o:div/div_int.c div/div_int.h
	gcc -c -o div/div_int.o div/div_int.c

# 生成main.o的规则
main.o:main.c add/add_int.h sub/sub_int.h mul/mul_int.h div/div_int.h
	gcc -c -o main.o main.c -Iadd -Isub -Imul -Idiv

# 清理规则
clean:
	rm -f main add/*.o sub/*.o mul/*.o div/*.o *.o
```

6. 利用make工具进行编译
```
$ make
gcc -c -o add/add_int.o add/add_int.c
gcc -c -o sub/sub_int.o sub/sub_int.c
gcc -c -o mul/mul_int.o mul/mul_int.c
gcc -c -o div/div_int.o div/div_int.c
gcc -c -o main.o main.c -Iadd -Isub -Imul -Idiv
gcc -o main add/add_int.o sub/sub_int.o \
mul/mul_int.o div/div_int.o main.o
```
编译完成后在 add、sub、mul、div和当前目录中生成了*.o的目标文件，和main可执行程序。

清理工作
```
$ make clean
rm -f main add/*.o sub/*.o mul/*.o div/*.o *.o
```
则会将生成的目标文件都删除


### 1. 规则的书写规范
- 在书写规则的时候，为了使Makefile更加清晰，要用反斜杠`\` 将较长的行分解为多行，方便阅读；
- 命令行必须以Tab键开始，make程序把出现在一条规则之后的所有连续以Tab键开始的行都作为命令行处理。

### 2. 目标
Makefile的目标可以是具体的文件，也可以是某个动作。例如目标main就是生成main的规则，有很多依赖项及相关的命令动作。而clean是清除当前生成文件的一个动作，不会生成任何目标项。

### 3. 依赖项
依赖项是目标生成所必须满足的条件，例如生成main需要依赖main.o，main.o必须存在才能执行生成main的命令，即依赖项的动作在TARGET的命令之前执行。依赖项之间的顺序按照**自左向右**的顺序检查或者执行。例如，下面的规则：
```
# 生成main.o的规则
main.o:main.c add/add_int.h sub/sub_int.h mul/mul_int.h div/div_int.h
	gcc -c -o main.o main.c -Iadd -Isub -Imul -Idiv
```

### 4. 规则的嵌套
规则之间是可以嵌套的，这通常通过依赖项实现。例如生成main的规则依赖于很多的.o 文件，而每个.o 文件分别是一个规则。要执行规则 main 必须先执行它的依赖项，即 add_int.o、sub_int.o、mul_int.o、div_int.o、main.o，这5个依赖项生成或者存在之后才进行main的命令动作。

### 5. 文件的时间戳
make命令执行的时候会根据文件的时间戳判定是否执行相关的命令，并且执行依赖于此项的规则。例如对main.c文件进行修改后保存，文件的生成日期就发生了改变，再次调用make命令编译的时候，就会只编译main.c，并且执行规则main，重新链接程序。

### 6. 执行的规则
在调用make命令编译的时候，make程序会查找Makefile文件中的第1个规则，分析并执行相关的动作。例子中的第1个规则为main，所以make程序执行main规则。由于其依赖项包含5个，第1个为add_int.o，分析其依赖项，当add/add_int.c add_int.h 存在的时候执行如下命令动作：
```
gcc -c -o add/add_int.o add/add_int.c
```
当命令执行完毕的时候，会按照顺序执行第2个依赖项，生成sub/sub_int.o。当第5个依赖项满足时，即main.o生成的时候，会执行main的命令，链接生成执行文件main。

当把规则clean放到第一个的时候，再执行make命令不是生成main文件，而是清理文件。要生成main文件需要使用如下的make命令:
```
$ make main
```

### 7. 模式匹配
在上面的Makefile中，main.o规则的书写方式如下：
```
# 生成main.o的规则
main.o:main.c add/add_int.h sub/sub_int.h mul/mul_int.h div/div_int.h
	gcc -c -o main.o main.c -Iadd -Isub -Imul -Idiv
```
有一种简便的方法可以实现与上面相同的功能
```
CFLAGS=-Iadd -Isub -Imul -Idiv
main.o:%o:%c
	gcc -c $< -o $@ ${CFLAGS}
```
- "%o:%c" 的作用是将TARGET域的.o的扩展名替换为.c，即将main.o 替换为 main.c
- "$<" 表示依赖项的结果，即main.c
- "$@" 表示TARGET域的名称，即main.o
- "${CFLAGS}" 一个自定义的变量，查找所有所需头文件的目录


## Makefile中使用变量
### 1. Makefile中的用户自定义变量
使用Makefile进行规则定义的时候，用户可以定义自己的变量，称为用户自定义变量。

例如，可以用变量来表示上述的文件名，定义OBJS变量表示目标文件：
```
OBJS = add/add_int.o sub/sub_int.o mul/mul_int.o div/div_int.o main.o
main:${OBJS}
	gcc -o main ${OBJS}
```

用CC变量表示gcc，用CFLAGS表示编译选项，RM表示 rm -f，TARGET表示最终的生成目标main:
```
CC = gcc    # CC定义成gcc
CFLAGS = -Iadd -Isub -Imul -Idiv    # 加入头文件搜索路径
TARGET = main   # 最终生成目标
RM = rm -f      # 删除命令
```
之前冗长的Makefile可以简化成如下方式
```
# Makefile 版本2
OBJS = add/add_int.o sub/sub_int.o mul/mul_int.o div/div_int.o main.o
CC = gcc
CFLAGS = -Iadd -Isub -Imul -Idiv
TARGET = main
RM = rm -f

${TARGET}:${OBJS}
	${CC} -o ${TARGET} ${OBJS}

${OBJS}:%o:%c
	${CC} -c ${CFLAGS} $< -o $@

clean:
	${RM} ${TARGET} ${OBJS}
```

### 2. Makefile中预定义变量
在Makefile中有一些已经定义的变量，用户可以直接使用这些变量，不用进行定义。

变量名 | 含义 | 默认值
--- | --- | ---
AR | 生成静态库文件的程序名称 | ar
AS | 汇编编译器的名称 | as
CC | C语言编译器的名称 | cc
CPP | C语言编译器的名称 | ${CC} -E
CXX | C++语言编译器的名称 | g++
FC | FORTRAN语言编译器的名称 | f77
RM | 删除文件程序的名称 | rm -f
ARFLAGS | 生成静态库库文件程序的选项 | 无默认值
ASFLAGS | 汇编语言编译器的编译选项 | 无默认值
CFLAGS | C语言编译器的编译选项 | 无默认值
CPPFLAGS | C语言预编译的编译选项 | 无默认值
CXXFLAGS | C++语言编译器的编译选项 | 无默认值
FFLAGS | FORTRAN语言编译器的编译选项| 无默认值


根据上述预定义的变量，Makefile可以再次简化:
```
# Makefile版本3
OBJS = add/add_int.o sub/sub_int.o \
mul/mul_int.o div/div_int.o main.o          # 目标文件
CFLAGS = -Iadd -Isub -Imul -Idiv            # 编译选项
TARGET = main                               # 目标文件

${TARGET}:${OBJS}                           # TARGET目标文件，需要先生成OBJS目标
	${CC} -o ${TARGET} ${OBJS} ${CFLAGS}    # 生成可执行文件

clean:                                      # 清理
	${RM} ${TARGET} ${OBJS}                 # 删除所有的目标文件和可执行文件
```

### 3. Makefile中的自动变量
Makefile中的变量除了用户自定义变量和预定义变量外，还有一类自动变量。Makefile中的编译语句中经常会出现目标文件和依赖文件，自动变量代表这些目标文件和依赖文件。

变量 | 含义
--- | ---
$* | 表示目标文件的名称，不包含目标文件的扩展名
$+ | 表示所有的依赖文件，这些依赖文件之间以空格分开，按照出现的先后顺序，其中可能包含重复的依赖文件
$< | 表示依赖项中第一个依赖文件的名称
$? | 依赖项中，所有目标文件时间戳晚的依赖文件，依赖文件之间以空格分开
$@ | 目标项中目标文件的名称
$^ | 依赖项中，所有不重复的依赖文件，这些文件之间以空格分开

按照Makefile中的自动变量，重新对上述示例Makefile进行编写：
```
# Makefile版本4
OBJS = add/add_int.o sub/sub_int.o mul/mul_int.o div/div_int.o main.o
CFLAGS = -Iadd -Isub -Imul -Idiv
TARGET = main

${TARGET}:${OBJS}
	${CC} -o $@ $^ ${CFLAGS}

${OBJS}:%o:%c
	${CC} -c $< ${CFLAGS} -o $@

clean:
	${RM} ${TARGET} ${OBJS}
```
- $@ 表示依赖项中的文件名称
- $< 表示目标文件的名称
- $^ 表示依赖项中，所有不重复的依赖文件，这些文件之间以空格分开

下列两段代码是一致的

使用自动变量：
```
${TARGET}:${OBJS}
	${CC} -o $@ $^ ${CFLAGS}
```

不使用自动变量：
```
${TARGET}:${OBJS}
	${CC} -o ${TARGET} ${OBJS} ${CFLAGS}
```

## 搜索路径
在大的系统中，通常存在很多目录，手动添加目录的方法不仅十分笨拙而且容易出错。Makefile 的目标搜索功能提供了一个解决此问题的方法，指定需要搜索的目录，make会自动找到指定文件的目录并添加到文件上，VPATH变量可以实现此目的。VPATH变量的使用方法如下：
```
VPATH=path1:path2:...
```
VPATH右边是冒号（:）分隔的路径名称，例如下面的指令
```
VPATH=add:sub:mul:div   # 加入add、sub、mul和div搜索路径
add_int.o:%o:%c
    ${CC} -c -o $@ $<
```
Makefile的搜索路径包含add、sub、mul和div目录。add_int.o规则自动扩展如下代码：
```
add_int.o:add/add_int.c
    cc -c -o add_int.o add/add_int.c
```

将路径名去掉以后可以重新编译程序，但是会发现目标文件都放到了当前的目录下，这样会对文件的规范造成危害。可以将输出的目标文件放到同一个目录下来解决此问题：
```
# Makefile版本5
OBJS = add_int.o sub_int.o mul_int.o div_int.o main.o
OBJSDIR = objs
VPATH = add:sub:mul:div:.
CFLAGS = -Iadd -Isub -Imul -Idiv
TARGET = main

${TARGET}:${OBJSDIR} ${OBJS}                # 要执行TARGET的命令，先查看OBJSDIR和OBJS依赖项是否存在
	${CC} -o $@ ${OBJSDIR}/*.o ${CFLAGS}    # 将OBJSDIR目录中的所有.o 文件链接成main

${OBJS}:%o:%c                               # 将扩展名为.o 的文件替换成扩展名为.c 的文件
	${CC} -c $< ${CFLAGS} -o ${OBJSDIR}/$@  # 生成目标文件，存放在OBJSDIR目录中

${OBJSDIR}:
	mkdir -p ./$@                           # 当OBJSDIR不存在时，建立目录，-p选项可以忽略父目录不存在的错误

clean:
	${RM} ${TARGET}                         # 删除main
	${RM} -r ${OBJSDIR}                     # 删除OBJSDIR目录
```

## 自动推导规则
使用命令make编译扩展名为.c和C语言文件的时候，源文件的编译规则不用明确给出。这是因为 make 进行编译的时候会使用一个默认的编译规则，按照默认规则完成对.c文件的编译，生成对应的.o文件。它执行命令cc -c来编译.c源文件。

在Makefile中只要给出需要重建的目标文件名（一个.o文件），make会自动为这个.o文件寻找合适的依赖文件（对应的.c文件），并且使用默认的命令来构建这个目标文件。

对于上述的例子，默认规则是使用命令cc -c main.c -o main.o 来创建文件main.o。对一个目标文件是“文件名.o”，依赖文件是“文件名.c”的规则，可以省略其编译规则的命令行，由make命令决定如何使用编译命令和选项。此默认规则称为**make的隐含规则**。

这样，在书写Makefile时，可以省略描述.c文件和.o依赖关系的规则，而只需要给出那些特定的规则描述（.o目标所需要的.h文件）。因此上面的例子可以使用更加简单的方式书写：
```
# Makefile版本6
OBJS = add_int.o sub_int.o mul_int.o div_int.o main.o
VPATH = add:sub:mul:div:.
CFLAGS = -Iadd -Isub -Imul -Idiv
TARGET = main

${TARGET}:${OBJS}               # OBJS依赖项的规则自动生成
	${CC} -o $@ $^ ${CFLAGS}    # 链接文件

clean:
	${RM} ${TARGET} ${OBJS}
```

## 递归make
当有多人在多个目录下进行程序开发，并且每个人负责一个模块，而文件在相对独立的目录中，这是由同一个Makefile维护代码的编译就会十分蹩脚，因为他们对自己目录下的文件增减都要修改此Makefile，这通常会造成项目的维护问题。

### 1. 递归调用的方式
make命令有递归调用的作用，它可以递归调用每个子目录中的Makefile。例如，在当前目录下有一个Makefile，而目录add、sub、mul和div及主控文件main.c由不同的人进行维护，可以用如下方式编译add中的文件：
```
add:
    cd add && ${MAKE}
```
它等价于
```
add:
    ${MAKE} -C add
```
上面两个例子的意思都是先进入add目录，然后执行make命令。

### 2. 总控Makefile
调用 "${MAKE} -C" 的Makefile叫做**总控Makefile**。如果总控Makefile中的一些变量需要传递给下层的Makefile，可以使用export命令，例如需要向下层Makefile传递目标文件的导出路径
```
export OBJSDIR = ./objs
```

例如上面的文件布局，需要在add、sub、mul和div目录下分别编译，总控Makefile代码如下：
```
# Makefile版本7
CFLAGS = -Iadd -Isub -Imul -Idiv
TARGET = main
export OBJSDIR = ${shell pwd}/objs          # 生成当前目录的路径字符串，并赋值给OBJSDIR，外部可调用

${TARGET}:${OBJSDIR} main.o
	${MAKE} -C add                          # 在目录add中递归调用make
	${MAKE} -C sub                          # 在目录sub中递归调用make
	${MAKE} -C mul                          # 在目录mul中递归调用make
	${MAKE} -C div                          # 在目录div中递归调用make
	${CC} -o ${TARGET} ${OBJSDIR}/*.o       # 生成main.o 放到OBJSDIR中

main.o:%o:%c                                # main.o 规则
	${CC} -c $< -o ${OBJSDIR}/$@ ${CFLAGS}

${OBJSDIR}:
	mkdir -p $@
clean:
	${RM} ${TARGET}
	${RM} -r ${OBJSDIR}
```

生成的目标文件放都到了./objs目录中，export了一个变量OBJSDIR。其中的${shell pwd}是执行一个shell命令pwd获取总控Makefile的当前目录。

### 3. 子目录Makefile的编写
add目录下的Makefile：
```
OBJS = add_int.o
all:${OBJS}
${OBJS}:%o:%c
	${CC} -c $< -o ${OBJSDIR}/$@
clean:
	${RM} ${OBJS}
```

sub目录下的Makefile：
```
OBJS = sub_int.o
all:${OBJS}
${OBJS}:%o:%c
	${CC} -c $< -o ${OBJSDIR}/$@

clean:
	${RM} ${OBJS}
```

mul目录下的Makefile：
```
OBJS = mul_int.o
all:${OBJS}
${OBJS}:%o:%c
	${CC} -c $< -o ${OBJSDIR}/$@

clean:
	${RM} ${OBJS}
```

div目录下的Makefile：
```
OBJS = div_int.o
all:${OBJS}
${OBJS}:%o:%c
	${CC} -c $< -o ${OBJSDIR}/$@

clean:
	${RM} ${OBJS}
```

## Makefile中的函数
在比较大的工程中，经常需要一些匹配操作或者自动生成规则的功能。
### 1. 获取匹配模式的文件名wildcard
这个函数的功能是查找当前目录下所有符合**模式PATTERN**的文件名，其返回值是以空格分割的、当前目录下的所有符合模式PATTERN的文件名列表。其原型如下：
```
${wildcard PATTERN}
```
例如，如下模式返回当前目录下所有扩展名为.c的文件列表：
```
${wildcard *.c}
```

### 2. 模式替换函数patsubst
这个函数的功能是查找字符串text中按照空格分开的单词，将符合模式pattern的字符串替换成replacement。pattern中的模式可以使用通配符，%代表0个到n个字符，当pattern和replacement中都有%时，符合条件的字符串将被replacement中的替换。函数返回值时替换后的新字符串。其原型如下：
```
${patsubst pattern, replacement, text}
```
返回值：例如需要将C文件替换为.o的目标文件可以使用如下模式：
```
${patsubst %.c,%.o, add.c}
```
上面的模式将add.c字符串作为输入，当扩展名为.c时符合模式%.c，其中%在这里代表add，替换为add.o，并作为输出字符串。

```
${patsubst %.c,%.o, ${wildcard *.c}}
```
输出的字符串将当前扩展名为.c的文件替换成扩展名为.o的文件列表。

### 3. 循环函数foreach
函数原型：
```
${foreach VAR,LIST,TEXT}
```
函数的功能为foreach将List字符串中一个空格分隔的单词，先传给临时变量VAR，然后执行TEXT表达式，TEXT表达式处理结束后输出。其返回值是空格分隔表达式TEXT的计算结果。

例如，对于存在add和sub的两个目录，设置DIRS为"add sub mul div ./" 包含目录add、sub、mul、div和当前目录。表达式${wildcard ${dir}/*.c}，可以取出目录add、sub、mul、div和当前目录中的所有扩展名为.c的C语言源文件
```
DIRS = sub add mul div ./
FILES = ${foreach dir, ${DIRS}, ${wildcard ${dir}/*.c}}
```

利用上面几个函数对原有的Makefile文件进行重新编写，使新的Makefile可以自动更新各个目录下的C语言源文件：
```
# Makefile版本8
CFLAGS = -Iadd -Isub -Imul -Idiv
TARGET = main
DIRS = add sub mul div .            #DIRS字符串的值为目录add、sub、mul、div和当前目录
FILES = ${foreach dir, ${DIRS}, ${wildcard ${dir}/*.c}}     # 查找所用目录下的扩展名为.c的文件，赋值给变量FILES
OBJS = ${patsubst %.c,%.o,${FILES}}     # 替换字符串，将扩展名为.c的替换成扩展名为.o
${TARGET}:${OBJS}                       # OBJS依赖项规则默认生成
	${CC} -o ${TARGET} ${OBJS}          # 生成main
clean:
	${RM} ${TARGET}
	${RM} ${OBJS}
```

## make命令的参数
参数 | 含义
--- | ---
-h, --help | 帮助文档
-v, --version | 打印 make 的版本号并退出
-n, --just-print, --dry-run, --recon | 模拟执行make、make clean命令
-f [FILE], --file=FILE |  指定文件执行 make 命令。例如 make -f Makefile1
-j [N], --jobs[=N] | 同时允许 N 个任务；无参数表明允许无限个任务
-b, -m | 忽略兼容性
-B, --always-make  | 无条件 make 所有目标。
-C DIRECTORY, --directory=DIRECTORY | 在执行前先切换到 DIRECTORY 目录
-d  | 打印大量调试信息
--debug[=FLAGS] | 打印各种调试信息
-e, --environment-overrides | 环境变量覆盖 makefile 中的变量
--eval=字串 | 将 <字串> 作为 makefile 语句估值
-i, --ignore-errors  | 忽略来自命令配方的错误
-I DIRECTORY, --include-dir=DIRECTORY | 在 DIRECTORY 中搜索被包含的 makefile
-k, --keep-going | 当某些目标无法创建时仍然继续
-l [N], --load-average[=N], --max-load[=N] |  N 时不启动多任务。
-L, --check-symlink-times | 使用软链接及软链接目标中修改时间较晚的一个
-n, --just-print, --dry-run, --recon | 只打印命令配方，不实际执行。
-o FILE, --old-file=FILE, --assume-old=FILE | 将 FILE 当做很旧，不必重新生成
-O[类型], --output-sync[=类型] | 使用 <类型> 方式同步并行任务输出。
-p, --print-data-base | 打印 make 的内部数据库
-q, --question  | 不运行任何配方；退出状态说明是否已全部更新
-r, --no-builtin-rules | 禁用内置隐含规则
-R, --no-builtin-variables | 禁用内置变量设置
-s, --silent, --quiet  | 不输出配方命令
-S, --no-keep-going, --stop | 关闭 -k
-t, --touch  | touch 目标而不是重新创建它们
--trace  | 打印跟踪信息
-v, --version   | 打印 make 的版本号并退出
-w, --print-directory  | 打印当前目录
--no-print-directory |  关闭 -w，即使 -w 默认开启
-W FILE, --what-if=FILE, --new-file=FILE, --assume-new=FILE | 将 FILE 当做最新
--warn-undefined-variables | 当引用未定义变量的时候发出警告
