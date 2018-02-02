#ifndef GAME_TIMING_H
#define GAME_TIMING_H

class Timing
{
private:
  bool    m_bInitialized;
  double  m_ticksPerMs;

public:
  Timing();
  bool init();
  double getTimeMs();
};

#endif