//
// Created by liu on 19-11-21.
//
#include <IDescriptor.h>

#include "IDescriptor.h"
#include "Tools.h"

int mygc::IDescriptor::getSlotIndex() {
  if (mSlotIndex == -1) {
    mSlotIndex = Tools::getLastOneFromRight(typeSize());
  }
  return mSlotIndex;
}
