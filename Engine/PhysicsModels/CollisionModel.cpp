#include "CollisionModel.h"
#include "../PhysicsMgr.h"
#include "../Logger.h"
#include "CollisionModels/AABB.h"

CollisionModel::CollisionModel()
{
  m_type = COLLISION_MODEL_NONE;
}


CollisionModelType CollisionModel::getType()
{
  return m_type;
}


bool CollisionModel::modelsCollide(PmModelStorage *pFirst, PmModelStorage *pSecond)
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
      switch (secondType)
      {
        case COLLISION_MODEL_AABB:
        {
          bCollision = modelsCollideAabbAabb(pFirst, pSecond);
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


bool CollisionModel::modelsCollideAabbAabb(PmModelStorage *pFirst, PmModelStorage *pSecond)
{
  if (!pFirst || !pSecond) return false;
  if (!pFirst->in.pModel || !pSecond->in.pModel) return false;

  CollisionModel *firstModel = pFirst->in.pModel->getCollisionModel();
  CollisionModel *secondModel = pSecond->in.pModel->getCollisionModel();

  if (!firstModel || !secondModel) return false;

  Pos3 firstObjPos, secondObjPos, firstBoxPos, secondBoxPos;

  firstObjPos = pFirst->out.pos;
  secondObjPos = pSecond->out.pos;

  firstBoxPos = static_cast<AABB*>(firstModel)->getPos();
  secondBoxPos = static_cast<AABB*>(secondModel)->getPos();

  Pos3 firstDim = static_cast<AABB*>(firstModel)->getDim();
  Pos3 secondDim = static_cast<AABB*>(secondModel)->getDim();

  bool intersects;
  float firstVal, secondVal, lowVal;

  /* Find the left-most box and declare intersection, at least for this axis, if the leftmost
  box's rightmost index is greater than the rightmost box's left index. */
  firstVal = firstObjPos.pos.x + firstBoxPos.pos.x;
  secondVal = secondObjPos.pos.x + secondBoxPos.pos.x;
  lowVal = fminf(firstVal, secondVal);
  if (lowVal == firstVal) intersects = (firstVal + firstDim.pos.x / 2 >= secondVal - secondDim.pos.x / 2);
  else intersects = (secondVal + secondDim.pos.x / 2 >= firstVal - firstDim.pos.x / 2);
  if (!intersects) return false;

  firstVal = firstObjPos.pos.y + firstBoxPos.pos.y;
  secondVal = secondObjPos.pos.y + secondBoxPos.pos.y;
  lowVal = fminf(firstVal, secondVal);
  if (lowVal == firstVal) intersects = (firstVal + firstDim.pos.y / 2 >= secondVal - secondDim.pos.y / 2);
  else intersects = (secondVal + secondDim.pos.y / 2 >= firstVal - firstDim.pos.y / 2);
  if (!intersects) return false;

  firstVal = firstObjPos.pos.z + firstBoxPos.pos.z;
  secondVal = secondObjPos.pos.z + secondBoxPos.pos.z;
  lowVal = fminf(firstVal, secondVal);
  if (lowVal == firstVal) intersects = (firstVal + firstDim.pos.z / 2 >= secondVal - secondDim.pos.z / 2);
  else intersects = (secondVal + secondDim.pos.z / 2 >= firstVal - firstDim.pos.z / 2);
  return intersects;
}


void CollisionModel::onCollision(PmModelStorage *pIo)
{

}


bool CollisionModel::releaseCollisionModel(CollisionModel *pModel)
{
  return true;
}
