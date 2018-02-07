#include "GravityModel.h"
#include "../../CommonPhysConsts.h"
#include "../../Logger.h"


GravityModel::GravityModel()
{
  m_type = PHYSICS_UPDATE_MODEL_GRAVITY;
}


bool GravityModel::run(
  PModelInput &pModelInput,
  PModelInput *otherModels[],
  PModelOutput &output)
{
  float yVel = pModelInput.vel.pos.y;
  yVel += (GRAVITY_MODEL_G_MPSPS * MPSPS_TO_UNIT_PER_STEP_PER_STEP);
  yVel = max(yVel, GRAVITY_MODEL_MIN_V_MPS * MPS_TO_UNITS_PER_STEP);
  yVel = min(yVel, GRAVITY_MODEL_MAX_V_MPS * MPS_TO_UNITS_PER_STEP);
  
  // Everything is in terms of units / step now.
  output.vel.pos.y = yVel;
  output.pos.pos.y += yVel;

  output.pos.pos.x += pModelInput.vel.pos.x;
  output.pos.pos.z += pModelInput.vel.pos.z;
  output.rot.pos.x += pModelInput.rotVel.pos.x;
  output.rot.pos.y += pModelInput.rotVel.pos.y;

  return true;
}

bool GravityModel::release()
{
  return PhysicsUpdateModel::release();
}
