#ifndef TEX_RECT_H
#define TEX_RECT_H

#include "TexPoly.h"

class TexRect : public TexPoly
{
public:
  TexRect::TexRect();

  bool init(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    Pos3 topLeftPt,
    Pos3 topRightPt,
    Pos3 bottomRightPt,
    std::string &texFileName,
    bool bStaticScreenLoc = false);
};

#endif