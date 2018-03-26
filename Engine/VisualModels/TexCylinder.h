#ifndef TEX_CYLINDER_H
#define TEX_CYLINDER_H

#include "TexPoly.h"

class TexCylinder : public TexPoly
{
private:
  uint32_t m_numFaces;
public:
  TexCylinder();

  virtual bool init(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    float radius,
    float height,
    std::string &texFileName,
    uint32_t numFaces = 8,
    float texScaleU = 1.0,
    float texScaleV = 1.0,
    bool bStaticScreenLoc = false);

  void updateLength(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    float L);
};

#endif