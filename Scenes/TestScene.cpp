#include "TestScene.h"
#include "../Engine/CommonPhysConsts.h"
#include "../Engine/PolyObj.h"
#include "../Engine/DebugOverlay.h"
#include "../Engine/ControllableObj.h"

TestScene::TestScene()
{
  m_type = SCENE_TYPE_TEST;
}


bool TestScene::init(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
  ControllableObj *pContObj = new ControllableObj;
  pContObj->init(dev, devcon);
  m_objs.push_back(pContObj);


  // These coords are relative the object's center, currently at the origin.
  std::vector<Pos3Uv2> triVerts =
  { { -1.0f,   0.0f,   0.0f,   0.0f,   0.0f },
    {  1.0f,   1.0f,   0.0f,   1.0f,   0.0f },
    {  0.0f,  -1.0f,   0.0f,   0.0f,   1.0f },
  };
  PolyObj *pObj = new PolyObj;
  pObj->init(
    dev, devcon,
    std::string("Textures/cat.dds"),
    triVerts);
  // Now set the global position.
  pObj->setPos(Pos3(2.0, 1.0, -5.0));
  m_objs.push_back(pObj);


  TexRect *pRect = new TexRect;
  // These coords are relative the object's center, currently at the origin.
  pRect->init(
    dev, devcon,
    Pos3( -0.4f,  0.4f,  0.0f),
    Pos3(  0.4f,  0.4f,  0.0f),
    Pos3(  0.4f, -0.4f,  0.0f),
    std::string("Textures/cat.dds")
    );
  pObj = new PolyObj;
  pObj->init(pRect);
  // Now set the global position.
  pObj->setPos(Pos3(-3.0, 0.0, -5.0));
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
  const float Z_SPIN_SPEED_RAD_PER_SEC = 2 * PHYS_CONST_PI / 1.3;
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
  m_objs[2]->setRot(tempRot);


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
}