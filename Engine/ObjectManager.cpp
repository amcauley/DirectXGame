#include <fstream>
#include <sstream>

#include "Logger.h"
#include "ObjectManager.h"
#include "Objects/PolyObj.h"
#include "VisualModels/TexBox.h"
#include "PhysicsModels/CollisionModels/AABB.h"

// Read in a file and parse it into a vector of newly allocated objects.
// Deleting objects once done with them is the responsibility of the caller.
void ObjectManager::generateFromFile(
  std::string filename,
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  uint32_t idOffset)
{
  std::ifstream fs(filename);
  std::string curLine;

  if (std::getline(fs, curLine))
  {
    LOGI("Parsing file: %s", filename);
  }
  else
  {
    LOGE("Error reading scene: %s", filename);
  }

  fs.clear();
  fs.seekg(0, std::ios::beg);

  int lineCnt = 0;

  /* Start by counting the number of objects. */
  while (std::getline(fs, curLine))
  {
    lineCnt++;

    std::istringstream lineStream(curLine);
    std::string curWord;

    lineStream >> curWord;

    // TODO: Unify specifications with MapParser.py in a single location (JSON / XML / etc.)
    if (curWord == "B") // Block: 'B {loc} {dim} {texture}
    {
      std::string tex;
      float locX, locY, locZ, dimX, dimY, dimZ;
      lineStream >> locX >> locY >> locZ >> dimX >> dimY >> dimZ >> tex;
      TexBox *pVObj = new TexBox;
      pVObj->init(
        dev, devcon,
        dimX, dimY, dimZ,
        tex,
        dimX, dimY, dimZ);

      PolyObj *pObj = new PolyObj;
      pObj->init(pVObj);
      pObj->setPos(Pos3(locX, locY, locZ));
      pObj->setPModel(new PhysicsModel);
      pObj->getPModel()->setCollisionModel(new AABB(dimX, dimY, dimZ));
      pObj->getPModel()->getCollisionModel()->setType(COLLISION_MODEL_AABB_IMMOBILE);
      pObj->getPModel()->getCollisionModel()->setPos(Pos3(0.0, 0.0, 0.0));

      addObject(lineCnt + idOffset, pObj);
    }
    else /* Default case: */
    {
      continue;
    }
  }
}

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
