#ifndef PHYSICS_MODEL_H
#define PHYSICS_MODEL_H

#include "CommonTypes.h"
#include <vector>

class PhysicsModel;
class PhysicsUpdateModel;

typedef struct PModelInput_t
{
  PhysicsModel  *pModel;
  Pos3          pos;      // In units.
  Pos3          vel;      // In units per step.
  Pos3          rot;      // x component is rotation about x axis in radians.
  Pos3          rotVel;   // In radians per step.

  PModelInput_t()
  {
    pModel = NULL;
  }
} PModelInput;


typedef struct PModelOutput_t
{
  Pos3  pos;
  Pos3  vel;
  Pos3  rot;
  Pos3  rotVel;
} PModelOutput;

// Base class for physics (including user input) handling.
class PhysicsModel
{
protected:
  PhysicsUpdateModel *m_pUpdateModel;

public:
  PhysicsModel();

  static bool runPuModel(
    PModelInput &pModelInput,
    PModelInput *otherModels[],
    PModelOutput &output);

  void setPuModel(PhysicsUpdateModel *pUpdateModel);
  PhysicsUpdateModel* getPuModel();

  // Any derived class that has new dynamic memory should implement its own release().
  virtual bool release();
};

#endif
