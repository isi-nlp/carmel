#ifndef GRAEHL_CARMEL_ARC_H
#define GRAEHL_CARMEL_ARC_H

#include <graehl/shared/config.h>
#include <graehl/shared/weight.h>
#include <graehl/shared/2hash.h>
#include <graehl/shared/stream_util.hpp>
#include <boost/config.hpp>

namespace graehl {

struct FSTArc {
    BOOST_STATIC_CONSTANT(unsigned,no_group=(unsigned)-1);
    BOOST_STATIC_CONSTANT(unsigned,locked_group=0);
    BOOST_STATIC_CONSTANT(int,epsilon=0);
    
    typedef FSTArc self_type;
    int in;
    int out;
    int dest;
    Weight weight;
    typedef unsigned group_t;
    
    group_t groupId;
    
//    enum {no_group=-1,locked_group=0};

    FSTArc(int i, int o, int d, Weight w,group_t g = no_group) :
        in(i), out(o), dest(d), weight(w), groupId(g)
    {}
    bool isNormal() const {
        //return groupId == no_group;
        return groupId < 0;
    }
    bool isLocked() const {
        return groupId == locked_group;
    }
    bool isTied() const {
        return groupId > 0;
    }
    bool isTiedOrLocked() const {
        return groupId >= 0;
    }
    void setLocked() 
    {
        setGroup(locked_group);
    }
    void setGroup(int group) 
    {
        groupId = group;
    }
    template <class O> void print(O&o) const
    {
        o<<"(->" << dest << ' ' << in << '/' << out << ' ' << weight;
        if (groupId!=no_group) {
            o << '!';
            if (groupId!=locked_group)
                o << groupId;
        }
        o <<')';
    }
    TO_OSTREAM_PRINT
        
};

typedef FSTArc *HalfArc;

struct UnArc {
    int in;
    int out;
    int dest;
    bool operator == (const UnArc& r) const {
        return in == r.in && out == r.out && dest == r.dest;
    }
    size_t hash() const
    {
        return uint32_hash((in * 193 + out * 6151 + dest));
    }
};

/*
  HASHNS_B
  template<>
  struct hash<UnArc>
  {
  size_t operator()(const UnArc &x) const {
  return x.hash();
  }
  };
  HASHNS_E
*/

}

BEGIN_HASH(graehl::UnArc) {
    return x.hash();
} END_HASH

#endif
