#include "TexCylinder.h"
#include <math.h>
#include "..\CommonPhysConsts.h"

TexCylinder::TexCylinder()
{
  m_numFaces = 0;
  m_type = VISUAL_MODEL_TEX_CYLINDER;
}


bool TexCylinder::init(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  float radius,
  float height,
  std::string &texFileName,
  uint32_t numFaces,
  float texScaleU,
  float texScaleV,
  bool bStaticScreenLoc)
{
  m_numFaces = numFaces;

  std::vector<Pos3Uv2> triList;

  Pos3 pt;
  Pos2 uv;

  float uStep = texScaleU / m_numFaces;

  float radStep = PHYS_CONST_PI * 2 / m_numFaces;

  for (uint32_t face = 0; face < m_numFaces; face++)
  {
    pt.pos.y = 0.0f;
    pt.pos.x = radius * cos(radStep * face);
    pt.pos.z = -radius * sin(radStep * face);
    uv.pos.x = uStep * face;
    uv.pos.y = texScaleV;
    triList.push_back(Pos3Uv2(pt, uv));

    pt.pos.y = height;
    uv.pos.y = 0.0f;
    triList.push_back(Pos3Uv2(pt, uv));
  }

  // Add in the final face which closes back to the starting face.
  pt.pos.y = 0.0f;
  pt.pos.x = radius;
  pt.pos.z = 0.0f;
  uv.pos.x = texScaleU;
  uv.pos.y = texScaleV;
  triList.push_back(Pos3Uv2(pt, uv));

  pt.pos.y = height;
  uv.pos.y = 0.0f;
  triList.push_back(Pos3Uv2(pt, uv));

  return TexPoly::init(
    dev,
    devcon,
    texFileName,
    triList,
    bStaticScreenLoc);
}


void TexCylinder::updateLength(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  float L)
{
  for (uint32_t face = 0; face <= m_numFaces; face++)
  {
    // Change the non-zero vertices, i.e. the odd ones (see init method for original vector creation).
    m_vertices[2 * face + 1].pos.y = L;
  }

  updatePoints(dev, devcon);
}

