#ifndef MEMORY_STATS_HPP
#define MEMORY_STATS_HPP

#include <cstdlib>
#if _MACOSX
# include <malloc/malloc.h>
#else
# include <malloc.h>
#endif
#include <graehl/shared/io.hpp>
#include <graehl/shared/size_mega.hpp>
#include <graehl/shared/stream_util.hpp>
#include <graehl/shared/auto_report.hpp>

namespace graehl {

#if defined(__unix__)
typedef struct mallinfo malloc_info;
#elif _MACOSX
typedef mstats malloc_info;
#endif

struct memory_stats  {
#if defined(__unix__) || _MACOSX
    malloc_info info;
#endif

  //   struct mallinfo {
//   int arena;    /* total space allocated from system */
//   int ordblks;  /* number of non-inuse chunks */
//   int smblks;   /* unused -- always zero */
//   int hblks;    /* number of mmapped regions */
//   int hblkhd;   /* total space in mmapped regions */
//   int usmblks;  /* unused -- always zero */
//   int fsmblks;  /* unused -- always zero */
//   int uordblks; /* total allocated space */
//   int fordblks; /* total non-inuse space */
//   int keepcost; /* top-most, releasable (via malloc_trim) space */
// };
//
    memory_stats()  {
        refresh();
    }
    void refresh() {
#if defined(__unix__)
        info=mallinfo();
#elif _MACOSX
        info=mstats();
#endif
    }
#if defined(__unix__) || _MACOSX
    operator const malloc_info & () const {
        return info;
    }
#endif
    typedef size_bytes size_type;

    // includes memory mapped
    size_type total_allocated() const
    {
        return program_allocated()+memory_mapped();
    }
    
    size_type program_allocated() const 
	{
#if defined(__unix__)
        return size_type((unsigned)info.uordblks);
#elif _MACOSX
        return size_type(info.bytes_used);
#else
		return 0;
#endif
    }

    // may only grown monotonically (may not reflect free())
    size_type system_allocated() const 
    {
#if defined(__unix__)
        return size_type((unsigned)info.arena);
#elif _MACOSX
        return size_type(info.bytes_total);
#else
        return 0;
#endif
    }
    
    size_type memory_mapped() const
    {
#if defined(__unix__)
        return size_type((unsigned)info.hblkhd);
#else
		return 0;
#endif
    }
};

#define GRAEHL__MEMSTAT_DIFF(field) ret.info.field=after.info.field-before.info.field
#if defined(__unix__)
inline memory_stats operator - (memory_stats after,memory_stats before) 
{
    memory_stats ret;
GRAEHL__MEMSTAT_DIFF(arena);    /* total space allocated from system */
GRAEHL__MEMSTAT_DIFF(ordblks);  /* number of non-inuse chunks */
GRAEHL__MEMSTAT_DIFF(smblks);   /* unused -- always zero */
GRAEHL__MEMSTAT_DIFF(hblks);    /* number of mmapped regions */
GRAEHL__MEMSTAT_DIFF(hblkhd);   /* total space in mmapped regions */
GRAEHL__MEMSTAT_DIFF(usmblks);  /* unused -- always zero */
GRAEHL__MEMSTAT_DIFF(fsmblks);  /* unused -- always zero */
GRAEHL__MEMSTAT_DIFF(uordblks); /* total allocated space */
GRAEHL__MEMSTAT_DIFF(fordblks); /* total non-inuse space */
GRAEHL__MEMSTAT_DIFF(keepcost); /* top-most, releasable (via malloc_trim) space */
#undef GRAEHL__MEMSTAT_DIFF
return ret;
//    using namespace memory_stats_detail;
//    return transform2_array_coerce<unsigned>(after,before,difference_f<int>());
}
#elif _MACOSX
inline memory_stats operator - (memory_stats after, memory_stats before)
{
    memory_stats ret;
    GRAEHL__MEMSTAT_DIFF(bytes_total);
    GRAEHL__MEMSTAT_DIFF(chunks_used);
    GRAEHL__MEMSTAT_DIFF(bytes_used);
    GRAEHL__MEMSTAT_DIFF(chunks_free);
    GRAEHL__MEMSTAT_DIFF(bytes_free);
    return ret;
}
#endif

template <class C,class T> inline std::basic_ostream<C,T> &
operator << (std::basic_ostream<C,T> &o, const memory_stats &s) {
    return o << "["<<s.program_allocated()<<" allocated, " << s.system_allocated() << " from system, "<<s.memory_mapped()<<" memory mapped]";
}

struct memory_change
{
    memory_stats before;
    static char const* default_desc() 
    { return "\nmemory used: "; }
    template <class O>
    void print(O &o) const
    {
        memory_stats after;
        typedef memory_stats::size_type S;
        S pre=before.total_allocated();
        S post=after.total_allocated();
        if (post >= pre) {
            o << "+" << S(post-pre);
        } else
            o << "-" << S(pre-post);
        o << " (" << pre << " -> " << post << ")";
    }
    typedef memory_change self_type;
    TO_OSTREAM_PRINT
};

typedef auto_report<memory_change> memory_report;

}//graehl

#endif
