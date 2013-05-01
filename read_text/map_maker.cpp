#include <iostream>
#include <fstream>
#include <cstdio>


#include "etmmcl.h"

// Our window title/name (can be anything you want, obviously)
const char* WIN_DOH = "Yum, donuts";
#ifdef RUNNING_DETECTOR
const char* WIN_DER = "Holy macaroni";
#endif

int main(int argc, char* argv[])
{
	string filename = "./tempy.xml.gz";
	int iter, r;

	etmmcl::Map mapper("test");

	etmmcl::TagList *lister;

	FileStorage fs(filename, FileStorage::WRITE);

	etmmcl::Tag tagster;

	lister = new etmmcl::TagList("primary");

	for (iter=0; iter<5; iter++)
	{
		r = system("uuidgen >> output");
		fstream fin("output");
		getline(fin, tagster.uuid);
		fin.close();
		r = system("rm output");
		tagster.time = iter+1;

		lister->add(tagster);
	}


	r = mapper.update("primary", *lister);
	assert( r == iter );



	delete lister;
	lister = new etmmcl::TagList("secondary");


	for (iter=0; iter<7; iter++)
	{
		r= system("uuidgen >> output");
		fstream fin("output");
		getline(fin, tagster.uuid);
		fin.close();
		r = system("rm output");
		tagster.time = iter+1;

		lister->add(tagster);
	}

	r = mapper.update("secondary", *lister);
	assert( r == iter );


// purge does not work
//	cout << lister.purge("") << endl;


	delete lister;
	lister = new etmmcl::TagList("tertiary");


	for (iter=0; iter<3; iter++)
	{
		r= system("uuidgen >> output");
		fstream fin("output");
		getline(fin, tagster.uuid);
		fin.close();
		r = system("rm output");
		tagster.time = iter+1;

		lister->add(tagster);
	}

	r = mapper.update("tertiary", *lister);
	assert( r == iter );



	fs << "ETMMCL_Map" << mapper;




/*
// Tag and TagList testing
	{
	FileStorage fs(filename, FileStorage::WRITE);

	ETMMCL_Tag tagster;
	ETMMCL_TagList lister("primary");

	for (int iter=0; iter<5; iter++)
	{
		int r= system("uuidgen >> output");
		fstream fin("output");
		getline(fin, tagster.uuid);
		r = system("rm output");
		tagster.time = iter+1;

		lister.add(tagster);
	}

	fs << lister.name << lister;
	}

	{
	FileStorage fs(filename, FileStorage::READ);

    if (!fs.isOpened())
    {
      cerr << "failed to open " << filename << endl;
      return 1;
    }

	ETMMCL_TagList custer;
	fs["primary"] >> custer;

	cout << "length: " << custer.num << endl;
	}

*/

	return 0;
}
