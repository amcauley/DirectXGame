#include "PhysicsModel.h"
#include "PhysicsModels/CollisionModel.h"
#include "PhysicsModels/PhysicsUpdateModel.h"
#include "Logger.h"
#include "PhysicsModels/CollisionModel.h"


PhysicsModel::PhysicsModel()
{
  m_pUpdateModel = NULL;
  m_pCollisionModel = NULL;
}


void PhysicsModel::setPuModel(PhysicsUpdateModel *pUpdateModel)
{
  m_pUpdateModel = pUpdateModel;
}


PhysicsUpdateModel* PhysicsModel::getPuModel()
{
  return m_pUpdateModel;
}


void PhysicsModel::setCollisionModel(CollisionModel *pCollisionModel)
{
  m_pCollisionModel = pCollisionModel;
}


CollisionModel* PhysicsModel::getCollisionModel()
{
  return m_pCollisionModel;
}


// Transfers outputs from one physics step into the input to the next step. Useful for multiple steps per frame.
void PhysicsModel::interStepOutputToInputTransfer(PModelOutput &out, PModelInput &in)
{
  in.pos    = out.pos;
  in.vel    = out.vel;
  in.rot    = out.rot;
  in.rotVel = out.rotVel;
}

// Initialize the output to the input - useful for NULL models in which case we want to just maintain position, rotation, etc.
void PhysicsModel::prePhysInputToOutputTransfer(PModelInput &in, PModelOutput &out)
{
  out.pos    = in.pos;
  out.vel    = in.vel;
  out.rot    = in.rot;
  out.rotVel = in.rotVel;
}


bool PhysicsModel::release()
{
  bool bSuccess = true;
  if (m_pUpdateModel)
  {
    bSuccess &= PhysicsUpdateModel::releasePuModel(m_pUpdateModel);
    m_pUpdateModel = NULL;
  }

  if (m_pCollisionModel)
  {
    bSuccess &= CollisionModel::releaseCollisionModel(m_pCollisionModel);
    m_pCollisionModel = NULL;
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

