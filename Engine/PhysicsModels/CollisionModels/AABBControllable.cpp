#include "AABBControllable.h"
#include "../../CommonPhysConsts.h"
#include "../../Logger.h"
#include "../../Util.h"


AABBControllable::AABBControllable()
{
  m_type = COLLISION_MODEL_AABB_CONTROLLABLE;
}


AABBControllable::AABBControllable(float w, float h, float d)
{
  m_type = COLLISION_MODEL_AABB_CONTROLLABLE;
  m_width = w;
  m_height = h;
  m_depth = d;
}


void AABBControllable::onCollision(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo)
{
  CollisionModelType otherType = pOtherModelIo->in.pModel->getCollisionModel()->getType();
  //LOGD("AABBControllable onCollision with type %u", otherType);
  switch (otherType)
  {
    case COLLISION_MODEL_AABB_IMMOBILE:
    {
      onCollisionWithAabbImmobile(pPrimaryIo, pOtherModelIo);
      break;
    }
    default:
    {
      break;
    }
  }
}


void AABBControllable::onCollisionWithAabbImmobile(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo)
{
  // This model checks which face first collided with the corresponding face of the other model's aabb.
  // This assumes that the velocity that caused primary movement is stored in the IO input for primary, i.e.
  // the physics update model computed the output position was based only on input position and velocity.
  // This assumption should hold for GravityModel, but may not for others. Need to be careful to use compatible
  // update/collision models.

  // Collision handling is running after first pass physics, so the latest info is in othe out structs.
  Pos3 primaryPos = pPrimaryIo->out.pos;
  Pos3 primaryVel = pPrimaryIo->out.vel;
  Pos3 otherPos = pOtherModelIo->out.pos;
  Pos3 reverseVec;
  float primaryCenterU, primaryCenterV;
  float primaryUWidth, primaryVHeight;
  float otherCenterU, otherCenterV;
  float otherUWidth, otherVHeight;

  bool bXHit = false, bYHit = false, bZHit = false;

  AABBControllable* pPrimaryAabb = this;
  AABB* pOtherAabb = static_cast<AABB*>(pOtherModelIo->in.pModel->getCollisionModel());

  float vel;
  float distX = 0.0, distY = 0.0, distZ = 0.0;
  float collisionTimeInPastX = 0.0, collisionTimeInPastY = 0.0, collisionTimeInPastZ = 0.0;

  //LOGD("Primary In posZ %f velZ %f, Secondary In posZ %f, Primary Out posZ %f velZ %f", primaryPos.pos.z, primaryVel.pos.z, otherPos.pos.z, pPrimaryIo->out.pos.pos.z, pPrimaryIo->out.vel.pos.z);
  //LOGD("Collision Test, posX %f, velX %f", pPrimaryIo->out.pos.pos.x, pPrimaryIo->out.vel.pos.x);
  //LOGD("Collision Test, posY %f, velY %f", pPrimaryIo->out.pos.pos.y, pPrimaryIo->out.vel.pos.y);
 // LOGD("Collision Test, posZ %f, velZ %f", pPrimaryIo->out.pos.pos.z, pPrimaryIo->out.vel.pos.z);

  vel = pPrimaryIo->in.vel.pos.x;
  if (vel != 0)
  {
    // moving right face against stationary left face
    if (vel > 0)
    {
      distX = (primaryPos.pos.x + pPrimaryAabb->getPos().pos.x + pPrimaryAabb->getDim().pos.x / 2) -
              (otherPos.pos.x + pOtherAabb->getPos().pos.x - pOtherAabb->getDim().pos.x / 2);
    }
    // moving left face against stationary right face
    else 
    {
      distX = (primaryPos.pos.x + pPrimaryAabb->getPos().pos.x - pPrimaryAabb->getDim().pos.x / 2) -
              (otherPos.pos.x + pOtherAabb->getPos().pos.x + pOtherAabb->getDim().pos.x / 2);
    }

    collisionTimeInPastX = distX / vel;

    // Time is in units of number of physics steps.
    if (collisionTimeInPastX > 0.0 && distX * distX <= MAX_ACTIONABLE_DIST_2)
    {
      reverseVec.pos.y = -primaryVel.pos.y * collisionTimeInPastX;
      reverseVec.pos.z = -primaryVel.pos.z * collisionTimeInPastX;

      primaryCenterU = primaryPos.pos.y + pPrimaryAabb->getPos().pos.y + reverseVec.pos.y;
      primaryCenterV = primaryPos.pos.z + pPrimaryAabb->getPos().pos.z + reverseVec.pos.z;
      primaryUWidth = pPrimaryAabb->getDim().pos.y;
      primaryVHeight = pPrimaryAabb->getDim().pos.z;

      otherCenterU = otherPos.pos.y + pPrimaryAabb->getPos().pos.y;
      otherCenterV = otherPos.pos.z + pPrimaryAabb->getPos().pos.z;
      otherUWidth = pOtherAabb->getDim().pos.y;
      otherVHeight = pOtherAabb->getDim().pos.z;

      bXHit = squaresOverlap(
        Pos2(primaryCenterU, primaryCenterV),
        Pos2(primaryUWidth, primaryVHeight),
        Pos2(otherCenterU, otherCenterV),
        Pos2(otherUWidth, otherVHeight)
        );
    }
  }

  vel = pPrimaryIo->out.vel.pos.y;
  if (vel != 0)
  {
    // moving top face against stationary bottom face
    if (vel > 0)
    {
      distY = (primaryPos.pos.y + pPrimaryAabb->getPos().pos.y + pPrimaryAabb->getDim().pos.y / 2) -
        (otherPos.pos.y + pOtherAabb->getPos().pos.y - pOtherAabb->getDim().pos.y / 2);
      //LOGD("Moving top, stationary bottom, distY %f", distY);
    }
    // moving bottom face against stationary top face
    else
    {
      distY = (primaryPos.pos.y + pPrimaryAabb->getPos().pos.y - pPrimaryAabb->getDim().pos.y / 2) -
        (otherPos.pos.y + pOtherAabb->getPos().pos.y + pOtherAabb->getDim().pos.y / 2);
    }

    collisionTimeInPastY = distY / vel;
    //LOGD("distY %f, timeInPast %f", distY, collisionTimeInPastY);

    if (collisionTimeInPastY > 0.0 && distY * distY <= MAX_ACTIONABLE_DIST_2)
    {
      reverseVec.pos.x = -primaryVel.pos.x * collisionTimeInPastY;
      reverseVec.pos.z = -primaryVel.pos.z * collisionTimeInPastY;

      primaryCenterU = primaryPos.pos.x + pPrimaryAabb->getPos().pos.x + reverseVec.pos.x;
      primaryCenterV = primaryPos.pos.z + pPrimaryAabb->getPos().pos.z + reverseVec.pos.z;
      primaryUWidth = pPrimaryAabb->getDim().pos.x;
      primaryVHeight = pPrimaryAabb->getDim().pos.z;

      otherCenterU = otherPos.pos.x + pPrimaryAabb->getPos().pos.x;
      otherCenterV = otherPos.pos.z + pPrimaryAabb->getPos().pos.z;
      otherUWidth = pOtherAabb->getDim().pos.x;
      otherVHeight = pOtherAabb->getDim().pos.z;

      bYHit = squaresOverlap(
        Pos2(primaryCenterU, primaryCenterV),
        Pos2(primaryUWidth, primaryVHeight),
        Pos2(otherCenterU, otherCenterV),
        Pos2(otherUWidth, otherVHeight)
        );
    }
  }

  vel = pPrimaryIo->out.vel.pos.z;
  if (vel != 0)
  {
    // moving front face against stationary back face
    if (vel > 0)
    {
      distZ = (primaryPos.pos.z + pPrimaryAabb->getPos().pos.z + pPrimaryAabb->getDim().pos.z / 2) -
        (otherPos.pos.z + pOtherAabb->getPos().pos.z - pOtherAabb->getDim().pos.z / 2);
    }
    // moving back face against stationary front face
    else
    {
      distZ = (primaryPos.pos.z + pPrimaryAabb->getPos().pos.z - pPrimaryAabb->getDim().pos.z / 2) -
        (otherPos.pos.z + pOtherAabb->getPos().pos.z + pOtherAabb->getDim().pos.z / 2);
    }

    collisionTimeInPastZ = distZ / vel;

    if (collisionTimeInPastZ > 0.0 && distZ * distZ <= MAX_ACTIONABLE_DIST_2)
    {
      reverseVec.pos.x = -primaryVel.pos.x * collisionTimeInPastZ;
      reverseVec.pos.y = -primaryVel.pos.y * collisionTimeInPastZ;

      primaryCenterU = primaryPos.pos.x + pPrimaryAabb->getPos().pos.x + reverseVec.pos.x;
      primaryCenterV = primaryPos.pos.y + pPrimaryAabb->getPos().pos.y + reverseVec.pos.y;
      primaryUWidth = pPrimaryAabb->getDim().pos.x;
      primaryVHeight = pPrimaryAabb->getDim().pos.y;

      otherCenterU = otherPos.pos.x + pPrimaryAabb->getPos().pos.x;
      otherCenterV = otherPos.pos.y + pPrimaryAabb->getPos().pos.y;
      otherUWidth = pOtherAabb->getDim().pos.x;
      otherVHeight = pOtherAabb->getDim().pos.y;

      bZHit = squaresOverlap(
        Pos2(primaryCenterU, primaryCenterV),
        Pos2(primaryUWidth, primaryVHeight),
        Pos2(otherCenterU, otherCenterV),
        Pos2(otherUWidth, otherVHeight)
        );
    }
  }

  // If no hit, no processing required
  if (!bXHit && !bYHit && !bZHit)
  {
    return;
  }

  Pos3 outPos = pPrimaryIo->out.pos;
  Pos3 outVel = pPrimaryIo->out.vel;

  if (bXHit)
  {
    //LOGD("Hit X, vel %f -> 0, pos %f -> %f", outVel.pos.x, outPos.pos.x, outPos.pos.x - distX);
    outVel.pos.x = 0;
    outPos.pos.x -= distX;
  }
  if (bYHit)
  {
    //LOGD("Hit Y, vel %f -> 0, pos %f -> %f", outVel.pos.y, outPos.pos.y, outPos.pos.y - distY);
    outVel.pos.y = 0;
    outPos.pos.y -= distY;
  }
  if (bZHit)
  {
   //LOGD("Hit Z, vel %f -> 0, pos %f -> %f", outVel.pos.z, outPos.pos.z, outPos.pos.z - distZ);
    outVel.pos.z = 0;
    outPos.pos.z -= distZ;
  }

  // Write out updated location/velocity.
  pPrimaryIo->out.pos = outPos;
  pPrimaryIo->out.vel = outVel;
}
