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

namespace etmmcl
{
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tag::Tag(string id, string str)
: uuid(id)
, time(0)
, tag(str)
, x(0)
, y(0)
, font(25.4)
//, w(0.0)
{}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tag::Tag(void)
: uuid()
, time(0)
, tag()
, x(0)
, y(0)
, font(25.4)
//, w(0.0)
{}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Tag::write(FileStorage& fs) const
{
	fs	<< "{"
		<< "UUID"			<< uuid
		<< "Timestamp"		<< time
		<< "TagText"		<< tag
		<< "Position_X"		<< x
		<< "Position_Y"		<< y
		<< "FontSize"		<< font
//		<< "Weight"			<< w
		<< "}";
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Tag::read(const FileNode& node)
{
	node["UUID"]			>> uuid;
	node["Timestamp"]		>> time;
	node["TagText"]			>> tag;
	node["Position_X"]		>> x;
	node["Position_Y"]		>> y;
	node["FontSize"]		>> font;
//	node["Weight"]			>> w;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TagList::TagList(string str)
: name(str)
, num(0)
, tags()
, weight(0.5)
{
	// pass desired tag list name (primary, secondary, tertiary)
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TagList::TagList(void)
: name()
, num(0)
, tags()
, weight(0.5)
{
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TagList::write(FileStorage& fs) const
{
	int len, iter;
	len = tags.size();
	assert( len == num );
	fs	<< "{";
	fs	<< "length" << num;
	fs	<< "weight" << weight;

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
void TagList::read(const FileNode& node)
{
	int iter, n;
	node["weight"] >> weight;
	node["length"] >> n;
//	std::cout << n << endl;
	stringstream stst (stringstream::in | stringstream::out);

	for (iter=0; iter<n; iter++)
	{
		stst.clear();stst.str("");
		stst << "tag_" << iter;
		Tag temp;
		node[stst.str()] >> temp;
//		std::cout << temp.uuid << endl;
		tags.push_back(temp);
		num++;
	}
	assert(num == n);
	assert(num == tags.size());
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vector<Tag> TagList::get(string str)
{
	vector<Tag> matches;
	vector<Tag>::iterator head = tags.begin();
	vector<Tag>::iterator tail = tags.end();
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
int TagList::add(Tag tacky)
{
	if (tacky.uuid.empty())
		return 0;

	tags.push_back(tacky);
	num++;
	assert(num == tags.size());
	return num;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int TagList::purge(string tacky)
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
int TagList::remove(string id)
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
Map::Map(string str)
: name_(str)
, map_()
, x_(0)
, y_(0)
, res_(0.01)
, tags_()
{
	// pass desired map name
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Map::Map(void)
: name_()
, map_()
, x_(0)
, y_(0)
, res_(0.01)
, tags_()
{
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Map::write(FileStorage& fs) const
{
	fs	<< "{"
		<< "name"			<< name_
		<< "res"			<< res_
		<< "origin_x"		<< x_
		<< "origin_y"		<< y_;

	vector<string> listnames;

	int numlists = tags_.size();
	int iter;
	for (iter=0; iter<numlists; iter++)
	{
		fs << tags_.at(iter).name << tags_.at(iter);
		listnames.push_back(tags_.at(iter).name);
	}

	fs	<< "lists"			<< listnames
		<< "map"			<< map_
		<< "}";
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Map::read(const FileNode& node)
{
	node["name"]			>> name_;
	node["res"]				>> res_;
	node["origin_x"]		>> x_;
	node["origin_y"]		>> y_;

	node["map"]				>> map_;

	vector<string> listnames;
	node["lists"]			>> listnames;

	int numlists = listnames.size();
	int iter;
	for (iter=0; iter<numlists; iter++)
	{
		TagList tempy;
		node[listnames.at(iter)]	>> 	tempy;
		tags_.push_back(tempy);
	}

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Map::update(TagList tacky)
{
	int numlists = tags_.size();
	int iter;
	for (iter=0; iter<numlists; iter++)
	{
		string iname = tags_.at(iter).name;
		if (tacky.name.compare(iname)==0)
		{
			tags_.erase(tags_.begin()+iter);
			tags_.push_back(tacky);
			return tags_.back().num;
		}
	}
	tags_.push_back(tacky);

	return 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Map::update(string str, Mat m)
{
	name_ = str;
	map_ = m.clone();
	return 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Map::purge(string str, string tacky)
{
	int n=0;

	int numlists = tags_.size();
	int iter;
	for (iter=0; iter<numlists; iter++)
	{
		string iname = tags_.at(iter).name;
		if (str.compare(iname)==0)
		{
			vector<Tag>::iterator head = (tags_.at(iter)).tags.begin();
			vector<Tag>::iterator tail = (tags_.at(iter)).tags.end();

			while (head != tail)
			{
				if (head->tag.compare(tacky)==0)
				{
					tags_.at(iter).tags.erase(head);
					tags_.at(iter).num--;
					n++;
				}
				else
					head++;
			}
			break;
		}
	}

	return n;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Map::remove(string str, string id)
{
/*
	TagList *lister;
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

	vector<Tag>::iterator head = lister->tags.begin();
	vector<Tag>::iterator tail = lister->tags.end();

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
*/
	return 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vector<Particle> Map::check(string str)
{
	vector<Particle> matches;

	// Search taglists of map

	// Generate particles

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double Map::raytrace(Particle pv)
{
	// Check if particle is already out of valid map area
	if ( map_.at<unsigned char>(pv.x,pv.y) < 255 )
		return -2.0;

	// Just to be safe
	double theta = atan2(sin(pv.t), cos(pv.t));

	int iter = 1;
	while(iter>0)
	{
		int yp = round(pv.y + sin(theta)*iter);
		int xp = round(pv.x + cos(theta)*iter);

		if ( map_.at<unsigned char>(xp,yp) < 255 )
		{
			double len = sqrt( (double)((xp-pv.x)*(xp-pv.x)) + ((double)(yp-pv.y)*(yp-pv.y)) ) * res_;
			if (len > 0.0)
				return len;
			else
				return 0.0;
		}

		if (yp < 0 || xp < 0 || yp >= map_.rows || xp >= map_.cols)
			return -1.0;

		iter++;
	}
	return -3.0;
}






//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Interface::pull_floorplan(string str)
{
	string filename = "./maps/" + str + ".xml";

	FileStorage fs(filename, FileStorage::READ);

	fs["ETMMCL_Map"] >> map_;

	return 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Interface::push_floorplan(void)
{
	string filename = "./maps/" + map_.id() + ".xml";

	FileStorage fs(filename, FileStorage::WRITE);

	fs << "ETMMCL_Map" << map_;

	return 0;
}






//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ETMMCL::ETMMCL(void)
: pz_()
, pmin_(500)
, pmax_(5000)
, phi_(0.1)
, dslow_(0.1)
, dfast_(0.001)
{

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ETMMCL::init(string str)
{
	// Create uniform distribution of particle over map

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ETMMCL::init(Particle pos)
{
	// Create ~80% uniform distribution, ~15% with pos+noise, and ~5% with pos
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ETMMCL::process(void)
{
	updateOdom();
	updateText();
	updateDist();
	resample();

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern "C" {
#include <gsl/gsl_randist.h>
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ETMMCL::resample(void)
{
	// seed the rng
	long seed = time (NULL);

	// gsl rng pointer
	gsl_rng *rng;
	// gsl rng from discrete distribution
	gsl_ran_discrete_t *rng_dd;

	// create the gsl rng
	rng = gsl_rng_alloc (gsl_rng_rand48);

	// seed the gls rng
	gsl_rng_set (rng, seed);


	// Get particle weights into an array
	size_t num_particles = pz_.size();
	size_t iter;

	double *W = new double[num_particles];

	for (iter=0; iter<num_particles; iter++)
	{
		W[iter] = pz_[iter].w;
	}

	// Do gsl rng pre-processing for discrete distribution
	rng_dd = gsl_ran_discrete_preproc(num_particles, W);


	// Create a temp sample set
	vector<Particle> potent;
	double sigma_w = 0.0;

	do
	{
		Particle prime;
		/// Forward MCL (sample last set to get new particle)
		if (gsl_ran_flat(rng,0,1) < (1-phi_))
		{
			size_t indi = gsl_ran_discrete(rng, rng_dd);

			prime = pz_[indi];

			fmcl(prime);
			potent.push_back(prime);
		}
		/// Dual MCL (generate new particle from observation)
		else
		{
			prime = dmcl();
			potent.push_back(prime);
		}
		sigma_w += prime.w;

	} while ( (sigma_w<1000) && (potent.size()>pmin_) && (potent.size()<pmax_) );




	gsl_ran_discrete_free(rng_dd);
	gsl_rng_free (rng);


    time_odom_last_ = odom_.timestamp;
    time_dist_last_ = text_.timestamp;
    time_sampled_last_ = time(NULL);

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ETMMCL::fmcl(Particle& sampl)
{
	// Received new odometry, so update position
	if (time_odom_last_ < odom_.timestamp)
	{
		double	tp;
		int xp, yp;

		// Transform X to X' using odom
		xp = 0;
		yp = 0;
		tp = 0.0;

		// Update pose
		sampl.x = xp;
		sampl.y = yp;
		sampl.t = tp;
	}
	else
	{
		// No new odometry, so keep last position
	}

	// Received new observation
	if (time_dist_last_ < dist_.timestamp)
	{
		// Get distance from bot to map wall
		double d = mapper_.raytrace(sampl);

		// Invalid sensor data and/or out of range
		if (dist_.scans[0].D > 1.4)
		{
			if (d > 1.4)
			{
				// if particle is far from wall, decay slow
				sampl.w *= dslow_;
			}
			else
			{
				// if particle is close to wall, decay fast
				sampl.w *= dfast_;
			}
		}
		else
		{
			// Calculate new weight (arbitrary calculation)
			double wp = (0.8) / abs(dist_.scans[0].D - d);
			if (wp > 2.0)
			{
				wp = 2.0;
			}
			sampl.w = wp;
		}
	}
	else
	{
		// No new observation, so discount weight (slow)
		sampl.w *= dslow_;
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Particle ETMMCL::dmcl(void)
{
	Particle sampl;

	if (text_.pz.size() > 0)
	{
		// Have useful text tag particles
	}
	else
	{

	}


	// Received new observation
	if (time_dist_last_ < dist_.timestamp)
	{
		// Get distance from bot to map wall
		double d = mapper_.raytrace(sampl);

		// Invalid sensor data and/or out of range
		if (dist_.scans[0].D > 1.4)
		{
			if (d > 1.4)
			{
				// if particle is far from wall, decay slow
				sampl.w *= dslow_;
			}
			else
			{
				// if particle is close to wall, decay fast
				sampl.w *= dfast_;
			}
		}
		else
		{
			// Calculate new weight (arbitrary calculation)
			double wp = (0.8) / abs(dist_.scans[0].D - d);
			if (wp > 2.0)
			{
				wp = 2.0;
			}
			sampl.w = wp;
		}
	}
	else
	{
		// No new observation, so discount weight (slow)
		sampl.w *= dslow_;
	}


	// Received new odometry, so update position
	if (time_odom_last_ < odom_.timestamp)
	{
		double	tp;
		int xp, yp;

		// Transform X to X' using odom
		xp = 0;
		yp = 0;
		tp = 0.0;

		// Update pose
		sampl.x = xp;
		sampl.y = yp;
		sampl.t = tp;
	}
	else
	{
		// No new odometry, so keep last position
	}

	return sampl;
}

}
