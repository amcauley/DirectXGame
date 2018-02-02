#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "CommonTypes.h"
#include "PhysicsModel.h"
#include <map>


typedef struct PmModelStorage_
{
  bool          bActive;  // Mark active when registered, cleared after run (so we can remove/reuse entries from model map.
  PModelInput   in;
  PModelOutput  out;
} PmModelStorage;

class PhysicsManager
{
private:
  double    m_stepSizeMs;
  uint32_t  m_maxStepsPerFrame;
  double    m_lastTimeMs;
  double    m_accumTimeMs;

  std::map<uint64_t, PmModelStorage> m_registeredModelMap;

public:
  PhysicsManager();
  ~PhysicsManager();
  bool release();
  bool registerModel(uint64_t uuid, PModelInput *pModelInput);
  bool run(double timeMs);
  bool getResult(uint64_t uuid, PModelOutput &modelOutput);
};

#endif