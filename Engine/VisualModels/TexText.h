#ifndef TEX_TEXT_H
#define TEX_TEXT_H

#include "../CommonTypes.h"
#include "../VisualModel.h"
#include <vector>
#include <map>


#define TEX_TEXT_ENTRY_INTERPRETATION   "I"
#define TEX_TEXT_ENTRY_FILENAME         "F"
#define TEX_TEXT_ENTRY_SIZES            "S"
#define TEX_TEXT_ENTRY_CHARACTERS       "C"

typedef enum TexTextInterp_
{
  TEX_TEXT_INTERP_MONO = 0
} TexTextInterp;

class TexText : public VisualModel
{
private:
  ID3D11VertexShader  *m_pVs;               // the pointer to the vertex shader
  ID3D11PixelShader   *m_pPs;               // the pointer to the pixel shader
  ID3D11Buffer        *m_pVBuffer;          // the pointer to the vertex buffer
  ID3D11InputLayout   *m_pLayout;           // the pointer to the input layout

  ID3D11ShaderResourceView  *m_pTexture;
  ID3D11SamplerState        *m_pSampleState;

  std::vector<Pos3Uv2>      m_vertices;

  std::string               m_text;
  std::string               m_fontDescTxtFileName;

  bool parseDescFile(ID3D11Device *dev, std::string &fontDescFileName);
  bool parseDescFileMono(ID3D11Device *dev, std::ifstream &fs);

  // Values from parsed description file:
  std::string               m_texFileName;
  Pos3                      m_position;
  float                     m_charHeight;
  float                     m_charWidth;

  // Map from character to it's uv coords (upper left, lower right).
  std::map<char, std::pair<Pos2, Pos2>>   m_charMap;

public:
  TexText();
  ~TexText();

  bool init(
    ID3D11Device *dev,                // D3D Device
    ID3D11DeviceContext *devcon,      // Device Context
    std::string displayText,          // Text String
    std::string fontDescTxtFileName,  // Font Description Filename
    Pos3 position,                    // X,Y,Z location of upper left corner
    float charWidth,                  // width of each character
    float charHeight,                 // height ...
    bool bStaticScreenLoc = false
    );

  void render(
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon);

  bool release();

  bool updateText(
    std::string &text,
    ID3D11Device *dev,
    ID3D11DeviceContext *devcon);
};

#endif