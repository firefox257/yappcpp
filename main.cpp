#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <stdio.h>
#include <unordered_map>
#include <list>
#include <utility>
#include "dir.cpp"

void eval(ostream & o, istream & f);



#include "factory.cpp"
js_vm jsvm;
mapper map;

using namespace std;


bool init_factory()
{
	factory::get_jsvm(jsvm);
	factory::get_mapper(map);
	return true;
}
bool have_factory = init_factory();


string js_eval_state_string = "";

unordered_map<string, unordered_map<string, string> > state_file;
string at_file_name = "";

bool isalpha(char c)
{
	if((c >='a' && c <='z') || (c >='A' && c <='Z')) return true;
	return false;
}
bool isnum(char c)
{
	if(c >='0' && c <='9' || c == '.') return true;
	return false;
}
bool isword(char c)
{
	if(isalpha(c) || isnum(c) || c == '#' || c == '$' || c == '_' || c == '.' || c == '$') return true;
	return false;
}
bool isspace(char c)
{
	if(c == ' ' || c == '\t' || c == '\r' || c == '\n')return true;
	return false;
}
bool ishspace(char c)
{
	if(c == ' ' || c == '\t')return true;
	return false;
}

void getword(ostream & o, istream & f, string & s)
{
	stringstream b1;
	char c;
	
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(!isword(c))
		{
			
			f.unget();
			break;
		}
		b1 << c;
	}
	s = b1.str();
}


void jsfileinclude(ostream & o, istream & f, string & s);

void getjscode(ostream & o, istream & f, string & s)
{
	stringstream b1;
	char c;
	string word;
	while(true)
	{
		
		c = f.get();
		if(f.eof()) break;
		if(c == '#')
		{
			getword(o, f, word);
			if(word == "endjs")
			{
				break;
			}
			else if(word == "jsinclude")
			{
				string code1= "";
				jsfileinclude(o, f, code1);
				b1 << code1;
			}
			else
			{
				b1 << word;
			}
		}
		else if(c == 'R')
		{
			char c1 = f.get();
			if(c1 == '"')
			{	f.get();
				b1 << '"';
				while(true)
				{
					c = f.get();
					if(c == '\\')
					{
						b1 << "\\\\";
						c = f.get();
						if(c == '"')
						{
							b1 << "\\\"";
						}
						else
						{
							b1 << c;
						}
					}
					else if(c == '\t')
					{
						b1 << "\\t";
					}
					else if(c == '\r')
					{
						b1 << "\\r";
					}
					else if(c == '\n')
					{
						b1 << "\\n";
					}
					else if(c == '"')
					{
						b1 << "\\\"";
					}
					else if(c == ')')
					{
						
						c1 = f.get();
						if(c1 == '"')
						{
							b1 << c1;
							break;
						}
						else
						{
							f.unget();
							b1 << c;
						}
						
					}
					else
					{
						b1 << c;
					}
				}//end of R"( )"
			}
			else
			{
				b1 << c << c1;
			}
		}
		else
		{
			if(c == '\r') b1 << c;
			else if(c == '\n') b1 << c;
			b1 << c;
			
		}
	}
	s = b1.str();
}
void pass_multiline_string(ostream & o, istream & f)
{
	char c;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(f.eof())
		{
			cout << "\r\nError out eval on R\"(string)\" eof.\r\n";
			break;
		}
		if(c == '\\')
		{
			o << c;
			c = f.get();
			o << c;
		}
		else if(c == ')')
		{
			o << c;
			c = f.get();
			if(c == '"')
			{
				o << c;
				break;
			}
		}
		o << c;
	}
	
}
void pass_double_string(ostream & o, istream & f)
{
	char c;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(f.eof())
		{
			cout << "\r\nError out eval on \"string\" eof.\r\n";
			break;
		}
		if(c == '\\')
		{
			o << c;
			c = f.get();
			o << c;
		}
		else if(c == '"')
		{
			o << c;
			break;
		}
		else
		{
			o << c;
		}
	}
}
void pass_single_string(ostream & o, istream & f)
{
	char c;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(f.eof())
		{
			cout << "\r\nError out eval on 'string; eof.\r\n";
			break;
		}
		if(c == '\\')
		{
			o << c;
			c = f.get();
			o << c;
		}
		else if(c == '\'')
		{
			o << c;
			break;
		}
		else
		{
			o << c;
		}
	}
	
}

void get_code_string_param(ostream & o, istream & f, string & s, stringstream & spaces)
{
	stringstream b1;
	char c;
	int ccount = 0;
	b1 << '"';
	b1 << spaces.str();
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(c == '{' || c == '(' || c == '[')
		{
			ccount++;
			b1 << c;
		}
		else if(c == '}' || c == ']')
		{
			ccount--;
			b1 << c;
		}
		else if(c == ')')
		{
			
			if(ccount <= 0)
			{
				f.unget();
				break;
			}
			ccount--;
			b1 << c;
		}
		else if(c == ',')
		{
			if(ccount <= 0) 
			{
				f.unget();
				break;
			}
			b1 << c;
		}
		else if(c == '\\')
		{
			b1 << "\\\\";
		}
		else if(c == '\t')
		{
			b1 << "\\t";
		}
		else if(c == '\r')
		{
			b1 << "\\r";
		}
		else if(c == '\n')
		{
			b1 << "\\n";
		}
		else if(c == '"')
		{
			b1 << "\\\"";
			
			while(true)
			{
				c = f.get();
				if(c == '\\')
				{
					b1 << "\\\\";
					c = f.get();
					if(c == '"')
					{
						b1 << "\\\"";
					}
					else
					{
						b1 << c;
					}
					
				}
				else if(c == '\t')
				{
					b1 << "\\t";
				}
				else if(c == '"')
				{
					b1 << "\\\"";
					break;
				}
				else
				{
					b1 << c;
				}
				
			}
		}
		else if(c == 'R')
		{
			b1 << c;
			c = f.get();
			if(c == '"')
			{
				b1 << '\\' << c;
				while(true)
				{
					c = f.get();
					if(c == '\\')
					{
						b1 << "\\\\";
						c = f.get();
						b1 << c;
					}
					else if(c == '\t')
					{
						b1 << "\\t";
					}
					else if(c == '\r')
					{
						b1 << "\\r";
					}
					else if(c == '\n')
					{
						b1 << "\\n";
					}
					else if(c == ')')
					{
						b1 << c;
						c = f.get();
						if(c == '"')
						{
							b1 << '\\' << c;
							break;
						}
						else
						{
							b1 << c;
						}
						
					}
					else
					{
						b1 << c;
					}
					
				}// end R"(  )" string
			}
			else
			{
				b1 << c;
			}
			
		}
		else
		{

			b1 << c;
			
		}
	}

	b1 << '"';
	s = b1.str();
	
	
}

void get_double_encode_string(ostream & o, istream & f, string & s)
{
	stringstream b1;
	char c;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(c == '\\')
		{
			b1 << "\\\\";
			b1 << c;

		}
		else if(c == '"')
		{
			break;
		}

		b1 << c;
		
	}

	s = b1.str();
}
void get_num_param(ostream & o, istream & f, string &s, stringstream & spaces)
{
	stringstream b1;
	char c;
	b1 << spaces.str();
		
	
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(!isnum(c) && !ishspace(c))
		{
			f.unget();
			break;
		}
		b1 << c;
	}
	
	s = b1.str();
	
	
}
void get_num_param(ostream & o, istream & f, string &s)
{
	stringstream b1;
	char c;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(!isnum(c) && !ishspace(c))
		{
			f.unget();
			break;
		}
		b1 << c;
	}
	
	s = b1.str();
}

void get_double_string(ostream & o, istream & f, string &s)
{
	stringstream b1;
	b1 << '"';
	char c;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(c == '\\')
		{
			b1 << c;
			c = f.get();
			b1 << c;
		}
		else if(c == '"')
		{
			b1 << c;
			break;
		}
		else
		{
			b1 << c;
		}
	}
	s = b1.str();
}
void get_single_string(ostream & o, istream & f, string &s)
{
	stringstream b1;
	b1 << '\'';
	char c;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(c == '\\')
		{
			b1 << c;
			c = f.get();
			b1 << c;
		}
		else if(c == '\'')
		{
			b1 << c;
			break;
		}
		else
		{
			b1 << c;
		}
	}
	s = b1.str();
}
void square_param(ostream & o, istream & f, string & s)
{
	s = "";
	char c;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(isspace(c))
		{
			//ignore
		}
		else if(c == '"')
		{
			get_double_string(o, f, s);
			
			break;
		}
		else if(c == '\'')
		{
			get_single_string(o, f, s);
			break;
		}
		else if(isnum(c))
		{
			f.unget();
			get_num_param(o, f, s);
			break;
		}
		else
		{
			break;
		}
	}
	
}
void code_param(ostream & o, istream & f, string & s, bool & isend)
{
	s = "";
	char c;
	stringstream spaces;
	bool gotstring = false;
	while(true)
	{
		c = f.get();
		if(isspace(c))
		{
			if(c == '\t') spaces << "\\t";
			else if(c == '\r') spaces << "\\r";
			else if(c == '\n') spaces << "\\n";
			else spaces << c;
		}
		else if(c == '"')
		{
			gotstring = true;
			get_double_string(o, f, s);
		}
		else if(c == '{' || c == '(' || c == '[')
		{
			gotstring = true;
			f.unget();
			get_code_string_param(o, f, s, spaces);
		}
		else if(isnum(c))
		{
			gotstring = true;
			f.unget();
			get_num_param(o, f, s, spaces);
		}
		else if( c == ',')
		{
			isend = false;
			break;
		}
		else if(c == ')')
		{
			isend = true;
			break;
		}
		else
		{
			gotstring = true;
			f.unget();
			get_code_string_param(o, f, s, spaces);
		}
	}
	if(!gotstring)
	{
		s = (string)"\"" + spaces.str() + "\"";
	}
}
void jsfunc(ostream & o, istream & f, bool & retisfunc, string & retstr)
{
	stringstream func;
	string word1;
	getword(o, f, word1);
	
	js_eval_state_string = (string)"@" + word1;
	
	func << word1;
	char c;
	bool isfunc = false;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(isspace(c))
		{
			//ignore
		}
		else if(c == '(')
		{
			isfunc = true;
			
			// get params;
			bool isend = false;
			string str1;

			func << "(";
			while(!isend)
			{
				code_param(o, f, str1, isend);
				
				if(isend)
				{
					func << str1;
					func << ")";
				}
				else
				{
					func << str1;
					func << ",";
				}
			}
		}
		else if(c == '.')
		{
			isfunc = false;
			getword(o, f, word1);
			func <<'.' << word1;
			
		}
		else if(c == '[')
		{
			//if string or number only
			isfunc = false;
			string str1;
			square_param(o, f, str1);
			func << '[' << str1 << ']';
			f.get();
		}
		else if(c == ';')
		{
			break;
		}
		else
		{
			f.unget();
			break;
		}
	}
	
	retisfunc = isfunc;
	//cout << "func: " << func.str() << "\r\n";
	if(isfunc)
	{
		func << ';';
		retstr = func.str();
		
	}
	else
	{
		retstr = (string)"out(" + func.str() + ");";
	}
	
			
	
	
	
};



list<string> includejslist;
bool file_exists(const string& name) 
{
    if (FILE *file = fopen(name.c_str(), "r")) 
	{
        fclose(file);
        return true;
    } 
	else 
	{
        return false;
    }   
}

void jsfileinclude(ostream & o, istream & f, string & s)
{
	stringstream ss;
	char c = ' ';
	while(c != '"' && c != '<') c = f.get();
	//c = ' ';
	char endc;
	if(c == '"') endc = '"';
	else endc = '>';
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(c == endc) break;
		ss << c;
	}
	
	//same functionality as c/c++ #include <> look thru -I source directories. 
	//stringstream fs;
	ifstream ff;
	if(endc == '"')
	{
		ff.open(ss.str(), ifstream::in);
	}
	else
	{
		string name1 = ss.str();
		if(!file_exists(name1))
		{
			bool found = false;
			list<string>::iterator it;
			for(it = includejslist.begin(); it != includejslist.end(); it++)
			{
				string name2 = (*it) + name1;
				
				if(file_exists(name2))
				{
					found = true;
					ff.open(name2, ifstream::in);
					break;
				}
			}
			if(!found)
			{
				cout << "Could not find " << name1 << " for jsinclude.\r\n";
				return;
			}
			
		}
		else
		{
			ff.open(name1, ifstream::in);
		}
	}
	
	
	if(ff.is_open())
	{
		getjscode(o, ff, s);
		//cout << "code: " << s;
		ff.close();
	}
	else
	{
		s = "";
	}
}

void eval(ostream & o, istream & f)
{
	char c;
	string word;
	string jscode;
	string err;
	bool js_has_error = false;
	while(true)
	{
		c = f.get();
		if(f.eof()) break;
		if(c == '#')
		{
			getword(o, f, word);
			if(word == "js")
			{
				getjscode(o, f, jscode);
				
				js_has_error = jsvm->eval(jscode, err, 0);
				
			}
			else if(word == "jsinclude")
			{
				jsfileinclude(o, f, jscode);
				
				js_has_error = jsvm->eval(jscode, err, 0);
			}
			else if(word == "jsdepend")
			{
				getjscode(o, f, jscode);
				
				string strcode = (string)"function _depend_state(){ " + jscode + "};";
				js_has_error = jsvm->eval(strcode, err, 0);
				js_has_error = !jsvm->depend_state(map);
				if(js_has_error)
				{
					err = "Dependancy not meet.\r\n";
				}
			}
			else
			{
				o << '#' << word;
			}
		}
		else if(c == '\'')
		{
			o << c;
			pass_single_string(o, f);
		}
		else if(c == '"')
		{
			o << c;
			pass_double_string(o, f);
		}
		else if(c == 'R')
		{
			//check for multi line string
			char c1 = f.get();
			char c2 = f.get();
			o << c << c1 << c2;
			if(c1 == '"' && c2 == '(')
			{
				pass_multiline_string(o, f);
			}
		}
		else if(c == '@')
		{
			bool isfunc;
			string codestr;
			jsfunc(o, f, isfunc, codestr);
			
			js_has_error = jsvm->eval(codestr, err, 0);
			js_eval_state_string = "";
		}
		else
		{
			o << c;
		}
		if(js_has_error) 
		{
			cout << "Error: " << err << "\r\n";
			break;
		}
	}
	
	
}



bool endswith(const char * s1, string & s2)
{
	int at1 = 0;
	while(s1[at1] != 0) at1++;
	at1--;
	int at2 = s2.size()-1;

	while(at1 >=0)
	{
		if(s1[at1] != s2[at2])
		{
			return false;
		}
		at1--;
		at2--;
	}
	return true;
}


int processfile(string & filename, string & outfilename)
{
	cout << "processing: " << filename<< "\r\n";
	ifstream infile;
	infile.open(filename.c_str(), ifstream::in);
	
	ofstream outfile;
	outfile.open(outfilename, ofstream::out);
	
	jsvm->new_context(outfile, filename);
	eval(outfile, infile);
	if(!jsvm->dependancy_flag) 
	{
		jsvm->end_state(map);
	}
	
	infile.close();
	outfile.close();
	return jsvm->dependancy_flag;
}

void processdir(char * dir)
{
	list<pair<string,string>> processlist;
	DirInfo info = DirInfo(dir);
	for(info.begin(); info.not_end(); info++)
	{
		if(info->isfile)
		{
			if(endswith("yap", info->name) && !endswith(".jsyap", info->name))
			{
				
				at_file_name = info->name;
				string filename = (string)dir + info->name ;
				
				string name2 = (string)dir + info->name ;
				name2.resize(name2.size() -3);
				if(processfile(filename, name2))
				{
					processlist.push_back(pair<string, string>(filename, name2));
				}
			}
		}
		else if(info->isdir && info->name != "." && info->name != "..")
		{
			
			string dir2 = (string)dir + info->name + "/";
			processdir((char*)dir2.c_str());
		}
	}
	//becouse of state dependinces try processing the files again. 
	if(processlist.size()>0)
	{
		cout << "Reprocessing for dependancy states.\r\n";
		int tries = 0;
		while(tries < 3 && processlist.size() > 0)
		{
			int size1 = processlist.size();
			for(list<pair<string, string>>::iterator at = processlist.begin(); at != processlist.end(); at++)
			{
				if(processfile(at->first, at->second))
				{
					processlist.pop_front();
					processlist.push_back(pair<string, string>(at->first, at->second));
				}
				else
				{
					processlist.pop_front();
				}
				
			}
			int size2 = processlist.size();
			if(size1 == size2) tries++;
			else tries = 0;
		}
	}
	
}
int main(int argc, char *argv[]) 
{
	int count = 1;
	//get all -I for include dir.
	while(count < argc)
	{
		string cmd = argv[count];
		if(cmd == "-I")
		{
			count++;
			string ifile = argv[count];
			if(!endswith("/", ifile))
			{
				ifile = ifile + "/";
			}
			includejslist.push_back(ifile);
			count++;
		}
		else
		{
			count++;
		}
	}
	count = 1;
	while(count < argc)
	{
		string cmd = argv[count];
		if(cmd == "-stdio")
		{
			string fname = "cin";
			jsvm->new_context(cout, fname);
			eval(cout, cin);
			
			break;
		}
		else if(cmd == "-I")
		{
			//ignore
			count++;
			count++;
		}
		else if(cmd == "-i")
		{
			count++;
			ifstream f;
			//see if file exits
			bool file_exists1 = file_exists(argv[count]);
			
			if(file_exists1)
			{
				f.open(argv[count], ifstream::in);
				string fname = argv[count];
				count++;
				
				jsvm->new_context(cout, fname);
				eval(cout, f);
				f.close();
			}
			else
			{
				cout << "File does not exist " << argv[count] << "\r\n";
			}
			
			break;
		}
		else if(cmd == "-o")
		{
			
			
			count++;
			ofstream f;
			f.open(argv[count], ofstream::out);
			count++;
			
			string fname = "cin";
			jsvm->new_context(f, fname );
			eval(f, cin);
			f.close();
			
			break;
		}
		else if(cmd == "-io")
		{
			
			count++;
			ifstream infile;
			infile.open(argv[count], ifstream::in);
			string fname = argv[count];
			count++;
			ofstream outfile;
			outfile.open(argv[count], ofstream::out);
			count++;
			
			jsvm->new_context(outfile, fname);
			eval(outfile, infile);
			infile.close();
			outfile.close();
			
			break;
		}
		else if(cmd == "-h"  || cmd == "--help")
		{
			cout << R"(
			-h or --help get this help.
			-stdio using standard input output for processing
			-i input file outputs to standard output
			-o standared input to output file
			-io inputfile outputfile
			-I include path for source search #jsinclude <name>.
				Have multiple source search entries. 
			-d process directory
				Directory processing can have multiple directory entries.  
			)";
			count++;
			break;
		}
		else if(cmd == "-d")
		{
			count++;
			processdir(argv[count]);
			count++;
		}
		else
		{
			cout << "Unrecognized command " << cmd << "\r\n";
			break;
		}
	}
	return 0;
}