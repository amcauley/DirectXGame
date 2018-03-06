#ifndef DX_GAME_SOUND_MGR_H
#define DX_GAME_SOUND_MGR_H

// http ://www.rastertek.com/dx11tut14.html

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <map>

struct WaveHeaderType
{
  char chunkId[4];
  uint32_t chunkSize;
  char format[4];
  char subChunkId[4];
  uint32_t subChunkSize;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t bytesPerSecond;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
  char dataChunkId[4];
  uint32_t dataSize;
};

#define SOUND_MGR_INVALID_HANDLE 0

class SoundMgr
{
private:
  IDirectSound8 *m_pDirectSound;
  IDirectSoundBuffer *m_pPrimaryBuffer;

  uint32_t m_handleCnt;
  std::map<uint32_t, IDirectSoundBuffer8*> m_handleToSoundMap;

public:
  SoundMgr();
  ~SoundMgr();

  bool init(HWND hwnd);
  bool release();

  // Register a file into the sound manager. Scenes, or anything else that uses this interface, can
  // then use the handle when issuing commands related to this file, ex. stop, play, etc.
  bool registerSound(std::string filename, uint32_t &handle);

  bool playSound(uint32_t handle, bool bLoop=false);
};

#endif