#include "CollisionModel.h"
#include "../PhysicsMgr.h"
#include "../Logger.h"
#include "CollisionModels/AABB.h"
#include "CollisionModels/AABBControllable.h"
#include "../Util.h"

CollisionModel::CollisionModel()
{
  m_type = COLLISION_MODEL_NONE;
}


CollisionModelType CollisionModel::getType()
{
  return m_type;
}


// Only intended to manually set types that have identical handling within the type, but
// other objects that collide with it may change handling. For example, COLLISION_MODEL_AABB
// and COLLISION_MODEL_AABB_IMMOBILE have identical handling themselves, but COLLISION_MODEL_AABB_CONTROLLABLE
// will pass through the standard AABB but not the immobile version.
void CollisionModel::setType(CollisionModelType type)
{
  m_type = type;
}


Pos3 CollisionModel::getPos()
{
  return m_pos;
}


void CollisionModel::setPos(Pos3 pos)
{
  m_pos = pos;
}


// Check if two models collide, and if so, calculate a metric used for ordering collisions.
bool CollisionModel::modelsCollide(PmModelStorage *pFirst, PmModelStorage *pSecond, OrderingMetric *pCollisionOrderMetric)
{
  if (!pFirst || !pSecond) return false;
  if (!pFirst->in.pModel || !pSecond->in.pModel) return false;

  CollisionModel *firstModel = pFirst->in.pModel->getCollisionModel();
  CollisionModel *secondModel = pSecond->in.pModel->getCollisionModel();

  if (!firstModel || !secondModel) return false;

  CollisionModelType firstType = firstModel->getType();
  CollisionModelType secondType = secondModel->getType();

  // Order model enums in ascending order to cut down on inner switch statement checks.
  if (static_cast<uint32_t>(firstType) > static_cast<uint32_t>(secondType))
  {
    CollisionModelType temp = firstType;
    firstType = secondType;
    secondType = temp;
  }

  bool bCollision = false;
  switch (firstType)
  {
    case COLLISION_MODEL_NONE:
      bCollision = false;
    case COLLISION_MODEL_AABB:
    case COLLISION_MODEL_AABB_IMMOBILE:
      switch (secondType)
      {
        case COLLISION_MODEL_AABB:
        case COLLISION_MODEL_AABB_IMMOBILE:
        case COLLISION_MODEL_AABB_CONTROLLABLE:
        {
          bCollision = modelsCollideAabbAabb(pFirst, pSecond, pCollisionOrderMetric);
          break;
        }
        default:
        {
          LOGW("Unexpected second collision model %u", secondType);
          break;
        }
      }
      break;
    default:
    {
      LOGW("Unexpected first collision model %u", firstType);
      bCollision = false;
      break;
    }
  }

  return bCollision;
}


bool CollisionModel::modelsCollideAabbAabb(PmModelStorage *pFirst, PmModelStorage *pSecond, OrderingMetric *pCollisionOrderMetric)
{
  if (!pFirst || !pSecond) return false;
  if (!pFirst->in.pModel || !pSecond->in.pModel) return false;

  CollisionModel *firstModel = pFirst->in.pModel->getCollisionModel();
  CollisionModel *secondModel = pSecond->in.pModel->getCollisionModel();

  if (!firstModel || !secondModel) return false;

  Pos3 firstObjPos, secondObjPos, firstBoxPos, secondBoxPos, firstObjVel;

  firstObjPos = pFirst->out.pos;
  firstObjVel = pFirst->out.vel;
  secondObjPos = pSecond->out.pos;

  AABB *pFirstModelAabb = static_cast<AABB*>(firstModel);
  AABB *pSecondModelAabb = static_cast<AABB*>(secondModel);

  // TODO: Implement +/- operator for Pos3
  firstBoxPos = pFirstModelAabb->getPos();
  firstBoxPos.pos.x += firstObjPos.pos.x;
  firstBoxPos.pos.y += firstObjPos.pos.y;
  firstBoxPos.pos.z += firstObjPos.pos.z;

  secondBoxPos = pSecondModelAabb->getPos();
  secondBoxPos.pos.x += secondObjPos.pos.x;
  secondBoxPos.pos.y += secondObjPos.pos.y;
  secondBoxPos.pos.z += secondObjPos.pos.z;

  Pos3 firstDim = pFirstModelAabb->getDim();
  Pos3 secondDim = pSecondModelAabb->getDim();

  bool bOverlap = cubesOverlap(
    firstBoxPos,
    firstDim,
    secondBoxPos,
    secondDim);

  // Provide a score for collision ordering based on collision timing (based on velocities).
  // Smaller collision metrics get processed first, so more negative times, i.e. further in the past, are first.
  // Tiebreaker is distance between object centers.
  if (bOverlap)
  {
    bool bHitX = false, bHitY = false, bHitZ = false;
    float distX = 0.0, distY = 0.0, distZ = 0.0;

    AABBControllable::CheckHitsWImmobileBasedOnVel(
      firstBoxPos,
      firstObjVel,
      pFirstModelAabb,
      secondBoxPos,
      pSecondModelAabb,
      bHitX,
      distX,
      bHitY,
      distY,
      bHitZ,
      distZ);

    float minDist = 0.0;
    if (firstObjVel.pos.x != 0.0) minDist = std::fminf(minDist, -distX / firstObjVel.pos.x);
    if (firstObjVel.pos.y != 0.0) minDist = std::fminf(minDist, -distY / firstObjVel.pos.y);
    if (firstObjVel.pos.z != 0.0) minDist = std::fminf(minDist, -distZ / firstObjVel.pos.z);

    pCollisionOrderMetric->primary    = minDist;
    pCollisionOrderMetric->secondary  = dist2(firstBoxPos, secondBoxPos);

    //LOGD("coll w obj at (X,Y) (%f, %f): distX %f Y %f Z %f min %f dist2 %f",
    //  secondBoxPos.pos.x, secondBoxPos.pos.y,
    //  distX, distY, distZ,
    //  minDist,
    //  pCollisionOrderMetric->secondary);
  }

  return bOverlap;
}

// This one should be defined per Collision Model type.
void CollisionModel::onCollision(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo)
{

}


// Handle collision between two models.
// Only apply the handling to the first model, but not the second.
// This allows callers to have more control over when models get processed.
void CollisionModel::handleCollision(PmModelStorage *pFirstIo, PmModelStorage *pSecondIo)
{
  CollisionModel *pActiveModel = pFirstIo->in.pModel->getCollisionModel();
  CollisionModelType type = pActiveModel->getType();

  //LOGD("handleCollision activeType %u, otherType %u", type, pOtherModel->getType());
  switch (type)
  {
    case COLLISION_MODEL_AABB:
    case COLLISION_MODEL_AABB_IMMOBILE:
    {
      static_cast<AABB*>(pActiveModel)->onCollision(pFirstIo, pSecondIo);
      break;
    }
    case COLLISION_MODEL_AABB_CONTROLLABLE:
    {
      static_cast<AABBControllable*>(pActiveModel)->onCollision(pFirstIo, pSecondIo);
      break;
    }
    default:
    {
      LOGW("Unexpected collision model %u", type);
      break;
    }
  }
}


bool CollisionModel::releaseCollisionModel(CollisionModel *pModel)
{
  return true;
}
