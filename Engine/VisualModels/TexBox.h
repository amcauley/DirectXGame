#ifndef TEX_BOX_H
#define TEX_BOX_H

#include "TexPoly.h"
#include "TexBox.h"

class TexBox : public TexPoly
{
public:
  TexBox();

  virtual bool init(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    float width,
    float height,
    float depth,
    std::string &texFileName,
    float texScaleU = 1.0,
    float texScaleV = 1.0,
    bool bStaticScreenLoc = false);

  void render(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon);
};

#endif