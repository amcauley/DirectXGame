#include "ControllableObj.h"
#include "../PhysicsModels/PhysicsUpdateModels/GravityModel.h"
#include "../CommonPhysConsts.h"
#include "../Logger.h"
#include "../PhysicsModels/CollisionModels/AABBControllable.h"
#include "../SoundMgr.h"
#include <cmath>

ControllableObj::ControllableObj()
{
  m_type = GAME_OBJECT_CONTROLLABLE;
  m_pVModel = NULL;
  m_pPModel = new PhysicsModel;
  m_pPModel->setCollisionModel(new AABBControllable(PLAYER_HITBOX_W, PLAYER_HITBOX_H, PLAYER_HITBOX_D));
  m_pos.pos.y = PLAYER_HITBOX_H / 2;
  m_pPModel->setPuModel(new GravityModel);
  m_lastJumpMs = 0.0;
  m_pingSoundHandle = SOUND_MGR_INVALID_HANDLE;

  LOGD("GameObject %lu = type %d", static_cast<unsigned long>(m_uuid), m_type);
}

bool ControllableObj::init(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
  return true;
}


bool ControllableObj::prelimUpdate(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  float timeMs,
  InputApi &input,
  SoundMgr *pSoundMgr)
{
  LOGD("ControllableObj [%] prelimUpdate", m_uuid);
  if (!pSoundMgr)
  {
    LOGW("NULL pSoundMgr");
    return false;
  }

  return pSoundMgr->registerSound(
    std::string("Sounds/TestPing.wav"),
    m_pingSoundHandle);
}


bool ControllableObj::update(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  float timeMs,
  InputApi &input,
  SoundMgr *pSoundMgr)
{
  Pos3 tempVel = getVel();
  const double JUMP_COOLDOWN_MS = 300;
  if ((input.bSpace) &&
      (timeMs - m_lastJumpMs > JUMP_COOLDOWN_MS) &&
      static_cast<AABBControllable*>(m_pPModel->getCollisionModel())->getJumpEn())
  {
    m_lastJumpMs = timeMs;
    tempVel.pos.y = (JUMP_VELOCITY_MPS * MPS_TO_UNITS_PER_STEP);

    pSoundMgr->playSound(m_pingSoundHandle);
  }

  // Need to explicitly reset jump flag - it's set in the physics collision model, but cleared by the object.
  static_cast<AABBControllable*>(m_pPModel->getCollisionModel())->setJumpEn(false);

  // Handle non-jump movement
  float velUp = input.keyUp;
  float velRight = input.keyRight;
  
  float normFactor = std::sqrt(velUp*velUp + velRight*velRight);

  // Don't bother normalizing unless we're moving too fast. This should allow us to move slowly if desired instead of
  // forcing constant speed all the time.
  if (normFactor > MOVEMENT_VEL_MPS * MOVEMENT_VEL_MPS)
  {
    float invNormFactor = 1.0 / normFactor;
    velUp *= normFactor * invNormFactor;
    velRight *= normFactor * invNormFactor;
  }

  // Rotations
  Pos3 tempRotVel = getRotVel();
  tempRotVel.pos.x = input.pitchUp * TURN_RATE_RAD_PS * SEC_PER_STEP;
  tempRotVel.pos.y = input.yawCw * TURN_RATE_RAD_PS * SEC_PER_STEP;
  setRotVel(tempRotVel);


  Pos3 tempRot = getRot();

  if (tempRot.pos.x > MAX_PITCH_RADS)
  {
    tempRot.pos.x = MAX_PITCH_RADS;
  }
  else if (tempRot.pos.x < MIN_PITCH_RADS)
  {
    tempRot.pos.x = MIN_PITCH_RADS;
  }

  setRot(tempRot);

  float sinFactor = std::sin(tempRot.pos.y);
  float cosFactor = std::cos(tempRot.pos.y);
  float origVelUp = velUp;
  velUp           = velUp * cosFactor - velRight * sinFactor;
  velRight        = velRight * cosFactor + origVelUp * sinFactor;

  float speedBoost  = input.bSprint ? SPRINT_BOOST : 1.0;
  tempVel.pos.x     = velRight * MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP * speedBoost;
  tempVel.pos.z     = -velUp * MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP * speedBoost;

  setVel(tempVel);

  //LOGD("x: %f, vx %f", getPos().pos.x, getVel().pos.x);
  //LOGD("xRot: %f, vXRot %f", getRot().pos.x, getRotVel().pos.x);

  return true;
}


bool ControllableObj::release()
{
  return GameObject::release();
}