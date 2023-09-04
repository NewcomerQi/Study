#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <my_math.h>

int main()
{
	int a = 10, b = 5;
	typedef MyMath *(*create_my_math)();
	typedef void (*destroy_my_math)(MyMath *);
	void *phandle = NULL;
	phandle = dlopen("./lib/libmy_math.so", RTLD_LAZY);

	if(!phandle)
	{
		const char *perr = dlerror();
		if(perr != NULL)
		{
			printf("error: %s\n",perr);
			exit(-1);
		}
		exit(EXIT_FAILURE);
	}

	create_my_math construct = (create_my_math)dlsym(phandle, "Construct");	// 得到函数地址
	destroy_my_math destruct = (destroy_my_math)dlsym(phandle, "Destruct");

	MyMath *my_math = construct();
	printf("%d + %d = %d\n",a,b, my_math->my_add(a,b));
	printf("%d - %d = %d\n",a,b, my_math->my_sub(a,b));
	printf("%d x %d = %d\n",a,b, my_math->my_mul(a,b));
	printf("%d / %d = %d\n",a,b, my_math->my_div(a,b));

	destruct(my_math);

	dlclose(phandle);
	return 0;
}
