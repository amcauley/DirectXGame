#include "GameMgr.h"
#include "Logger.h"
#include "GameObject.h"
#include "Objects/DebugOverlay.h"
#include "CommonPhysConsts.h"

GameMgr::GameMgr()
{
  m_pActiveScene = NULL;
  memset(&m_sceneIo, 0, sizeof(m_sceneIo));
  memset(&m_inputState, 0, sizeof(m_inputState));
}


GameMgr::~GameMgr()
{
  release();
}


bool GameMgr::init(
  HINSTANCE hinst,
  HWND hwnd,
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  Scene* pStartingScene,
  uint32_t width,
  uint32_t height)
{
  m_pActiveScene = pStartingScene;
  m_width = width;
  m_height = height;

  if (!m_timing.init())
  {
    LOGE("Failed to init GameMgr timer");
    return false;
  }

  if (!m_inputMgr.init(hinst, hwnd, width, height))
  {
    LOGE("Failed to init input mgr");
    return false;
  }

  if (!m_soundMgr.init(hwnd))
  {
    LOGW("Failed to init sound mgr");
  }
  else
  {
    m_sceneIo.pSoundMgr = &m_soundMgr;
  }

  // Set up camera
  m_gm.initConstBuffer(dev, devcon);
  m_gm.setPerspective(
    PHYS_CONST_PI / 2.0f,
    1.0f * m_width / m_height,
    RENDER_NEAR_DIST_M * UNITS_PER_METER,
    RENDER_FAR_DIST_M * UNITS_PER_METER);

  m_gm.setPosAndRot(
    Pos3(0.0f, 0.0f, 0.0f),
    Pos3(0.0f, 0.0f, 0.0f)
    );

  // Initial camera settings
  m_sceneIo.camEye = Pos3(0.0f, 0.0f, 0.0f);
  m_sceneIo.camLookAt = Pos3(0.0f, 0.0f, -1.0f);
  m_sceneIo.camUp = Pos3(0.0f, 1.0f, 0.0f);

  m_sceneIo.pGraphicsMgr = &m_gm;
  m_sceneIo.pPhysicsMgr = &m_pm;

  DebugOverlay *pDbgOverlay = new DebugOverlay();
  pDbgOverlay->init(dev, devcon);
  m_objs[GMO_DBG_OVERLAY] = pDbgOverlay;

  return true;
}


bool GameMgr::update(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon)
{
  if (!m_pActiveScene)
  {
    LOGE("Null active scene");
  }

  if (!m_sceneIo.pGraphicsMgr)
  {
    LOGE("Null Graphic Mgr");
    return false;
  }

  m_inputMgr.getUpdate(m_inputState);
  m_sceneIo.input = m_inputState;

  m_sceneIo.timeMs = m_timing.getTimeMs();
  bool bSuccess = Scene::updateScene(m_pActiveScene, dev, devcon, m_sceneIo);

  // Update camera settings
  m_gm.setCamera(
    m_sceneIo.camEye,
    m_sceneIo.camLookAt,
    m_sceneIo.camUp
    );

  // GameMgr can display some visuals if needed, but won't run the objects physics managers.
  // Physics is handled only within scene updates.
  for (auto it = m_objs.begin(); it != m_objs.end(); ++it)
  {
    if (!GameObject::updateGameObject(it->second, dev, devcon, m_sceneIo.timeMs, m_sceneIo.input))
    {
      LOGE("Failed to update obj [%u], continuing", it->first);
      return false;
    }

    m_sceneIo.pGraphicsMgr->setPosAndRot(it->second->getPos(), it->second->getRot());
    m_sceneIo.pGraphicsMgr->renderModel(it->second->getVModel(), dev, devcon);
  }

  return true;
}


bool GameMgr::release()
{
  bool bSuccess = true;
  for (auto it = m_objs.begin(); it != m_objs.end(); ++it)
  {
    if (!GameObject::releaseGameObject(it->second))
    {
      LOGE("Failed to release obj [%u], continuing", it->first);
      bSuccess = false;
    }
  }

  if (!Scene::releaseScene(m_pActiveScene))
  {
    LOGE("Failed to release active scene");
    bSuccess = false;
  }

  return bSuccess;
}
