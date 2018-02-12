#ifndef AXIS_ALIGNED_BOUNDING_BOX_H
#define AXIS_ALIGNED_BOUNDING_BOX_H

#include "../CollisionModel.h"

class AABB : public CollisionModel
{
protected:
  float m_width;
  float m_height;
  float m_depth;

public:
  AABB();
  AABB(float width, float height, float depth);

  Pos3 getDim();
  float getWidth();
  float getHeight();
  float getDepth();
};

#endif
