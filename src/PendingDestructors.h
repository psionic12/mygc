//
// Created by liu on 19-11-13.
//

#ifndef MYGC_NONTRIVIALLIST_H
#define MYGC_NONTRIVIALLIST_H
namespace mygc {
class ObjectRecord;
class PendingDestructors {
 public:
  PendingDestructors();
  ~PendingDestructors();
  void add(ObjectRecord *record);
  /// try to remove a record from the list
  /// This method do not call object's destructor
  void tryRemove(ObjectRecord *record);
  /// clear the list
  /// will call objects' destructors
  void clear();
 private:
  ObjectRecord *mHead;
  ObjectRecord *mTail;
};
}//namespace mygc
#endif //MYGC_NONTRIVIALLIST_H
