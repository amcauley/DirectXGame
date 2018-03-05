#ifndef CONTROLLABLE_OBJ_H
#define CONTROLLABLE_OBJ_H

#include "../GameObject.h"
#include "../Timing.h"

class SoundMgr;

class ControllableObj : public GameObject
{
private:
  double m_lastJumpMs;
public:
  ControllableObj();
  bool init(ID3D11Device *dev, ID3D11DeviceContext *devcon);
  bool update(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    float timeMs,
    InputApi &input,
    SoundMgr *pSoundMgr);
  bool release();
};

#endif