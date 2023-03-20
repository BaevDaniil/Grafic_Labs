#pragma once

#include "framework.h"
#include "camera.h"

#include <vector>

class Shape
{
protected:
    struct WorldMatrixBuffer
    {
        DirectX::XMMATRIX worldMatrix;
    };
public:
    Shape():
        pVertexBuffer_(nullptr),
        pIndexBuffer_(nullptr),
        pInputLayout_(nullptr),
        pVertexShader_(nullptr),
        pRasterizerState_(nullptr),
        pPixelShader_(nullptr),
        rasterizerState_(nullptr)
    {};

    Shape(const Shape&) = delete;
    Shape(const Shape&&) = delete;

    ~Shape() 
    {
        SafeRelease(pVertexBuffer_);
        SafeRelease(pIndexBuffer_);
        SafeRelease(pInputLayout_);
        SafeRelease(pVertexShader_);
        SafeRelease(pRasterizerState_);
        SafeRelease(pPixelShader_);
        SafeRelease(rasterizerState_);

        for (auto resource : resources_)
            SafeRelease(resource);
    }

    virtual HRESULT createGeometry(ID3D11Device* m_pDevice) = 0;
    virtual HRESULT createShaders(ID3D11Device* m_pDevice) = 0;
    virtual HRESULT createTextures(ID3D11Device* m_pDevice) = 0;

    virtual HRESULT update(ID3D11DeviceContext* m_pDeviceContext) = 0;
    virtual void draw(ID3D11Buffer* pViewMatrixBuffer, ID3D11DeviceContext* pDeviceContext) = 0;

    virtual HRESULT setRasterizerState(ID3D11Device* m_pDevice, D3D11_CULL_MODE cullMode);

    void translate(DirectX::XMMATRIX translateMatrix);
    void scale(DirectX::XMMATRIX scaleMatrix);
    void rotate(DirectX::XMMATRIX rotateMatrix);
protected:
    ID3D11Buffer* pVertexBuffer_;
    ID3D11Buffer* pIndexBuffer_;
    ID3D11InputLayout* pInputLayout_;
    ID3D11VertexShader* pVertexShader_;
    ID3D11RasterizerState* pRasterizerState_;
    ID3D11PixelShader* pPixelShader_;

    ID3D11RasterizerState* rasterizerState_;

    std::vector<ID3D11ShaderResourceView*> resources_;

    DirectX::XMMATRIX worldMatrix_ = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX translateMatrix_ = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    DirectX::XMMATRIX scaleMatrix_ = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    DirectX::XMMATRIX rotateMatrix_ = DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.0f);
};


class Cube: public Shape
{
    struct Vertex
    {
        float x, y, z;
        float u, v;
    };
public:
    Cube():
        pWorldMatrixBuffer_(nullptr)
    {};

    Cube(const Cube&) = delete;
    Cube(const Cube&&) = delete;

    ~Cube()
    {
        SafeRelease(pWorldMatrixBuffer_);
    }

    HRESULT createGeometry(ID3D11Device* m_pDevice) final;
    HRESULT createShaders(ID3D11Device* m_pDevice) final;
    HRESULT createTextures(ID3D11Device* m_pDevice) final;
    HRESULT update(ID3D11DeviceContext* m_pDeviceContext) final;
    void draw(ID3D11Buffer* pViewMatrixBuffer, ID3D11DeviceContext* pDeviceContext) final;
private:
    ID3D11Buffer* pWorldMatrixBuffer_;
};

class Rect: public Shape 
{
    struct TransparentVertex {
        float x, y, z;
        COLORREF color;
    };
public:
    Rect():
        pWorldMatrixBuffer_(nullptr),
        color_(RGB(255, 0, 0))
    {};

    Rect(const Rect&) = delete;
    Rect(const Rect&&) = delete;

    ~Rect()
    {
        SafeRelease(pWorldMatrixBuffer_);
    }
    HRESULT createGeometry(ID3D11Device* m_pDevice) final;
    HRESULT createShaders(ID3D11Device* m_pDevice) final;
    HRESULT createTextures(ID3D11Device* m_pDevice) final { return S_OK; };
    HRESULT update(ID3D11DeviceContext* m_pDeviceContext) final;
    void draw(ID3D11Buffer* pViewMatrixBuffer, ID3D11DeviceContext* pDeviceContext) final;
    void SetColor(COLORREF color, ID3D11DeviceContext* m_pDeviceContext);
private:
    ID3D11Buffer* pWorldMatrixBuffer_;
    COLORREF color_;
};