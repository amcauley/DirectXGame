#include "VisualModel.h"
#include "Logger.h"
#include "TexText.h"
#include "TexPoly.h"
#include "TexRect.h"
#include "TexBox.h"

VisualModelType VisualModel::getType()
{
  return m_type;
}


void VisualModel::setStaticScreenLoc(bool bStaticScreenLoc)
{
  m_bStaticScreenLoc = bStaticScreenLoc;
}


bool VisualModel::getStaticScreenLoc()
{
  return m_bStaticScreenLoc;
}


void VisualModel::render(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon)
{
}

bool VisualModel::release()
{
  return true;
}


bool VisualModel::renderVModel(
  VisualModel *pModel,
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon)
{
  if (!pModel)
  {
    //LOGE("Null VModel");
    return true;
  }

  int modelType = pModel->getType();
  switch (modelType)
  {
    case VISUAL_MODEL_NONE:
    {
      pModel->render(dev, devcon);
      break;
    }
    case VISUAL_MODEL_TEX_POLY:
    {
      static_cast<TexPoly*>(pModel)->render(dev, devcon);
      break;
    }
    case VISUAL_MODEL_TEX_RECT:
    {
      static_cast<TexRect*>(pModel)->render(dev, devcon);
      break;
    }
    case VISUAL_MODEL_TEX_BOX:
    {
      static_cast<TexBox*>(pModel)->render(dev, devcon);
      break;
    }
    case VISUAL_MODEL_TEX_TEXT:
    {
      static_cast<TexText*>(pModel)->render(dev, devcon);
      break;
    }
    default:
    {
      LOGE("Unexpected render model type %d", modelType);
      return false;
    }
  }

  // TODO: Change render methods to return bool, just return from each case.
  return true;
}

bool VisualModel::releaseVModel(VisualModel *pModel)
{
  if (!pModel)
  {
    //LOGW("Null pModel");
    return true;
  }

  VisualModelType vmType = pModel->getType();
  switch (vmType)
  {
    case VISUAL_MODEL_NONE:
    {
      return pModel->release();
    }
    case VISUAL_MODEL_TEX_TEXT:
    {
      return static_cast<TexText*>(pModel)->release();
    }
    case VISUAL_MODEL_TEX_POLY:
    {
      return static_cast<TexPoly*>(pModel)->release();
    }
    case VISUAL_MODEL_TEX_RECT:
    {
      return static_cast<TexRect*>(pModel)->release();
    }
    case VISUAL_MODEL_TEX_BOX:
    {
      return static_cast<TexBox*>(pModel)->release();
    }
    default:
    {
      LOGE("VModel type not recognized: %d", vmType);
      return false;
    }
  }

  return false;
}
