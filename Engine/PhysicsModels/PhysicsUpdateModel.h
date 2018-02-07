#ifndef PHYSICS_UPDATE_MODEL_H
#define PHYSICS_UPDATE_MODEL_H

#include "../CommonTypes.h"
#include "../PhysicsModel.h"
#include <vector>

class PhysicsModel;

typedef enum PhysicsUpdateModelType_
{
  PHYSICS_UPDATE_MODEL_NONE = 0,
  PHYSICS_UPDATE_MODEL_GRAVITY
} PhysicsUpdateModelType;


// Base class for physics (including user input) handling.
class PhysicsUpdateModel
{
protected:
  PhysicsUpdateModelType m_type = PHYSICS_UPDATE_MODEL_NONE;

public:
  static bool releasePuModel(PhysicsUpdateModel *pModel);
  static bool runPuModel(
    PModelInput &pModelInput,
    PModelInput *otherModels[],
    PModelOutput &output);

  PhysicsUpdateModelType getType();

  virtual bool run(
    PModelInput &pModelInput,
    PModelInput *otherModels[],
    PModelOutput &output
    );

  // Any derived class that has new dynamic memory should implement its own release().
  virtual bool release();
};

#endif
