#ifndef GAME_MGR_H
#define GAME_MGR_H

#include "Timing.h"
#include "GameObject.h"
#include "Scene.h"
#include "GraphicsManager.h"
#include "PhysicsMgr.h"
#include "InputMgr.h"

class GameMgr
{
private:
  enum GAME_MGR_OBJ_NAMES
  {
    GMO_DBG_OVERLAY = 0
  };

protected:
  static const unsigned int DEFAULT_WIDTH = 960;
  static const unsigned int DEFAULT_HEIGHT = 540;

  // Objects without a particular scene. Ex Debug Overlay is persistent across scenes.
  std::map<uint32_t, GameObject*> m_objs;
  Scene* m_pActiveScene;
  SceneIo m_sceneIo;

  Timing m_timing;

  GraphicsManager m_gm;
  PhysicsManager m_pm;

  InputApi m_inputState;
  InputMgr m_inputMgr;

  uint32_t m_width;
  uint32_t m_height;

public:
  GameMgr();
  ~GameMgr();

  bool init(
    HINSTANCE hinst,
    HWND hwnd,
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    Scene *pStartingScene,
    uint32_t width = DEFAULT_WIDTH,
    uint32_t height = DEFAULT_HEIGHT);
  
  bool update(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon);

  bool release();
};

#endif