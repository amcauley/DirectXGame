#ifndef HOOKSHOT_H
#define HOOKSHOT_H

#include "../GameObject.h"

class Hookshot : public GameObject
{
private:

public:
  Hookshot();

  bool init(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    std::string &texFile,
    double length,
    double radius,
    uint32_t numFaces = 8,
    bool bStaticScreenLoc = false);

  bool release();
};

#endif