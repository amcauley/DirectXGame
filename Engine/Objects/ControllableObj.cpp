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
  m_lastWallJumpEnMs = 0.0;
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

  /* ~~~       ~~~ */
  /* ~~  JUMPS  ~~ */
  /* ~~~       ~~~ */
  // JumpEn seems to be a bit finnicky, sometimes on/off.
  // Prob depends on number of time chunks processed and how far collisions have been offset from ground collisions.
  // Apply some historesis, ex. if it was on within last x ms, keep it on.
  // Should reset after a jump, to prevent multi-jumping.
  if (pPhysModel->getJumpEn())
  {
    m_lastJumpEnMs = timeMs;
  }
  bool bJumpEn = (timeMs - m_lastJumpEnMs < JUMP_EN_COOLDOWN_MS);

  // Hysteresis for wall jumps.
  Pos2 wallJumpNormal = pPhysModel->getWallJumpNormal();
  if ((wallJumpNormal.pos.x != 0) || (wallJumpNormal.pos.y != 0))
  {
    m_lastWallJumpEnMs = timeMs;
    m_lastWallJumpNormal = wallJumpNormal;
  }
  bool bWallJumpEn = (timeMs - m_lastWallJumpEnMs < WALL_JUMP_EN_COOLDOWN_MS);

  bool bJump = false;
  if ((input.bSpaceDown) && (timeMs - m_lastJumpMs > JUMP_COOLDOWN_MS))
  {
    bJump = bJumpEn || bWallJumpEn;

    if (bJumpEn)
    {
      tempVel.pos.y = (JUMP_VELOCITY_MPS * MPS_TO_UNITS_PER_STEP);
    }
    else if (bWallJumpEn)
    {
      // Wall jumps freeze control inputs for a while.
      tempVel.pos.x = m_lastWallJumpNormal.pos.x * (JUMP_VELOCITY_MPS * MPS_TO_UNITS_PER_STEP);
      tempVel.pos.z = m_lastWallJumpNormal.pos.y * (JUMP_VELOCITY_MPS * MPS_TO_UNITS_PER_STEP);
      tempVel.pos.y = (JUMP_VELOCITY_MPS * MPS_TO_UNITS_PER_STEP);
      //LOGD("Wall jump xz vel: (%f, %f)", tempVel.pos.x, tempVel.pos.z);
      m_lastWallJumpMs = timeMs;
    }
  }

  if (bJump)
  {
    m_lastJumpMs = timeMs;
    // Clear jumpEn historesis by activing as if the last enable time was long ago.
    m_lastJumpEnMs = -JUMP_EN_COOLDOWN_MS;
    m_lastWallJumpEnMs = -WALL_JUMP_EN_COOLDOWN_MS;

    pSoundMgr->playSound(m_pingSoundHandle);
  }


  /* ~~~           ~~~ */
  /* ~~  ROTATIONS  ~~ */
  /* ~~~           ~~~ */
  Pos3 tempRot = getRot();
  {
    Pos3 tempRotVel = getRotVel();

    tempRotVel.pos.x = input.pitchUp * TURN_RATE_RAD_PS * SEC_PER_STEP;
    tempRotVel.pos.y = input.yawCw * TURN_RATE_RAD_PS * SEC_PER_STEP;
    setRotVel(tempRotVel);

    if (tempRot.pos.x > MAX_PITCH_RADS)
    {
      tempRot.pos.x = MAX_PITCH_RADS;
    }
    else if (tempRot.pos.x < MIN_PITCH_RADS)
    {
      tempRot.pos.x = MIN_PITCH_RADS;
    }

    setRot(tempRot);
  }


  /* ~~~                   ~~~ */
  /* ~~  LATERAL MOVEMENTS  ~~ */
  /* ~~~                   ~~~ */
  {
    float accelUp = input.keyUp * MOVEMENT_ACCEL_MPSPS * MPSPS_TO_UNIT_PER_STEP_PER_STEP;
    float accelRight = input.keyRight * MOVEMENT_ACCEL_MPSPS * MPSPS_TO_UNIT_PER_STEP_PER_STEP;

    float sinFactor = std::sin(tempRot.pos.y);
    float cosFactor = std::cos(tempRot.pos.y);
    float origAccelUp = accelUp;
    accelUp = accelUp * cosFactor - accelRight * sinFactor;
    accelRight = accelRight * cosFactor + origAccelUp * sinFactor;

    float speedBoost = input.bSprint ? SPRINT_BOOST : 1.0;
    float controlAccelX = accelRight * speedBoost;
    float controlAccelZ = -accelUp * speedBoost;

    // If not actively accelerating, decelerate.
    float frictionDecelFactor = bJumpEn ? FRICTION_DECEL_FACTOR_GROUND : FRICTION_DECEL_FACTOR_AIR;
    float accelX = controlAccelX == 0 ? -tempVel.pos.x * frictionDecelFactor : controlAccelX;
    float accelZ = controlAccelZ == 0 ? -tempVel.pos.z * frictionDecelFactor : controlAccelZ;

    // Skip controller updates if we've recently wall-jumped.
    if (timeMs - m_lastWallJumpMs > WALL_JUMP_CONTROL_COOLDOWN_MS)
    {
      tempVel.pos.x += accelX;
      tempVel.pos.z += accelZ;
    }

    // Limit min speed.
    tempVel.pos.x = std::abs(tempVel.pos.x) < VEL_STOP_THRESH_MPS * MPS_TO_UNITS_PER_STEP ? 0.0 : tempVel.pos.x;
    tempVel.pos.z = std::abs(tempVel.pos.z) < VEL_STOP_THRESH_MPS * MPS_TO_UNITS_PER_STEP ? 0.0 : tempVel.pos.z;

    // Limit max speed.
    float maxVelUps = MAX_MOVEMENT_VEL_MPS * MPS_TO_UNITS_PER_STEP * speedBoost;
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


  /* ~~~         ~~~ */
  /* ~~  CLEANUP  ~~ */
  /* ~~~         ~~~ */
  {
    // Need to explicitly reset jump flags - it's set in the physics collision model, but cleared by the object.
    pPhysModel->setJumpEn(false);
    pPhysModel->setWallJumpNormal(Pos2(0.0, 0.0));

    setVel(tempVel);

    //LOGD("x: %f, vx %f", getPos().pos.x, getVel().pos.x);
    //LOGD("xRot: %f, vXRot %f", getRot().pos.x, getRotVel().pos.x);
  }

  return true;
}


bool ControllableObj::release()
{
  return GameObject::release();
}