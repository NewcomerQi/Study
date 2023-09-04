#ifndef MY_MATH_H_
#define MY_MATH_H_

class MyMath
{
	public:
		int my_add(int a, int b);
		int my_sub(int a, int b);
		int my_mul(int a, int b);
		int my_div(int a, int b);
};

// extern "C"
// {
extern "C" MyMath *Construct()
{
	return new MyMath;
}
extern "C" void Destruct(MyMath *my_math)
{
	delete my_math;
}
// };

#endif
