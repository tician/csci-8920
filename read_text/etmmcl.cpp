#include "etmmcl.h"
/*
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

ETMMCL_Tag ETMMCL_Tag::read(const FileNode& node)
{
	node["UUID"]		>> uuid;
	node["Timestamp"]	>> time;
	node["TagText"]		>> tag;
	node["Position_X"]	>> x;
	node["Position_Y"]	>> y;
	node["Weight"]		>> w;
}

*/


ETMMCL_TagList::ETMMCL_TagList(string str)
: name(str)
, num(0)
, tags()
{
	// pass desired tag list name (primary, secondary, tertiary)
//	uint64_t iter, n=0;

}

ETMMCL_TagList::ETMMCL_TagList(void)
: name()
, num(0)
, tags()
{
}


void ETMMCL_TagList::write(FileStorage& fs) const
{
	Vector<ETMMCL_Tag>::iterator head = (Vector<ETMMCL_Tag>::iterator) tags.begin();
	Vector<ETMMCL_Tag>::iterator tail = (Vector<ETMMCL_Tag>::iterator) tags.end();
	assert( (tail-head) == num );
	assert(num == tags.size());
	fs << "{" << "NumberTags" << (int)(tail-head);
	for (; head!=tail; head++)
	{
		(*head).write(fs);
	}
	fs << "}";
}

void ETMMCL_TagList::read(const FileNode& node)
{
	int iter, n;
	node["NumberTags"] >> n;
	for (iter=0; iter<n; iter++)
	{
		ETMMCL_Tag temp;
		temp.read(node);
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



/*


int ETMMCL_Map::purge(string str, string tacky)
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
		if (head->tag == tacky)
		{
			lister->tags.erase(head);
			num--;
		}
		else
			head++;
	}

	return num;
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
	tags_p_.open()
}


int ETMMCL_Interface::push_floorplan(void)
{

}

*/
