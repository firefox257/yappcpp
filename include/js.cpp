#ifndef JS_CPP
#define JS_CPP
#include <iostream>
#include <string>
#include <mapper.cpp>

void eval(ostream & o, istream & f);

class js_vm_abstract
{
	private:
	string file_name;
	//in the javavm must have out function, str_out function, hash function, end_state, depend_state.
	public:
	int dependancy_flag = 0;
	
	js_vm_abstract()
	{
	}
	~js_vm_abstract()
	{
		clear();
	}
	//creates new context. ostream is for out and str_out put.
	virtual void new_context(ostream & o, string & fname)
	{
		dependancy_flag = 0;
		file_name = fname;
	}
	//shuts down context. 
	virtual void clear()
	{
	}
	//line_number is the start line in the file. It is used to calculate the true line number in the file not in the string. 
	//
	virtual bool eval(string & code, string & reterr, int  line_number)
	{
		return false;
	}
	//pass or fail if fail dependancy needs to be meet.
	virtual bool depend_state(mapper & map)
	{
		return true;
	}
	virtual void end_state(mapper & map)
	{
	}
};

class js_vm
{
	public:
	js_vm_abstract * js;
	js_vm()
	{
		js = 0;
	}
	~js_vm()
	{
		if(js)
		{
			delete(js);
			js = 0;
		}
	}
	inline js_vm_abstract * operator ->()
	{
		return js;
	}
};



#endif