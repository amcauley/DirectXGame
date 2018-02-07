#include "PhysicsModel.h"
#include "PhysicsModels/PhysicsUpdateModel.h"
#include "Logger.h"


PhysicsModel::PhysicsModel()
{
  m_pUpdateModel = NULL;
}


void PhysicsModel::setPuModel(PhysicsUpdateModel *pUpdateModel)
{
  m_pUpdateModel = pUpdateModel;
}


PhysicsUpdateModel *PhysicsModel::getPuModel()
{
  return m_pUpdateModel;
}


bool PhysicsModel::release()
{
  bool bSuccess = true;
  if (m_pUpdateModel)
  {
    bSuccess = PhysicsUpdateModel::releasePuModel(m_pUpdateModel);
    m_pUpdateModel = NULL;
  }

  return true;
}


bool PhysicsModel::runPuModel(
  PModelInput &pModelInput,
  PModelInput *otherModels[],
  PModelOutput &output)
{
  if (!pModelInput.pModel)
  {
    //LOGE("Null PModel");
    return true;
  }

  return PhysicsUpdateModel::runPuModel(pModelInput, otherModels, output);
}

