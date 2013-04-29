#include "etmmcl.h"

void ETMMCL_Tag::write(FileStorage& fs) const
{
	fs	<< "{"
		<< "UUID"			<< uuid
		<< "Timestamp"		<< time
		<< "TagText"		<< tag
		<< "Position_X"		<< x
		<< "Position_Y"		<< y
		<< "Weight"			<< w
		<< "}";
}

void ETMMCL_Tag::read(const FileNode& node)
{
	node["UUID"]			>> uuid;
	node["Timestamp"]		>> time;
	node["TagText"]			>> tag;
	node["Position_X"]		>> x;
	node["Position_Y"]		>> y;
	node["Weight"]			>> w;
}


ETMMCL_TagList::ETMMCL_TagList(string str)
: name(str)
, num(0)
, tags()
{
	// pass desired tag list name (primary, secondary, tertiary)
}


ETMMCL_TagList::ETMMCL_TagList(void)
: name()
, num(0)
, tags()
{
}


void ETMMCL_TagList::write(FileStorage& fs) const
{
	int len, iter;
	len = tags.size();
	assert( len == num );
	fs	<< "{";
	fs	<< "length" << num;

	stringstream stst (stringstream::in | stringstream::out);
	for (iter=0; iter<len; iter++)
	{
		stst.clear();stst.str("");
		stst << "tag_" << iter;
		fs << stst.str() << tags[iter];
//		fs << "tag" << tags[iter];
//		tags[iter].write(fs);
	}
	fs << "}";
}

void ETMMCL_TagList::read(const FileNode& node)
{
	int iter, n;
	node["length"] >> n;
	std::cout << n << endl;
	stringstream stst (stringstream::in | stringstream::out);

	for (iter=0; iter<n; iter++)
	{
		stst.clear();stst.str("");
		stst << "tag_" << iter;
		ETMMCL_Tag temp;
//		temp.read(node);
		node[stst.str()] >> temp;
		std::cout << temp.uuid << endl;
		tags.push_back(temp);
		num++;
	}
	assert(num == n);
	assert(num == tags.size());
}


Vector<ETMMCL_Tag> ETMMCL_TagList::get(string str)
{
	Vector<ETMMCL_Tag> matches;
	Vector<ETMMCL_Tag>::iterator head = tags.begin();
	Vector<ETMMCL_Tag>::iterator tail = tags.end();
	while (head != tail)
	{
		if (head->tag == str)
			matches.push_back(*head);
		else
			head++;
	}

	return matches;
}

int ETMMCL_TagList::add(ETMMCL_Tag tacky)
{
	if (tacky.uuid.empty())
		return 0;

	tags.push_back(tacky);
	num++;
	assert(num == tags.size());
	return num;
}







void ETMMCL_Map::write(FileStorage& fs) const
{
	fs	<< "{"
		<< "name"			<< name_
		<< "map"			<< map_
		<< "res"			<< res_
		<< "origin_x"		<< x_
		<< "origin_y"		<< y_
		<< "primary"		<< pri_
		<< "secondary"		<< sec_
		<< "tertiary"		<< ter_
		<< "}";
}

void ETMMCL_Map::read(const FileNode& node)
{
	node["name"]			>> name_;
	node["map"]				>> map_;
	node["res"]				>> res_;
	node["origin_x"]		>> x_;
	node["origin_y"]		>> y_;
	node["primary"]			>> pri_;
	node["secondary"]		>> sec_;
	node["tertiary"]		>> ter_;
}


int ETMMCL_Map::purge(string str, string tacky)
{
	int n=0;
	ETMMCL_TagList *lister;
	if (str.compare("primary"))
	{
		lister = pri_;
	}
	else if (str.compare("secondary"))
	{
		lister = sec_;
	}
	else if (str.compare("tertiary"))
	{
		lister = ter_;
	}
	else
	{
		return n;
	}

	Vector<ETMMCL_Tag>::iterator head = lister->tags.begin();
	Vector<ETMMCL_Tag>::iterator tail = lister->tags.end();

	while (head != tail)
	{
		if (head->tag == tacky)
		{
			lister->tags.erase(head);
			num--;
			n++;
		}
		else
			head++;
	}

	return n;
}

int ETMMCL_Map::remove(string str, string uuid)
{
	ETMMCL_TagList *lister;
	if (str.compare("primary"))
	{
		lister = pri_;
	}
	else if (str.compare("secondary"))
	{
		lister = sec_;
	}
	else if (str.compare("tertiary"))
	{
		lister = ter_;
	}
	else
	{
		return 0;
	}

	Vector<ETMMCL_Tag>::iterator head = lister->tags.begin();
	Vector<ETMMCL_Tag>::iterator tail = lister->tags.end();

	while (head != tail)
	{
		if (head->uuid == uuid)
		{
			lister->tags.erase(head);
			num--;
		}
		else
			head++;
	}

	return num;
}




int ETMMCL_Interface::pull_floorplan(string str)
{
	FileStorage fs(str, FileStorage::READ);

	fs[str] >> cmap_;
}


int ETMMCL_Interface::push_floorplan(void)
{

}

*/
