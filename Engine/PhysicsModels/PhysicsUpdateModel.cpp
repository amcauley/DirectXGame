#include "PhysicsUpdateModel.h"
#include "../Logger.h"
#include "PhysicsUpdateModels\GravityModel.h"

PhysicsUpdateModelType PhysicsUpdateModel::getType()
{
  return m_type;
}


bool PhysicsUpdateModel::run(
  PModelInput &pModelInput,
  PModelInput *otherModels[],
  PModelOutput &output)
{
  return true;
}

bool PhysicsUpdateModel::release()
{
  return true;
}


bool PhysicsUpdateModel::runPuModel(
  PModelInput &pModelInput,
  PModelInput *otherModels[],
  PModelOutput &output)
{
  if (!pModelInput.pModel)
  {
    return true;
  }

  PhysicsUpdateModel *pModel = pModelInput.pModel->getPuModel();
  if (!pModel)
  {
    return true;
  }

  int modelType = pModel->getType();
  switch (modelType)
  {
    case PHYSICS_UPDATE_MODEL_NONE:
    {
      return pModel->run(pModelInput, otherModels, output);
    }
    case PHYSICS_UPDATE_MODEL_GRAVITY:
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

bool PhysicsUpdateModel::releasePuModel(PhysicsUpdateModel *pModel)
{
  if (!pModel)
  {
    //LOGW("Null pModel");
    return true;
  }

  PhysicsUpdateModelType pmType = pModel->getType();
  switch (pmType)
  {
    case PHYSICS_UPDATE_MODEL_NONE:
    {
      return pModel->release();
    }
    case PHYSICS_UPDATE_MODEL_GRAVITY:
    {
      return static_cast<GravityModel*>(pModel)->release();
    }
    default:
    {
      LOGE("PuModel type not recognized: %d", pmType);
      return false;
    }
  }

  return false;
}
