#ifndef AABB_CONTROLLABLE_H
#define AABB_CONTROLLABLE_H

#include "AABB.h"

class AABBControllable : public AABB
{
protected:
  void onCollisionWithAabbImmobile(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo);

public:
  AABBControllable();
  AABBControllable(float w, float h, float d);

  virtual void onCollision(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo);
};

#endif
