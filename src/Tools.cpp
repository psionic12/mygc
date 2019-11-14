//
// Created by liu on 19-11-14.
//

#include "Tools.h"

#ifdef __GNUC__
#define clz(x) __builtin_clzll(x)
#define ffs(x) __builtin_ffsll(x)
#else
#define clz(x) assert(false && "no clz implementation found")
#endif

uint64_t mygc::Tools::getFirstZero(uint64_t n) {
  return clz(~n);
}
uint64_t mygc::Tools::getFirstOne(uint64_t n) {
  return 64 - clz(n);
}
