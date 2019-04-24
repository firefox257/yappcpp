#ifndef JS_DUKTAPE_CPP
#define JS_DUKTAPE_CPP

#include <string>
#include <sstream>
#include <duktape.h>


class js_duktape: public js_vm_abstract
{
	private:
	
	string file_name;
	duk_context *ctx;
	
	static ostream * outp;//could be file or cout manyly for out and str_out;
	public:
	mapper * pmap;
	js_duktape()
	{
		ctx = 0;
		
	}
	~js_duktape()
	{
		clear();
	}
	//creates new context.
	virtual void new_context(ostream & o, string & fname)
	{
		dependancy_flag = 0;
		file_name = fname;
		outp = &o;
		if(ctx) clear();
		ctx = duk_create_heap_default();
		
		duk_push_c_function(ctx, js_duktape::native_out, DUK_VARARGS);
		duk_put_global_string(ctx, "out");
		
		duk_push_c_function(ctx, js_duktape::native_str_out, DUK_VARARGS);
		duk_put_global_string(ctx, "str_out");
		
		duk_push_c_function(ctx, js_duktape::native_hash, DUK_VARARGS);
		duk_put_global_string(ctx, "hash");
		
		duk_push_c_function(ctx, js_duktape::native_end_state, DUK_VARARGS);
		duk_put_global_string(ctx, "__c_end_state");
		
		duk_push_c_function(ctx, js_duktape::native_depend_state, DUK_VARARGS);
		duk_put_global_string(ctx, "__c_depend_state");
		
		//unsigned long long  d1 = (unsigned long long)((void*)this);
		//string setptr = (string)"var ___SELF_PTR = " + d1 + ";";
		//duk_eval_string(ctx, setptr.c_str());
		stringstream ss;
		ss << "var ___SELF_PTR = " << this << ";";
		duk_eval_string(ctx, ss.str().c_str());
		
		duk_eval_string(ctx, "var _state;");
		
		duk_eval_string(ctx, R"(
		var JSON = {
			stringify: function(d)
			{
				return Duktape.enc('jx', d);
			}
		};
		)");
		
		
	}
	//shuts down context. 
	virtual void clear()
	{
		duk_pop(ctx);  /* pop eval result */
		duk_destroy_heap(ctx);
	}
	
	//line_number is the start line in the file. It is used to calculate the true line number in the file not in the string. 
	virtual bool eval(string & code, string & reterr, int line_number)
	{
		bool haserror = false;
		int errorint = duk_peval_string(ctx, code.c_str());
		if ( errorint!= 0) 
		{
			reterr =  (string)"js failed: |" +  duk_safe_to_string(ctx, -1) +  "|\r\n";
			//+ js_eval_state_string << "\r\n";
			haserror = true;
		}
		return haserror;
	} 
	
	virtual void end_state(mapper & map)
	{
		pmap = &map;
		 string cmd = (string)R"(
				
			if(typeof _end_state !=="undefined")
			{
				_state = function(g, n, d)
				{
					g = g.trim();
					n = n.trim();
					__c_end_state(___SELF_PTR, g, n,  Duktape.enc('jx', d));
				}
				_end_state();
			}
				
		)";
		duk_eval_string(ctx, cmd.c_str());
		
	}
	
	bool dependancymeet;
	virtual bool depend_state(mapper & map)
	{
		dependancymeet = true;
		pmap = &map;
		string cmd = (string)R"(
			if(typeof _depend_state !=="undefined")
			{
				_state = function(g, n)
				{
					g = g.trim();
					n = n.trim();
					__c_depend_state(___SELF_PTR, g, n);
				}
				_depend_state();
			}
		)";
		duk_eval_string(ctx, cmd.c_str());
		if(!dependancymeet) dependancy_flag = 1;
		return dependancymeet;
	}
	
	private:
	static duk_ret_t native_out(duk_context *ctx) 
	{

		int nargs = duk_get_top(ctx);
		if(nargs <= 0) return 0;
		stringstream ss;
		ss.str(duk_safe_to_string(ctx, 0));
		
		::eval((*outp), ss);
		
		return 0;
	}

	static duk_ret_t native_str_out(duk_context *ctx) 
	{
		int nargs = duk_get_top(ctx);
		if(nargs <= 0) return 0;
		(*outp) << duk_safe_to_string(ctx, 0);
		
		return 0;
	}

	static duk_ret_t native_hash(duk_context *ctx) 
	{
		int nargs = duk_get_top(ctx);
		if(nargs <= 0) return 0;
		string str1 = duk_safe_to_string(ctx, 0);
		
		hash<string> hash_fn;
	 
		unsigned long long hash1 = hash_fn(str1);
		string hashs = to_string(hash1);
		duk_push_string(ctx, hashs.c_str());
		
		return 1;  /* one return value */
	}

	static duk_ret_t native_end_state(duk_context *ctx) 
	{
		int nargs = duk_get_top(ctx);
		if(nargs != 4) 
		{
			cout << "End state does not have the right amount of arguments.\r\n";
			return 0;
		}
		js_duktape* jptr = (js_duktape*)duk_to_uint32(ctx, 0);//ptr
		
		string arg1 = duk_safe_to_string(ctx, 1);//var global name
		string arg2 = duk_safe_to_string(ctx, 2);//var name data
		string arg3 = duk_safe_to_string(ctx, 3);//json data
		
		//cout << "end state file_name: " << jptr->file_name << " arg1: " << arg1 << " arg2: " << arg2 << " arg3: " << arg3 << "\r\n";
		(*jptr->pmap)->set(jptr->file_name, arg1, arg2, arg3);
		
		return 0;
	}
	
	static duk_ret_t native_depend_state(duk_context *ctx) 
	{
		int nargs = duk_get_top(ctx);
		
		if(nargs != 3) 
		{
			cout << "depend state does not have the right amount of arguments.\r\n";
			return 0;
		}
		js_duktape* jptr = (js_duktape*)duk_to_uint32(ctx, 0);//ptr
		
		string arg1 = duk_safe_to_string(ctx, 1);//var global name
		string arg2 = duk_safe_to_string(ctx, 2);//var name data
		
		//cout << "dpend state file_name: " << jptr->file_name << " arg1: " << arg1 << " arg2: " << arg2 << "\r\n";
		string v1;
		bool havevalue = (*jptr->pmap)->get(arg1, arg2, v1);
		//cout << "v1 " << v1 << "\r\n";
		
		if(havevalue && jptr->dependancymeet)
		{
			string cmd = "var " + arg2 + " = " + v1 + ";";
			duk_eval_string(jptr->ctx, cmd.c_str());
		}
		else
		{
			jptr->dependancymeet = false;
		}
		
		
		return 0;
	}
	
	
	
};
ostream * js_duktape::outp;


#endif