#include <stdlib.h>


class A;

class Base
{
public:
	virtual ~Base() {}
	Base() {}
	virtual void func(A* s) = 0;

	int x;
};

class A
{
public:
	~A(){}
	A() {}

	Base *a;
	char arr[500];
};

class B : public Base
{
public:
	void func(A* s)
	{
		s->a = this;
	}

	size_t a;
};

#include <iostream>

int main()
{


	A x;

	B *z = new B;

	z->func(&x);

	std::cout << "B *z = " << z << " | A &x = " << &x << " | Base *x.a = " << x.a << std::endl;
	std::cout << "static_cast Z " << static_cast<B *>(x.a) << std::endl;
}