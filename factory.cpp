#ifndef FACTORY_CPP
#define FACTORY_CPP
#include <js.cpp>
#include <js_duktape.cpp>
#include <mem_mapper.cpp>
class factory
{
	public:
	static void get_jsvm(js_vm & vm)
	{
		vm.js = new js_duktape();
	}
	static void get_mapper(mapper & map)
	{
		map.map = new mem_mapper();
	}
};


#endif

