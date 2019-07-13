#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "ObjectManager.h"
#include <map>

class Scene;
class GraphicsManager;
class PhysicsManager;
class SoundMgr;

typedef enum SceneType_
{
  SCENE_TYPE_NONE = 0,
  SCENE_TYPE_TEST
} SceneType;

typedef struct SceneIo_
{
  // Inputs:
  double timeMs;
  GraphicsManager *pGraphicsMgr;
  PhysicsManager  *pPhysicsMgr;
  SoundMgr        *pSoundMgr;
  InputApi input;

  // Outputs:
  Scene *pNextScene;

  Pos3   camEye;
  Pos3   camLookAt;
  Pos3   camUp;

  // Inputs + Outputs:

  SceneIo_()
  {
    pGraphicsMgr  = NULL;
    pPhysicsMgr   = NULL;
    pSoundMgr     = NULL;
    pNextScene    = NULL;
  }

} SceneIo;


class Scene
{
protected:
  SceneType m_type = SCENE_TYPE_NONE;
  ObjectManager m_objMgr;

public:
  static bool updateScene(
    Scene* pScene,
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    SceneIo &sceneIo);

  // Happens before the first update is called, useful for loading sounds, other resources that'll be used later.
  static bool prelimUpdateScene(
    Scene* pScene,
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    SceneIo &sceneIo);

  static bool releaseScene(Scene* pScene);

  SceneType getType();

  Scene();
  ~Scene();
  virtual bool init(ID3D11Device *dev, ID3D11DeviceContext *devcon);
  virtual bool release();
  virtual bool update(ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo);
  virtual bool prelimUpdate(ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo);
  virtual void handleCollision(GameObject* obj, PModelOutput *pModelOut);
};

#endif