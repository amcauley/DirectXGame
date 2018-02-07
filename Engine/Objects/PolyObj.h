#ifndef POLY_OBJ_H
#define POLY_OBJ_H

#include "../GameObject.h"
#include "../VisualModels/TexRect.h"

class PolyObj : public GameObject
{
private:

public:
  PolyObj();

  bool init(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    std::string &texFile,
    std::vector<Pos3Uv2> &verts,
    bool bStaticScreenLoc = false);

  bool init(TexPoly *pPoly);
  bool init(TexRect *pRect);

  bool release();
};

#endif