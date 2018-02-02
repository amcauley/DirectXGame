#ifndef VISUAL_MODEL_H
#define VISUAL_MODEL_H

#include "CommonTypes.h"
#include <vector>

typedef enum VisualModelType_
{
  VISUAL_MODEL_NONE = 0,
  VISUAL_MODEL_TEX_POLY,
  VISUAL_MODEL_TEX_RECT,
  VISUAL_MODEL_TEX_TEXT
} VisualModelType;

// Base class for visual/graphics thangs.
class VisualModel
{
protected:
  VisualModelType m_type = VISUAL_MODEL_NONE;
  bool m_bStaticScreenLoc = false;

public:
  static bool releaseVModel(VisualModel *pModel);
  static bool renderVModel(
    VisualModel *pModel,
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon);

  VisualModelType getType();

  virtual void setStaticScreenLoc(bool bStaticScreenLoc);
  virtual bool getStaticScreenLoc();

  virtual void render(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon);

  // Any derived class that has new dynamic memory should implement its own release().
  virtual bool release();
};

#endif
