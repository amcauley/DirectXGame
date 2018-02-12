#include "TestScene.h"
#include "../Engine/CommonPhysConsts.h"
#include "../Engine/Objects/PolyObj.h"
#include "../Engine/Objects/DebugOverlay.h"
#include "../Engine/Objects/ControllableObj.h"
#include "../Engine/VisualModels/TexBox.h"
#include "../Engine/PhysicsModels/CollisionModels/AABB.h"

TestScene::TestScene()
{
  m_type = SCENE_TYPE_TEST;
}


bool TestScene::init(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
  ControllableObj *pContObj = new ControllableObj;
  pContObj->init(dev, devcon);
  m_objs.push_back(pContObj);

  TexBox *pBox = NULL;
  PolyObj *pObj = NULL;

  ///TODO: Add string -> obj ptr map to give names to each object.

  // These coords are relative the object's center, currently at the origin.
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
  pObj->setPos(Pos3(2.0, 1.0, -5.0));
  pObj->setPModel(new PhysicsModel);
  pObj->getPModel()->setCollisionModel(new AABB(2.0, 2.0, 0.1));
  pObj->getPModel()->getCollisionModel()->setType(COLLISION_MODEL_AABB);
  m_objs.push_back(pObj);


  pBox = new TexBox;
  // These coords are relative the object's center, currently at the origin.
  pBox->init(
    dev, devcon,
    0.8, 0.8, 0.8,
    std::string("Textures/cat.dds"),
    2.0, 3.0
  );
  pObj = new PolyObj;
  pObj->init(pBox);
  // Now set the global position.
  pObj->setPos(Pos3(-3.0, 0.0, -5.0));
  pObj->setPModel(new PhysicsModel);
  pObj->getPModel()->setCollisionModel(new AABB(1.2, 1.2, 1.2));
  pObj->getPModel()->getCollisionModel()->setType(COLLISION_MODEL_AABB_IMMOBILE);
  m_objs.push_back(pObj);


  // Floor
  pBox = new TexBox;
  pBox->init(
    dev, devcon,
    50, 0.1, 100,
    std::string("Textures/TestPattern.dds"),
    50, 100
    );
  pObj = new PolyObj;
  pObj->init(pBox);
  // Now set the global position.
  pObj->setPos(Pos3(0.0, -1.5, 0.0));
  m_objs.push_back(pObj);


  return true;
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

  tempPos = m_objs[1]->getPos();
  tempPos.pos.x += MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP * stepsPerFrame * translateDir;

  const int translateTimeFrames = 5000;
  if (++translateCnt % translateTimeFrames == 0)
  {
    translateDir *= -1;
    translateCnt = 0;
  }
  m_objs[1]->setPos(tempPos);


  static long int spinCnt = 0;
  const float X_SPIN_SPEED_RAD_PER_SEC = 2 * PHYS_CONST_PI / 100.0;
  const float Z_SPIN_SPEED_RAD_PER_SEC = 2 * PHYS_CONST_PI / 10.0;
  const float X_SPIN_SPEED_RAD_PER_STEP = X_SPIN_SPEED_RAD_PER_SEC * SEC_PER_STEP;
  const float Z_SPIN_SPEED_RAD_PER_STEP = Z_SPIN_SPEED_RAD_PER_SEC * SEC_PER_STEP;

  tempRot = m_objs[2]->getRot();
  // Periodicity is short enough that overflow shouldn't be an issue.
  if (++spinCnt >= (2 * PHYS_CONST_PI / X_SPIN_SPEED_RAD_PER_STEP) * (2 * PHYS_CONST_PI / Z_SPIN_SPEED_RAD_PER_STEP))
  {
    spinCnt = 0;
  }
  tempRot.pos.x = X_SPIN_SPEED_RAD_PER_STEP * spinCnt * stepsPerFrame;
  tempRot.pos.z = Z_SPIN_SPEED_RAD_PER_STEP * spinCnt * stepsPerFrame;
  //m_objs[2]->setRot(tempRot);

  // Camera follows the controllable object (in location 0).
  tempPos = m_objs[0]->getPos();

  // Camera rotation
  tempRot               =  m_objs[0]->getRot();
  float upY             =  std::cos(tempRot.pos.x);
  float horizComponent  =  std::sin(tempRot.pos.x);
  float upX             = -horizComponent * std::sin(tempRot.pos.y);
  float upZ             =  horizComponent * std::cos(tempRot.pos.y);
  sceneIo.camUp         =  Pos3(upX, upY, upZ);

  // Camera translation
  float deltaX = tempPos.pos.x - sceneIo.camEye.pos.x;
  float deltaY = tempPos.pos.y - sceneIo.camEye.pos.y;
  float deltaZ = tempPos.pos.z - sceneIo.camEye.pos.z;
  sceneIo.camEye.pos.x = tempPos.pos.x;
  sceneIo.camEye.pos.y = tempPos.pos.y;
  sceneIo.camEye.pos.z = tempPos.pos.z;

  // Camera view direction
  float lookDirY          =  std::sin(tempRot.pos.x);
  horizComponent          =  std::cos(tempRot.pos.x);
  float lookDirX          =  horizComponent * std::sin(tempRot.pos.y);
  float lookDirZ          = -horizComponent * std::cos(tempRot.pos.y);
  sceneIo.camLookAt.pos.x =  tempPos.pos.x + lookDirX;
  sceneIo.camLookAt.pos.y =  tempPos.pos.y + lookDirY;
  sceneIo.camLookAt.pos.z =  tempPos.pos.z + lookDirZ;


  return Scene::update(dev, devcon, sceneIo);

  ///TODO: Get output from collisions and run any scene-specific collision handling
}