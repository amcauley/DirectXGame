#ifndef AABB_CONTROLLABLE_H
#define AABB_CONTROLLABLE_H

#include "AABB.h"

class AABBControllable : public AABB
{
protected:
  void onCollisionWithAabbImmobile(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo, int cnt);

  // Flag indicating if the controllable model is in a state that allows jumping, ex. colliding with a floor underneath it.
  // This is set during collision checks, but should be cleared by the parent object.
  bool m_bJumpEn{ false };

public:
  AABBControllable();
  AABBControllable(float w, float h, float d);

  static void CheckHitWImmobileBasedOnVel(
    Pos3 &vel,
    Pos3 &mainPos,
    Pos3 &mainPosAabb,
    Pos3 &mainDimAabb,
    Pos3 &otherPos,
    Pos3 &otherPosAabb,
    Pos3 &otherDimAabb,
    bool &bHitZ,
    float &distZ);

  static void CheckHitsWImmobileBasedOnVel(
    Pos3 &primaryPos,
    Pos3 &primaryVel,
    AABB *pPrimaryAabb,
    Pos3 &otherPos,
    AABB *pOtherAabb,
    bool &bHitX,
    float &distX,
    bool &bHitY,
    float &distY,
    bool &bHitZ,
    float &distZ);

  static void CheckClearWImmobileBasedOnVel(
    Pos3 &vel,
    Pos3 &mainPos,
    Pos3 &mainPosAabb,
    Pos3 &mainDimAabb,
    Pos3 &otherPos,
    Pos3 &otherPosAabb,
    Pos3 &otherDimAabb,
    float &clearTimeInFutureZ);

  static void CheckClearsWImmobileBasedOnVel(
    Pos3 &primaryPos,
    Pos3 &primaryVel,
    AABB *pPrimaryAabb,
    Pos3 &otherPos,
    AABB *pOtherAabb,
    float &clearTimeInFutureX,
    float &clearTimeInFutureY,
    float &clearTimeInFutureZ);

  virtual void setJumpEn(bool bJumpEn);
  virtual bool getJumpEn();

  virtual void onCollision(PmModelStorage *pPrimaryIo, PmModelStorage *pOtherModelIo, int cnt);
};

#endif
