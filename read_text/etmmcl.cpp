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


#include "etmmcl.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ETMMCL_Tag::ETMMCL_Tag(string id, string str)
: uuid(id)
, time(0)
, tag(str)
, x(0)
, y(0)
, font(25.4)
, w(0.0)
{}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ETMMCL_Tag::ETMMCL_Tag(void)
: uuid()
, time(0)
, tag()
, x(0)
, y(0)
, font(25.4)
, w(0.0)
{}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ETMMCL_Tag::write(FileStorage& fs) const
{
	fs	<< "{"
		<< "UUID"			<< uuid
		<< "Timestamp"		<< time
		<< "TagText"		<< tag
		<< "Position_X"		<< x
		<< "Position_Y"		<< y
		<< "FontSize"		<< font
		<< "Weight"			<< w
		<< "}";
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ETMMCL_Tag::read(const FileNode& node)
{
	node["UUID"]			>> uuid;
	node["Timestamp"]		>> time;
	node["TagText"]			>> tag;
	node["Position_X"]		>> x;
	node["Position_Y"]		>> y;
	node["FontSize"]			>> font;
	node["Weight"]			>> w;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ETMMCL_TagList::ETMMCL_TagList(string str)
: name(str)
, num(0)
, tags()
{
	// pass desired tag list name (primary, secondary, tertiary)
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ETMMCL_TagList::ETMMCL_TagList(void)
: name()
, num(0)
, tags()
{
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
	}
	fs << "}";
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
		node[stst.str()] >> temp;
//		std::cout << temp.uuid << endl;
		tags.push_back(temp);
		num++;
	}
	assert(num == n);
	assert(num == tags.size());
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vector<ETMMCL_Tag> ETMMCL_TagList::get(string str)
{
	vector<ETMMCL_Tag> matches;
	vector<ETMMCL_Tag>::iterator head = tags.begin();
	vector<ETMMCL_Tag>::iterator tail = tags.end();
	while (head != tail)
	{
		if (head->tag.compare(str)==0)
			matches.push_back(*head);
		else
			head++;
	}

	return matches;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ETMMCL_TagList::add(ETMMCL_Tag tacky)
{
	if (tacky.uuid.empty())
		return 0;

	tags.push_back(tacky);
	num++;
	assert(num == tags.size());
	return num;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ETMMCL_TagList::purge(string tacky)
{
	int len, iter, n=0;
	len = tags.size();
	assert( len == num );

	int indicies[100] = {};

	for (iter=0; iter<len; iter++)
	{
		if ( (n < 100) && ( tags[iter].tag.compare(tacky)==0 ) )
		{
			indicies[n++] = iter;
			num--;
		}
		else
		{
			break;
		}
	}

	for (iter=0; iter<n; iter++)
	{
		// decreasing length of vector when erasing
		assert( (indicies[iter]-iter) < tags.size() );
		tags.erase(tags.begin() + (indicies[iter]-iter) );
	}

	return n;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ETMMCL_TagList::remove(string id)
{
	int len, iter, n=0;
	len = tags.size();
	assert( len == num );

	for (iter=0; iter<len; iter++)
	{
		if ( tags[iter].uuid.compare(id)==0 )
		{
			tags.erase(tags.begin()+iter);
			num--;
			n++;
			break;
		}
	}

	return n;
}







//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ETMMCL_Map::ETMMCL_Map(string str)
: name_(str)
, map_()
, res_(1000)
, x_(0)
, y_(0)
, pri_()
, sec_()
, ter_()
{
	// pass desired map name
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ETMMCL_Map::ETMMCL_Map(void)
: name_()
, map_()
, res_(1000)
, x_(0)
, y_(0)
, pri_()
, sec_()
, ter_()
{
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
//		<< pri_.name		<< pri_
//		<< sec_.name		<< sec_
//		<< ter_.name		<< ter_
		<< "}";
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ETMMCL_Map::update(string str, ETMMCL_TagList tacky)
{
	if (str.compare("primary")==0)
	{
		pri_ = tacky;//.clone();
//		cout << "primary set: " << pri_.num << endl;
		return pri_.num;
	}
	else if (str.compare("secondary")==0)
	{
		sec_ = tacky;//.clone();
//		cout << "secondary set: " << sec_.num << endl;
		return sec_.num;
	}
	else if (str.compare("tertiary")==0)
	{
		ter_ = tacky;//.clone();
//		cout << "tertiary set: " << ter_.num << endl;
		return ter_.num;
	}
	else
		return 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ETMMCL_Map::purge(string str, string tacky)
{
	int n=0;
	ETMMCL_TagList *lister;
	if (str.compare("primary")==0)
	{
		lister = &pri_;
	}
	else if (str.compare("secondary")==0)
	{
		lister = &sec_;
	}
	else if (str.compare("tertiary")==0)
	{
		lister = &ter_;
	}
	else
	{
		return n;
	}

	vector<ETMMCL_Tag>::iterator head = lister->tags.begin();
	vector<ETMMCL_Tag>::iterator tail = lister->tags.end();

	while (head != tail)
	{
		if (head->tag.compare(tacky)==0)
		{
			lister->tags.erase(head);
			lister->num--;
			n++;
		}
		else
			head++;
	}

	return n;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ETMMCL_Map::remove(string str, string id)
{
	ETMMCL_TagList *lister;
	if (str.compare("primary")==0)
	{
		lister = &pri_;
	}
	else if (str.compare("secondary")==0)
	{
		lister = &sec_;
	}
	else if (str.compare("tertiary")==0)
	{
		lister = &ter_;
	}
	else
	{
		return 0;
	}

	vector<ETMMCL_Tag>::iterator head = lister->tags.begin();
	vector<ETMMCL_Tag>::iterator tail = lister->tags.end();

	while (head != tail)
	{
		if (head->uuid.compare(id)==0)
		{
			lister->tags.erase(head);
			lister->num--;
		}
		else
			head++;
	}

	return lister->num;
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ETMMCL_Interface::pull_floorplan(string str)
{
	string filename = "./maps/" + str + ".xml";

	FileStorage fs(filename, FileStorage::READ);

	fs["ETMMCL_Map"] >> map_;

	return 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ETMMCL_Interface::push_floorplan(void)
{
	string filename = "./maps/" + map_.id() + ".xml";

	FileStorage fs(filename, FileStorage::WRITE);

	fs << "ETMMCL_Map" << mapper;

	return 0;
}

