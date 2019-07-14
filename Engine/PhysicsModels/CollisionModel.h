#ifndef COLLISION_MODEL_H
#define COLLISION_MODEL_H

#include "../PhysicsMgr.h"
#include "../CommonTypes.h"

class AABB;

typedef enum CollisionModelType_t
{
  COLLISION_MODEL_NONE = 0,
  COLLISION_MODEL_AABB,
  COLLISION_MODEL_AABB_IMMOBILE,
  COLLISION_MODEL_AABB_CONTROLLABLE
} CollisionModelType;


class CollisionModel
{
protected:
  CollisionModelType m_type;

  Pos3 m_pos; // Position (relative to owner object's position)

public:
  static bool releaseCollisionModel(CollisionModel *pModel);

  static bool modelsCollide(PmModelStorage *pFirst, PmModelStorage *pSecond, OrderingMetric *pCollisionOrderMetric);
  static bool modelsCollideAabbAabb(PmModelStorage *pFirst, PmModelStorage *pSecond, OrderingMetric *pCollisionOrderMetric);

  // Handle collision between two models.
  static void handleCollision(PmModelStorage *pFirstIo, PmModelStorage *pSecondIo);

  Pos3 getPos();
  void setPos(Pos3 pos);

  // Individual model processing
  virtual void onCollision(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo);

  CollisionModel();
  CollisionModelType getType();
  void setType(CollisionModelType type);
};

#endif
