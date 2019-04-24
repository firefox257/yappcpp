#ifndef DIR_CPP
#define DIR_CPP
#include <iostream>
#include <dirent.h>
#include <list>
using namespace std;

class DirInfo
{
	private: 
	
	DIR *dir;
	struct dirent *ent;
	
	protected:
	
	public:
	class Node
	{
		
		
		public:
		string name;
		bool isfile;
		bool isdir;
		
		Node()
		{
			name = "";
			isfile = false;
			isdir = false;
		}
		
		
		
	};
	list<DirInfo::Node> file_list;
	bool haserror;
	DirInfo(const char * dirname)
	{
		if ((dir = opendir (dirname)) != NULL) 
		{
			while ((ent = readdir (dir)) != NULL) 
			{
				Node n;
				
				if(ent->d_type == DT_DIR)
				{
					n.isdir = true;
					//cout << "|dir|";
				}
				else
				{
					n.isfile = true;
				}
				n.name = ent->d_name;
				file_list.push_back(n);
			}
			
		}
		else
		{
			haserror = true;
		}
	}
	
	~DirInfo()
	{
		if(dir)
		{
			closedir (dir);
		}
		file_list.clear();
	}
	
	private:
	list<Node>::iterator it;
	public:
	void begin()
	{
		it = file_list.begin();
	}
	void operator ++(int  ii)
	{
		it++;
	}
	DirInfo::Node * operator ->()
	{
		return (&(*it));
	}
	bool not_end()
	{
		return it != file_list.end();
	}
	int size()
	{
		return file_list.size();
	}
};






#endif