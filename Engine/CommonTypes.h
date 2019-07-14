#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <d3dx11.h>
#include <d3dx10.h>

typedef struct Pos2_
{
  D3DXVECTOR2 pos;

  Pos2_()
  {
    pos.x = pos.y = 0;
  }

  Pos2_(float x, float y)
  {
    pos.x = x;
    pos.y = y;
  }

} Pos2;

typedef struct Pos3_
{
  D3DXVECTOR3 pos;

  Pos3_()
  {
    pos.x = pos.y = pos.z = 0;
  }

  Pos3_(float x, float y, float z)
  {
    pos.x = x;
    pos.y = y;
    pos.z = z;
  }

} Pos3;

typedef struct Pos3Uv2_
{
  D3DXVECTOR3 pos;
  D3DXVECTOR2 uv;

  Pos3Uv2_()
  {
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    uv.x = 0;
    uv.y = 0;
  }

  Pos3Uv2_(Pos3 position, Pos2 uv_)
  {
    pos = position.pos;
    uv = uv_.pos;
  }

  Pos3Uv2_(float x, float y, float z, float u, float v)
  {
    pos.x = x;
    pos.y = y;
    pos.z = z;
    uv.x = u;
    uv.y = v;
  }

} Pos3Uv2;

typedef struct OrderingMetric_
{
  float primary{ 0.0 };    // Primary metric used for ordering.
  float secondary{ 0.0 };  // Tiebreaker metric.

  OrderingMetric_()
  {
  }

  bool operator< (const OrderingMetric_ &other) const
  {
    if (primary < other.primary)
    {
      return true;
    }
    else if (primary == other.primary)
    {
      return secondary < other.secondary;
    }
    return false;
  }

} OrderingMetric;

#endif