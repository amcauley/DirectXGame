#include "GameObject.h"
#include "Util.h"
#include "Logger.h"
#include "Objects/DebugOverlay.h"
#include "Objects/PolyObj.h"
#include "Objects/ControllableObj.h"

GameObject::GameObject()
{
  m_uuid    = genUUID();
  m_pos     = Pos3(0.0f, 0.0f, 0.0f);
  m_rot     = Pos3(0.0f, 0.0f, 0.0f);
  m_vel     = Pos3(0.0f, 0.0f, 0.0f);
  m_rotVel  = Pos3(0.0f, 0.0f, 0.0f);
  m_pVModel = NULL;
  m_pPModel = NULL;

  LOGD("Constructing GameObject %u", m_uuid);
}


GameObject::GameObject(VisualModel *pVModel)
{
  setVModel(pVModel);
}

// Each derived class should use a release() implementation to free memory.
GameObject::~GameObject()
{
  if (m_pVModel)
  {
    LOGE("GameObject of type %u had unreleased VModel", m_type);
  }
  DELETE_AND_NULL(m_pVModel);
}

bool GameObject::init(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
  return true;
}

// Any derived class should use a release() implementation to free memory.
bool GameObject::release()
{
  LOGD("Releasing GameObject %lu", m_uuid);

  VisualModel::releaseVModel(m_pVModel);
  DELETE_AND_NULL(m_pVModel);
  return true;
}


void GameObject::setVModel(VisualModel* pVModel)
{
  m_pVModel = pVModel;
}


void GameObject::setPModel(PhysicsModel* pPModel)
{
  m_pPModel = pPModel;
}


VisualModel* GameObject::getVModel()
{
  return m_pVModel;
}


PhysicsModel* GameObject::getPModel()
{
  return m_pPModel;
}


// General purpose update. VisualModels and PhysicsModels are handled by their respective managers separately.
bool GameObject::update(ID3D11Device *dev, ID3D11DeviceContext *devcon, float timeMs, InputApi &input)
{
  return true;
}


GameObjectType GameObject::getType()
{
  return m_type;
}


uint64_t GameObject::getUuid()
{
  return m_uuid;
}


bool GameObject::updateGameObject(
  GameObject * pObj,
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  float timeMs,
  InputApi &input)
{
  if (!pObj)
  {
    LOGE("Null pObj");
    return false;
  }

  GameObjectType gOType = pObj->getType();
  switch (gOType)
  {
    case GAME_OBJECT_DEBUG_OVERLAY:
    {
      return static_cast<DebugOverlay*>(pObj)->update(dev, devcon, timeMs, input);
    }
    case GAME_OBJECT_CONTROLLABLE:
    {
      return static_cast<ControllableObj*>(pObj)->update(dev, devcon, timeMs, input);
    }
    case GAME_OBJECT_POLY_OBJ:
    {
      return static_cast<PolyObj*>(pObj)->update(dev, devcon, timeMs, input);
    }
    default:
    {
      LOGE("GameObject type not recognized: %d", gOType);
      return false;
    }
  }

  return false;
}


void GameObject::setPos(Pos3 &newPos)
{
  m_pos = newPos;
}


Pos3 GameObject::getPos()
{
  return m_pos;
}


void GameObject::setVel(Pos3 &newVel)
{
  m_vel = newVel;
}


Pos3 GameObject::getVel()
{
  return m_vel;
}


void GameObject::setRot(Pos3 &rot)
{
  m_rot = rot;
}


Pos3 GameObject::getRot()
{
  return m_rot;
}


void GameObject::setRotVel(Pos3 &rotVel)
{
  m_rotVel = rotVel;
}


Pos3 GameObject::getRotVel()
{
  return m_rotVel;
}


bool GameObject::releaseGameObject(GameObject * pObj)
{
  if (!pObj)
  {
    LOGW("Null pObj");
    return true;
  }

  GameObjectType gOType = pObj->getType();
  switch (gOType)
  {
    case GAME_OBJECT_DEBUG_OVERLAY:
    {
      return static_cast<DebugOverlay*>(pObj)->release();
    }
    case GAME_OBJECT_CONTROLLABLE:
    {
      return static_cast<ControllableObj*>(pObj)->release();
    }
    case GAME_OBJECT_POLY_OBJ:
    {
      return static_cast<PolyObj*>(pObj)->release();
    }
    default:
    {
      LOGE("GameObject type not recognized: %d", gOType);
      return false;
    }
  }

  return false;
}

void GameObject::handleCollision(PmModelStorage* pOtherObjStorage)
{
  //LOGD("Object level collision handling for obj %u", getUuid());
}
