#include <iostream>

using namespace std;


class try1
{
	public:
	int i1;
	try1()
	{
		i1 = 123;
	}
};
class try2
{
	public: 
	int i2;
	try2()
	{
		i2 = 124;
	}
};
class try3
{
	public:
	try1 t1;
	try2 t2;
	try3()
	{
		
	}
};

int main()
{
	try3 t3;
	t3.t1.i1 = 1111;
	try1 * t1 = (try1*)(&t3);
	t1->i1 = 22222;
	cout << t3.t1.i1 << "\r\n";
	
	return 0;
}