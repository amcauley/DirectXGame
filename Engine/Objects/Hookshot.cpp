#include "Hookshot.h"
#include "../VisualModels/TexCylinder.h"
#include "../Logger.h"
#include "../CommonPhysConsts.h"

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


void Hookshot::updateLength(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  double L)
{
  static_cast<TexCylinder*>(m_pVModel)->updateLength(dev, devcon, L);
}


void Hookshot::updateLength(double L)
{
  m_length = L;
}


bool Hookshot::release()
{
  return GameObject::release();
}


void Hookshot::setBasePos(Pos3 &pos)
{
  setPos(pos);
}


void Hookshot::setHookPos(Pos3 &pos)
{
  hookPos = pos;

  // Update length and compute rotation. Should make this a method of the Pos3 class.
  float distX = pos.pos.x - m_pos.pos.x;
  float distY = pos.pos.y - m_pos.pos.y;
  float distZ = pos.pos.z - m_pos.pos.z;
  float dist = sqrt(distX*distX + distY*distY + distZ*distZ);
  float rad = sqrt(distX*distX + distZ*distZ);

  updateLength(dist);

  // Model's default orientation is vertical. Tilt forward and rotate.

  float pitchAngle = 0;
  if (dist) pitchAngle = asin(rad / dist);
  if (distY < 0) pitchAngle = PHYS_CONST_PI - pitchAngle;

  float yawAngle = 0;
  if (rad) yawAngle = asin(distX / rad);
  if (distZ < 0) yawAngle = PHYS_CONST_PI - yawAngle;

  setRot(Pos3(pitchAngle, yawAngle, 0.0));
  //LOGD("setHookPos: Base (%f, %f, %f), Hook (%f, %f, %f), Length %f, Pitch %f, Yaw %f",
  //  m_pos.pos.x, m_pos.pos.y, m_pos.pos.z,
  //  pos.pos.x, pos.pos.y, pos.pos.z,
  //  m_length,
  //  pitchAngle, yawAngle);
}

bool Hookshot::update(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  float timeMs,
  InputApi &input,
  SoundMgr *pSoundMgr)
{
  updateLength(dev, devcon, m_length);
  return true;
}
