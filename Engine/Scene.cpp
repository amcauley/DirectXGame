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
  for (auto it = m_objs.begin(); it != m_objs.end(); ++it)
  {
    tempPmIn.pModel = it->second->getPModel();
    tempPmIn.pos    = it->second->getPos();
    tempPmIn.vel    = it->second->getVel();
    tempPmIn.rot    = it->second->getRot();
    tempPmIn.rotVel = it->second->getRotVel();
    if (!sceneIo.pPhysicsMgr->registerModel(it->second->getUuid(), &tempPmIn))
    {
      LOGE("Failed to update object [%u]", it->first);
      return false;
    }
  }

  // Run physics
  sceneIo.pPhysicsMgr->run(sceneIo.timeMs);

  // 2nd loop: get physics results
  PModelOutput tempPmOut;
  for (auto it = m_objs.begin(); it != m_objs.end(); ++it)
  {
    sceneIo.pPhysicsMgr->getResult(it->second->getUuid(), &tempPmOut);
   
    //LOGD("Handling obj %u", it->second->getUuid());

    // Object and Scene level collision handling.
    for (auto collIt = tempPmOut.collisionSet.begin(); collIt != tempPmOut.collisionSet.end(); ++collIt)
    {
      it->second->handleCollision(*collIt);
      handleCollision(it->second, &tempPmOut);
    }

    // Assign back to object.
    it->second->setPos(tempPmOut.pos);
    it->second->setVel(tempPmOut.vel);
    it->second->setRot(tempPmOut.rot);
    it->second->setRotVel(tempPmOut.rotVel);
  }

  // 3rd loop: (non-physics) update routines and rendering
  for (auto it = m_objs.begin(); it != m_objs.end(); ++it)
  {
    if (!GameObject::updateGameObject(it->second, dev, devcon, sceneIo.timeMs, sceneIo.input, sceneIo.pSoundMgr))
    {
      LOGE("Failed to update object [%u]", it->first);
      return false;
    }

    sceneIo.pGraphicsMgr->setPosAndRot(it->second->getPos(), it->second->getRot());
    sceneIo.pGraphicsMgr->renderModel(it->second->getVModel(), dev, devcon);
  }

  return true;
}

bool Scene::release()
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
  for (auto it = m_objs.begin(); it != m_objs.end(); ++it)
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
