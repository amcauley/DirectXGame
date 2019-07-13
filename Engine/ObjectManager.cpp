#include "Logger.h"
#include "ObjectManager.h"


void ObjectManager::init()
{

}


bool ObjectManager::release()
{
  bool bSuccess = true;
  for (auto it = m_objs.begin(); it != m_objs.end(); ++it)
  {
    if (!GameObject::releaseGameObject(it->second))
    {
      // Don't return yet, can still try to release other objects.
      LOGE("Failed to release obj [%u], continuing", it->first);
      bSuccess = false;
    }
  }

  return bSuccess;
}


void ObjectManager::addObject(uint32_t id, GameObject* pObj)
{
  m_objs[id] = pObj;
}


GameObject* ObjectManager::getObject(uint32_t id)
{
  return m_objs[id];
}


ObjectManagerObjMap::iterator ObjectManager::begin()
{
  return m_objs.begin();
}


ObjectManagerObjMap::iterator ObjectManager::end()
{
  return m_objs.end();
}


GameObject* ObjectManager::getFirstPObj()
{
  m_lastPItr = m_objs.begin();
  return (m_lastPItr == m_objs.end()) ? NULL : m_lastPItr->second;
}


GameObject* ObjectManager::getNextPObj()
{
  m_lastPItr++;
  return (m_lastPItr == m_objs.end()) ? NULL : m_lastPItr->second;
}


GameObject* ObjectManager::getFirstVObj()
{
  m_lastVItr = m_objs.begin();
  return (m_lastVItr == m_objs.end()) ? NULL : m_lastVItr->second;
}


GameObject* ObjectManager::getNextVObj()
{
  m_lastVItr++;
  return (m_lastVItr == m_objs.end()) ? NULL : m_lastVItr->second;
}
