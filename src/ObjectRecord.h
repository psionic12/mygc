//
// Created by liu on 19-10-30.
//

#ifndef MYGC_OBJECTRECORD_H
#define MYGC_OBJECTRECORD_H

class ObjectRecord {
 public:
  size_t size;
  IDescriptor *descriptor;
  const char *data[];
};

#endif //MYGC_OBJECTRECORD_H
