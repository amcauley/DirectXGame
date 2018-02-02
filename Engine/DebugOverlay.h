#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H

#include "GameObject.h"
#include "Timing.h"

class DebugOverlay : public GameObject
{
private:
  static const unsigned int MAX_FRAME_NUM = 999;
  static const unsigned int FRAME_NUM_DIGITS = 3; // Used for printing out frame info
  uint32_t m_frameCount;
  
  static const unsigned int FPS_NUM_DIGITS = 4;
  static const unsigned int MIN_FPS_ESTIMATION_PERIOD_MS = 500;
  uint32_t m_fpsEstimate;
  uint32_t m_frameDiff;
  float m_prevTimeStampMs;

public:
  DebugOverlay();
  bool init(ID3D11Device *dev, ID3D11DeviceContext *devcon);
  bool update(ID3D11Device *dev, ID3D11DeviceContext *devcon, float timeMs, InputApi &input);
  bool release();
};

#endif