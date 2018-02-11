#include "AABB.h"
#include "../../Logger.h"

AABB::AABB(float width, float height, float depth):
  m_width  (width),
  m_height (height),
  m_depth  (depth)
{
  m_type = COLLISION_MODEL_AABB;
}


Pos3 AABB::getPos()
{
  return m_pos;
}


float AABB::getWidth()
{
  return m_width;
}


float AABB::getHeight()
{
  return m_height;
}


float AABB::getDepth()
{
  return m_depth;
}


Pos3 AABB::getDim()
{
  return Pos3(getWidth(), getHeight(), getDepth());
}
