#include "DebugOverlay.h"
#include "TexText.h"
#include "Logger.h"

DebugOverlay::DebugOverlay()
{
  m_type = GAME_OBJECT_DEBUG_OVERLAY;
  m_frameCount = 0;
  m_frameDiff = 0;
  m_fpsEstimate = 0;
  m_prevTimeStampMs = 0.0;
  m_pVModel = new TexText;

  LOGD("GameObject %lu = type %d", static_cast<unsigned long>(m_uuid), m_type);
}

bool DebugOverlay::init(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
  if (!m_pVModel)
  {
    LOGE("No VModel at init");
    return false;
  }

  TexText *pTexText = static_cast<TexText*>(m_pVModel);
  if (!pTexText->init(
    dev, devcon,
    std::string("---"),
    std::string("Engine/Fonts/RobotoMono.txt"),
    Pos3(-0.8f, 0.45f, -0.5f),
    0.05f, 0.07f,
    true))
  {
    LOGE("Failed to init debug text");
    return false;
  }

  return true;
}


bool DebugOverlay::update(ID3D11Device *dev, ID3D11DeviceContext *devcon, float timeMs, InputApi &input)
{
  // Display order: Frame count, FPS
  std::string dbgString;
  std::string tempString;
  int numPaddingZeroes = 0;

  //Frame counter
  //m_frameCount = m_frameCount >= MAX_FRAME_NUM ? 0 : m_frameCount + 1;
  //tempString = std::to_string(m_frameCount);
  //
  //numPaddingZeroes = FRAME_NUM_DIGITS - tempString.length();
  //if (numPaddingZeroes > 0)
  //{
  //  tempString = std::string(numPaddingZeroes, '0') + tempString;
  //}
  //dbgString = "#" + tempString;

  // FPS update
  //dbgString += ",";
  float tsDiff = timeMs - m_prevTimeStampMs;
  if (tsDiff >= MIN_FPS_ESTIMATION_PERIOD_MS)
  {
    m_fpsEstimate = m_frameDiff * 1000.0 / tsDiff;
    m_prevTimeStampMs = timeMs;
    m_frameDiff = 0;
  }
  m_frameDiff++;

  tempString = std::to_string(m_fpsEstimate);
  numPaddingZeroes = FPS_NUM_DIGITS - tempString.length();
  if (numPaddingZeroes > 0)
  {
    tempString = std::string(numPaddingZeroes, '0') + tempString;
  }
  dbgString += tempString; // +"FPS";

  // User input updates
  //dbgString += ",";
  //tempString = "S" + std::to_string(input.bSpace) + ",U" + std::to_string(input.keyUp) + ",R" + std::to_string(input.keyRight);
  //dbgString += tempString;

  static_cast<TexText*>(m_pVModel)->updateText(
    dbgString,
    dev, devcon);

  return true;
}


bool DebugOverlay::release()
{
  return GameObject::release();
}