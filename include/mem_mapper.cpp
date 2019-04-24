#ifndef MEM_MAPPPER_CPP
#define MEM_MAPPPER_CPP

#include <string>
#include <unordered_map>
#include <mapper.cpp>

class mem_mapper: public mapper_abstract
{
	private:
	unordered_map<string, unordered_map<string, string> > map;
	unordered_map<string, string> fmap;
	unordered_map<string, string> gmap;
	public:
	mem_mapper()
	{
	}
	~mem_mapper()
	{
	}
	virtual void set(string &file_name, string & global_name, string & name, string & value)
	{
		map[global_name][name] = value;
		fmap[file_name] = global_name;
		gmap[global_name] = file_name;
	}
	virtual bool get(string & global_name, string & name, string & ret_value)
	{
		unordered_map<string,unordered_map < string, string> >::const_iterator got1 = map.find(global_name);
		if(got1 == map.end()) //not found
		{
			return false;
		}
		else 
		{
			unordered_map<string, string> * m1 = (&map[global_name]);
			unordered_map <string, string>::const_iterator got2 =(*m1).find(name);
			
			if(got2 == (*m1).end())
			{
				return false;
			}
			else
			{
				ret_value = (*m1)[name];
				return true;
			}
		}
		return false;
	}
	virtual void clear(string & global_name)
	{
		unordered_map<string,unordered_map < string, string> >::const_iterator got1 = map.find(global_name);
		if(got1 != map.end()) 
		{
			unordered_map<string, string> * m1 = (&map[global_name]);
			(*m1).clear();
			string filename = gmap[global_name];
			gmap.erase(global_name);
			fmap.erase(filename);
		}
	}
	virtual void clear_all()
	{
		for(auto it = map.begin(); it != map.end(); ++it)
		{
			map[it->first].clear();
		}
		map.clear();
	}
};









#endif
