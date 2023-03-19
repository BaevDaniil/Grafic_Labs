#include "Shape.h"

HRESULT Cube::createGeometry(ID3D11Device* pDevice)
{
    static const Vertex Vertices[] = {
        {-1.0, -1.0,  1.0, 0, 1},
        { 1.0, -1.0,  1.0, 1, 1},
        { 1.0, -1.0, -1.0, 1, 0},
        {-1.0, -1.0, -1.0, 0, 0},

        {-1.0,  1.0, -1.0, 0, 1},
        { 1.0,  1.0, -1.0, 1, 1},
        { 1.0,  1.0,  1.0, 1, 0},
        {-1.0,  1.0,  1.0, 0, 0},

        { 1.0, -1.0, -1.0, 0, 1},
        { 1.0, -1.0,  1.0, 1, 1},
        { 1.0,  1.0,  1.0, 1, 0},
        { 1.0,  1.0, -1.0, 0, 0},

        {-1.0, -1.0,  1.0, 0, 1},
        {-1.0, -1.0, -1.0, 1, 1},
        {-1.0,  1.0, -1.0, 1, 0},
        {-1.0,  1.0,  1.0, 0, 0},

        { 1.0, -1.0,  1.0, 0, 1},
        {-1.0, -1.0,  1.0, 1, 1},
        {-1.0,  1.0,  1.0, 1, 0},
        { 1.0,  1.0,  1.0, 0, 0},

        {-1.0, -1.0, -1.0, 0, 1},
        { 1.0, -1.0, -1.0, 1, 1},
        { 1.0,  1.0, -1.0, 1, 0},
        {-1.0,  1.0, -1.0, 0, 0}
    };
    static const USHORT Indices[] = {
        0, 2, 1, 0, 3, 2,
        4, 6, 5, 4, 7, 6,
        8, 10, 9, 8, 11, 10,
        12, 14, 13, 12, 15, 14,
        16, 18, 17, 16, 19, 18,
        20, 22, 21, 20, 23, 22
    };

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(Vertices);
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &Vertices;
    data.SysMemPitch = sizeof(Vertices);
    data.SysMemSlicePitch = 0;

    HRESULT hr = pDevice->CreateBuffer(&desc, &data, &pVertexBuffer_);

    if (SUCCEEDED(hr)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(Indices);
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &Indices;
        data.SysMemPitch = sizeof(Indices);
        data.SysMemSlicePitch = 0;

        hr = pDevice->CreateBuffer(&desc, &data, &pIndexBuffer_);
    }

    desc = {};
    desc.ByteWidth = sizeof(WorldMatrixBuffer);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    WorldMatrixBuffer worldMatrixBuffer;
    worldMatrixBuffer.worldMatrix = DirectX::XMMatrixIdentity();

    data = {};
    data.pSysMem = &worldMatrixBuffer;
    data.SysMemPitch = sizeof(worldMatrixBuffer);
    data.SysMemSlicePitch = 0;

    hr = pDevice->CreateBuffer(&desc, &data, &pWorldMatrixBuffer_);

    return hr;
}

HRESULT Cube::createShaders(ID3D11Device* pDevice) {
    static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;

    HRESULT result = S_OK;

    int flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    if (SUCCEEDED(result)) {
        result = D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, NULL);
        if (SUCCEEDED(result)) {
            result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &pVertexShader_);
        }
    }
    if (SUCCEEDED(result)) {
        result = D3DCompileFromFile(L"PixelShader.hlsl", NULL, NULL, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, NULL);
        if (SUCCEEDED(result)) {
            result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pPixelShader_);
        }
    }
    if (SUCCEEDED(result)) {
        int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
        result = pDevice->CreateInputLayout(InputDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &pInputLayout_);
    }

    SafeRelease(vertexShaderBuffer);
    SafeRelease(pixelShaderBuffer);

    return result;
}

HRESULT Cube::createTextures(ID3D11Device* pDevice) {
    ID3D11ShaderResourceView* m_pTextureView;

    HRESULT result = DirectX::CreateDDSTextureFromFile(pDevice, L"textures/metal.dds", nullptr, &m_pTextureView);

    if (SUCCEEDED(result))
    {
        resources.push_back(m_pTextureView);
    }

    return result;
}

void Cube::update(ID3D11DeviceContext* m_pDeviceContext) {
    worldMatrix = DirectX::XMMatrixIdentity();
    worldMatrix = rotateMatrix * scaleMatrix * translateMatrix;
    WorldMatrixBuffer worldMatrixBuffer;
    worldMatrixBuffer.worldMatrix = worldMatrix;

    m_pDeviceContext->UpdateSubresource(pWorldMatrixBuffer_, 0, nullptr, &worldMatrixBuffer, 0, 0);
}

void Cube::draw(ID3D11Buffer* pViewMatrixBuffer, ID3D11DeviceContext* pDeviceContext)
{
    pDeviceContext->RSSetState(rasterizerState);
    pDeviceContext->PSSetShaderResources(0, 1, resources.data());

    pDeviceContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);
    ID3D11Buffer* vertexBuffers[] = { pVertexBuffer_ };
    UINT strides[] = { 20 };
    UINT offsets[] = { 0 };
    pDeviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    pDeviceContext->IASetInputLayout(pInputLayout_);
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pDeviceContext->VSSetConstantBuffers(0, 1, &pWorldMatrixBuffer_);
    pDeviceContext->VSSetConstantBuffers(1, 1, &pViewMatrixBuffer);
    pDeviceContext->VSSetShader(pVertexShader_, nullptr, 0);
    pDeviceContext->PSSetShader(pPixelShader_, nullptr, 0);
    pDeviceContext->DrawIndexed(36, 0, 0);
}


HRESULT Shape::setRasterizerState(ID3D11Device* m_pDevice, D3D11_CULL_MODE cullMode)
{
    HRESULT result;
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

    rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = cullMode;
    result = m_pDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

    return result;
}

void Shape::translate(DirectX::XMMATRIX translateMatrix)
{
    this->translateMatrix = translateMatrix;
}

void Shape::scale(DirectX::XMMATRIX scaleMatrix)
{
    this->scaleMatrix = scaleMatrix;
}

void Shape::rotate(DirectX::XMMATRIX rotateMatrix)
{
    this->rotateMatrix = rotateMatrix;
}
