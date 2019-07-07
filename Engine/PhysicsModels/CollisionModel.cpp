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
    case COLLISION_MODEL_AABB_IMMOBILE:
      switch (secondType)
      {
        case COLLISION_MODEL_AABB:
        case COLLISION_MODEL_AABB_IMMOBILE:
        case COLLISION_MODEL_AABB_CONTROLLABLE:
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

  return cubesOverlap(
    firstBoxPos,
    firstDim,
    secondBoxPos,
    secondDim);
}

// This one should be defined per Collision Model type.
void CollisionModel::onCollision(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo)
{

}


void CollisionModel::handleCollision(PmModelStorage *pFirstIo, PmModelStorage *pSecondIo)
{
  PmModelStorage *pActiveIo = pFirstIo;
  PmModelStorage *pOtherIo = pSecondIo;
  CollisionModel *pActiveModel = pActiveIo->in.pModel->getCollisionModel();
  CollisionModel *pOtherModel = pOtherIo->in.pModel->getCollisionModel();

  for (int i = 0; i < 2; i++)
  {
    // Process the other model as active.
    if (i == 1)
    {
      pActiveIo = pSecondIo;
      pOtherIo = pFirstIo;

      CollisionModel *pTemp = pActiveModel;
      pActiveModel = pOtherModel;
      pOtherModel = pTemp;
    }

    CollisionModelType type = pActiveModel->getType();
    //LOGD("handleCollision activeType %u, otherType %u", type, pOtherModel->getType());
    switch (type)
    {
      case COLLISION_MODEL_AABB:
      case COLLISION_MODEL_AABB_IMMOBILE:
      {
        static_cast<AABB*>(pActiveModel)->onCollision(pActiveIo, pOtherIo);
        break;
      }
      case COLLISION_MODEL_AABB_CONTROLLABLE:
      {
        static_cast<AABBControllable*>(pActiveModel)->onCollision(pActiveIo, pOtherIo);
        break;
      }
      default:
      {
        LOGW("Unexpected collision model %u", type);
        break;
      }
    }
  }
}


bool CollisionModel::releaseCollisionModel(CollisionModel *pModel)
{
  return true;
}
