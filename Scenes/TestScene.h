#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "../Engine/Scene.h"

class TestScene : public Scene
{
public:
  TestScene();
  bool init(ID3D11Device *dev, ID3D11DeviceContext *devcon);
  bool update(ID3D11Device *dev, ID3D11DeviceContext *devcon, SceneIo &sceneIo);
};

#endif