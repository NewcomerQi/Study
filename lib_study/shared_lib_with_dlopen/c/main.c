#include <stdio.h>
#include <my_math.h>
#include <dlfcn.h>
#include <stdlib.h>
int main()
{
	int a = 10, b = 5;
	int (*my_add)(int a, int b);
	int (*my_sub)(int a, int b);
	int (*my_mul)(int a, int b);
	int (*my_div)(int a, int b);
	void* phandle = NULL;
	phandle = dlopen("./lib/libmy_math.so", RTLD_LAZY);
	if(!phandle)
	{
		const char *perr = dlerror();
		if(perr != NULL)
		{
			printf("%s\n", perr);
			exit(EXIT_FAILURE);
		}
	}
	my_add = dlsym(phandle, "my_add");
	my_sub = dlsym(phandle, "my_sub");
	my_mul = dlsym(phandle, "my_mul");
	my_div = dlsym(phandle, "my_div");

	printf("%d + %d = %d\n", a, b, my_add(a, b));
	printf("%d - %d = %d\n", a, b, my_sub(a, b));
	printf("%d x %d = %d\n", a, b, my_mul(a, b));
	printf("%d / %d = %d\n", a, b, my_div(a, b));

	dlclose(phandle);
	return 0;
}
