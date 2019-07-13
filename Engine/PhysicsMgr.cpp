#include "PhysicsMgr.h"
#include "CommonPhysConsts.h"
#include <algorithm>
#include "Logger.h"
#include "PhysicsModels/CollisionModel.h"

PhysicsManager::PhysicsManager()
{
  m_stepSizeMs        = STEP_SIZE_MS;
  m_maxStepsPerFrame  = MAX_STEPS_PER_FRAME;
  m_lastTimeMs        = 0.0;
  m_accumTimeMs       = 0.0;
}


PhysicsManager::~PhysicsManager()
{

}


bool PhysicsManager::release()
{
  return true;
}


///TODO: Internally PmModelStorage stores actual value of input struct instead of ref/ptr. Prob some minor perf gains available.
bool PhysicsManager::registerModel(uint64_t uuid, PModelInput *pModelInput)
{
  // Check if model was previously registered. If so, just copy over input data and clear done flag.
  std::map<uint64_t, PmModelStorage>::iterator it;
  it = m_registeredModelMap.find(uuid);
  if (it != m_registeredModelMap.end())
  {
    it->second.bActive = true;
    it->second.in = *pModelInput;
  }
  else
  {
    // New entry
    PmModelStorage storage;
    storage.bActive = true;
    storage.in = *pModelInput;
    m_registeredModelMap[uuid] = storage;
  }

  return true;
}


bool PhysicsManager::run(double timeMs)
{
  // Clean old models
  for (std::map<uint64_t, PmModelStorage>::iterator it = m_registeredModelMap.begin(); it != m_registeredModelMap.end(); /* No increment here */)
  {
    if (!it->second.bActive)
    {
      m_registeredModelMap.erase(it++);
    }
    else
    {
      // Clear old collision info.
      it->second.out.collisionSet.clear();
      ++it;
    }
  }

  // We'll iterate through the processing once per time block.
  double deltaMs = timeMs - m_lastTimeMs;
  m_lastTimeMs = timeMs;
  m_accumTimeMs += deltaMs;

  //LOGD("timeMs %f, lastTimeMs %f, deltaMs %f, accumMs %f", timeMs, m_lastTimeMs, deltaMs, m_accumTimeMs);
  uint32_t stepsToRun = m_accumTimeMs / m_stepSizeMs;
  stepsToRun = min(stepsToRun, m_maxStepsPerFrame);
  m_accumTimeMs -= stepsToRun * m_stepSizeMs;

  // If there's still a large backlog, just process what we can and start fresh for the next frame.
  if (m_accumTimeMs >= m_stepSizeMs)
  {
    LOGW("Clearing accumTimeMs: %f", m_accumTimeMs);
    m_accumTimeMs = 0.0;
  }

  //LOGD("Running %u steps", stepsToRun);
  int stepsCompleted = 0;
  do
  {
    bool bLastStep = stepsCompleted + 1 >= stepsToRun;
    bool bSkipProc = stepsCompleted >= stepsToRun;  //Should catch the case of 0 steps.

    for (std::map<uint64_t, PmModelStorage>::iterator it = m_registeredModelMap.begin(); it != m_registeredModelMap.end(); ++it)
    {
      // Copy input into output, i.e. NULL operation is default in case processing doesn't do anything (either by choice or mistake).
      PhysicsModel::prePhysInputToOutputTransfer(&it->second.in, &it->second.out);

      if (!bSkipProc)
      {
        // Currently not passing any other objects during processing.
        PhysicsModel::runPuModel(it->second.in, NULL, it->second.out);
      }
    }

    // 2nd loop: Now run collision checks on the updated locations, run any physics - model level collision handling.
    for (std::map<uint64_t, PmModelStorage>::iterator itFirst = m_registeredModelMap.begin(); itFirst != m_registeredModelMap.end(); ++itFirst)
    {
      if (!bSkipProc)
      {
        std::map<uint64_t, PmModelStorage>::iterator itSecond = itFirst;
        ++itSecond;
        for (; itSecond != m_registeredModelMap.end(); ++itSecond)
        {
          //LOGD("DBG: Checking collision, obj %u and %u", itFirst->first, itSecond->first);
          // TODO: Should run in order of collisions, i.e. handle the first hit, so that any subsequent hits
          // get handled using the result of the earlier ones.
          // Note that this doesn't account for any new objects that might be hit due to altered trajectories
          // from earlier hit handling.
          if (CollisionModel::modelsCollide(&(itFirst->second), &(itSecond->second)))
          {
            //LOGD("DBG: Model collision, obj %u and %u", itFirst->first, itSecond->first);
            CollisionModel::handleCollision(&(itFirst->second), &(itSecond->second));

            // Add each other to the collisions list for later object-level processing.
            // Set will automatically check that collision info isn't already present,
            // ex. from a previous step in the same frame processing.
            itFirst->second.out.collisionSet.insert(&(itSecond->second));
            itSecond->second.out.collisionSet.insert(&(itFirst->second));
          }
        }
      }

      if (!bLastStep)
      {
        // Copy over output into input in case we're running multiple steps.
        PhysicsModel::interStepOutputToInputTransfer(&itFirst->second.out, &itFirst->second.in);
      }
      else
      {
        // On last step (or nonexistent step in case we run 0 steps this frame), clear active flag.
        // If we have further processing for this object, it'll re-register for the next frame.
        itFirst->second.bActive = false;
      }
    }

    stepsCompleted++;
  } while (stepsCompleted < stepsToRun);

  return true;
}


bool PhysicsManager::getResult(uint64_t uuid, PModelOutput *pModelOutput)
{
  std::map<uint64_t, PmModelStorage>::iterator it;
  it = m_registeredModelMap.find(uuid);

  if (it == m_registeredModelMap.end())
  {
    LOGE("Couldn't find data for uuid %u", uuid);
    return false;
  }

  *pModelOutput = it->second.out;
  return true;
}