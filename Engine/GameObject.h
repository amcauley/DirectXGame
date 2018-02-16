#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "InputMgr.h"
#include "VisualModel.h"
#include "PhysicsModel.h"

typedef enum GameObjectType_
{
  GAME_OBJECT_NONE = 0,
  GAME_OBJECT_DEBUG_OVERLAY,
  GAME_OBJECT_CONTROLLABLE,
  GAME_OBJECT_POLY_OBJ
} GameObjectType;

class GameObject
{
protected:
  uint64_t        m_uuid;
  GameObjectType  m_type = GAME_OBJECT_NONE;

  Pos3            m_pos;
  Pos3            m_vel;
  Pos3            m_rot;
  Pos3            m_rotVel;

  VisualModel*    m_pVModel;
  PhysicsModel*   m_pPModel;
  
public:
  static bool releaseGameObject(GameObject *pObj);
  static bool updateGameObject(GameObject *pObj, ID3D11Device *dev, ID3D11DeviceContext *devcon, float timeMs, InputApi &input);
  GameObject();

  // GameObject takes ownership of any model passed to it and will free memory accordingly during destructor.
  GameObject(VisualModel* pVModel);
  // Don't rely on destructor to free any dynamic memory. This should be handled in the derived class release() method.
  ~GameObject();

  void setPos(Pos3 &newPos);
  Pos3 getPos();
  void setVel(Pos3 &newVel);
  Pos3 getVel();
  void setRot(Pos3 &newRot);
  Pos3 getRot();
  void setRotVel(Pos3 &newRotVel);
  Pos3 getRotVel();

  virtual bool init(ID3D11Device *dev, ID3D11DeviceContext *devcon);

  // General purpose update. VisualModels and PhysicsModels are handled by their respective managers separately.
  virtual bool update(ID3D11Device *dev, ID3D11DeviceContext *devcon, float timeMs, InputApi &input);

  // Derived classes should implement their own if they had any special memory allocations.
  virtual bool release();

  void setVModel(VisualModel* pVModel);
  VisualModel* getVModel();
  void setPModel(PhysicsModel* pPModel);
  PhysicsModel* getPModel();

  GameObjectType getType();

  uint64_t getUuid();

  void handleCollision(PmModelStorage* pOtherObjStorage);
};

#endif