#ifndef AXIS_ALIGNED_BOUNDING_BOX_H
#define AXIS_ALIGNED_BOUNDING_BOX_H

#include "../CollisionModel.h"
#include "../../CommonTypes.h"

class AABB : public CollisionModel
{
private:
  Pos3 m_pos; // Position (relative to owner object's position)
  float m_width;
  float m_height;
  float m_depth;

public:
  AABB(float width, float height, float depth);

  Pos3 getPos();
  Pos3 getDim();
  float getWidth();
  float getHeight();
  float getDepth();
};

#endif
