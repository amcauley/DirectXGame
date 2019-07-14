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


// Check if objects collide on the (input's) z axis.
// This calculation makes use of the main objects velocity.
void AABBControllable::CheckHitWImmobileBasedOnVel(
  Pos3 &vel,
  Pos3 &mainPos,
  Pos3 &mainPosAabb,
  Pos3 &mainDimAabb,
  Pos3 &otherPos,
  Pos3 &otherPosAabb,
  Pos3 &otherDimAabb,
  bool &bHitZ,
  float &distZ
  )
{
  float collisionTimeInPastZ;
  Pos2 reverseVec;
  float primaryCenterU, primaryCenterV;
  float primaryUWidth, primaryVHeight;
  float otherCenterU, otherCenterV;
  float otherUWidth, otherVHeight;

  bHitZ = false;

  if (vel.pos.z != 0)
  {
    // moving front face against stationary back face
    if (vel.pos.z > 0)
    {
      distZ = (mainPos.pos.z + mainPosAabb.pos.z + mainDimAabb.pos.z / 2) -
        (otherPos.pos.z + otherPosAabb.pos.z - otherDimAabb.pos.z / 2);
    }
    // moving back face against stationary front face
    else
    {
      distZ = (mainPos.pos.z + mainPosAabb.pos.z - mainDimAabb.pos.z / 2) -
        (otherPos.pos.z + otherPosAabb.pos.z + otherDimAabb.pos.z / 2);
    }

    collisionTimeInPastZ = distZ / vel.pos.z;

    if (collisionTimeInPastZ > 0.0 && distZ * distZ <= MAX_ACTIONABLE_DIST_2)
    {
      reverseVec.pos.x = -vel.pos.x * collisionTimeInPastZ;
      reverseVec.pos.y = -vel.pos.y * collisionTimeInPastZ;

      primaryCenterU = mainPos.pos.x + mainPosAabb.pos.x + reverseVec.pos.x;
      primaryCenterV = mainPos.pos.y + mainPosAabb.pos.y + reverseVec.pos.y;
      primaryUWidth = mainDimAabb.pos.x;
      primaryVHeight = mainDimAabb.pos.y;

      otherCenterU = otherPos.pos.x + otherPosAabb.pos.x;
      otherCenterV = otherPos.pos.y + otherPosAabb.pos.y;
      otherUWidth = otherDimAabb.pos.x;
      otherVHeight = otherDimAabb.pos.y;

      bHitZ = squaresOverlap(
        Pos2(primaryCenterU, primaryCenterV),
        Pos2(primaryUWidth, primaryVHeight),
        Pos2(otherCenterU, otherCenterV),
        Pos2(otherUWidth, otherVHeight)
        );
    }
    else
    {
      // Not considered a collision. Reset collision distance.
      // Important for, e.g., collision ordering based on distance.
      distZ = 0.0f;
    }
  }
}


void AABBControllable::CheckHitsWImmobileBasedOnVel(
  Pos3 &primaryPos,
  Pos3 &primaryVel,
  AABB *pPrimaryAabb,
  Pos3 &otherPos,
  AABB *pOtherAabb,
  bool &bHitX,
  float &distX,
  bool &bHitY,
  float &distY,
  bool &bHitZ,
  float &distZ
  )
{
  CheckHitWImmobileBasedOnVel(
    Pos3(primaryVel.pos.y, primaryVel.pos.z, primaryVel.pos.x),
    Pos3(primaryPos.pos.y, primaryPos.pos.z, primaryPos.pos.x),
    Pos3(pPrimaryAabb->getPos().pos.y, pPrimaryAabb->getPos().pos.z, pPrimaryAabb->getPos().pos.x),
    Pos3(pPrimaryAabb->getDim().pos.y, pPrimaryAabb->getDim().pos.z, pPrimaryAabb->getDim().pos.x),
    Pos3(otherPos.pos.y, otherPos.pos.z, otherPos.pos.x),
    Pos3(pOtherAabb->getPos().pos.y, pOtherAabb->getPos().pos.z, pOtherAabb->getPos().pos.x),
    Pos3(pOtherAabb->getDim().pos.y, pOtherAabb->getDim().pos.z, pOtherAabb->getDim().pos.x),
    bHitX,
    distX);
  
  CheckHitWImmobileBasedOnVel(
    Pos3(primaryVel.pos.x, primaryVel.pos.z, primaryVel.pos.y),
    Pos3(primaryPos.pos.x, primaryPos.pos.z, primaryPos.pos.y),
    Pos3(pPrimaryAabb->getPos().pos.x, pPrimaryAabb->getPos().pos.z, pPrimaryAabb->getPos().pos.y),
    Pos3(pPrimaryAabb->getDim().pos.x, pPrimaryAabb->getDim().pos.z, pPrimaryAabb->getDim().pos.y),
    Pos3(otherPos.pos.x, otherPos.pos.z, otherPos.pos.y),
    Pos3(pOtherAabb->getPos().pos.x, pOtherAabb->getPos().pos.z, pOtherAabb->getPos().pos.y),
    Pos3(pOtherAabb->getDim().pos.x, pOtherAabb->getDim().pos.z, pOtherAabb->getDim().pos.y),
    bHitY,
    distY);
  
  CheckHitWImmobileBasedOnVel(
    Pos3(primaryVel.pos.x, primaryVel.pos.y, primaryVel.pos.z),
    Pos3(primaryPos.pos.x, primaryPos.pos.y, primaryPos.pos.z),
    Pos3(pPrimaryAabb->getPos().pos.x, pPrimaryAabb->getPos().pos.y, pPrimaryAabb->getPos().pos.z),
    Pos3(pPrimaryAabb->getDim().pos.x, pPrimaryAabb->getDim().pos.y, pPrimaryAabb->getDim().pos.z),
    Pos3(otherPos.pos.x, otherPos.pos.y, otherPos.pos.z),
    Pos3(pOtherAabb->getPos().pos.x, pOtherAabb->getPos().pos.y, pOtherAabb->getPos().pos.z),
    Pos3(pOtherAabb->getDim().pos.x, pOtherAabb->getDim().pos.y, pOtherAabb->getDim().pos.z),
    bHitZ,
    distZ);
}


void AABBControllable::onCollisionWithAabbImmobile(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo)
{
  // Check if the moving AABB collides with a stationary AABB model.
  // If a collision happens, cancel out any movement that put the model into a collision state.
  // Use the model's output position/velocity, as this holds the most up-to-date info,
  // ex. updated if a previous collision altered the trajectory.
  // Need to be careful to use compatible update/collision models.

  // Collision handling is running after first pass physics, so the latest info is in othe out structs.
  Pos3 primaryPos = pPrimaryIo->out.pos;
  Pos3 primaryVel = pPrimaryIo->out.vel;
  Pos3 otherPos = pOtherModelIo->out.pos;

  bool bHitX = false, bHitY = false, bHitZ = false;
  float distX = 0.0, distY = 0.0, distZ = 0.0;

  AABBControllable* pPrimaryAabb = this;
  AABB* pOtherAabb = static_cast<AABB*>(pOtherModelIo->in.pModel->getCollisionModel());

  //LOGD("Primary In posZ %f velZ %f, Secondary In posZ %f, Primary Out posZ %f velZ %f", primaryPos.pos.z, primaryVel.pos.z, otherPos.pos.z, pPrimaryIo->out.pos.pos.z, pPrimaryIo->out.vel.pos.z);
  //LOGD("Collision Test, posX %f, velX %f", pPrimaryIo->out.pos.pos.x, pPrimaryIo->out.vel.pos.x);
  //LOGD("Collision Test, posY %f, velY %f", pPrimaryIo->out.pos.pos.y, pPrimaryIo->out.vel.pos.y);
 // LOGD("Collision Test, posZ %f, velZ %f", pPrimaryIo->out.pos.pos.z, pPrimaryIo->out.vel.pos.z);

  CheckHitsWImmobileBasedOnVel(
    primaryPos,
    primaryVel,
    pPrimaryAabb,
    otherPos,
    pOtherAabb,
    bHitX,
    distX,
    bHitY,
    distY,
    bHitZ,
    distZ);

  // If no hit, no processing required
  if (!bHitX && !bHitY && !bHitZ)
  {
    return;
  }

  Pos3 outPos = primaryPos;
  Pos3 outVel = primaryVel;

  if (bHitX)
  {
    //LOGD("Hit X, vel %f -> 0, pos %f -> %f", outVel.pos.x, outPos.pos.x, outPos.pos.x - distX);
    outVel.pos.x = 0;
    outPos.pos.x -= distX;
  }
  if (bHitY)
  {
    //LOGD("Hit Y, vel %f -> 0, pos %f -> %f", outVel.pos.y, outPos.pos.y, outPos.pos.y - distY);
    outVel.pos.y = 0;
    outPos.pos.y -= distY;
  }
  if (bHitZ)
  {
    //LOGD("Hit Z, vel %f -> 0, pos %f -> %f", outVel.pos.z, outPos.pos.z, outPos.pos.z - distZ);
    outVel.pos.z = 0;
    outPos.pos.z -= distZ;
  }

  // Write out updated location/velocity.
  pPrimaryIo->out.pos = outPos;
  pPrimaryIo->out.vel = outVel;
}
