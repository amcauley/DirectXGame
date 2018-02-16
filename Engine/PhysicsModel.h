#ifndef PHYSICS_MODEL_H
#define PHYSICS_MODEL_H

#include "CommonTypes.h"
#include <set>

class PhysicsModel;
class PhysicsUpdateModel;
class CollisionModel;
class PmModelStorage;

class PModelInput
{
public:
  PhysicsModel  *pModel;
  Pos3          pos;      // In units.
  Pos3          vel;      // In units per step.
  Pos3          rot;      // x component is rotation about x axis in radians.
  Pos3          rotVel;   // In radians per step.

  PModelInput()
  {
    pModel = NULL;
  }
};


class PModelOutput
{
public:
  Pos3  pos;
  Pos3  vel;
  Pos3  rot;
  Pos3  rotVel;

  // Collection of other objects (via their PmModelStorage ptrs) that this object collided with.
  std::set<PmModelStorage*> collisionSet;
};

// Base class for physics (including user input) handling.
class PhysicsModel
{
protected:
  PhysicsUpdateModel *m_pUpdateModel;
  CollisionModel     *m_pCollisionModel;

public:
  PhysicsModel();

  static void prePhysInputToOutputTransfer(PModelInput *pIn, PModelOutput *pOut);
  static void interStepOutputToInputTransfer(PModelOutput *pOut, PModelInput *pIn);

  static bool runPuModel(
    PModelInput &pModelInput,
    PModelInput *otherModels[],
    PModelOutput &output);

  void setPuModel(PhysicsUpdateModel *pUpdateModel);
  PhysicsUpdateModel* getPuModel();

  void setCollisionModel(CollisionModel *pCollisionModel);
  CollisionModel* getCollisionModel();

  // Any derived class that has new dynamic memory should implement its own release().
  virtual bool release();
};

#endif
