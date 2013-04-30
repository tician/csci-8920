/*******************************************************************************
 * Environmental Text Mixture Monte Carlo Localization
 *******************************************************************************
 * Copyright (c) 2013, Matthew Paulishen.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */

/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Basically...
  -Every tag consists of: a location, a string, a uuid, a timestamp,
    an approximate font size (mm tall), and a confidence (how common
	 is this string of text and how likely is it that it has actually
	 been encountered).
  -Every taglist constists of multiple tags and a length counter.
  -Every map consists of three taglists (primary, secondary, and tertiary),
    as well as a map image, map resolution, map origin, and map name.
     Primary   - Immutable        - Room numbers
     Secondary - Human changeable - Uncommon text tags
     Tertiary  - Auto-added       - Misc. text tags

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Environmental Text Extraction via Stroke Width Transform
  Canny-edge -> sobel(x,y) -> gradient at edges -> stroke width of blob
   if mostly constant stroke width, then likely text -> send to tesseract
  recovered text -> primary tag list   -> high confidence   P(x'|o)
  recovered text -> secondary tag list -> medium confidence P(x'|o)
  recovered text -> tertiary tag list  -> low confidence    P(x'|o)

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 MMCL:
  phi~0.1
  m is variable depending on available cycles and confidence in pose and text
  <X,W> = initial estimates of pose and weight <x_i,w_i>
  while(1)
    for iter=1:1:m
      if urand(0,1)<(1-phi)
        generate random last pose x from X based on pose weight w (roulette or tournament?)
        generate random current pose x' from P(x'|x,a)
        estimate weight w' of pose x' from P(o|x')
        add <x',w'> to X'
      else
        generate random current pose x' from P(x'|o) (weighted: pri > sec >> ter)
        generate random last pose x from P(x'|x,a)
        estimate weight w' of pose x' from tag confidence and ~weight w of x
        add <x',w'> to <X',W'>
      end
    end
    normalize W'
    <X,W> = <X',W'>
  end

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

#ifndef ETMMCL_INTERFACE_H
#define ETMMCL_INTERFACE_H

#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <sstream>
#include <vector>

using namespace cv;
using namespace std;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef struct
{
	int			x,y;	// map coordinate/index (x,y)
	double		t;		// bot orientation (theta)
	double		w;		// weight of estimated pose
} ETMMCL_Pose;

typedef struct
{
	string		s;		// retrieved text string
	double		f;		// approximate font size (mm tall)
	double		d;		// approximate distance (mm)
	double		t;		// orientation (theta) of text relative to camera or base_link?
} ETMMCL_EnvText;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ETMMCL_Tag
{
public:
	ETMMCL_Tag(string,string);
	ETMMCL_Tag(void);

// Only (int, float, double and string) are permitted with filestorage (unsigned not allowed, yet)

	string				uuid;		// UUID of tag
	int					time;		// Timestamp (creation)
	string				tag;		// Text tag
	int					x,y;		// (X,Y) position
	double				font;		// Approximate font height
	double				w;			// weight/confidence?

public:
	void write(FileStorage& fs) const;

	void read(const FileNode& node);

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void write(FileStorage& fs, const std::string&, const ETMMCL_Tag& x)
{
    x.write(fs);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void read(const FileNode& node, ETMMCL_Tag& x, const ETMMCL_Tag& default_value = ETMMCL_Tag())
{
    if(node.empty())
        x = default_value;
    else
        x.read(node);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ETMMCL_TagList
{
public:

	ETMMCL_TagList(string);
	ETMMCL_TagList(void);

	string				name;		// Name of TagList
	int					num;		// Number of tags in list
	vector<ETMMCL_Tag>	tags;		// Vector of tags

	void write(FileStorage&) const;
	void read(const FileNode&);

	vector<ETMMCL_Tag> get(string);
	int add(ETMMCL_Tag);
	int purge(string);
	int remove(string);

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void write(FileStorage& fs, const std::string&, const ETMMCL_TagList& x)
{
    x.write(fs);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void read(const FileNode& node, ETMMCL_TagList& x, const ETMMCL_TagList& default_value = ETMMCL_TagList())
{
    if(node.empty())
        x = default_value;
    else
        x.read(node);
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ETMMCL_Map
{
public:
	ETMMCL_Map(string);
	ETMMCL_Map(void);

private:
	string				name_;		// Floorplan name
	Mat					map_;		// Map of floor
	int					x_;			// Map origin X
	int					y_;			// Map origin Y
	double				res_;		// Map resolution (# of mm/pixel)

	ETMMCL_TagList		pri_;		// Primary (immutable) TagList
	ETMMCL_TagList		sec_;		// Secondary (human-set) TagList
	ETMMCL_TagList		ter_;		// Tertiary (auto-set) TagList

public:
	int update(string, ETMMCL_TagList);
//	int add(ETMMCL_Tag);
	int purge(string, string);
	int remove(string, string);

	void write(FileStorage&) const;
	void read(const FileNode&);

	string id(void) {return name_;}
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void write(FileStorage& fs, const std::string&, const ETMMCL_Map& x)
{
    x.write(fs);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void read(const FileNode& node, ETMMCL_Map& x, const ETMMCL_Map& default_value = ETMMCL_Map())
{
    if(node.empty())
        x = default_value;
    else
        x.read(node);
}




















//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ETMMCL_Interface
{
private:
	int pull_floorplan(string);
	int push_floorplan(void);

	ETMMCL_Map map_;

public:
	int pull(string str)	{return pull_floorplan(str);}
	int push(void)			{return push_floorplan();}
	string id(void)		{return map_.id();}

	vector<ETMMCL_Pose> sample(vector<ETMMCL_EnvText>&);
};








class ETMMCL
{
private:


public:
	void spin(void);


};


#endif
