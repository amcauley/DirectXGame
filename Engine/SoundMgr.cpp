#include "SoundMgr.h"
#include "Util.h"
#include "Logger.h"

SoundMgr::SoundMgr()
{
  m_handleCnt = SOUND_MGR_INVALID_HANDLE;
  m_pDirectSound = NULL;
  m_pPrimaryBuffer = NULL;
}

SoundMgr::~SoundMgr()
{
  release();
}

bool SoundMgr::release()
{
  for (auto i = m_handleToSoundMap.begin(); i != m_handleToSoundMap.end(); ++i)
  {
    RELEASE_NON_NULL(i->second);
  }
  m_handleToSoundMap.clear();

  RELEASE_NON_NULL(m_pDirectSound);
  RELEASE_NON_NULL(m_pPrimaryBuffer);

  return true;
}

bool SoundMgr::init(HWND hwnd)
{
  HRESULT result;
  DSBUFFERDESC bufferDesc;
  WAVEFORMATEX waveFormat;

  // Initialize the direct sound interface pointer for the default sound device.
  if(HR_FAILED(DirectSoundCreate8(NULL, &m_pDirectSound, NULL)))
  {
    return false;
  }

  // Set the cooperative level to priority so the format of the primary sound buffer can be modified.
  if(HR_FAILED(m_pDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))
  {
    return false;
  }

  // Setup the primary buffer description.
  bufferDesc.dwSize = sizeof(DSBUFFERDESC);
  bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
  bufferDesc.dwBufferBytes = 0;
  bufferDesc.dwReserved = 0;
  bufferDesc.lpwfxFormat = NULL;
  bufferDesc.guid3DAlgorithm = GUID_NULL;

  // Get control of the primary sound buffer on the default sound device.
  if(HR_FAILED(m_pDirectSound->CreateSoundBuffer(&bufferDesc, &m_pPrimaryBuffer, NULL)))
  {
    return false;
  }

  // Setup the format of the primary sound bufffer.
  // In this case it is a .WAV file recorded at 44100 samples per second in 16-bit stereo (cd audio format).
  waveFormat.wFormatTag = WAVE_FORMAT_PCM;
  waveFormat.nSamplesPerSec = 44100;
  waveFormat.wBitsPerSample = 16;
  waveFormat.nChannels = 2;
  waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
  waveFormat.cbSize = 0;

  // Set the primary buffer to be the wave format specified.
  if(HR_FAILED(m_pPrimaryBuffer->SetFormat(&waveFormat)))
  {
    return false;
  }

  return true;
}

bool SoundMgr::registerSound(std::string filename, uint32_t &handle)
{
  int error;
  FILE* filePtr;
  unsigned int count;
  WaveHeaderType waveFileHeader;
  WAVEFORMATEX waveFormat;
  DSBUFFERDESC bufferDesc;
  HRESULT result;
  IDirectSoundBuffer* tempBuffer;
  unsigned char* waveData;
  unsigned char *bufferPtr;
  unsigned long bufferSize;

  LOGD("Starting sound registration for file %s", filename.c_str());

  // Open the wave file in binary.
  error = fopen_s(&filePtr, filename.c_str(), "rb");
  if (error != 0)
  {
    LOGW("Failed to open sound file, err %d", error);
    return false;
  }

  // Read in the wave file header.
  count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
  if (count != 1)
  {
    LOGW("Failed to read in header");
    return false;
  }

  // Check that the chunk ID is the RIFF format.
  if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
    (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
  {
    LOGW("Header not of type 'RIFF': '%c%c%c%c'",
      waveFileHeader.chunkId[0],
      waveFileHeader.chunkId[1],
      waveFileHeader.chunkId[2],
      waveFileHeader.chunkId[3]);
    return false;
  }

  // Check that the file format is the WAVE format.
  if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
    (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
  {
    LOGW("Format not of type 'WAVE': '%c%c%c%c'",
      waveFileHeader.format[0],
      waveFileHeader.format[1],
      waveFileHeader.format[2],
      waveFileHeader.format[3]);
    return false;
  }

  // Check that the sub chunk ID is the fmt format.
  if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
    (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
  {
    LOGW("SubChuckId not of type 'fmt ': '%c%c%c%c'",
      waveFileHeader.subChunkId[0],
      waveFileHeader.subChunkId[1],
      waveFileHeader.subChunkId[2],
      waveFileHeader.subChunkId[3]);
    return false;
  }

  // Check that the audio format is WAVE_FORMAT_PCM.
  if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
  {
    LOGW("AudioFormat not %u: %u", WAVE_FORMAT_PCM, waveFileHeader.audioFormat);
    return false;
  }

  // Check that the wave file was recorded in stereo format.
  if (waveFileHeader.numChannels != 2)
  {
    LOGW("NumChannels not 2: %u", waveFileHeader.numChannels);
    return false;
  }

  // Check that the wave file was recorded at a sample rate of 44.1 KHz.
  if (waveFileHeader.sampleRate != 44100)
  {
    LOGW("SampleRate not 44100: %u", waveFileHeader.sampleRate);
    return false;
  }

  // Ensure that the wave file was recorded in 16 bit format.
  if (waveFileHeader.bitsPerSample != 16)
  {
    LOGW("BitsPerSample not 16: %u", waveFileHeader.bitsPerSample);
    return false;
  }

  // Check for the data chunk header.
  if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
    (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
  {
    LOGW("DataChunkId not of type 'data': '%c%c%c%c'",
      waveFileHeader.dataChunkId[0],
      waveFileHeader.dataChunkId[1],
      waveFileHeader.dataChunkId[2],
      waveFileHeader.dataChunkId[3]);
    return false;
  }

  // Set the wave format of secondary buffer that this wave file will be loaded onto.
  waveFormat.wFormatTag = WAVE_FORMAT_PCM;
  waveFormat.nSamplesPerSec = 44100;
  waveFormat.wBitsPerSample = 16;
  waveFormat.nChannels = 2;
  waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
  waveFormat.cbSize = 0;

  // Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
  bufferDesc.dwSize = sizeof(DSBUFFERDESC);
  bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
  bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
  bufferDesc.dwReserved = 0;
  bufferDesc.lpwfxFormat = &waveFormat;
  bufferDesc.guid3DAlgorithm = GUID_NULL;

  if (!m_pDirectSound)
  {
    LOGW("DirectSound ptr is NULL");
    return false;
  }

  // Create a temporary sound buffer with the specific buffer settings.
  IDirectSoundBuffer* pTempBuffer;
  if(HR_FAILED(m_pDirectSound->CreateSoundBuffer(&bufferDesc, &pTempBuffer, NULL)))
  {
    return false;
  }

  // Test the buffer format against the direct sound 8 interface and create the secondary buffer.
  IDirectSoundBuffer* pNewSecondaryBuffer;
  if(HR_FAILED(pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&pNewSecondaryBuffer)))
  {
    return false;
  }

  m_handleCnt++;
  m_handleToSoundMap[m_handleCnt] = static_cast<IDirectSoundBuffer8*>(pNewSecondaryBuffer);
  handle = m_handleCnt;

  RELEASE_NON_NULL(pTempBuffer);

  // Move to the beginning of the wave data which starts at the end of the data chunk header.
  fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

  // Create a temporary buffer to hold the wave file data.
  waveData = new unsigned char[waveFileHeader.dataSize];
  if (!waveData)
  {
    LOGW("Failed to allocate buffer for wave data");
    return false;
  }

  // Read in the wave file data into the newly created buffer.
  count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
  if (count != waveFileHeader.dataSize)
  {
    LOGW("Failed to read data into new buffer");
    return false;
  }

  // Close the file once done reading.
  error = fclose(filePtr);
  if (error != 0)
  {
    LOGW("Failed to close soudn file");
    return false;
  }

  // Lock the secondary buffer to write wave data into it.
  if(HR_FAILED(pNewSecondaryBuffer->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0)))
  {
    LOGW("Secondary buffer lock failed");
    return false;
  }

  // Copy the wave data into the buffer.
  memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

  // Unlock the secondary buffer after the data has been written to it.
  if(HR_FAILED(pNewSecondaryBuffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0)))
  {
    LOGW("Secondary buffer unlock failed");
    return false;
  }

  // Release the wave data since it was copied into the secondary buffer.
  delete[] waveData;
  waveData = 0;

  return true;
}


bool SoundMgr::playSound(uint32_t handle)
{
  IDirectSoundBuffer8* pBuffer = NULL;
  auto it = m_handleToSoundMap.find(handle);
  if (it == m_handleToSoundMap.end())
  {
    LOGW("Play failed, unknown handle %u", handle);
    return false;
  }
  pBuffer = it->second;

  if (!pBuffer)
  {
    LOGW("NULL buffer for handle %u", handle);
    return false;
  }

  // Set position at the beginning of the sound buffer.
  if(HR_FAILED(pBuffer->SetCurrentPosition(0)))
  {
    return false;
  }

  // Set volume of the buffer to 100%.
  if(HR_FAILED(pBuffer->SetVolume(DSBVOLUME_MAX)))
  {
    return false;
  }

  // Play the contents of the secondary sound buffer.
  if(HR_FAILED(pBuffer->Play(0, 0, DSBPLAY_LOOPING)))
  {
    return false;
  }

  return true;
}
