#pragma once

#include <windows.h>
#include <vector>
#include "framework.h"
#include "camera.h"
#include "input.h"
//#include "Scene.h"
#include "Shape.h"

struct Vertex {
    float x, y, z;
    float u, v;
};

struct WorldMatrixBuffer {
    XMMATRIX worldMatrix;
};

struct ViewMatrixBuffer {
    XMMATRIX viewProjectionMatrix;
};

struct SkyboxVertex {
    float x, y, z;
};

struct SkyboxWorldMatrixBuffer {
    XMMATRIX worldMatrix;
    XMFLOAT4 size;
};

struct SkyboxViewMatrixBuffer {
    XMMATRIX viewProjectionMatrix;
    XMFLOAT4 cameraPos;
};

class Renderer
{
public:
    static Renderer& GetInstance();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;

    ~Renderer();
    void CleanAll();

    bool Init(HINSTANCE hInstance, const HWND hWnd);
    bool Render();
    bool Resize(const unsigned width, const unsigned height);

    static constexpr const unsigned defaultWidth = 640;
    static constexpr const unsigned defaultHeight = 360;

private:
    Renderer();

    //Scene* scene = nullptr;
    Cube cube_;

    HRESULT InitScene();
    void InputHandler();
    bool UpdateScene();

    ID3D11Device* pDevice_ = NULL;
    ID3D11DeviceContext* pDeviceContext_ = NULL;
    IDXGISwapChain* pSwapChain_ =  NULL;
    ID3D11RenderTargetView* pRenderTargetView_ = NULL;

    ID3D11Buffer* pVertexBuffer_[2] = {NULL, NULL};
    ID3D11Buffer* pIndexBuffer_[2] = { NULL, NULL };
    ID3D11InputLayout* pInputLayout_[2] = { NULL, NULL };
    ID3D11VertexShader* pVertexShader_[2] = { NULL, NULL };
    ID3D11PixelShader* pPixelShader_[2] = { NULL, NULL };

    ID3D11Buffer* pWorldMatrixBuffer_[2] = { NULL, NULL };
    ID3D11Buffer* pViewMatrixBuffer_[2] = { NULL, NULL };
    ID3D11RasterizerState* pRasterizerState_ = NULL;
    ID3D11SamplerState* pSampler_ = NULL;

    ID3D11ShaderResourceView* pTexture_[2] = { NULL, NULL };

    Camera* pCamera_;
    Input* pInput_;

    UINT width_;
    UINT height_;
    UINT numSphereTriangles_;
    float radius_;
};
