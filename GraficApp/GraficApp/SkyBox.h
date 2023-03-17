#pragma once

#include "framework.h"
#include "camera.h"
#include <vector>

class SkyBox
{
    struct SkyboxWorldMatrixBuffer {
        XMMATRIX worldMatrix;
        XMFLOAT4 size;
    };

    struct SkyboxViewMatrixBuffer {
        XMMATRIX viewProjectionMatrix;
        XMFLOAT4 cameraPos;
    };

    struct SkyboxVertex {
        float x, y, z;
    };
public:
    HRESULT createGeometry(ID3D11Device* m_pDevice);
    HRESULT createShaders(ID3D11Device* m_pDevice);
    HRESULT createTextures(ID3D11Device* m_pDevice);
    HRESULT setRasterizerState(ID3D11Device* m_pDevice, D3D11_CULL_MODE cullMode);
    HRESULT update(ID3D11DeviceContext* m_pDeviceContext, Camera* pCamera, XMMATRIX mProjection);
    void draw(ID3D11DeviceContext* m_pDeviceContext);


    void setRadius(UINT radius) { radius_ = radius; };
private:
    ID3D11Buffer* pVertexBuffer_ = NULL;
    ID3D11Buffer* pIndexBuffer_ = NULL;
    ID3D11InputLayout* pInputLayout_ = NULL;
    ID3D11VertexShader* pVertexShader_ = NULL;
    ID3D11RasterizerState* pRasterizerState_;
    ID3D11PixelShader* pPixelShader_ = NULL;

    ID3D11RasterizerState* rasterizerState;

    std::vector<ID3D11Buffer*> constBuffers;
    std::vector<ID3D11SamplerState*> samplers;
    std::vector<ID3D11ShaderResourceView*> resources;

    ID3D11Buffer* pWorldMatrixBuffer_;
    ID3D11Buffer* pViewMatrixBuffer_;
    //SkyboxWorldMatrixBuffer* pWorldMatrixBuffer_;

    UINT numSphereTriangles_;
    float radius_ = 1.0f;
};

