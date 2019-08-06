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
  m_lastWallJumpMs = 0.0;
  m_lastJumpEnMs = 0.0;
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
  AABBControllable *pPhysModel = static_cast<AABBControllable*>(m_pPModel->getCollisionModel());

  // JumpEn seems to be a bit finnicky, sometimes on/off.
  // Prob depends on number of time chunks processed and how far collisions have been offset from ground collisions.
  // Apply some historesis, ex. if it was on within last x ms, keep it on.
  // Should reset after a jump, to prevent multi-jumping.
  if (pPhysModel->getJumpEn())
  {
    m_lastJumpEnMs = timeMs;
  }
  bool bJumpEn = (timeMs - m_lastJumpEnMs < JUMP_EN_COOLDOWN_MS);

  bool bJump = false;
  if ((input.bSpace) && (timeMs - m_lastJumpMs > JUMP_COOLDOWN_MS))
  {
    if (bJumpEn)
    {
      bJump = true;
      tempVel.pos.y = (JUMP_VELOCITY_MPS * MPS_TO_UNITS_PER_STEP);
    }
    else
    {
      Pos2 wallJumpNormal = pPhysModel->getWallJumpNormal();
      bJump = (wallJumpNormal.pos.x != 0) || (wallJumpNormal.pos.y != 0);

      // Wall jumps freeze control inputs for a while.
      if (bJump)
      {
        tempVel.pos.x = wallJumpNormal.pos.x * (MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP);
        tempVel.pos.z = wallJumpNormal.pos.y * (MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP);
        tempVel.pos.y = (JUMP_VELOCITY_MPS * MPS_TO_UNITS_PER_STEP);
        m_lastWallJumpMs = timeMs;
      }
    }
  }

  if (bJump)
  {
    m_lastJumpMs = timeMs;
    // Clear jumpEn historesis by activing as if the last enable time was long ago.
    m_lastJumpEnMs = -JUMP_EN_COOLDOWN_MS;

    pSoundMgr->playSound(m_pingSoundHandle);
  }

  // Normal movements are applied on top of jump velocities.
  {
    float velUp = input.keyUp * MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP;
    float velRight = input.keyRight * MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP;

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
    velUp = velUp * cosFactor - velRight * sinFactor;
    velRight = velRight * cosFactor + origVelUp * sinFactor;

    float speedBoost = input.bSprint ? SPRINT_BOOST : 1.0;
    float controlVelX = velRight * speedBoost;
    float controlVelZ = -velUp * speedBoost;

    // If in the air, need to carry over starting velocity, ex. from previous jump.
    //LOGD("jumpEn %s, ts %f", bJumpEn ? "ON" : "OFF", timeMs);
    if (!bJumpEn)
    {
      // Skip controller updates if we've recently wall-jumped.
      if (timeMs - m_lastWallJumpMs > WALL_JUMP_CONTROL_COOLDOWN_MS)
      {
        tempVel.pos.x += controlVelX;
        tempVel.pos.z += controlVelZ;
      }
    }
    else
    {
      tempVel.pos.x = controlVelX;
      tempVel.pos.z = controlVelZ;
    }

    // Limit max speed.
    float maxVelUps = MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP * speedBoost;
    float normFactor = std::sqrt(tempVel.pos.x * tempVel.pos.x + tempVel.pos.z * tempVel.pos.z);

    // Don't bother normalizing unless we're moving too fast. This should allow us to move slowly if desired instead of
    // forcing constant speed all the time.
    //LOGD("Pre-scaling vel: (%f, %f), max %f UPS", tempVel.pos.x, tempVel.pos.z, maxVelUps);
    if (normFactor > maxVelUps)
    {
      float invNormFactor = maxVelUps / normFactor;
      tempVel.pos.x *= invNormFactor;
      tempVel.pos.z *= invNormFactor;
      //LOGD("Post-scaling vel: (%f, %f), scaling %f", tempVel.pos.x, tempVel.pos.z, invNormFactor);
    }
  }

  // Need to explicitly reset jump flags - it's set in the physics collision model, but cleared by the object.
  pPhysModel->setJumpEn(false);
  pPhysModel->setWallJumpNormal(Pos2(0.0, 0.0));

  setVel(tempVel);

  //LOGD("x: %f, vx %f", getPos().pos.x, getVel().pos.x);
  //LOGD("xRot: %f, vXRot %f", getRot().pos.x, getRotVel().pos.x);

  return true;
}


bool ControllableObj::release()
{
  return GameObject::release();
}