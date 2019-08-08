#include "TestScene.h"
#include "../Engine//Logger.h"
#include "../Engine/CommonPhysConsts.h"
#include "../Engine/Objects/PolyObj.h"
#include "../Engine/VisualModels/TexBox.h"
#include "../Engine/VisualModels/TexCylinder.h"
#include "../Engine/PhysicsModels/CollisionModels/AABB.h"
#include "../Engine/SoundMgr.h"

TestScene::TestScene()
{
  m_type = SCENE_TYPE_TEST;
  m_bgSoundHandle = SOUND_MGR_INVALID_HANDLE;
}


bool TestScene::init(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
  // File loading
  m_objMgr.generateFromFile("Tools/TestOut.txt", dev, devcon, NAMED_OBJECTS_COUNT);

  return true;
}


bool TestScene::prelimUpdate(ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo)
{
  SoundMgr *pSoundMgr = sceneIo.pSoundMgr;

  LOGD("TestScene prelimUpdate");
  if (!pSoundMgr)
  {
    LOGW("NULL pSoundMgr");
    return false;
  }

  if (pSoundMgr->registerSound(
    std::string("Sounds/Test0_120bpm.wav"),
    m_bgSoundHandle))
  {
    pSoundMgr->playSound(m_bgSoundHandle, true);
  }

  return Scene::prelimUpdate(dev, devcon, sceneIo);
}


bool TestScene::update(ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo)
{
  bool bSuccess = Scene::update(dev, devcon, sceneIo);

  // Camera follows the controllable object (in location 0).
  Pos3 tempPos = m_objMgr.getObject(TSO_PLAYER)->getPos();

  // 2D-style camera
  sceneIo.camEye.pos.x = tempPos.pos.x;
  sceneIo.camEye.pos.y = tempPos.pos.y + EYE_VERT_OFFSET;
  sceneIo.camEye.pos.z = tempPos.pos.z + 5.0;
  sceneIo.camLookAt.pos.x = tempPos.pos.x;
  sceneIo.camLookAt.pos.y = tempPos.pos.y + EYE_VERT_OFFSET;
  sceneIo.camLookAt.pos.z = tempPos.pos.z;

  ///TODO: Get output from collisions and run any scene-specific collision handling
  return bSuccess;
}


void TestScene::handleCollision(GameObject* obj, PModelOutput *pModelOut)
{
  //LOGD("TestScene level collision handling for obj %u", obj->getUuid());
}
