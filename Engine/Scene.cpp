#include "Scene.h"
#include "Logger.h"
#include "../Scenes/TestScene.h"
#include "GraphicsManager.h"
#include "PhysicsMgr.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::init(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
  return true;
}

bool Scene::update(ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo)
{
  //LOGD("~~~~~~~~~~ New Scene Update ~~~~~~~~~~");

  if (!sceneIo.pGraphicsMgr)
  {
    LOGE("Null pGraphicsMgr");
    return false;
  }

  if (!sceneIo.pPhysicsMgr)
  {
    LOGE("Null pPhysicsMgr");
    return false;
  }

  // 1st loop: Register objects with physics manager
  PModelInput tempPmIn;
  for (auto pObj = m_objMgr.getFirstPObj(); pObj != NULL; pObj = m_objMgr.getNextPObj())
  {
    tempPmIn.pModel = pObj->getPModel();
    tempPmIn.pos    = pObj->getPos();
    tempPmIn.vel    = pObj->getVel();
    tempPmIn.rot    = pObj->getRot();
    tempPmIn.rotVel = pObj->getRotVel();
    if (!sceneIo.pPhysicsMgr->registerModel(pObj->getUuid(), &tempPmIn))
    {
      LOGE("Failed to update object [%u]", pObj->getUuid());
      return false;
    }
  }

  // Run physics
  sceneIo.pPhysicsMgr->run(sceneIo.timeMs);

  // 2nd loop: get physics results
  PModelOutput tempPmOut;
  for (auto pObj = m_objMgr.getFirstPObj(); pObj != NULL; pObj = m_objMgr.getNextPObj())
  {
    sceneIo.pPhysicsMgr->getResult(pObj->getUuid(), &tempPmOut);
   
    //LOGD("Handling obj %u", pObj>getUuid());

    // Default update, ex. for controllable obj even if no collisions happened.
    pObj->setPos(tempPmOut.pos);
    pObj->setVel(tempPmOut.vel);
    pObj->setRot(tempPmOut.rot);
    pObj->setRotVel(tempPmOut.rotVel);

    // Object and Scene level collision handling.
    int cnt = 0;
    for (auto collIt = tempPmOut.collisions.begin(); collIt != tempPmOut.collisions.end(); ++collIt)
    {
      // Object level handling
      pObj->handleCollision(collIt->first, cnt++);

      // Scene level handling
      // Leaving out cnt for now - overall object order within a scene isn't well-definined, so collision ordering only has meaning within a particular object, not a scene-wide level.
      handleCollision(pObj, &tempPmOut);

      // Assign back to object.
      // Do this every loop so that any position resets applied from one collision handling can be accounted for in the next collision.
      // Ex) If first collision changes position/vel of object, the second collision handling can run based on the updated position/vel.
      pObj->setPos(tempPmOut.pos);
      pObj->setVel(tempPmOut.vel);
      pObj->setRot(tempPmOut.rot);
      pObj->setRotVel(tempPmOut.rotVel);
    }
  }

  // 3rd loop: (non-physics) update routines for visible objects.
  // TODO: Consider separating set of updateable objects from visible objects. Not necessarily the same.
  for (auto pObj = m_objMgr.getFirstVObj(); pObj != NULL; pObj = m_objMgr.getNextVObj())
  {
    if (!GameObject::updateGameObject(pObj, dev, devcon, sceneIo.timeMs, sceneIo.input, sceneIo.pSoundMgr))
    {
      LOGE("Failed to update object [%u]", pObj->getUuid());
      return false;
    }

    sceneIo.pGraphicsMgr->setPosAndRot(pObj->getPos(), pObj->getRot());
    sceneIo.pGraphicsMgr->renderModel(pObj->getVModel(), dev, devcon);
  }

  return true;
}

bool Scene::release()
{
  return m_objMgr.release();
}


void Scene::handleCollision(GameObject* obj, PModelOutput *pModelOut)
{
  //LOGD("Scene level collision handling for obj %u", obj->getUuid());
}


SceneType Scene::getType()
{
  return m_type;
}


bool Scene::updateScene(Scene* pScene, ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo)
{
  if (!pScene)
  {
    LOGE("Null update scene");
    return false;
  }

  int sType = pScene->getType();
  switch (sType)
  {
    case SCENE_TYPE_TEST:
    {
      static_cast<TestScene*>(pScene)->update(dev, devcon, sceneIo);
      return false;
    }
    default:
    {
      LOGE("Scene type not recognized: %d", sType);
      return false;
    }
  }

  return true;
}


bool Scene::releaseScene(Scene* pScene)
{
  if (!pScene)
  {
    LOGW("Null pScene");
    return true;
  }

  SceneType sType = pScene->getType();
  switch (sType)
  {
    case SCENE_TYPE_TEST:
    {
      return static_cast<TestScene*>(pScene)->release();
    }
    default:
    {
      LOGE("Scene type not recognized: %d", sType);
      return false;
    }
  }

  return false;
}


bool Scene::prelimUpdate(ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo)
{
  // Each object belonging to the scene should also run its prelimUpdate processing.
  bool bSuccess = true;
  for (auto it = m_objMgr.begin(); it != m_objMgr.end(); ++it)
  {
    if (!GameObject::prelimUpdateGameObject(it->second, dev, devcon, sceneIo.timeMs, sceneIo.input, sceneIo.pSoundMgr))
    {
      LOGW("Prelim update failed for obj [%u], continuing", it->first);
      bSuccess = false;
    }
    else
    {
      LOGD("Ran prelim update for obj [%u]", it->first);
    }
  }

  return bSuccess;
}


bool Scene::prelimUpdateScene(Scene* pScene, ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo)
{
  if (!pScene)
  {
    LOGE("Null prelim update scene");
    return false;
  }

  int sType = pScene->getType();
  switch (sType)
  {
    case SCENE_TYPE_TEST:
    {
      static_cast<TestScene*>(pScene)->prelimUpdate(dev, devcon, sceneIo);
      return false;
    }
    default:
    {
      LOGE("Scene type not recognized: %d", sType);
      return false;
    }
  }

  return true;
}
