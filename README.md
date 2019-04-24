# README #

### What is this repository for? ###

* Yappcpp is Yet another preprocessor for c++
* Version 0.01

### Overview ###

There are many preprocessors out there and this one (yappcpp) is just another preprocessor for c c++ or for  other c, c++ type syntax languages. It is built using c++ and duktape.

### How to setup? ###

Download the repository. winmake or linuxmak will compile  yappcpp/yappcpp.exe. 

### How to use? ###

* -h or --help get this help.
* -stdio using standard input output for processing
* -i input file outputs to standard output
* -o standared input to output file
* -io inputfile outputfile
* -I include path for source search #jsinclude <name>.
	* Have multiple source search entries. 
* -d process directory
	* Directory processing can have multiple directory entries.  

### What are the Goals? ###

* Provide easy usage while maintaining semblance of code structure. 
* Use javascript as the main processing language with added features such as ```c++ R”( )”``` multi line for template manipulation.
* Process any file by just adding yap at end of file extension.  Example:  try1.cpp to try1.cppyap, try1.java try1.javayap. Exception of .jsyap which is reserved for processing js source libraries. 
* Process directories and sub directories. 
* Include path .jsyapp source files in different locations. 
* Use of standard input/output for chaining processes. 

### Small Tutorial ###

For the source code processing using javascript with the added features. 
```
C++ R”( 

multi line  text
for template or output
in the javascript source.

“);
```

``` out( some string ); ```
The out function will evaluate the string to call out any other functions with in that string. 
str_out(some string); 
Simply dumps out anything in the string with out evaluating it. 

hash(some string) 
Simply calls the c++ hash<string>. 

To put javascript code in the source file you can for example:
```
#include <iostream>
using namespace std;
//
#js
var hi = ‘”hello world attribute”’;
function hi_func()
{
	str_out(‘“hello world function”’);

}
funcion try1()
{
	str_out(“try1”);
}
function saymore()
{
	//evaluates the string and calls try1. 
	out(R”(
	“
		saymore: @try1();
	“
	)”);
}

#endjs


int main()
{
	cout << @hi; << “\r\n”;
	cout << @hi_func(); << “\r\n”;
	cout << @saymore();
	return 0;
}
```
Notice the ```@str;```? That is a one code line of javascript. 
The ```@``` will call a global js function or output a global object. 
The js code can be put in to a separate file temp.jsyap. In the source code put in ```#jsinclude “temp.jsyap”``` or ```#jsinclude <temp.jsyap>```

compile code example   yappcpp -i file.cppyap 
Will output to standared output of the c++ code.
yappcpp -d ./ will compile everyting in that directory and its sub directories. 

```@funct``` code parameters.
Example
```
@funct(name, 
{
	some code here. 


});
```
The above will call the funct function. The paramaters are converted into strings. All returns and tabs and spaces are preserved. 
For example

```@func(     name      ,{}); ```
Argument 1 will have ```“    name     “.```  Argument 2 has ```“{}”```.
More complex example of preprocessing a class using method cascading/ flow. 
```
#js
//easy way to do templating.
var changeout = R"(
(function()
{
	var a1 = temp.split('@@');
	for(var i1 = 1; i1 < a1.length; i1 +=2)
	{
		eval('a1[i1] = ' + a1[i1] + ';');
	}
	temp = a1.join("");
})();
)";

function p_sg(x, y)
{
	x = x.trim();
	y = y.trim();// becouse of code string preserve spaces. 
	var temp = R"(
	protected:
	@@x@@ _@@y@@;
	public:
	N & @@y@@ (@@x@@ d)
	{
		_@@y@@ = d;
		return (*((N*)(this)));
	}
	@@x@@  @@y@@()
	{
		return _@@y@@;
	}
	)";
	eval(changeout);
	out(temp);
}
function tclass(name, code)
{
	name = name.trim();
	var temp = R"(
template<class N>
class @@name@@
{
	@@code@@
	@classpost();
};)";
	eval(changeout);
	out(temp);
}

function dclass(name, ex, code)
{
	name = name.trim();
	ex = ex.trim();
	
	var temp = R"(
class @@name@@: public @@ex@@<@@name@@>
{
	@@code@@
	
};)";
	eval(changeout);
	out(temp);
}


#endjs

@tclass(try1_t,

	@p_sg(int, x);
	@p_sg(int, y);
	@p_sg(char *, title);
	//c++ code 
	public:
	char * hello_world()
	{
		return (char*) "hi world";
	}
	int i;
);
@dclass(try1, try1_t,);
int main()
{
	try1 t1;
	t1.x(123).y(124).title(“hi there”);
	cout << t1.title()<< “\r\n”;
	return 0;
}
```
Hopefully this is readable. There is some code semblance as tclass is for templates. 
dclass is for defining class. Adding in a new primitive attribute is easy to do by just using ``` @p_sg(int, width); ```.

###Gochas!!!###

Right now this is just the first pass impulsive discovery of how to make yappcpp work. 
It is not optimized nor is it “SOLID” structured. 
There is no error detection in the duktape js right now. That will be put in later. 

In the prepcoessor becouse of the ```c++ R”(multi line )”``` strings doing js like this
```
function func()
{
	out(R”(

		char * name()
		{
			return (char*)”name”;
		}
	)”);

}
```
This will error because notice ```(char*)”```name the ```)”``` is the same for the multi line ending. Just put a space in it. 
```return (char*)  “name”;```

Since out function evaluates the string. Can not have partial strings. 
``` out(‘   char * title = “’);
out (‘ “;\r\n’);```

This will error. The code string paramater for ```@``` does bracket counting.
Code string inner strings  ```“” ‘’ R”()”``` are passed over thus need to be completed in the code string. That is why str_out() is needed. 
```str_out(‘   char * title = “’);
str_out (‘ “;\r\n’);```
This can do partial inner string output. 

```@ Code ```string arguments can not have ```,``` because that will look like another argument.
```@func(something, smeofoo(), 
, asdf
, code
, code);```

That is why yappcpp can’t handle all languages. 
```@func(something, somefor() [bla, code, code]);```
This will work becouse of the square bracket. 

```@func();``` needs to have ; to know that it is a single code line string. 
```
#js
function func()
{
	str_out(‘”hi there”’);
}
#endjs

int main()
{
	cout << @func(); << “\r\n”;
	//this is correct. 
	cout << @func() << “\r\n”;
	//is incorrect and will error or not have any output. 
```

### Todo's ###

* Session global object passing and differed dependency processing.
* Implement duktape js error and error exception handling for easy debugging. 
* Code against big projects.
* Optimize and restructure to be readable and reusable.
* Check for memory leaks.
* Implement shared library hooks.
* Integrate with gcc/g++ chain compilation if possible.
* Add in plugin for js function extensions.
* Language enhancements.  
