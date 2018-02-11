#ifndef COLLISION_MODEL_H
#define COLLISION_MODEL_H

#include "../PhysicsMgr.h"
#include "../CommonTypes.h"

class AABB;

typedef enum CollisionModelType_t
{
  COLLISION_MODEL_NONE = 0,
  COLLISION_MODEL_AABB
} CollisionModelType;


class CollisionModel
{
protected:
  CollisionModelType m_type;

public:
  static bool releaseCollisionModel(CollisionModel *pModel);

  static bool modelsCollide(PmModelStorage *pFirst, PmModelStorage *pSecond);
  static bool modelsCollideAabbAabb(PmModelStorage *pFirst, PmModelStorage *pSecond);

  static void onCollision(PmModelStorage *pIo);

  CollisionModel();
  CollisionModelType getType();
};

#endif
