#include "GraphicsManager.h"
#include "Util.h"
#include "Logger.h"
#include "TexPoly.h"
#include "TexRect.h"
#include "TexText.h"

GraphicsManager::GraphicsManager()
{
  D3DXMatrixIdentity(&m_worldMat);
  D3DXMatrixIdentity(&m_viewMat);
  D3DXMatrixIdentity(&m_projMat);
  D3DXMatrixIdentity(&m_totMat);
  m_pConstBuffer = NULL;
  memset(&m_vsConstData, 0, sizeof(m_vsConstData));
}


GraphicsManager::~GraphicsManager()
{
  release();
}


void GraphicsManager::initConstBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
  // Fill in a buffer description.
  D3D11_BUFFER_DESC cbDesc;
  ZeroMemory(&cbDesc, sizeof(cbDesc));
  // If the bind flag is D3D11_BIND_CONSTANT_BUFFER, you must set the ByteWidth value in multiples of 16.
  cbDesc.ByteWidth = sizeof(VS_CONST_BUFFER);
  cbDesc.Usage = D3D11_USAGE_DYNAMIC;
  cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  HRESULT err = dev->CreateBuffer(&cbDesc, NULL, &m_pConstBuffer); // Create the buffer.

  // Set the buffer. Only need to do once per program per buffer.
  devcon->VSSetConstantBuffers(0, 1, &m_pConstBuffer);
}


void GraphicsManager::setPosAndRot(Pos3 &pos, Pos3 &pitchYawRoll)
{
  // https://www.gamedev.net/forums/topic/682063-vector-and-matrix-multiplication-order-in-directx-and-opengl/

  // https://msdn.microsoft.com/en-us/library/windows/desktop/bb206365(v=vs.85).aspx
  D3DXMATRIX MatTemp;  // Temp matrix for rotations.
  D3DXMATRIX MatRot;   // Final rotation matrix, applied to 
                       // pMatWorld.

                       // Using the left-to-right order of matrix concatenation,
                       // apply the translation to the object's world position
                       // before applying the rotations.
  D3DXMatrixTranslation(&m_worldMat, pos.pos.x, pos.pos.y, pos.pos.z);
  D3DXMatrixIdentity(&MatRot);

  // Now, apply the orientation variables to the world matrix
  if (pitchYawRoll.pos.x || pitchYawRoll.pos.y || pitchYawRoll.pos.z) {
    // Produce and combine the rotation matrices.
    D3DXMatrixRotationX(&MatTemp, pitchYawRoll.pos.x);         // Pitch
    D3DXMatrixMultiply(&MatRot, &MatRot, &MatTemp);
    D3DXMatrixRotationY(&MatTemp, pitchYawRoll.pos.y);         // Yaw
    D3DXMatrixMultiply(&MatRot, &MatRot, &MatTemp);
    D3DXMatrixRotationZ(&MatTemp, pitchYawRoll.pos.z);         // Roll
    D3DXMatrixMultiply(&MatRot, &MatRot, &MatTemp);

    // Apply the rotation matrices to complete the world matrix.
    D3DXMatrixMultiply(&m_worldMat, &MatRot, &m_worldMat);
  }
}


void GraphicsManager::setCamera(Pos3 &eye, Pos3 &lookAt, Pos3 &up)
{
  D3DXMatrixLookAtRH(&m_viewMat, &eye.pos, &lookAt.pos, &up.pos);
}


void GraphicsManager::resetCamera()
{
  D3DXMatrixIdentity(&m_viewMat);
}


void GraphicsManager::setPerspective(float fovy, float aspect, float nearDist, float farDist)
{
  D3DXMatrixPerspectiveFovRH(&m_projMat, fovy, aspect, nearDist, farDist);
}


void GraphicsManager::renderModel(VisualModel *pModel, ID3D11Device *dev, ID3D11DeviceContext *devcon)
{
  if (!pModel)
  {
    return;
  }

  D3DXMATRIX tempMat;

  // View Matrix can be skipped for objects that should have static locations on the screen, ex. text overlays.
  if (pModel->getStaticScreenLoc())
  {
    D3DXMatrixMultiply(&m_totMat, &m_worldMat, &m_projMat);
  }
  else
  {
    D3DXMatrixMultiply(&tempMat, &m_worldMat, &m_viewMat);
    D3DXMatrixMultiply(&m_totMat, &tempMat, &m_projMat);
  }

  // Could directly target m_vsConstData.mat with final D3DXMatrixMultiply above.
  memcpy(&m_vsConstData.mat, &m_totMat, sizeof(m_totMat));

  // Need to program buffer values and map them every time they change.
  D3D11_MAPPED_SUBRESOURCE resource;
  devcon->Map(m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
  memcpy(resource.pData, &m_vsConstData, sizeof(m_vsConstData));
  devcon->Unmap(m_pConstBuffer, 0);

  VisualModel::renderVModel(pModel, dev, devcon);
}


void GraphicsManager::release()
{
  RELEASE_NON_NULL(m_pConstBuffer);
}
