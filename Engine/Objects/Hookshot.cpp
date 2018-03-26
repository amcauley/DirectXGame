#include "Hookshot.h"
#include "../VisualModels/TexCylinder.h"
#include "../Logger.h"

Hookshot::Hookshot()
{
  m_type = GAME_OBJECT_HOOKSHOT;
  m_pVModel = NULL;

  LOGD("GameObject %lu = type %d", static_cast<unsigned long>(m_uuid), m_type);
}


bool Hookshot::init(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  std::string &texFile,
  double length,
  double radius,
  uint32_t numFaces,
  bool bStaticScreenLoc)
{
  TexCylinder* pTexCylinder = new TexCylinder;
  if(!pTexCylinder->init(dev, devcon, radius, length, texFile, numFaces, 1.0, 1.0, bStaticScreenLoc))
  {
    LOGE("Failed to init TexCylinder for Hookshot");
    return false;
  }

  m_pVModel = pTexCylinder;
  return true;
}


bool Hookshot::release()
{
  return GameObject::release();
}