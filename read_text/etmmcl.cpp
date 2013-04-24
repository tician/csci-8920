#include "etmmcl.h"

void ETMMCL_Tag::write(FileStorage& fs) const
{
	fs	<< "{"
		<< "Timestamp"		<< head->time
		<< "TagText"		<< head->name
		<< "Position_X"		<< head->x
		<< "Position_Y"		<< head->y
		<< "Weight"			<< head->w
		<< "}";
}

tag_t ETMMCL_TagList::read(const FileNode& node)
{
	uint64_t iter, n;
	node["NumberTags"] >> n;
	for (iter=0; iter<n; iter++)
	{
		node["Timestamp"] >>
	}
}



uint64_t ETMMCL_TagList::ETMMCL_TagList(string floorname)
{
	uint64_t iter, n;
}
tag_t ETMMCL_TagList::get(string str)
{
	Vector<tag_t>::iterator head = t.begin();
	Vector<tag_t>::iterator tail = t.end();
	while (head != tail)
	{
		if (head->tag == str)
			return *head;
		else
			head++;
	}

	tag_t bleh;
	bleh.time = 0;
	bleh.tag = "NULL";

	return bleh;
}

void ETMMCL_TagList::write(FileStorage& fs) const
{
	Vector<tag_t>::iterator head = t_.begin();
	Vector<tag_t>::iterator tail = t_.end();
	fs << "{" << "NumberTags" << (tail-head);
	for (; head!=tail; head++)
	{
		fs	<< "{"
				<< "Timestamp"		<< head->time
				<< "TagText"		<< head->name
				<< "Position_X"		<< head->x
				<< "Position_Y"		<< head->y
				<< "Weight"			<< head->w
			<< "}";
	}
	fs << "}";
}

tag_t ETMMCL_TagList::read(const FileNode& node)
{
	uint64_t iter, n;
	node["NumberTags"] >> n;
	for (iter=0; iter<n; iter++)
	{
		node["Timestamp"] >>
	}
}











int ETMMCL_Interface::pull_floorplan(string str)
{
	tags_p_.open()
}


int ETMMCL_Interface::push_floorplan(void)
{

}

