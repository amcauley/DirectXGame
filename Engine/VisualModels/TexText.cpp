#include "TexText.h"
#include <fstream>
#include <sstream>
#include <string>
#include "../Util.h"
#include "../Logger.h"

TexText::TexText()
{
  m_type = VISUAL_MODEL_TEX_TEXT;
  m_pVs = NULL;
  m_pPs = NULL;
  m_pVBuffer = NULL;
  m_pLayout = NULL;
  m_pTexture = NULL;
  m_pSampleState = NULL;
  m_position = Pos3(0.0f, 0.0f, 0.0f);
  m_charHeight = 0.0f;
  m_charWidth = 0.0f;
}


bool TexText::init(
  ID3D11Device *dev,                // D3D Device
  ID3D11DeviceContext *devcon,      // Device Context
  std::string displayText,          // Text String
  std::string fontDescTxtFileName,  // Font Description Filename
  Pos3 position,                    // X,Y,Z location of upper left corner
  float charWidth,                  // Width of each character
  float charHeight,                 // height ...
  bool bStaticScreenLoc
  )
{
  m_bStaticScreenLoc = bStaticScreenLoc;
  m_position = position;
  m_charHeight = charHeight;
  m_charWidth = charWidth;

  // Load and compile the shaders.
  LOGD("Begin texture load");
  ID3D10Blob *Vs, *Ps;
  D3DX11CompileFromFile("Engine/Shaders/shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &Vs, 0, 0);
  D3DX11CompileFromFile("Engine/Shaders/shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &Ps, 0, 0);
  LOGD("Texture load finished");

  // Encapsulate both shaders into shader objects.
  dev->CreateVertexShader(Vs->GetBufferPointer(), Vs->GetBufferSize(), NULL, &m_pVs);
  dev->CreatePixelShader(Ps->GetBufferPointer(), Ps->GetBufferSize(), NULL, &m_pPs);

  // Grab params for this particular font.
  parseDescFile(dev, fontDescTxtFileName);

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

  return updateText(displayText, dev, devcon);
}


bool TexText::updateText(
  std::string &text,
  ID3D11Device *dev,
  ID3D11DeviceContext *devcon)
{
  m_text = text;
  uint32_t prevVertSize = m_vertices.size();
  m_vertices.clear();

  // Triangle list formatting.
  Pos3 topLeftCorner = m_position;
  for (int charIdx = 0; charIdx < m_text.length(); charIdx++)
  {
    // Handle newline: reset horizontal pos, increment vert.
    if (m_text[charIdx] == '\n')
    {
      topLeftCorner.pos.x = m_position.pos.x;
      topLeftCorner.pos.y -= m_charHeight;
      continue;
    }

    // Unmapped character. Set it to solid color.
    if (m_charMap.find(m_text[charIdx]) == m_charMap.end())
    {
      m_charMap[m_text[charIdx]] = std::pair<Pos2, Pos2>(Pos2(0.0f, 0.0f), Pos2(0.0f, 0.0f));
    }

    // Top Left
    m_vertices.push_back(Pos3Uv2(
      topLeftCorner.pos.x,
      topLeftCorner.pos.y,
      topLeftCorner.pos.z,
      m_charMap[m_text[charIdx]].first.pos.x,
      m_charMap[m_text[charIdx]].first.pos.y
      ));

    // Top Right
    m_vertices.push_back(Pos3Uv2(
      topLeftCorner.pos.x + m_charWidth,
      topLeftCorner.pos.y,
      topLeftCorner.pos.z,
      m_charMap[m_text[charIdx]].second.pos.x,
      m_charMap[m_text[charIdx]].first.pos.y
      ));

    // Bottom Left
    m_vertices.push_back(Pos3Uv2(
      topLeftCorner.pos.x,
      topLeftCorner.pos.y - m_charHeight,
      topLeftCorner.pos.z,
      m_charMap[m_text[charIdx]].first.pos.x,
      m_charMap[m_text[charIdx]].second.pos.y
      ));

    // Bottom Left
    m_vertices.push_back(Pos3Uv2(
      topLeftCorner.pos.x,
      topLeftCorner.pos.y - m_charHeight,
      topLeftCorner.pos.z,
      m_charMap[m_text[charIdx]].first.pos.x,
      m_charMap[m_text[charIdx]].second.pos.y
      ));

    // Top Right
    m_vertices.push_back(Pos3Uv2(
      topLeftCorner.pos.x + m_charWidth,
      topLeftCorner.pos.y,
      topLeftCorner.pos.z,
      m_charMap[m_text[charIdx]].second.pos.x,
      m_charMap[m_text[charIdx]].first.pos.y
      ));

    // Bottom Right
    m_vertices.push_back(Pos3Uv2(
      topLeftCorner.pos.x + m_charWidth,
      topLeftCorner.pos.y - m_charHeight,
      topLeftCorner.pos.z,
      m_charMap[m_text[charIdx]].second.pos.x,
      m_charMap[m_text[charIdx]].second.pos.y
      ));

    // Update Top Left position for next character
    topLeftCorner.pos.x += m_charWidth;
  }

  if (m_vertices.size() == 0)
  {
    LOGW("TexText size 0");
    return false;
  }

  // Create the vertex buffer if it doesn't already exist at the correct size.
  if (prevVertSize != m_vertices.size())
  {
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;                // Write access by CPU and GPU
    bd.ByteWidth = m_vertices.size() * sizeof(m_vertices[0]);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // Use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // Allow CPU to write in buffer

    RELEASE_NON_NULL(m_pVBuffer);
    dev->CreateBuffer(&bd, NULL, &m_pVBuffer);

    return true;
  }

  // Copy the vertices into their buffers.
  D3D11_MAPPED_SUBRESOURCE ms;
  devcon->Map(m_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);              // map the buffer
  memcpy(ms.pData, m_vertices.data(), m_vertices.size() * sizeof(m_vertices[0])); // copy the data
  devcon->Unmap(m_pVBuffer, NULL);                                                // unmap the buffer
}


bool TexText::parseDescFile(ID3D11Device *dev, std::string &fontDescFileName)
{
  std::ifstream fs(fontDescFileName);
  std::string curLine;

  bool bParsed = false;
  while (!bParsed && std::getline(fs, curLine))
  {
    std::istringstream lineStream(curLine);
    std::string curWord;

    lineStream >> curWord;

    if (curWord == TEX_TEXT_ENTRY_INTERPRETATION)
    {
      int interp;
      lineStream >> interp;
      switch (interp)
      {
        case(TEX_TEXT_INTERP_MONO) :
        {
          parseDescFileMono(dev, fs);
          bParsed = true;
          break;
        }
        default:
        {
          break;
        }
      }
    }
  }

  return true;
}


bool TexText::parseDescFileMono(ID3D11Device *dev, std::ifstream &fs)
{
  std::string curLine;

  int texWidth = 0, texHeight = 0;

  // Params read from tex descr.
  int charRows = 0, charCols = 0, startVertOffset = 0, startHorizOffset = 0,
    charHeight = 0, charWidth = 0, vertStepSize = 0, horizStepSize = 0;

  while (std::getline(fs, curLine))
  {
    std::istringstream lineStream(curLine);
    std::string curWord;

    lineStream >> curWord;

    if (curWord == TEX_TEXT_ENTRY_FILENAME)
    {
      lineStream >> m_texFileName;
      // Process texture info.
      D3DX11CreateShaderResourceViewFromFile(dev, m_texFileName.c_str(), NULL, NULL, &m_pTexture, NULL);

      // https://stackoverflow.com/questions/17658392/dimensions-of-id3d11shaderresourceview
      ID3D11Resource *pResource = NULL;
      m_pTexture->GetResource(&pResource);
      ID3D11Texture2D *pTex2d = static_cast<ID3D11Texture2D*>(pResource);
      D3D11_TEXTURE2D_DESC desc;
      pTex2d->GetDesc(&desc);

      texWidth = desc.Width;
      texHeight = desc.Height;

      RELEASE_NON_NULL(pTex2d);
      RELEASE_NON_NULL(pResource);
    }
    else if (curWord == TEX_TEXT_ENTRY_SIZES)
    {
      // Num characters rows and cols, starting vert offset, starting horiz offset,
      // char height, char width, vert step size, horiz step size.
      lineStream >> charRows >> charCols >> startVertOffset >> startHorizOffset >>
        charHeight >> charWidth >> vertStepSize >> horizStepSize;
    }
    else if (curWord == TEX_TEXT_ENTRY_CHARACTERS)
    {
      std::string charStr;
      lineStream >> charStr;
      float uOffset = static_cast<float>(startHorizOffset) / texWidth;
      float vOffset = static_cast<float>(startVertOffset) / texHeight;
      float horizStep = static_cast<float>(horizStepSize) / texWidth;
      float vertStep = static_cast<float>(vertStepSize) / texHeight;

      for (int charIdx = 0; charIdx < charStr.length(); charIdx++)
      {
        int charCol = charIdx % charCols;
        int charRow = charIdx / charCols;
        float topLeftUPos = uOffset + static_cast<float>(charWidth) * charCol  / texWidth;
        float topLeftVPos = vOffset + static_cast<float>(charHeight) * charRow / texHeight;
        float bottomRightUPos = topLeftUPos + horizStep;
        float bottomRightVPos = topLeftVPos + vertStep;

        m_charMap[charStr[charIdx]] = std::pair<Pos2, Pos2>(
          Pos2(topLeftUPos, topLeftVPos),
          Pos2(bottomRightUPos, bottomRightVPos));
      }
    }
  }

  return true;
}


void TexText::render(
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
  devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Draw the vertex buffer to the back buffer.
  devcon->Draw(m_vertices.size(), 0);
}


bool TexText::release()
{
  RELEASE_NON_NULL(m_pVs);
  RELEASE_NON_NULL(m_pPs);
  RELEASE_NON_NULL(m_pVBuffer);
  RELEASE_NON_NULL(m_pLayout);
  RELEASE_NON_NULL(m_pTexture);
  RELEASE_NON_NULL(m_pSampleState);

  return VisualModel::release();
}


TexText::~TexText()
{
  release();
}