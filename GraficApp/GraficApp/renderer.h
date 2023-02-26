#pragma once

#include <windows.h>
#include "framework.h"
#include "camera.h"
#include "input.h"

struct Vertex {
    float x, y, z;
    COLORREF color;
};

struct WorldMatrixBuffer {
    XMMATRIX worldMatrix;
};

struct ViewMatrixBuffer {
    XMMATRIX viewProjectionMatrix;
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

    HRESULT InitScene();
    void InputHandler();
    bool UpdateScene();

    ID3D11Device* pDevice_;
    ID3D11DeviceContext* pDeviceContext_;
    IDXGISwapChain* pSwapChain_;
    ID3D11RenderTargetView* pRenderTargetView_;

    ID3D11Buffer* pVertexBuffer_;
    ID3D11Buffer* pIndexBuffer_;
    ID3D11InputLayout* pInputLayout_;
    ID3D11VertexShader* pVertexShader_;
    ID3D11PixelShader* pPixelShader_;

    ID3D11Buffer* pWorldMatrixBuffer_;
    ID3D11Buffer* pViewMatrixBuffer_;
    ID3D11RasterizerState* pRasterizerState_;

    Camera* pCamera_;
    Input* pInput_;

    UINT width_;
    UINT height_;
};
