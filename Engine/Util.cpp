#include "Util.h"
#include "Logger.h"


uint64_t genUUID(void)
{
  // If we generate one every micro-sec, we won't overflow for over 500 thousand years.
  static uint64_t uuidCnt = 0;
  return uuidCnt++;
}


// Relies on Logger already being initialized.
bool HR_FAILED(HRESULT hr)
{
  if (hr < 0)
  {
    LOGE("ERROR %d", hr);
    return true;
  }

  return false;
}

bool squaresOverlap(Pos2 &center0, Pos2 &wh0, Pos2 &center1, Pos2 &wh1)
{
  return true;
}
