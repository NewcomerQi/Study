#include <stdio.h>
#include "add_int.h"
#include "sub_int.h"
#include "mul_int.h"
#include "div_int.h"


int main()
{
	int a = 10, b = 5;
	printf("%d + %d = %d\n", a, b, add_int(a, b));
	printf("%d - %d = %d\n", a, b, sub_int(a, b));
	printf("%d x %d = %d\n", a, b, mul_int(a, b));
	printf("%d / %d = %d\n", a, b, div_int(a, b));
	return 0;
}
