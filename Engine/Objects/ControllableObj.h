#ifndef CONTROLLABLE_OBJ_H
#define CONTROLLABLE_OBJ_H

#include "../GameObject.h"
#include "../Timing.h"

class SoundMgr;

class ControllableObj : public GameObject
{
private:
  double m_lastJumpMs;
  double m_lastWallJumpMs;

  double m_lastJumpEnMs;
  double m_lastWallJumpEnMs;
  Pos2 m_lastWallJumpNormal;

  uint32_t m_pingSoundHandle;

public:
  ControllableObj();

  bool init(ID3D11Device *dev, ID3D11DeviceContext *devcon);

  bool update(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    float timeMs,
    InputApi &input,
    SoundMgr *pSoundMgr);

  bool ControllableObj::prelimUpdate(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    float timeMs,
    InputApi &input,
    SoundMgr *pSoundMgr);

  bool release();
};

#endif