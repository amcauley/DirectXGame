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
  bool intersects;
  float firstVal, secondVal, lowVal;

  /* Find the left-most box and declare intersection, at least for this axis, if the leftmost
  box's rightmost index is greater than the rightmost box's left index. */
  firstVal = center0.pos.x;
  secondVal = center1.pos.x;
  lowVal = fminf(firstVal, secondVal);
  if (lowVal == firstVal) intersects = (firstVal + wh0.pos.x / 2 >= secondVal - wh1.pos.x / 2);
  else intersects = (secondVal + wh1.pos.x / 2 >= firstVal - wh0.pos.x / 2);
  if (!intersects) return false;

  firstVal = center0.pos.y;
  secondVal = center1.pos.y;
  lowVal = fminf(firstVal, secondVal);
  if (lowVal == firstVal) intersects = (firstVal + wh0.pos.y / 2 >= secondVal - wh1.pos.y / 2);
  else intersects = (secondVal + wh1.pos.y / 2 >= firstVal - wh0.pos.y / 2);
  return intersects;
}
