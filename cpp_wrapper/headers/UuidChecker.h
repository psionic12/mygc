//
// Created by liu on 19-10-25.
//

#ifndef MYGC_UUIDCHECKER_H
#define MYGC_UUIDCHECKER_H

namespace boost {
namespace uuids {
struct uuid;
}
}
namespace mygc {
class UuidChecker {
 public:
  UuidChecker();
  bool check();
  const boost::uuids::uuid * mUuid;

};
}//namespace mygc
#endif //MYGC_UUIDCHECKER_H
