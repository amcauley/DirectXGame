#include "TestScene.h"
#include "../Engine//Logger.h"
#include "../Engine/CommonPhysConsts.h"
#include "../Engine/Objects/PolyObj.h"
#include "../Engine/Objects/DebugOverlay.h"
#include "../Engine/Objects/ControllableObj.h"
#include "../Engine/Objects/Hookshot.h"
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
  TexBox *pBox = NULL;
  PolyObj *pObj = NULL;

  LOGD("Creating TSO_PLAYER");
  ControllableObj *pContObj = new ControllableObj;
  pContObj->init(dev, devcon);
  pBox = new TexBox;
  pBox->init(
    dev, devcon,
    PLAYER_HITBOX_W, PLAYER_HITBOX_H, PLAYER_HITBOX_D,
    std::string("Textures/cat.dds"),
    1.0, 1.0
    );
  pContObj->setVModel(pBox);
  m_objs[TSO_PLAYER] = pContObj;

  // These coords are relative the object's center, currently at the origin.
  LOGD("Creating TSO_CAT_TRIANGLE");
  std::vector<Pos3Uv2> triVerts =
  { { -1.0f,   0.0f,   0.0f,   0.0f,   0.0f },
    {  1.0f,   1.0f,   0.0f,   1.0f,   0.0f },
    {  0.0f,  -1.0f,   0.0f,   0.0f,   1.0f },
  };
  pObj = new PolyObj;
  pObj->init(
    dev, devcon,
    std::string("Textures/cat.dds"),
    triVerts);
  // Now set the global position.
  pObj->setPos(Pos3(2.0, 1.0, 0.0));
  pObj->setPModel(new PhysicsModel);
  pObj->getPModel()->setCollisionModel(new AABB(2.0, 2.0, 0.1));
  pObj->getPModel()->getCollisionModel()->setType(COLLISION_MODEL_AABB);
  pObj->getPModel()->getCollisionModel()->setPos(Pos3(0.0, 0.0001, 0.0));
  m_objs[TSO_CAT_TRIANGLE] = pObj;


  LOGD("Creating TSO_CAT_BOX_1");
  pBox = new TexBox;
  // These coords are relative the object's center, currently at the origin.
  pBox->init(
    dev, devcon,
    2.0, 0.5, 2.0,
    std::string("Textures/cat.dds"),
    2.0, 1.0
  );
  pObj = new PolyObj;
  pObj->init(pBox);
  // Now set the global position.
  pObj->setPos(Pos3(-3.0, 0.0, 0.0));
  pObj->setPModel(new PhysicsModel);
  pObj->getPModel()->setCollisionModel(new AABB(2.0, 0.5, 2.0));
  pObj->getPModel()->getCollisionModel()->setType(COLLISION_MODEL_AABB_IMMOBILE);
  pObj->getPModel()->getCollisionModel()->setPos(Pos3(0.0, 0.0, 0.0));
  m_objs[TSO_CAT_BOX_1] = pObj;

  LOGD("Creating TSO_CAT_BOX_2");
  pBox = new TexBox;
  // These coords are relative the object's center, currently at the origin.
  pBox->init(
    dev, devcon,
    2.0, 1.5, 2.0,
    std::string("Textures/cat.dds"),
    2.0, 1.0
    );
  pObj = new PolyObj;
  pObj->init(pBox);
  // Now set the global position.
  pObj->setPos(Pos3(-5.0, 0.0, 0.0));
  pObj->setPModel(new PhysicsModel);
  pObj->getPModel()->setCollisionModel(new AABB(2.0, 1.5, 2.0));
  pObj->getPModel()->getCollisionModel()->setType(COLLISION_MODEL_AABB_IMMOBILE);
  pObj->getPModel()->getCollisionModel()->setPos(Pos3(0.0, 0.0, 0.0));
  m_objs[TSO_CAT_BOX_2] = pObj;

  // Floor
  LOGD("Creating TSO_FLOOR");
  pBox = new TexBox;
  pBox->init(
    dev, devcon,
    20, 0.1, 20,
    std::string("Textures/TestPattern.dds"),
    20, 20
    );
  pObj = new PolyObj;
  pObj->init(pBox);
  // Now set the global position.
  pObj->setPos(Pos3(0.0, -0.05, 0.0));
  pObj->setPModel(new PhysicsModel);
  pObj->getPModel()->setCollisionModel(new AABB(20, 0.1, 20));
  pObj->getPModel()->getCollisionModel()->setType(COLLISION_MODEL_AABB_IMMOBILE);
  m_objs[TSO_FLOOR] = pObj;


  // Cylinder
  LOGD("Creating TSO_CYLINDER");
  TexCylinder *pCylinder = NULL;
  pCylinder = new TexCylinder;
  pCylinder->init(
    dev, devcon,
    1.0, 5.0,
    std::string("Textures/cat.dds"),
    4,
    2.0, 3.0);
  pObj = new PolyObj;
  pObj->init(pCylinder);
  // Now set the global position.
  pObj->setPos(Pos3(3.0, 0.0, 0.0));
  m_objs[TSO_CYLINDER] = pObj;

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
  Pos3 tempPos, tempRot;

  // Until we have actual fps limiting, just hardcode what value is expected for the demo.
  const float expectedDemoFps = 2000;
  const float stepsPerFrame = 1.0 / (SEC_PER_STEP * expectedDemoFps);

  static int translateCnt = 0;
  static int translateDir = 1;
  const float TEST_MOVEMENT_MPS = 1.0;

  tempPos = m_objs[TSO_CAT_TRIANGLE]->getPos();
  tempPos.pos.x += MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP * stepsPerFrame * translateDir;

  const int translateTimeFrames = 5000;
  if (++translateCnt % translateTimeFrames == 0)
  {
    translateDir *= -1;
    translateCnt = 0;
  }
  m_objs[TSO_CAT_TRIANGLE]->setPos(tempPos);

  static int cylCnt = 0;
  TexCylinder *pCylinderVModel = static_cast<TexCylinder*>(m_objs[TSO_CYLINDER]->getVModel());
  const float CYLINDER_GROW_RATE_PER_SEC = 0.2;
  pCylinderVModel->updateLength(dev, devcon, 1.0 + CYLINDER_GROW_RATE_PER_SEC * SEC_PER_STEP * cylCnt);
  cylCnt = cylCnt > 40000 / STEP_SIZE_MS ? 0 : cylCnt + 1;

  static float boxHeight = 0;
  const float BOX_HEIGHT_GROW_RATE_PER_SEC = 0.1;
  boxHeight = boxHeight > 2 ? 0 : boxHeight + BOX_HEIGHT_GROW_RATE_PER_SEC * SEC_PER_STEP;
  m_objs[TSO_CAT_BOX_2]->setPos(Pos3(-5.0, boxHeight, 0.0));

  static long int spinCnt = 0;
  const float X_SPIN_SPEED_RAD_PER_SEC = 2 * PHYS_CONST_PI / 100.0;
  const float Z_SPIN_SPEED_RAD_PER_SEC = 2 * PHYS_CONST_PI / 10.0;
  const float X_SPIN_SPEED_RAD_PER_STEP = X_SPIN_SPEED_RAD_PER_SEC * SEC_PER_STEP;
  const float Z_SPIN_SPEED_RAD_PER_STEP = Z_SPIN_SPEED_RAD_PER_SEC * SEC_PER_STEP;

  tempRot = m_objs[TSO_CAT_BOX_1]->getRot();
  // Periodicity is short enough that overflow shouldn't be an issue.
  if (++spinCnt >= (2 * PHYS_CONST_PI / X_SPIN_SPEED_RAD_PER_STEP) * (2 * PHYS_CONST_PI / Z_SPIN_SPEED_RAD_PER_STEP))
  {
    spinCnt = 0;
  }
  tempRot.pos.x = X_SPIN_SPEED_RAD_PER_STEP * spinCnt * stepsPerFrame;
  tempRot.pos.z = Z_SPIN_SPEED_RAD_PER_STEP * spinCnt * stepsPerFrame;
  //m_objs[TSO_CAT_BOX_1]->setRot(tempRot);

  // Camera follows the controllable object (in location 0).
  tempPos = m_objs[TSO_PLAYER]->getPos();

  // Camera rotation
  //tempRot               =  m_objs[TSO_PLAYER]->getRot();
  //float upY             =  std::cos(tempRot.pos.x);
  //float horizComponent  =  std::sin(tempRot.pos.x);
  //float upX             = -horizComponent * std::sin(tempRot.pos.y);
  //float upZ             =  horizComponent * std::cos(tempRot.pos.y);
  //sceneIo.camUp         =  Pos3(upX, upY, upZ);

  // Camera translation
  sceneIo.camEye.pos.x = tempPos.pos.x;
  sceneIo.camEye.pos.y = tempPos.pos.y + EYE_VERT_OFFSET;
  //sceneIo.camEye.pos.z = tempPos.pos.z;

  // Camera view direction
  //float lookDirY          =  std::sin(tempRot.pos.x);
  //horizComponent          =  std::cos(tempRot.pos.x);
  //float lookDirX          =  horizComponent * std::sin(tempRot.pos.y);
  //float lookDirZ          = -horizComponent * std::cos(tempRot.pos.y);
  //sceneIo.camLookAt.pos.x =  tempPos.pos.x + lookDirX;
  //sceneIo.camLookAt.pos.y =  tempPos.pos.y + EYE_VERT_OFFSET + lookDirY;
  //sceneIo.camLookAt.pos.z =  tempPos.pos.z + lookDirZ;

  // 2D-style camera
  sceneIo.camEye.pos.z = tempPos.pos.z + 5.0;
  sceneIo.camLookAt.pos.x =  tempPos.pos.x;
  sceneIo.camLookAt.pos.y =  tempPos.pos.y + EYE_VERT_OFFSET;
  sceneIo.camLookAt.pos.z =  tempPos.pos.z;

  return Scene::update(dev, devcon, sceneIo);

  ///TODO: Get output from collisions and run any scene-specific collision handling
}


void TestScene::handleCollision(GameObject* obj, PModelOutput *pModelOut)
{
  //LOGD("TestScene level collision handling for obj %u", obj->getUuid());
}
