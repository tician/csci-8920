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
	: time(0)
	, name("")
	, x(0)
	, y(0)
	, w(0.0)
	{}

private:
	uint64_t			time;		// Timestamp (creation)
	string				tag;		// Text tag
	int64_t			x,y;		// (X,Y) position
	double				w;			// weight/confidence?
public:

	void read(const FileNode& node);
	void write(FileStorage& fs) const;
};

class ETMMCL_TagList
{
private:
	uint64_t			n_;			// Number of tag_points in list
	Vector<ETMMCL_Tag>	t_;			// Vector of tag_points

public:
	uint64_t ETMMCL_TagList(string);

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
