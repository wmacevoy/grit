#include <set>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "Tape.h"
#include "CSVSplit.h"
#include "CSVRead.h"


using namespace std;

void Tape::write(BodyMover &mover) const
{
  for (Paths::const_iterator i = paths.begin(); i != paths.end(); ++i) {
    const string &name = i->first;
    const Path &path = i->second;
    ServoMover *servoMover = mover.getMover(name);
    if (servoMover != 0) {
      float simTime0 = path.size() > 0 ? path.begin()->first : 0.0;
      float simTime1 = path.size() > 0 ? path.rbegin()->first : 0.0;
      std::map < float , float > path0;
      for (Path::const_iterator j = path.begin(); j != path.end(); ++j) {
	path0[j->first-simTime0]=j->second;
      }
      servoMover->setup(path0,simTime0,simTime1);
      cout << "tape " << name << ": ";
      cout << "s=[" << simTime0 << "," << simTime1 << "]";
      for (Path::const_iterator j=path.begin(); j != path.end(); ++j) {
	cout << " " << j->first << "->" << j->second;
      }
      cout << endl;
    }
  }
}

void Tape::read(BodyMover &mover)
{
  for (Paths::iterator i = paths.begin(); i != paths.end(); ++i) {
    const string &name = i->first;
    Path &path = i->second;
    path.clear();
    ServoMover *servoMover = mover.getMover(name);
    if (servoMover != 0) {
      Curve::Knots &knots = servoMover->angles.knots;
      for (Curve::Knots::iterator i = knots.begin(); i != knots.end(); ++i) {
	path[i->x]=i->y;
      }
    }
  }
}

float Tape::minSimTime() const
{
  bool found = false;
  float ans=0;
  for (Paths::const_iterator i = paths.begin(); i != paths.end(); ++i) {
    if (!i->second.empty()) { 
      float t=i->second.begin()->first;
      if (found) {
	ans=fmin(ans,t);
      } else {
	found  = true;
	ans = t;
      }
    }
  }
  return ans;
}

float Tape::maxSimTime() const
{
  bool found = false;
  float ans=0;
  for (Paths::const_iterator i = paths.begin(); i != paths.end(); ++i) {
    if (!i->second.empty()) { 
      float t=i->second.begin()->first;
      if (found) {
	ans=fmax(ans,t);
      } else {
	found  = true;
	ans = t;
      }
    }
  }
  return ans;
}

void Tape::clear()
{
  for (Paths::iterator i = paths.begin(); i != paths.end(); ++i) {
    Path &path = i->second;
    path.clear();
  }
}

void Tape::same(float s)
{
  for (Paths::iterator i = paths.begin(); i != paths.end(); ++i) {
    Path &path = i->second;
    if (!path.empty()) {
      float at=path.rbegin()->second;
      path[s]=at;
    }
  }
}

void Tape::remap(float new_s0, float new_s1)
{
  float old_s0=minSimTime();
  float old_s1=maxSimTime();
  remap(new_s0,new_s1,old_s0,old_s1);
}

void Tape::rescale(float lambda)
{
  float old_s0=minSimTime();
  float old_s1=maxSimTime();
  float new_s0=lambda*old_s0;
  float new_s1=lambda*old_s1;
  remap(new_s0,new_s1,old_s0,old_s1);
}

void Tape::shift(float delta)
{
  float old_s0=minSimTime();
  float old_s1=maxSimTime();
  float new_s0=old_s0+delta;
  float new_s1=old_s1+delta;
  remap(new_s0,new_s1,old_s0,old_s1);
}

void Tape::cluster(float epsilon)
{
  Paths clustered;
  std::vector<Path*> unclusteredPaths;
  std::vector<Path*> clusteredPaths;

  for (Paths::iterator i=paths.begin(); i != paths.end(); ++i) {
    clusteredPaths.push_back(&clustered[i->first]);
    unclusteredPaths.push_back(&(i->second));
  }

  map < float,set < int > > clusters;
  for (size_t i=0; i<unclusteredPaths.size(); ++i) {
    Path &path=*unclusteredPaths[i];
    for (Path::iterator j=path.begin(); j!=path.end(); ++j) {
      clusters[j->first].insert(i);
    }
  }

  map < float,set < int > > :: iterator i = clusters.begin();
  while (i!=clusters.end()) {
    map < float , set < int > > :: iterator j = i;
    while (j->first-i->first <= epsilon) {
      set < int > &s = j->second;
      for (set < int > :: iterator k = s.begin(); k != s.end(); ++k) {
	(*clusteredPaths[*k])[i->first]=(*unclusteredPaths[*k])[j->first];
      }
      ++j;
    }
    i=j;
  }
  paths.swap(clustered);
}

void Tape::include(const Tape &inc)
{
  for (Paths::const_iterator i=inc.paths.begin(); i != inc.paths.end(); ++i) {
    for (Path::const_iterator j=i->second.begin(); j!=i->second.end(); ++j) {
      paths[i->first][j->first]=j->second;
    }
  }
}

void Tape::remap(float new_s0,float new_s1,float old_s0, float old_s1)
{
  double a=(old_s1 != old_s0) ? 1.0/(old_s1-old_s0) : 0.0;
  for (Paths::iterator i=paths.begin(); i != paths.end(); ++i) {
    Path dpath;
    for (Path::iterator j=i->second.begin(); j!=i->second.end(); ++j) {
      float old_s=j->first;
      float new_s=new_s0;
      if (a != 0) {
	float c0=a*(old_s-old_s1);
	float c1=a*(old_s-old_s0);
	new_s = c0*new_s0+c1*new_s1;
      }
      dpath[new_s]=j->second;
    }
    i->second.swap(dpath);
  }
}

bool Tape::save(const std::string &file)
{
  std::ofstream out(file.c_str());
  if (!out) return false;
  std::vector < Path::iterator > indexes;
  std::vector < Path::iterator > ends;

  out << "t";
  for (Paths::iterator i=paths.begin(); i!=paths.end(); ++i) {
    out << "," << i->first;
  }
  out << endl;

  for (Paths::iterator i=paths.begin(); i!=paths.end(); ++i) {
    indexes.push_back(i->second.begin());
    ends.push_back(i->second.end());
  }

  for (;;) {
    bool done = true;

    for (size_t ii=0; ii != indexes.size(); ++ii) {
      if (indexes[ii] != ends[ii]) done = false;
    }

    if (done) break;

    float s=0;
    bool found=false;
    for (size_t ii=0; ii != indexes.size(); ++ii) {
      if (indexes[ii] != ends[ii]) {
	float si=indexes[ii]->first;
	if (found) {
	  s=fmin(s,si);
	} else {
	  s=si;
	  found=true;
	}
      }
    }

    out << std::setprecision(10) << s << ",";
    for (size_t ii=0; ii != indexes.size(); ++ii) {
      if (indexes[ii] != ends[ii] && indexes[ii]->first == s) {
	out << "," << std::setprecision(4) << indexes[ii]->second;
      } else {
	out << ",\"-\"";
      }
    }
    out << endl;

    for (size_t ii=0; ii != indexes.size(); ++ii) {
      if (indexes[ii] != ends[ii] && indexes[ii]->first == s) {
	++indexes[ii];
      }
    }
  }
  return true;
}

bool Tape::load(const std::string &file)
{
  paths.clear();

  string heading;
  std::vector < std::string > headings;

  {
    ifstream in(file.c_str());
    if (!in) return false;
    for (;;) {
      string line;
      getline(in,line);
      if (!in) return false;
      CSVSplit(line,headings);
      if (headings.size() > 0 && headings[0] == "t") break;

      heading="t";

      for (size_t i=0; i != headings.size(); ++i) {
	const std::string &hi=headings[i];
	bool ok = true;
	for (size_t j=0; j<hi.length(); ++j) {
	  if (!(
		(hi[j]>='A' && hi[j] <='Z') 
		||(j>0 && hi[j]>='0' && hi[j] <='9')
		||(j>0 && hi[j]=='_')))
	    ok = false;
	}
	if (ok) { 
	  heading += ","; 
	  heading += hi;
	}
      }
    }

    CSVSplit(heading,headings);

    std::vector < std::vector < std::string > > table;
    if (!CSVRead(file,heading,table)) return false;

    for (size_t r=0; r != table.size(); ++r) {
      float t=atof(table[r][0].c_str());
      for (size_t c=1; c != table[r].size(); ++c) {
	if (table[r][c] != "-" && table[r][c] != "") {
	  paths[headings[r]][t]=atof(table[r][c].c_str());
	}
      }
    }
    return true;
  }
}

Tape::~Tape() {}
