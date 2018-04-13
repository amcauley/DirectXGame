#ifndef HOOKSHOT_H
#define HOOKSHOT_H

#include "../GameObject.h"

class Hookshot : public GameObject
{
private:
  float m_length;

  // Default object position is the base of the cable. Need a second position to represent the tip.
  Pos3 hookPos;

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

  void updateLength(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    double L);

  void updateLength(double L);

  bool release();

  void setBasePos(Pos3 &pos);
  void setHookPos(Pos3 &pos);

  bool update(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    float timeMs,
    InputApi &input,
    SoundMgr *pSoundMgr);
};

#endif