#include "Timing.h"
#include "Windows.h"
#include "Logger.h"

Timing::Timing()
{
  m_ticksPerMs = 1.0f;
  m_bInitialized = false;
}


bool Timing::init()
{
  INT64 freq = 0;
  QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
  if (freq == 0)
  {
    LOGE("Failed QueryPerformanceFrequency check");
    return false;
  }

  m_ticksPerMs = (float)(freq / 1000);
  LOGD("System ticks/ms: %f", m_ticksPerMs);

  if (!m_ticksPerMs)
  {
    LOGE("Ticks per ms = 0");
    return false;
  }

  m_bInitialized = true;
  return true;
}


double Timing::getTimeMs()
{
  double tsMs = 0.0f;
  INT64 ts = 0;
  if (m_bInitialized)
  {
    QueryPerformanceCounter((LARGE_INTEGER*)&ts);
    tsMs = ts / m_ticksPerMs;
  }

  //LOGD("Updated timeMs: %f (%lu ticks)", tsMs, ts);
  return tsMs;
}