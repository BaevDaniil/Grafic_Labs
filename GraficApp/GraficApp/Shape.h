#pragma once

#include "framework.h"
#include "camera.h"

#include <vector>

class Shape
{
public:
    virtual HRESULT createGeometry(ID3D11Device* m_pDevice) = 0;
    virtual HRESULT createShaders(ID3D11Device* m_pDevice) = 0;
    virtual HRESULT createTextures(ID3D11Device* m_pDevice) = 0;

    virtual void update(ID3D11DeviceContext* m_pDeviceContext) = 0;
    virtual void draw(ID3D11Buffer* pViewMatrixBuffer, ID3D11DeviceContext* pDeviceContext) = 0;

    virtual HRESULT setRasterizerState(ID3D11Device* m_pDevice, D3D11_CULL_MODE cullMode);

    void translate(DirectX::XMMATRIX translateMatrix);
    void scale(DirectX::XMMATRIX scaleMatrix);
    void rotate(DirectX::XMMATRIX rotateMatrix);
protected:
    ID3D11Buffer* pVertexBuffer_ = NULL;
    ID3D11Buffer* pIndexBuffer_ = NULL;
    ID3D11InputLayout* pInputLayout_ = NULL;
    ID3D11VertexShader* pVertexShader_ = NULL;
    ID3D11RasterizerState* pRasterizerState_;
    ID3D11PixelShader* pPixelShader_ = NULL;

    ID3D11RasterizerState* rasterizerState;

    std::vector<ID3D11ShaderResourceView*> resources;

    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX translateMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    DirectX::XMMATRIX rotateMatrix = DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.0f);
};


class Cube: public Shape
{
    struct WorldMatrixBuffer
    {
	 DirectX::XMMATRIX worldMatrix;
    };

    struct Vertex
    {
	 float x, y, z;
	 float u, v;
    };
public:
    HRESULT createGeometry(ID3D11Device* m_pDevice) final;
    HRESULT createShaders(ID3D11Device* m_pDevice) final;
    HRESULT createTextures(ID3D11Device* m_pDevice) final;
    void update(ID3D11DeviceContext* m_pDeviceContext) final;
    void draw(ID3D11Buffer* pViewMatrixBuffer, ID3D11DeviceContext* pDeviceContext) final;
private:
    ID3D11Buffer* pWorldMatrixBuffer_;
};