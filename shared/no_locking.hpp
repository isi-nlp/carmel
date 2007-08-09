#ifndef GRAEHL_SHARED__NO_LOCKING_HPP
#define GRAEHL_SHARED__NO_LOCKING_HPP

#include <boost/thread/mutex.hpp>

/*

intent:

template <class Locking=graehl::no_locking> // or graehl::locking
struct collection : private Locking
{
void some_operation() 
{
    typename Locking::lock(*this);
// or    bool do_lock=...;
//    typename Locking::scoped_lock(*this,do_lock);
    // (locks if do_lock)
}
};
*/

namespace graehl {

struct no_locking 
{
    typedef no_locking self_type;
    struct scoped_lock 
    {
        scoped_lock(self_type const& l) {}
        scoped_lock(self_type const& l,bool b) {}
    };
};

    
}


#endif
