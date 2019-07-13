#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include "GameObject.h"
#include <map>

typedef std::map<uint32_t, GameObject*> ObjectManagerObjMap;

class ObjectManager
{
protected:
  // Map from ID -> Object Ptr.
  ObjectManagerObjMap m_objs;
  ObjectManagerObjMap::iterator m_lastPItr;
  ObjectManagerObjMap::iterator m_lastVItr;

public:
  virtual void init();
  virtual bool release();

  virtual void addObject(uint32_t id, GameObject* pObj);
  virtual GameObject* getObject(uint32_t id);

  // Multiple ways to iterate through objects.
  // 1) Iterate over all objects.
  // 2) Iterate over only those objects that need physics processing.
  // 3) Iterate over only those objects that need visual processing.
  // Don't insert/remove any objects in the middle of iterating.
  virtual ObjectManagerObjMap::iterator begin();
  virtual ObjectManagerObjMap::iterator end();
  virtual GameObject* getFirstPObj();
  virtual GameObject* getNextPObj();
  virtual GameObject* getFirstVObj();
  virtual GameObject* getNextVObj();
};

#endif
