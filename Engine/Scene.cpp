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
  for (int i = 0; i < m_objs.size(); ++i)
  {
    tempPmIn.pModel = m_objs[i]->getPModel();
    tempPmIn.pos    = m_objs[i]->getPos();
    tempPmIn.vel    = m_objs[i]->getVel();
    tempPmIn.rot    = m_objs[i]->getRot();
    tempPmIn.rotVel = m_objs[i]->getRotVel();
    if (!sceneIo.pPhysicsMgr->registerModel(m_objs[i]->getUuid(), &tempPmIn))
    {
      LOGE("Failed to update object [%u]", i);
      return false;
    }
  }

  // Run physics
  sceneIo.pPhysicsMgr->run(sceneIo.timeMs);

  // 2nd loop: get physics results
  PModelOutput tempPmOut;
  for (int i = 0; i < m_objs.size(); ++i)
  {
    sceneIo.pPhysicsMgr->getResult(m_objs[i]->getUuid(), tempPmOut);
   
    ///TODO: run object-level collision handling based on any collisions

    // Assign back to object.
    m_objs[i]->setPos(tempPmOut.pos);
    m_objs[i]->setVel(tempPmOut.vel);
    m_objs[i]->setRot(tempPmOut.rot);
    m_objs[i]->setRotVel(tempPmOut.rotVel);
  }

  // 3rd loop: (non-physics) update routines and rendering
  for (int i = 0; i < m_objs.size(); ++i)
  {
    if (!GameObject::updateGameObject(m_objs[i], dev, devcon, sceneIo.timeMs, sceneIo.input))
    {
      LOGE("Failed to update object [%u]", i);
      return false;
    }

    sceneIo.pGraphicsMgr->setPosAndRot(m_objs[i]->getPos(), m_objs[i]->getRot());
    sceneIo.pGraphicsMgr->renderModel(m_objs[i]->getVModel(), dev, devcon);
  }

  return true;
}

bool Scene::release()
{
  bool bSuccess = true;
  for (int i = 0; i < m_objs.size(); ++i)
  {
    if (!GameObject::releaseGameObject(m_objs[i]))
    {
      // Don't return yet, can still try to release other objects.
      LOGE("Failed to release obj [%u], continuing", i);
      bSuccess = false;
    }
  }

  return bSuccess;
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
