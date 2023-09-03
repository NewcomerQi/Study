#include <stdio.h>
#include <my_math.h>

int main()
{
	MyMath my_math;
	int a = 10, b = 5;
	printf("%d + %d = %d\n", a, b, my_math.my_add(a, b));
	printf("%d - %d = %d\n", a, b, my_math.my_sub(a, b));
	printf("%d x %d = %d\n", a, b, my_math.my_mul(a, b));
	printf("%d / %d = %d\n", a, b, my_math.my_div(a, b));
	return 0;
}
