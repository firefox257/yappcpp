#ifndef MAPPER_CPP
#define MAPPER_CPP



class mapper_abstract
{
	public:
	mapper_abstract()
	{
	}
	~mapper_abstract()
	{
	}
	virtual void set(string & file_name, string & global_name, string & name, string & value)
	{
	}
	virtual bool get(string & global_name, string & name, string & ret_value)
	{
		return false;
	}
	virtual void clear(string & global_name)
	{
	}
	virtual void clear_all()
	{
	}
	
};



class mapper
{
	public:
	mapper_abstract * map;
	mapper()
	{
		map = 0;
	}
	~mapper()
	{
		if(map) 
		{
			delete(map);
		}
	}
	mapper_abstract * operator ->()
	{
		return map;
	}
};


#endif