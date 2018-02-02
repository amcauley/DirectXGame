#include "PhysicsModel.h"
#include "Logger.h"
#include "GravityModel.h"

PhysicsModelType PhysicsModel::getType()
{
  return m_type;
}


bool PhysicsModel::run(
  PModelInput &pModelInput,
  PModelInput *otherModels[],
  PModelOutput &output)
{
  return true;
}

bool PhysicsModel::release()
{
  return true;
}


bool PhysicsModel::runPModel(
  PModelInput &pModelInput,
  PModelInput *otherModels[],
  PModelOutput &output)
{
  if (!pModelInput.pModel)
  {
    //LOGE("Null PModel");
    return true;
  }

  PhysicsModel *pModel = pModelInput.pModel;
  int modelType = pModel->getType();
  switch (modelType)
  {
    case PHYSICS_MODEL_NONE:
    {
      return pModel->run(pModelInput, otherModels, output);
    }
    case PHYSICS_MODEL_GRAVITY:
    {
      return static_cast<GravityModel*>(pModel)->run(pModelInput, otherModels, output);
    }
    default:
    {
      LOGE("Unexpected physics model type %d", modelType);
      return false;
    }
  }

  return true;
}

bool PhysicsModel::releasePModel(PhysicsModel *pModel)
{
  if (!pModel)
  {
    //LOGW("Null pModel");
    return true;
  }

  PhysicsModelType pmType = pModel->getType();
  switch (pmType)
  {
    case PHYSICS_MODEL_NONE:
    {
      return pModel->release();
    }
    case PHYSICS_MODEL_GRAVITY:
    {
      return static_cast<GravityModel*>(pModel)->release();
    }
    default:
    {
      LOGE("PModel type not recognized: %d", pmType);
      return false;
    }
  }

  return false;
}
