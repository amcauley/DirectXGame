#include "PolyObj.h"
#include "../Logger.h"

PolyObj::PolyObj()
{
  m_type = GAME_OBJECT_POLY_OBJ;
  m_pVModel = NULL;

  LOGD("GameObject %lu = type %d", static_cast<unsigned long>(m_uuid), m_type);
}


bool PolyObj::init(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  std::string &texFile,
  std::vector<Pos3Uv2> &verts,
  bool bStaticScreenLoc)
{
  TexPoly* pTexPoly = new TexPoly;
  if(!pTexPoly->init(dev, devcon, texFile, verts, bStaticScreenLoc))
  {
    LOGE("Failed to init TexPoly for PolyObj");
    return false;
  }

  m_pVModel = pTexPoly;
  return true;
}


bool PolyObj::init(TexPoly *pPoly)
{
  m_pVModel = pPoly;
  return true;
}


bool PolyObj::init(TexRect *pRect)
{
  m_pVModel = pRect;
  return true;
}


bool PolyObj::release()
{
  return GameObject::release();
}