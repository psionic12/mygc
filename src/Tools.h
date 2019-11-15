//
// Created by liu on 19-11-14.
//

#ifndef MYGC_TOOLS_H
#define MYGC_TOOLS_H

#ifdef __GNUC__
#define clz(x) __builtin_clzll(x)
#else
#define clz(x) static_assert(false && "no clz implementation found")
#endif

#include <cstdint>
namespace mygc {
class Tools {
 public:
  static inline unsigned int getFirstZeroFromLeft(uint64_t n) {
    return clz(~n);
  }
  static inline unsigned int getLastOneFromRight(uint64_t n) {
    return 64 - clz(n);
  }
};

}//namespace mygc
#endif //MYGC_TOOLS_H
