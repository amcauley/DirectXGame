#include "TexPoly.h"
#include "Util.h"
#include "Logger.h"

TexPoly::TexPoly()
{
  m_type              = VISUAL_MODEL_TEX_POLY;
  m_pVs               = NULL;
  m_pPs               = NULL;
  m_pVBuffer          = NULL;
  m_pLayout           = NULL;
  m_pTexture          = NULL;
  m_pSampleState      = NULL;
}


// Texture Polygon
bool TexPoly::init(
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon,
  std::string &texFileName,
  std::vector<Pos3Uv2> &vertices,
  bool bStaticScreenLoc)
{
  m_bStaticScreenLoc = bStaticScreenLoc;
  m_vertices = vertices;
  m_texFileName = texFileName;

  // Load and compile the shaders.
  LOGD("Begin texture load");
  ID3D10Blob *Vs, *Ps;
  D3DX11CompileFromFile("Engine/Shaders/shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &Vs, 0, 0);
  D3DX11CompileFromFile("Engine/Shaders/shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &Ps, 0, 0);
  LOGD("Texture load finished");

  // Encapsulate both shaders into shader objects.
  dev->CreateVertexShader(Vs->GetBufferPointer(), Vs->GetBufferSize(), NULL, &m_pVs);
  dev->CreatePixelShader(Ps->GetBufferPointer(), Ps->GetBufferSize(), NULL, &m_pPs);

  // Process texture info.
  D3DX11CreateShaderResourceViewFromFile(dev, texFileName.c_str(), NULL, NULL, &m_pTexture, NULL);

  D3D11_SAMPLER_DESC samplerDesc;
  // Create a texture sampler state description.
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.BorderColor[0] = 0;
  samplerDesc.BorderColor[1] = 0;
  samplerDesc.BorderColor[2] = 0;
  samplerDesc.BorderColor[3] = 0;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  // Create the texture sampler state.
  dev->CreateSamplerState(&samplerDesc, &m_pSampleState);

  // create the input layout object
  D3D11_INPUT_ELEMENT_DESC ied[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };

  dev->CreateInputLayout(ied, 2, Vs->GetBufferPointer(), Vs->GetBufferSize(), &m_pLayout);

  // Create the vertex buffer.
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  bd.Usage = D3D11_USAGE_DYNAMIC;                // Write access by CPU and GPU
  bd.ByteWidth = m_vertices.size() * sizeof(m_vertices[0]);
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // Use as a vertex buffer
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // Allow CPU to write in buffer
  dev->CreateBuffer(&bd, NULL, &m_pVBuffer);

  // Copy the vertices into their buffers.
  D3D11_MAPPED_SUBRESOURCE ms;
  devcon->Map(m_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);              // map the buffer
  memcpy(ms.pData, m_vertices.data(), m_vertices.size() * sizeof(m_vertices[0])); // copy the data
  devcon->Unmap(m_pVBuffer, NULL);                                                // unmap the buffer

  return true;
}


void TexPoly::render(
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

  // Draw the vertex buffer to the back buffer.
  devcon->Draw(m_vertices.size(), 0);
}


bool TexPoly::release()
{
  RELEASE_NON_NULL(m_pVs);
  RELEASE_NON_NULL(m_pPs);
  RELEASE_NON_NULL(m_pVBuffer);
  RELEASE_NON_NULL(m_pLayout);
  RELEASE_NON_NULL(m_pTexture);
  RELEASE_NON_NULL(m_pSampleState);

  return VisualModel::release();
}


TexPoly::~TexPoly()
{
  release();
}