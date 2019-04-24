#include <iostream>
#include <chrono>

using namespace std;

#define t_milli std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define ulong unsigned long long



ulong l1 = 0;
void func1()
{
	l1++;
}

class try1
{
	public:
	static void func2()
	{
		func1();
	}
	
};


int main()
{
	ulong t1, t2;
	t1 = t_milli;
	for(int i1 = 0; i1 < 50000000; i1++)
	{
		func1();
	}
	t2 = t_milli;
	cout << (t2 - t1) << "\r\n";
	
	try1 tt1;
	t1 = t_milli;
	for(int i1 = 0; i1 < 50000000; i1++)
	{
		tt1.func2();
		//func1();
	}
	t2 = t_milli;
	cout << (t2 - t1) << "\r\n";
	cout.flush();
	
	return 0;
	
}