#include <iostream>
#include <fstream>
#include <cstdio>

#include "etmmcl.h"

int map_maker(void);
int run_detection(int argc, char* argv[]);

int main(int argc, char* argv[])
{
//	map_maker();
//	return 0;

	string filename = "./tempy.xml.gz";
	int iter, r;

	etmmcl::Map mapper;

	FileStorage fs(filename, FileStorage::READ);

	if (!fs.isOpened())
    {
      cerr << "failed to open " << filename << endl;
      return 1;
    }

	fs["ETMMCL_Map"] >> mapper;

	cout << "Map Name: " << mapper.id() << endl;

	return 0;

}
