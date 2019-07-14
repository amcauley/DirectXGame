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
  if (lowVal == firstVal) intersects = (firstVal + wh0.pos.x / 2 > secondVal - wh1.pos.x / 2);
  else intersects = (secondVal + wh1.pos.x / 2 > firstVal - wh0.pos.x / 2);
  if (!intersects) return false;

  firstVal = center0.pos.y;
  secondVal = center1.pos.y;
  lowVal = fminf(firstVal, secondVal);
  if (lowVal == firstVal) intersects = (firstVal + wh0.pos.y / 2 > secondVal - wh1.pos.y / 2);
  else intersects = (secondVal + wh1.pos.y / 2 > firstVal - wh0.pos.y / 2);

  //if (intersects)
  //{
  //  LOGD("Intersection: center0 (%f, %f), width (%f, %f), center1 (%f, %f), width (%f, %f)",
  //    center0.pos.x,
  //    center0.pos.y,
  //    wh0.pos.x,
  //    wh0.pos.y,
  //    center1.pos.x,
  //    center1.pos.y,
  //    wh1.pos.x,
  //    wh1.pos.y);
  //}

  return intersects;
}

// Note: Could probably speed things up slightly by rewriting the checks in 3D, but better to have SPOT, at least for early dev.
bool cubesOverlap(Pos3 &center0, Pos3 &wh0, Pos3 &center1, Pos3 &wh1)
{
  bool bOverlap = squaresOverlap(
    Pos2(center0.pos.x, center0.pos.y),
    Pos2(wh0.pos.x, wh0.pos.y),
    Pos2(center1.pos.x, center1.pos.y),
    Pos2(wh1.pos.x, wh1.pos.y));

  if (!bOverlap) return false;

  return squaresOverlap(
    Pos2(center0.pos.x, center0.pos.z),
    Pos2(wh0.pos.x, wh0.pos.z),
    Pos2(center1.pos.x, center1.pos.z),
    Pos2(wh1.pos.x, wh1.pos.z));
}

// Find the squared distance between two Pos3 points.
float dist2(Pos3 &first, Pos3 &second)
{
  return
    (first.pos.x - second.pos.x) * (first.pos.x - second.pos.x) +
    (first.pos.y - second.pos.y) * (first.pos.y - second.pos.y) +
    (first.pos.z - second.pos.z) * (first.pos.z - second.pos.z);
}
