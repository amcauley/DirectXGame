#include "TexRect.h"


TexRect::TexRect()
{
  m_type = VISUAL_MODEL_TEX_RECT;
}


bool TexRect::init(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  Pos3 topLeftPt,
  Pos3 topRightPt,
  Pos3 bottomRightPt,
  std::string &texFileName,
  bool bStaticScreenLoc)
{
  m_bStaticScreenLoc = bStaticScreenLoc;

  Pos3 bottomLeftPt;
  bottomLeftPt.pos.x = topLeftPt.pos.x + (bottomRightPt.pos.x - topRightPt.pos.x);
  bottomLeftPt.pos.y = topLeftPt.pos.y + (bottomRightPt.pos.y - topRightPt.pos.y);
  bottomLeftPt.pos.z = topLeftPt.pos.z + (bottomRightPt.pos.z - topRightPt.pos.z);

  Pos3Uv2 topLeft(    topLeftPt,     Pos2(0.0f, 0.0f) );
  Pos3Uv2 topRight(   topRightPt,    Pos2(1.0f, 0.0f) );
  Pos3Uv2 bottomLeft( bottomLeftPt,  Pos2(0.0f, 1.0f) );
  Pos3Uv2 bottomRight(bottomRightPt, Pos2(1.0f, 1.0f) );

  std::vector<Pos3Uv2> rectPoints = { topLeft, topRight, bottomLeft, bottomRight };

  TexPoly::init(dev, devcon, texFileName, rectPoints);

  return true;
}


bool TexRect::init(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  float width,
  float height,
  std::string &texFileName,
  bool bStaticScreenLoc)
{
  float halfHeight = height * 0.5;
  float halfWidth = width * 0.5;
  Pos3 topLeft(-halfWidth, halfHeight, 0.0);
  Pos3 topRight(halfWidth, halfHeight, 0.0);
  Pos3 bottomRight(halfWidth, -halfHeight, 0.0);

  return init(
    dev,
    devcon,
    topLeft,
    topRight,
    bottomRight,
    texFileName,
    bStaticScreenLoc);
}
