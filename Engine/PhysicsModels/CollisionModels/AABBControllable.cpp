#include "AABBControllable.h"
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
  // the physics update model computed the output position was based only on input position and velocity. \
  // This assumption should hold for GravityModel, but may not for others. Need to be careful to use compatible
  // update/collision models.

  Pos3 primaryPos = pPrimaryIo->in.pos;
  Pos3 primaryVel = pPrimaryIo->in.vel;
  Pos3 otherPos = pOtherModelIo->in.pos;
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

  //LOGD("Collision, posY %f, velY %f", pPrimaryIo->out.pos.pos.y, pPrimaryIo->out.vel.pos.y);

  // Minimum distance at which past hits will be considered. Keep this small to avoid reacting to
  // some far away wall. Ex) if we're moving at 1 unit/step, don't react to something 1000 steps behind us.
  // We would have hit it in the past, but it's too long ago - not relevant to current calcs. Also don't just
  // use time directly, this was original design and led to gravity slowly sinking player through floor, so
  // hit in past was too long ago (due to slow velocity) to count.
  const float MAX_ACTIONABLE_DIST_2 = 0.05;

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

  vel = pPrimaryIo->in.vel.pos.y;
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

  vel = pPrimaryIo->in.vel.pos.z;
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

  collisionTimeInPastX = bXHit ? collisionTimeInPastX : std::numeric_limits<float>::infinity();
  collisionTimeInPastY = bYHit ? collisionTimeInPastY : std::numeric_limits<float>::infinity();
  collisionTimeInPastZ = bZHit ? collisionTimeInPastZ : std::numeric_limits<float>::infinity();

  float minTimeInPast = min(min(collisionTimeInPastX, collisionTimeInPastY), collisionTimeInPastZ);

  Pos3 outPos = pPrimaryIo->out.pos;
  Pos3 outVel = pPrimaryIo->out.vel;

  if (minTimeInPast == collisionTimeInPastX)
  {
    //LOGD("Hit X");
    outVel.pos.x = 0;
    outPos.pos.x -= distX;
  }
  else if (minTimeInPast == collisionTimeInPastY)
  {
    //LOGD("Hit Y, vel %f -> 0, pos %f -> %f, timeInPast %f", outVel.pos.y, outPos.pos.y, outPos.pos.y - distY, minTimeInPast);
    outVel.pos.y = 0;
    outPos.pos.y -= distY;
  }
  else
  {
    //LOGD("Hit Z");
    outVel.pos.z = 0;
    outPos.pos.z -= distZ;
  }

  // Write out updated location/velocity.
  pPrimaryIo->out.pos = outPos;
  pPrimaryIo->out.vel = outVel;
}
