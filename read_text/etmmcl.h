#ifndef ETMMCL_INTERFACE_H
#define ETMMCL_INTERFACE_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

typedef struct
{
	ETMMCL_Tag()
	: time(0)
	, name("")
	, x(0)
	, y(0)
	, w(0.0)
	{}

	uint64_t			time;		// Timestamp (creation)
	string				tag;		// Text tag
	int64_t			x,y;		// (X,Y) position
	double				w;			// weight/confidence?

	void read(const FileNode& node)
	{
		node["Timestamp"]	>> time;
		node["TagText"]		>> name;
		node["Position_X"]	>> x;
		node["Position_Y"]	>> y;
		node["Weight"]		>> w;
	}

	void write(FileStorage& fs) const
	{
		fs	<< "{"
			<< "Timestamp"		<< time
			<< "TagText"		<< name
			<< "Position_X"		<< x
			<< "Position_Y"		<< y
			<< "Weight"			<< w
			<< "}";
	}

} ETMMCL_Tag;

static void write(FileStorage& fs, const std::string&, const ETMMCL_Tag& x)
{
    x.write(fs);
}

static void read(const FileNode& node, ETMMCL_Tag& x, const ETMMCL_Tag& default_value = ETMMCL_Tag()){
    if(node.empty())
        x = default_value;
    else
        x.read(node);
}


class ETMMCL_TagList
{
private:
	string				name;		// Name of TagList
	uint64_t			n;			// Number of tags in list
	Vector<ETMMCL_Tag>	t;			// Vector of tags

public:
	uint64_t ETMMCL_TagList(string str)
	: name(str)
	, n(0)
	, t()
	{}

	void write(FileStorage&);
	void read(FileNode&);

	tag_t get(string);



};

class ETMMCL_Map
{
private:
	string				name_;		// Floorplan name
	Mat					map_;		// Map of floor
	int64_t			x_;			// Map origin X
	int64_t			y_;			// Map origin Y
	uint64_t			res_;		// Map resolution (# of mm/pixel)

	ETMMCL_TagList		pri_;		// Primary (immutable) TagList
	ETMMCL_TagList		sec_;		// Secondary (human-set) TagList
	ETMMCL_TagList		ter_;		// Tertiary (auto-set) TagList

public:
	add
};


class ETMMCL_Interface
{
private:
	int pull_floorplan(string str);
	int push_floorplan(void);

	ETMMCL_Map map_;

public:
	int load(string str)	{return pull_floorplan(str);}
	int update(void)		{return push_floorplan();}
	string check(void)		{return map_.name;}


};



#endif
