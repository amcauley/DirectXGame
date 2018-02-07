#ifndef GRAVITY_MODEL_H
#define GRAVITY_MODEL_H

#include "../../CommonTypes.h"
#include <vector>
#include "../../PhysicsModel.h"
#include "../PhysicsUpdateModel.h"


class GravityModel : public PhysicsUpdateModel
{
protected:

public:
  GravityModel();

  virtual bool run(
    PModelInput &pModelInput,
    PModelInput *otherModels[],
    PModelOutput &output
    );

  // Any derived class that has new dynamic memory should implement its own release().
  virtual bool release();
};

#endif
