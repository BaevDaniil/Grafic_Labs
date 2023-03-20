﻿#include "renderer.h"

Renderer& Renderer::GetInstance() {
    static Renderer instance;
    return instance;
}

Renderer::Renderer() :
    pDevice_(NULL),
    pDeviceContext_(NULL),
    pSwapChain_(NULL),
    pRenderTargetView_(NULL),
    pRasterizerState_(NULL),
    pSampler_(NULL),
    pCamera_(NULL),
    pInput_(NULL),
    pDepthBuffer_(NULL),
    pDepthBufferDSV_(NULL),
    pBlendState_(NULL),
    width_(defaultWidth),
    height_(defaultHeight),
    numSphereTriangles_(0),
    radius_(1.0)
{
}

Renderer::~Renderer()
{
    CleanAll();
}

void Renderer::CleanAll()
{
    if (NULL != pDeviceContext_)
        pDeviceContext_->ClearState();

    SafeRelease(pRenderTargetView_);
    SafeRelease(pDeviceContext_);
    SafeRelease(pSwapChain_);
    SafeRelease(pRasterizerState_);
    SafeRelease(pSampler_);
    SafeRelease(pDepthBuffer_);
    SafeRelease(pDepthBufferDSV_);
    SafeRelease(pBlendState_);

    /*SafeRelease(pVertexBuffer_[0]);
    SafeRelease(pVertexBuffer_[1]);
    SafeRelease(pVertexBuffer_[2]);

    SafeRelease(pIndexBuffer_[0]);
    SafeRelease(pIndexBuffer_[1]);
    SafeRelease(pIndexBuffer_[2]);

    SafeRelease(pInputLayout_[0]);
    SafeRelease(pInputLayout_[1]);
    SafeRelease(pInputLayout_[2]);

    SafeRelease(pVertexShader_[0]);
    SafeRelease(pVertexShader_[1]);
    SafeRelease(pVertexShader_[2]);

    SafeRelease(pPixelShader_[0]);
    SafeRelease(pPixelShader_[1]);
    SafeRelease(pPixelShader_[2]);

    SafeRelease(pViewMatrixBuffer_[0]);
    SafeRelease(pViewMatrixBuffer_[1]);

    SafeRelease(pWorldMatrixBuffer_[0]);
    SafeRelease(pWorldMatrixBuffer_[1]);
    SafeRelease(pWorldMatrixBuffer_[2]);
    SafeRelease(pWorldMatrixBuffer_[3]);
    SafeRelease(pWorldMatrixBuffer_[4]);

    SafeRelease(pTexture_[0]);
    SafeRelease(pTexture_[1]);*/

    SafeRelease(pDepthState_[0]);
    SafeRelease(pDepthState_[1]);

    if (pCamera_) {
        delete pCamera_;
        pCamera_ = NULL;
    }
    if (pInput_) {
        delete pInput_;
        pInput_ = NULL;
}

#ifdef _DEBUG
    if (pDevice_ != NULL) {
        ID3D11Debug* d3dDebug = NULL;
        pDevice_->QueryInterface(IID_PPV_ARGS(&d3dDebug));

        UINT references = pDevice_->Release();
        pDevice_ = NULL;
        if (references > 1) {
            d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        }
        SafeRelease(d3dDebug);
    }
#endif
    SafeRelease(pDevice_);
}

bool Renderer::Init(HINSTANCE hInstance, const HWND hWnd)
{
    // Create a DirectX graphics interface factory.​
    IDXGIFactory* pFactory = nullptr;
    HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
    // Select hardware adapter
    IDXGIAdapter* pSelectedAdapter = NULL;
    if (SUCCEEDED(result)) {
        IDXGIAdapter* pAdapter = NULL;
        UINT adapterIdx = 0;
        while (SUCCEEDED(pFactory->EnumAdapters(adapterIdx, &pAdapter))) {
            DXGI_ADAPTER_DESC desc;
            pAdapter->GetDesc(&desc);
            if (wcscmp(desc.Description, L"Microsoft Basic Render Driver")) {
                pSelectedAdapter = pAdapter;
                break;
            }
            pAdapter->Release();
            adapterIdx++;
        }
    }
    if (pSelectedAdapter == NULL) {
        SafeRelease(pFactory);
        return false;
    }

    // Create DirectX11 device
    D3D_FEATURE_LEVEL level;
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG
    result = D3D11CreateDevice(
        pSelectedAdapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        NULL,
        flags,
        levels,
        1,
        D3D11_SDK_VERSION,
        &pDevice_,
        &level,
        &pDeviceContext_
    );
    if (D3D_FEATURE_LEVEL_11_0 != level || !SUCCEEDED(result)) {
        SafeRelease(pFactory);
        SafeRelease(pSelectedAdapter);
        CleanAll();
        return false;
    }

    // Create swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = defaultWidth;
    swapChainDesc.BufferDesc.Height = defaultHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = 0;
    result = pFactory->CreateSwapChain(pDevice_, &swapChainDesc, &pSwapChain_);

    ID3D11Texture2D* pBackBuffer = NULL;
    if (SUCCEEDED(result)) {
        result = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    }
    if (SUCCEEDED(result)) {
        result = pDevice_->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView_);
    }
    if (SUCCEEDED(result)) {
        result = InitScene();
    }
    SafeRelease(pFactory);
    SafeRelease(pSelectedAdapter);
    SafeRelease(pBackBuffer);
    if (SUCCEEDED(result)) {
        pCamera_ = new Camera;
        if (!pCamera_) {
            result = S_FALSE;
        }
    }
    if (SUCCEEDED(result)) {
        pInput_ = new Input;
        if (!pInput_) {
            result = S_FALSE;
        }
    }
    if (SUCCEEDED(result)) {
        result = pInput_->Init(hInstance, hWnd);
    }
    if (FAILED(result)) {
        CleanAll();
    }

    return SUCCEEDED(result);
}

HRESULT Renderer::InitScene() {
    HRESULT result = S_OK;

    for (int i = 0; i <= 3; ++i) {
        Cube* cube = new Cube;

        if (SUCCEEDED(result))
            result = cube->createGeometry(pDevice_);
        if (SUCCEEDED(result))
            result = cube->createShaders(pDevice_);
        if (SUCCEEDED(result))
            result = cube->setRasterizerState(pDevice_, D3D11_CULL_BACK);
        if (SUCCEEDED(result))
            cube->createTextures(pDevice_);
        if (SUCCEEDED(result))
            shapes_.push_back(cube);
    }
    //shapes_[0]->translate(DirectX::XMMatrixTranslation(4.0f, 0.0f, -5.0f));
    shapes_[1]->translate(DirectX::XMMatrixTranslation(4.0f, 0.0f, 0.0f));
    //shapes_[2]->translate(DirectX::XMMatrixTranslation(4.0f, 0.0f, 5.0f));
    //shapes_[3]->translate(DirectX::XMMatrixTranslation(10.0f, 0.0f, 0.0f));
    shapes_[3]->scale(DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f));

    for (int i = 0; i < 2; ++i) {
        Rect* rect = new Rect;

        if (SUCCEEDED(result))
            result = rect->createGeometry(pDevice_);
        if (SUCCEEDED(result))
            result = rect->createShaders(pDevice_);
        if (SUCCEEDED(result))
            result = rect->setRasterizerState(pDevice_, D3D11_CULL_NONE);
        if (SUCCEEDED(result))
            shapes_.push_back(rect);
    }
    shapes_[4]->translate(DirectX::XMMatrixTranslation(10.0f, 0.0f, 0.0f));
    shapes_[5]->translate(DirectX::XMMatrixTranslation(6.0f, 0.0f, 0.0f));
    shapes_[4]->scale(DirectX::XMMatrixScaling(0.0f, 3.0f, 2.0f));
    shapes_[5]->scale(DirectX::XMMatrixScaling(0.0f, 3.0f, 2.0f));

    if (SUCCEEDED(result))
        skybox_.createGeometry(pDevice_);
    if (SUCCEEDED(result))
        skybox_.createShaders(pDevice_);
    if (SUCCEEDED(result))
        skybox_.setRasterizerState(pDevice_, D3D11_CULL_BACK);
    if (SUCCEEDED(result))
        skybox_.createTextures(pDevice_);



    if (SUCCEEDED(result)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(WorldMatrixBuffer);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        WorldMatrixBuffer worldMatrixBuffer;
        worldMatrixBuffer.worldMatrix = DirectX::XMMatrixIdentity();

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &worldMatrixBuffer;
        data.SysMemPitch = sizeof(worldMatrixBuffer);
        data.SysMemSlicePitch = 0;

        desc = {};
        desc.ByteWidth = sizeof(ViewMatrixBuffer);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        result = pDevice_->CreateBuffer(&desc, nullptr, &pViewMatrixBuffer_);
    }
    if (SUCCEEDED(result)) {
        D3D11_SAMPLER_DESC desc = {};

        desc.Filter = D3D11_FILTER_ANISOTROPIC;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MinLOD = -D3D11_FLOAT32_MAX;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 1.0f;

        result = pDevice_->CreateSamplerState(&desc, &pSampler_);
    }
    if (SUCCEEDED(result)) {
        D3D11_DEPTH_STENCIL_DESC dsDesc = { };
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
        dsDesc.StencilEnable = FALSE;

        result = pDevice_->CreateDepthStencilState(&dsDesc, &pDepthState_[0]);
    }
    if (SUCCEEDED(result)) {
        D3D11_DEPTH_STENCIL_DESC dsDesc = { };
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
        dsDesc.StencilEnable = FALSE;

        result = pDevice_->CreateDepthStencilState(&dsDesc, &pDepthState_[1]);
    }
    if (SUCCEEDED(result)) {
        D3D11_BLEND_DESC desc = { 0 };
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED |
            D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;

        result = pDevice_->CreateBlendState(&desc, &pBlendState_);
    }

    dynamic_cast<Rect*>(shapes_[4])->SetColor(RGB(0, 0, 255), pDeviceContext_);
    return result;
}

void Renderer::InputHandler() {
    XMFLOAT3 mouse = pInput_->ReadMouse();
    pCamera_->Rotate(mouse.x / 200.0f, mouse.y / 200.0f);
    pCamera_->Zoom(-mouse.z / 100.0f);

    unsigned char* keyboard = pInput_->ReadKeyboard();
    if (nullptr == keyboard)
        return;
    float di = 0.0, dj = 0.0, dz = 0.0;

    if (keyboard[DIK_UP] || keyboard[DIK_W])
        dj += 0.05f;
    if (keyboard[DIK_DOWN] || keyboard[DIK_S])
        dj -= 0.05f;
    if (keyboard[DIK_LEFT] || keyboard[DIK_A])
        di += 0.05f;
    if (keyboard[DIK_RIGHT] || keyboard[DIK_D])
        di -= 0.05f;
    if (keyboard[DIK_LCONTROL])
        dz -= 0.05f;
    if (keyboard[DIK_LSHIFT])
        dz += 0.05f;

    pCamera_->Move(di, dj, dz);
}

bool Renderer::UpdateScene() {
    HRESULT result;

    InputHandler();

    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0) {
        timeStart = timeCur;
    }
    t = (timeCur - timeStart) / 1000.0f;

    XMMATRIX mView = pCamera_->GetViewMatrix();

    XMMATRIX mProjection = XMMatrixPerspectiveFovLH(XM_PI / 3, width_ / (FLOAT)height_, 100.0f, 0.01f);

    D3D11_MAPPED_SUBRESOURCE subresource;
    result = pDeviceContext_->Map(pViewMatrixBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    if (SUCCEEDED(result)) {
        ViewMatrixBuffer& sceneBuffer = *reinterpret_cast<ViewMatrixBuffer*>(subresource.pData);
        sceneBuffer.viewProjectionMatrix = XMMatrixMultiply(mView, mProjection);
        pDeviceContext_->Unmap(pViewMatrixBuffer_, 0);
    }

    shapes_[3]->translate(DirectX::XMMatrixTranslation(0.0f, sinf(t) * 12.0f, cosf(t) * 12.0f));
    shapes_[0]->translate(DirectX::XMMatrixTranslation(sinf(t) * 4.0f + 4.0f, 0.0f, cosf(t) * 4.0f));
    shapes_[2]->translate(DirectX::XMMatrixTranslation(sinf(t) * -4.0f + 4.0f, 0.0f, cosf(t) * -4.0f));

    for (int i = 0; i < 3; ++i) {
        shapes_[i]->rotate(XMMatrixRotationY(t));
    }
    for (Shape* shape : shapes_) {
        shape->update(pDeviceContext_);
    }
    
    result = skybox_.update(pDeviceContext_, pCamera_, mProjection);

    return SUCCEEDED(result);
}

bool Renderer::Render()
{
    if (!UpdateScene())
        return false;

    pDeviceContext_->ClearState();

    ID3D11RenderTargetView* views[] = { pRenderTargetView_ };
    pDeviceContext_->OMSetRenderTargets(1, views, pDepthBufferDSV_);

    static const FLOAT backColor[4] = { 0.4f, 0.2f, 0.4f, 1.0f };
    pDeviceContext_->ClearRenderTargetView(pRenderTargetView_, backColor);
    pDeviceContext_->ClearDepthStencilView(pDepthBufferDSV_, D3D11_CLEAR_DEPTH, 0.0f, 0);

    pDeviceContext_->OMSetBlendState(pBlendState_, nullptr, 0xFFFFFFFF);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)width_;
    viewport.Height = (FLOAT)height_;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    pDeviceContext_->RSSetViewports(1, &viewport);

    D3D11_RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = width_;
    rect.bottom = height_;
    pDeviceContext_->RSSetScissorRects(1, &rect);

    pDeviceContext_->RSSetState(pRasterizerState_);
    pDeviceContext_->OMSetDepthStencilState(pDepthState_[0], 0);

    ID3D11SamplerState* samplers[] = { pSampler_ };
    pDeviceContext_->PSSetSamplers(0, 1, samplers);
    XMMATRIX mView = pCamera_->GetViewMatrix();
    XMMATRIX mProjection = XMMatrixPerspectiveFovLH(XM_PI / 4, width_ / (FLOAT)height_, 0.01f, 100.0f);

    for (Shape* shape : shapes_)
        shape->draw(pViewMatrixBuffer_, pDeviceContext_);


    pDeviceContext_->OMSetDepthStencilState(pDepthState_[1], 0);

    skybox_.draw(pDeviceContext_);
    
    HRESULT result = pSwapChain_->Present(0, 0);
    

    return SUCCEEDED(result);
}

bool Renderer::Resize(const unsigned width, const unsigned height)
{
    if (pSwapChain_ == NULL)
        return false;

    SafeRelease(pRenderTargetView_);

    width_ = max(width, 8);
    height_ = max(height, 8);

    auto result = pSwapChain_->ResizeBuffers(2, width_, height_, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (!SUCCEEDED(result))
        return false;

    ID3D11Texture2D* pBuffer;
    result = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBuffer);
    if (!SUCCEEDED(result))
        return false;

    result = pDevice_->CreateRenderTargetView(pBuffer, NULL, &pRenderTargetView_);
    SafeRelease(pBuffer);
    if (!SUCCEEDED(result))
        return false;

    SafeRelease(pDepthBuffer_);
    SafeRelease(pDepthBufferDSV_);
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = DXGI_FORMAT_D32_FLOAT;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Height = height_;
    desc.Width = width_;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    result = pDevice_->CreateTexture2D(&desc, NULL, &pDepthBuffer_);
    if (!SUCCEEDED(result))
        return false;

    result = pDevice_->CreateDepthStencilView(pDepthBuffer_, NULL, &pDepthBufferDSV_);
    if (!SUCCEEDED(result))
        return false;

    float n = 0.01f;
    float fov = XM_PI / 3;
    float halfW = tanf(fov / 2) * n;
    float halfH = height_ / float(width_) * halfW;
    radius_ = sqrtf(n * n + halfH * halfH + halfW * halfW) * 1.1f;

    skybox_.setRadius(radius_);

    return true;
}
