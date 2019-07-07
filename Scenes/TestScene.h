#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "../Engine/Scene.h"

class TestScene : public Scene
{
private:
  // Test Scene named Objects
  enum NAMED_OBJECTS
  {
    TSO_PLAYER = 0,
    TSO_CAT_TRIANGLE,
    TSO_FLOOR,
    TSO_CYLINDER,
    TSO_CAT_BOX_1,
    TSO_CAT_BOX_2,
  };

  // Might move to Scene base class eventually...
  uint32_t m_bgSoundHandle;

public:
  TestScene();
  bool init(ID3D11Device *dev, ID3D11DeviceContext *devcon);
  bool update(ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo);
  bool prelimUpdate(ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo);

  void handleCollision(GameObject* obj, PModelOutput *pModelOut);
};

#endif