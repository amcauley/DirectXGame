#ifndef GRAPHICS_MANAGER_H
#define GRAPHICS_MANAGER_H

#include "CommonTypes.h"
#include "VisualModel.h"

typedef struct VS_CONST_BUFFER_T
{
  D3DMATRIX mat;
} VS_CONST_BUFFER;


class GraphicsManager
{
private:
  D3DXMATRIX m_worldMat;
  D3DXMATRIX m_viewMat;
  D3DXMATRIX m_projMat;
  D3DXMATRIX m_totMat;

  ID3D11Buffer* m_pConstBuffer;       // Used for passing values to shader(s).
  VS_CONST_BUFFER m_vsConstData;

public:
  GraphicsManager();
  ~GraphicsManager();
  void release();
  void initConstBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon);
  void setPosAndRot(Pos3 &position, Pos3 &rollYawPitch);
  void resetCamera();
  void setCamera(Pos3 &eye, Pos3 &lookAt, Pos3 &up);
  void setPerspective(float fovy, float aspect, float nearDist, float farDist);
  void renderModel(VisualModel *pModel, ID3D11Device *dev, ID3D11DeviceContext *devcon);
};

#endif