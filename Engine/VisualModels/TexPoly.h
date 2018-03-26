#ifndef TEX_POLY_H
#define TEX_POLY_H

#include "../CommonTypes.h"
#include "../VisualModel.h"
#include <vector>

class TexPoly : public VisualModel
{
protected:
  ID3D11VertexShader  *m_pVs;               // the pointer to the vertex shader
  ID3D11PixelShader   *m_pPs;               // the pointer to the pixel shader
  ID3D11Buffer        *m_pVBuffer;          // the pointer to the vertex buffer
  ID3D11InputLayout   *m_pLayout;           // the pointer to the input layout

  ID3D11ShaderResourceView  *m_pTexture;
  ID3D11SamplerState        *m_pSampleState;

  std::vector<Pos3Uv2>      m_vertices;

  std::string               m_texFileName;

public:
  TexPoly();
  ~TexPoly();

  virtual bool init(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon,
    std::string &texFileName,
    std::vector<Pos3Uv2> &vertices,
    bool bStaticScreenLoc = false);

  void render(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon);

  bool release();

  void updatePoints(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon);
};

#endif