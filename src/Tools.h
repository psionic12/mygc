//
// Created by liu on 19-11-14.
//

#ifndef MYGC_TOOLS_H
#define MYGC_TOOLS_H

#include <cstdint>
namespace mygc {
class Tools {
 public:
  static uint64_t getFirstZero(uint64_t n);
  static uint64_t getFirstOne(uint64_t n);
};
}//namespace mygc
#endif //MYGC_TOOLS_H
