#ifndef ETMMCL_INTERFACE_H
#define ETMMCL_INTERFACE_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

class ETMMCL_Tag
{
public:
	ETMMCL_Tag()
	: uuid()
	, time(0)
	, tag()
	, x(0)
	, y(0)
	, w(0.0)
	{}

// Only (int, float, double and string) are permitted with filestorage (writeScalar() not yet implemented for others)

	string				uuid;		// UUID of tag
//	uint64_t			time;		// Timestamp (creation)
	int			time;		// Timestamp (creation)
	string				tag;		// Text tag
//	int64_t			x,y;		// (X,Y) position
	int			x,y;		// (X,Y) position
	double				w;			// weight/confidence?

public:
	void write(FileStorage& fs) const
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

	void read(const FileNode& node)
	{
		node["UUID"]		>> uuid;
		node["Timestamp"]	>> time;
		node["TagText"]		>> tag;
		node["Position_X"]	>> x;
		node["Position_Y"]	>> y;
		node["Weight"]		>> w;
	}

};

static void write(FileStorage& fs, const std::string&, const ETMMCL_Tag& x)
{
    x.write(fs);
}

static void read(const FileNode& node, ETMMCL_Tag& x, const ETMMCL_Tag& default_value = ETMMCL_Tag())
{
    if(node.empty())
        x = default_value;
    else
        x.read(node);
}


class ETMMCL_TagList
{
public:

	ETMMCL_TagList(string);
	ETMMCL_TagList(void);

	string				name;		// Name of TagList
//	uint64_t			num;		// Number of tags in list
	int			num;		// Number of tags in list
	Vector<ETMMCL_Tag>	tags;		// Vector of tags

	void write(FileStorage&) const;
	void read(const FileNode&);

	Vector<ETMMCL_Tag> get(string);
	int add(ETMMCL_Tag);
};

static void write(FileStorage& fs, const std::string&, const ETMMCL_TagList& x)
{
    x.write(fs);
}

static void read(const FileNode& node, ETMMCL_TagList& x, const ETMMCL_TagList& default_value = ETMMCL_TagList())
{
    if(node.empty())
        x = default_value;
    else
        x.read(node);
}


/*

class ETMMCL_Map
{
private:
	string				name_;		// Floorplan name
	Mat					map_;		// Map of floor
	int			x_;			// Map origin X
	int			y_;			// Map origin Y
	int			res_;		// Map resolution (# of mm/pixel)

	ETMMCL_TagList		pri_;		// Primary (immutable) TagList
	ETMMCL_TagList		sec_;		// Secondary (human-set) TagList
	ETMMCL_TagList		ter_;		// Tertiary (auto-set) TagList

public:
	int add(ETMMCL_Tag);
	int purge(string, string);
	int remove(string, string);

};


class ETMMCL_Interface
{
private:
	int pull_floorplan(string);
	int push_floorplan(void);

	ETMMCL_Map map_;

public:
	int load(string str)	{return pull_floorplan(str);}
	int update(void)		{return push_floorplan();}
	string check(void)		{return map_.name;}


};


*/


#endif
