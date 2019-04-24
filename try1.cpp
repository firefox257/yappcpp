/*
 *  Very simple example program
 */
#include <iostream>
#include <string>
using namespace std;
#include "duktape.c"

static duk_ret_t native_print(duk_context *ctx) 
{
	//const char * end1 = " ";
	//duk_push_string(ctx, end1);
	//duk_insert(ctx, 0);
	//duk_join(ctx, duk_get_top(ctx) - 1);
	//printf("%s\n", duk_safe_to_string(ctx, -1));
	
	duk_idx_t check1 =  duk_get_top(ctx);
	cout << "idx: " << check1 << "\r\n";
	for(int i1 = 0; i1 < check1; i1++)
	{
		const char * str1 = duk_to_string(ctx, i1);
		if(str1 == 0) cout << "is end";
		cout << str1 << "||\r\n";
		//duk_pop(ctx);
	}
	
	
		

	
	
	/*cout << duk_safe_to_string(ctx, -1) << "\r\n";
	duk_pop(ctx);
	cout << duk_safe_to_string(ctx, -1) << "\r\n";
	duk_pop(ctx);
	cout << duk_safe_to_string(ctx, -1) << "\r\n";
	duk_pop(ctx);*/
	return 0;
}

static duk_ret_t native_adder(duk_context *ctx) {
	int i;
	int n = duk_get_top(ctx);  /* #args */
	double res = 0.0;

	for (i = 0; i < n; i++) {
		res += duk_to_number(ctx, i);
	}

	duk_push_number(ctx, res);
	return 1;  /* one return value */
}

int main(int argc, char *argv[]) {
	duk_context *ctx = duk_create_heap_default();

	(void) argc; (void) argv;  /* suppress warning */

	duk_push_c_function(ctx, native_print, DUK_VARARGS);
	duk_put_global_string(ctx, "print");
	duk_push_c_function(ctx, native_adder, DUK_VARARGS);
	duk_put_global_string(ctx, "adder");

	duk_eval_string(ctx, R"(
	
	var v1 = {hi: 123, "hi there": 'and "here" \''};
	var s1 =  Duktape.enc('jx', v1);
	
	
	print(s1);
	
	
	function hi()
	{
		eval.call(this, "var i1 = 123;");
	}
	
	eval.call(this, "var i2 = 124;");
	hi();
	print(i1);
	print (i2);
	;)");
	duk_eval_string(ctx, "print('Hello world!', 'next');");

	//duk_eval_string(ctx, "print('2+3=' + adder(2, 3));");
	duk_pop(ctx);  /* pop eval result */

	duk_destroy_heap(ctx);

	return 0;
}
