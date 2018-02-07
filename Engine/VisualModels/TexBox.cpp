#include "TexBox.h"


TexBox::TexBox()
{
  m_type = VISUAL_MODEL_TEX_BOX;
}


bool TexBox::init(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  float width,
  float height,
  float depth,
  std::string &texFileName,
  float texScaleU,
  float texScaleV,
  bool bStaticScreenLoc)
{
  float halfWidth = width * 0.5;
  float halfHeight = height * 0.5;
  float halfDepth = depth * 0.5;

  //     4--------5
  //    /|       /|
  //   0--------1 |
  //   | |      | |
  //   | 6------|-7
  //   |/       |/
  //   2--------3
  //

  Pos3 pt0(-halfWidth,  halfHeight,  halfDepth);
  Pos3 pt1( halfWidth,  halfHeight,  halfDepth);
  Pos3 pt2(-halfWidth, -halfHeight,  halfDepth);
  Pos3 pt3( halfWidth, -halfHeight,  halfDepth);
  Pos3 pt4(-halfWidth,  halfHeight, -halfDepth);
  Pos3 pt5( halfWidth,  halfHeight, -halfDepth);
  Pos3 pt6(-halfWidth, -halfHeight, -halfDepth);
  Pos3 pt7( halfWidth, -halfHeight, -halfDepth);

  Pos2 uv0(0.0, 0.0);
  Pos2 uv1(texScaleU, 0.0);
  Pos2 uv2(0.0, texScaleV);
  Pos2 uv3(texScaleU, texScaleV);

  std::vector<Pos3Uv2> triList;
  
  // Front face
  triList.push_back(Pos3Uv2(pt0, uv0));
  triList.push_back(Pos3Uv2(pt1, uv1));
  triList.push_back(Pos3Uv2(pt2, uv2));
  triList.push_back(Pos3Uv2(pt3, uv3));

  // Back face
  triList.push_back(Pos3Uv2(pt5, uv0));
  triList.push_back(Pos3Uv2(pt4, uv1));
  triList.push_back(Pos3Uv2(pt7, uv2));
  triList.push_back(Pos3Uv2(pt6, uv3));

  // Right face
  triList.push_back(Pos3Uv2(pt1, uv0));
  triList.push_back(Pos3Uv2(pt5, uv1));
  triList.push_back(Pos3Uv2(pt3, uv2));
  triList.push_back(Pos3Uv2(pt7, uv3));

  // Left face
  triList.push_back(Pos3Uv2(pt4, uv0));
  triList.push_back(Pos3Uv2(pt0, uv1));
  triList.push_back(Pos3Uv2(pt6, uv2));
  triList.push_back(Pos3Uv2(pt2, uv3));

  // Top face
  triList.push_back(Pos3Uv2(pt4, uv0));
  triList.push_back(Pos3Uv2(pt5, uv1));
  triList.push_back(Pos3Uv2(pt0, uv2));
  triList.push_back(Pos3Uv2(pt1, uv3));

  // Bottom face
  triList.push_back(Pos3Uv2(pt2, uv0));
  triList.push_back(Pos3Uv2(pt3, uv1));
  triList.push_back(Pos3Uv2(pt6, uv2));
  triList.push_back(Pos3Uv2(pt7, uv3));

  return TexPoly::init(
    dev,
    devcon,
    texFileName,
    triList,
    bStaticScreenLoc);
}


void TexBox::render(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon)
{
  // Set the shader objects.
  devcon->VSSetShader(m_pVs, 0, 0);
  devcon->PSSetShader(m_pPs, 0, 0);

  devcon->IASetInputLayout(m_pLayout);

  // Set the sampler state in the pixel shader.
  devcon->PSSetSamplers(0, 1, &m_pSampleState);

  devcon->PSSetShaderResources(0, 1, &m_pTexture);

  // Select which vertex buffer to display.
  UINT stride = sizeof(Pos3Uv2);
  UINT offset = 0;
  devcon->IASetVertexBuffers(0, 1, &m_pVBuffer, &stride, &offset);

  // Select which primtive type we are using.
  devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // Draw the vertex buffer to the back buffer. Need to do this once per face since vertex buffer for this class is
  // 6 separate triangle lists.
  const int NUM_FACES = 6;
  const int VERTICES_PER_FACE = 4;
  for (int i = 0; i < NUM_FACES; i++)
  {
    devcon->Draw(VERTICES_PER_FACE, i * VERTICES_PER_FACE);
  }
}
