//
// Created by liu on 19-11-18.
//

#ifndef MYGC_DYNAMICSLOTS_H
#define MYGC_DYNAMICSLOTS_H
#include <cstddef>
#include <vector>
#include "Tools.h"
namespace mygc {
template<typename SlotType>
class DynamicSlots {
 public:
  void *safeGetSlot(size_t index) {
    size_t naturalIndex = index + 1;
    size_t base = Tools::getLastOneFromRight(naturalIndex) - 1;
    while (mSlots.size() <= base) {
      mSlots.push_back(new SlotType[1 << mSlots.size()]);
    }
    size_t offset = naturalIndex & ~(1ul << base);
    return mSlots[base] + offset;
  }
  const std::vector<SlotType *> &data() {
    return mSlots;
  }
 private:
  std::vector<SlotType *> mSlots;
};
}//namespace mygc


#endif //MYGC_DYNAMICSLOTS_H
