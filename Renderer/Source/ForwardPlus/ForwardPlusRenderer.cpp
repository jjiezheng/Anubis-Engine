#include "Renderer_pch.h"
#include "ForwardPlusRenderer.h"

#include "Final\Resources\ResourceData.h"
#include "Final\General.h"

#include "Resource.h"
#include "ResourceCache.h"
#include "../Resources/MeshResource.h"

using namespace Anubis;

ForwardPlusRenderer::ForwardPlusRenderer()
{
	m_pZPrepassShaders = new ShaderBunch();
	m_pLightCullingShader = new ComputeShader();
	m_pFinalShadingShaders = new ShaderBunch();

	m_pLinearZTexture = new Texture2D();
	m_pLinearZSRV = new ShaderResourceView();
	m_pLinearZRTV = new RenderTargetView();

	m_psbLightIdx = new StructuredBuffer();
	m_pLightIdxUAV = new UnorderedAccessView();
	m_pLightIdxSRV = new ShaderResourceView();

	m_psbLowerBound = new StructuredBuffer();
	m_pLowerBoundUAV = new UnorderedAccessView();
	m_pLowerBoundSRV = new ShaderResourceView();

	m_psbHigherBound = new StructuredBuffer();
	m_pHigherBoundUAV = new UnorderedAccessView();
	m_pHigherBoundSRV = new ShaderResourceView();

	m_psbLightCounter = new StructuredBuffer();
	m_pLightCounterUAV = new UnorderedAccessView();
	m_pLightCounterSRV = new ShaderResourceView();

	m_psbLightGeometry = new StructuredBuffer();
	m_pLightGeometrySRV = new ShaderResourceView();

	m_psbLights = new StructuredBuffer();
	m_pLightsSRV = new ShaderResourceView();

	m_pcbCameraCulling = new ConstantBuffer();

	m_pSkyTexture = new TextureCube();
	m_pSkySRV = new ShaderResourceView();
	m_pSkyShaders = new ShaderBunch();
	m_pAtmoShaders = new ShaderBunch();

	m_pSphericalCoeffShader = new ComputeShader();
	m_pIrradianceMapShader = new ComputeShader();
	m_pIrradianceMap = new TextureCube();
	m_pIrradianceSRV = new ShaderResourceView();
	//m_pIrradianceRTV = new RenderTargetView();
}

ForwardPlusRenderer::~ForwardPlusRenderer()
{
	SAFE_DELETE(m_pFinalShadingShaders);
	SAFE_DELETE(m_pLightCullingShader);
	SAFE_DELETE(m_pZPrepassShaders);

	SAFE_DELETE_ARRAY(m_pDefaultLayout);
	SAFE_DELETE_ARRAY(m_pZPrepassLayout);

	SAFE_DELETE(m_pLinearZRTV);
	SAFE_DELETE(m_pLinearZSRV);
	SAFE_DELETE(m_pLinearZTexture);

	SAFE_DELETE(m_pLightIdxUAV);
	SAFE_DELETE(m_pLightIdxSRV);
	SAFE_DELETE(m_psbLightIdx);	

	SAFE_DELETE(m_pLowerBoundUAV);
	SAFE_DELETE(m_pLowerBoundSRV);
	SAFE_DELETE(m_psbLowerBound);

	SAFE_DELETE(m_pHigherBoundUAV);
	SAFE_DELETE(m_pHigherBoundSRV);
	SAFE_DELETE(m_psbHigherBound);

	SAFE_DELETE(m_pLightCounterUAV);
	SAFE_DELETE(m_pLightCounterSRV);
	SAFE_DELETE(m_psbLightCounter);
	
	SAFE_DELETE(m_pLightGeometrySRV);
	SAFE_DELETE(m_psbLightGeometry);

	SAFE_DELETE(m_pLightsSRV);
	SAFE_DELETE(m_psbLights);

	SAFE_DELETE(m_pcbCameraCulling);

	SAFE_DELETE(m_pSkySRV);
	SAFE_DELETE(m_pSkyTexture); 
	SAFE_DELETE(m_pSkyShaders);
	SAFE_DELETE(m_pAtmoShaders);
	SAFE_DELETE(m_pSkyLayout);

	//SAFE_DELETE_ARRAY(m_pIrradianceRTV);
	SAFE_DELETE(m_pIrradianceSRV);
	SAFE_DELETE(m_pIrradianceMap);
	SAFE_DELETE(m_pSphericalCoeffShader);
	SAFE_DELETE(m_pIrradianceMapShader);
}

ABOOL ForwardPlusRenderer::VInitialize(HWND hWnd, AUINT32 width, AUINT32 height)
{
	if (!Renderer::VInitialize(hWnd, width, height))
		{
			assert(0 && "Some problem initializing Renderer occured");
		}

	////////////////////////////////////////////////////
	//Initialize data for z prepass
	////////////////////////////////////////////////////
	Texture2DParams texParams;
	texParams.Init(SCREEN_WIDTH, SCREEN_HEIGHT, 1, TEX_R32G32B32A32_FLOAT, true, false, true, false, 8, 1,
		1, true, false, false);
	m_pLinearZTexture->Create(&texParams);

	ShaderResourceViewParams srvParams;
	srvParams.InitForTexture2D(texParams.Format, 1, 0, true);
	m_pLinearZTexture->CreateShaderResourceView(&m_pLinearZSRV->m_pView, &srvParams);

	RenderTargetViewParams rtvParams;
	rtvParams.InitForTexture2D(texParams.Format, 0, true);
	m_pLinearZTexture->CreateRenderTargetView(&m_pLinearZRTV->m_pView, &rtvParams);

	m_pZPrepassLayout = new INPUT_LAYOUT[1];

	m_pZPrepassLayout[0].SemanticName			= "POSITION";
	m_pZPrepassLayout[0].SemanticIndex			= 0;
	m_pZPrepassLayout[0].Format					= TEX_R32G32B32_FLOAT;
	m_pZPrepassLayout[0].InputSlot				= 0;
	m_pZPrepassLayout[0].AlignedByteOffset		= 0;
	m_pZPrepassLayout[0].InputSlotClass			= IA_PER_VERTEX_DATA;
	m_pZPrepassLayout[0].InstanceDataStepRate	= 0;

	m_pZPrepassShaders->VSetVertexShader(L"ZPrepass.hlsl", "zprepass_vs", m_pZPrepassLayout, 1, TOPOLOGY_TRIANGLELIST, "vs_5_0");
	m_pZPrepassShaders->VSetPixelShader(L"ZPrepass.hlsl", "zprepass_ps", "ps_5_0");

	////////////////////////////////////////
	//compile light culling shader
	BlobDX11* pErrors = new BlobDX11();
	m_pLightCullingShader->CreateAndCompile(L"LightCulling.hlsl", "light_culling_cs", "cs_5_0", pErrors);
	delete pErrors;

	m_pDefaultLayout = new INPUT_LAYOUT[5];

	m_pDefaultLayout[0].SemanticName			= "POSITION";			m_pDefaultLayout[1].SemanticName			= "TEXCOORDS";
	m_pDefaultLayout[0].SemanticIndex			= 0;					m_pDefaultLayout[1].SemanticIndex			= 0;
	m_pDefaultLayout[0].Format					= TEX_R32G32B32_FLOAT;	m_pDefaultLayout[1].Format					= TEX_R32G32_FLOAT;
	m_pDefaultLayout[0].InputSlot				= 0;					m_pDefaultLayout[1].InputSlot				= 1;
	m_pDefaultLayout[0].AlignedByteOffset		= 0;					m_pDefaultLayout[1].AlignedByteOffset		= 0;
	m_pDefaultLayout[0].InputSlotClass			= IA_PER_VERTEX_DATA;	m_pDefaultLayout[1].InputSlotClass			= IA_PER_VERTEX_DATA;
	m_pDefaultLayout[0].InstanceDataStepRate	= 0;					m_pDefaultLayout[1].InstanceDataStepRate	= 0;

	m_pDefaultLayout[2].SemanticName			= "NORMAL";				m_pDefaultLayout[3].SemanticName			= "TANGENT";
	m_pDefaultLayout[2].SemanticIndex			= 0;					m_pDefaultLayout[3].SemanticIndex			= 0;
	m_pDefaultLayout[2].Format					= TEX_R32G32B32_FLOAT;	m_pDefaultLayout[3].Format					= TEX_R32G32B32_FLOAT;
	m_pDefaultLayout[2].InputSlot				= 2;					m_pDefaultLayout[3].InputSlot				= 2;
	m_pDefaultLayout[2].AlignedByteOffset		= 0;					m_pDefaultLayout[3].AlignedByteOffset		= 12;
	m_pDefaultLayout[2].InputSlotClass			= IA_PER_VERTEX_DATA;	m_pDefaultLayout[3].InputSlotClass			= IA_PER_VERTEX_DATA;
	m_pDefaultLayout[2].InstanceDataStepRate	= 0;					m_pDefaultLayout[3].InstanceDataStepRate	= 0;

	m_pDefaultLayout[4].SemanticName			= "BINORMAL";			
	m_pDefaultLayout[4].SemanticIndex			= 0;					
	m_pDefaultLayout[4].Format					= TEX_R32G32B32_FLOAT;	
	m_pDefaultLayout[4].InputSlot				= 2;					
	m_pDefaultLayout[4].AlignedByteOffset		= 24;					
	m_pDefaultLayout[4].InputSlotClass			= IA_PER_VERTEX_DATA;	
	m_pDefaultLayout[4].InstanceDataStepRate	= 0;

	m_pFinalShadingShaders->VSetVertexShader(L"forwardp_shading_vs.hlsl", "shading_vs", m_pDefaultLayout, 5, TOPOLOGY_TRIANGLELIST, "vs_5_0");
	m_pFinalShadingShaders->VSetPixelShader(L"forwardp_shading_ps.hlsl", "shading_ps", "ps_5_0");

	//////////////////////////////////////////////////////
	//Initialize structured buffers for Forward+ rendering
	//LightParams testParams;
	//testParams.color = Vector(1.0f, 1.0f, 1.0f, 1.0f);
	//testParams.dirANDrange = Vector(0.0f, -1.0f, 0.0f, 30.0f);
	//testParams.pos = Vector(0.0f, 3.0f, 0.0f, 1.0f);
	//testParams.spotAngles = Vector(0.8, 1.2, 0.0f, 0.0f);

	//LightGeometry testGeometry;
	//testGeometry.posANDrang = Vector(0.0f, 3.0f, 0.0f, 20.0f);

	BufferParams params;
	params.FillStructredBufferParams(4, 256*3600, false, true);
	m_psbLightIdx->Create(&params, nullptr);

	srvParams.InitForBuffer(TEX_UNKNOWN, 0, 256*3600);
	m_psbLightIdx->CreateShaderResourceView(&m_pLightIdxSRV->m_pView, &srvParams);

	UnorderedAccessViewParams uavParams;
	uavParams.InitForStructuredBuffer(TEX_UNKNOWN, 0, 256*3600, D3D11_BUFFER_UAV_FLAG_COUNTER);
	m_psbLightIdx->CreateUnorderedAccessView(&m_pLightIdxUAV->m_pView, &uavParams);

	/*****************************************/
	//lower bound
	params.FillStructredBufferParams(4, 3600, false, true);
	m_psbLowerBound->Create(&params, nullptr);

	srvParams.InitForBuffer(TEX_UNKNOWN, 0, 3600);
	m_psbLowerBound->CreateShaderResourceView(&m_pLowerBoundSRV->m_pView, &srvParams);

	uavParams.InitForStructuredBuffer(TEX_UNKNOWN, 0, 3600, D3D11_BUFFER_UAV_FLAG_COUNTER);
	m_psbLowerBound->CreateUnorderedAccessView(&m_pLowerBoundUAV->m_pView, &uavParams);

	/*****************************************/
	//higher bound
	params.FillStructredBufferParams(4, 3600, false, true);
	m_psbHigherBound->Create(&params, nullptr);

	srvParams.InitForBuffer(TEX_UNKNOWN, 0, 3600);
	m_psbHigherBound->CreateShaderResourceView(&m_pHigherBoundSRV->m_pView, &srvParams);

	uavParams.InitForStructuredBuffer(TEX_UNKNOWN, 0, 3600, D3D11_BUFFER_UAV_FLAG_COUNTER);
	m_psbHigherBound->CreateUnorderedAccessView(&m_pHigherBoundUAV->m_pView, &uavParams);

	/*****************************************/
	//Light Counter
	params.FillStructredBufferParams(4, 1, false, true);
	m_psbLightCounter->Create(&params, nullptr);

	srvParams.InitForBuffer(TEX_UNKNOWN, 0, 1);
	m_psbLightCounter->CreateShaderResourceView(&m_pLightCounterSRV->m_pView, &srvParams);

	uavParams.InitForStructuredBuffer(TEX_UNKNOWN, 0, 1, D3D11_BUFFER_UAV_FLAG_COUNTER);
	m_psbLightCounter->CreateUnorderedAccessView(&m_pLightCounterUAV->m_pView, &uavParams);

	/*****************************************/
	//light geometry params
	const int numLights = 256;
	m_pLightGeometryData = new LightGeometry[numLights];
	for (int i =0; i < numLights; i++)
		//for (int j =0; j < 9; j++)
	{
		int n = i / 16;
		int m = i % 16;
		m_pLightGeometryData[i].posANDrang = Vector(-50 + n*8, -12, -45 + m*8, 10);
	}

	SubresourceData geometryData;
	geometryData.SetData(m_pLightGeometryData);
	params.FillStructredBufferParams(sizeof(LightGeometry), numLights, true, false);
	//m_psbLightGeometry->Create(&params, &geometryData);
	m_psbLightGeometry->Create(&params, nullptr);
	//m_psbLightGeometry->UpdateSubresource(0, nullptr, &testGeometry, 0, 0);

	srvParams.InitForBuffer(TEX_UNKNOWN, 0, numLights);
	m_psbLightGeometry->CreateShaderResourceView(&m_pLightGeometrySRV->m_pView, &srvParams);

	/*****************************************/
	//light params
	m_pLightParams = new LightParams[numLights];
	for (int i = 0; i < numLights; i++)
		//for (int j = 0; j < 9; j++)
	{
		int index = i;
		int n = i / 16;
		int m = i % 16;
		if ( i % 3 == 0)
			m_pLightParams[index].color = Vector(1.0f, 1.0f, 1.0f, 1.0f);
		else if (3 == 0)
			m_pLightParams[index].color = Vector(0.0f, 1.0f, 0.0f, 1.0f);
		else if ( 2 == 0)
			m_pLightParams[index].color = Vector(0.0f, 0.0f, 1.0f, 1.0f);
		else
			m_pLightParams[index].color = Vector(1.0f, 0.0f, 0.0f, 1.0f);

		m_pLightParams[index].dirANDrange = Vector(0.0f, -1.0f, 0.0f, 10);
		m_pLightParams[index].pos = Vector(-50 + n*8,-12,-45 + m*8,1);
		m_pLightParams[index].spotAngles = Vector(0.2, 1.3, 0, 0.1);
	}

	SubresourceData paramsData;
	paramsData.SetData(m_pLightParams);
	params.FillStructredBufferParams(sizeof(LightParams), numLights, true, false);
	//m_psbLights->Create(&params, &paramsData);
	m_psbLights->Create(&params, nullptr);

	srvParams.InitForBuffer(TEX_UNKNOWN, 0, numLights);
	m_psbLights->CreateShaderResourceView(&m_pLightsSRV->m_pView, &srvParams);

	//////////////////////////////////////////////////////
	//Initialize camera constant buffer for light culling
	//Initialize structure to fill buffer properties
	BufferParams * pcb_params = new BufferParams();

	pcb_params->FillConstantBufferParams(sizeof(CameraCullingData), true, false, false);
	m_pcbCameraCulling = new ConstantBuffer();
	if (!m_pcbCameraCulling->Create(pcb_params, NULL))	return false;


	pcb_params->FillConstantBufferParams(sizeof(AUINT32)*4, true, false, false);
	m_pcbCullingData = new ConstantBuffer();
	if (!m_pcbCullingData->Create(pcb_params, NULL)) return false;

	delete pcb_params;

	///////////////////////////////////////////////
	//Initialize sky resources
	///////////////////////////////////////////////
	Texture2DParams tex2DParams;
	tex2DParams.InitCubeTexture(512, 512, 1, TEX_R8G8B8A8_UNORM, true, false, false, false, 1, 0,
		1, true, false, false);
	//m_pSkyTexture->CreateFromFile(L"church_cubemap.dds");
	m_pSkyTexture->CreateFromFile(L"beach_cubemap_512.dds");

	ShaderResourceViewParams srvSkyParams;
	srvSkyParams.InitForCubeTexture(TEX_R8G8B8A8_UNORM, 1, 0);
	m_pSkyTexture->CreateShaderResourceView(&m_pSkySRV->m_pView, &srvSkyParams);

	////////////////////////////////////////////////
	//Initialize irradiance map
	/*tex2DParams.InitCubeTexture(128, 128, 1, TEX_R8G8B8A8_UNORM, true, true, true, false, 1, 0, 1, true, false, false);
	m_pIrradianceMap->Create(&tex2DParams);

	ShaderResourceViewParams srvIrradianceParams;
	srvIrradianceParams.InitForCubeTexture(TEX_R8G8B8A8_UNORM, 1, 0);
	m_pIrradianceMap->CreateShaderResourceView(&m_pIrradianceSRV->m_pView, &srvIrradianceParams);

	RenderTargetViewParams rtvIrradianceParams;
	rtvIrradianceParams.InitForTexture2DArray(6, tex2DParams.Format, 0, 0);
	//m_pIrradianceMap->CreateRenderTargetView(&m_pShadowRTV->m_pView, &rtvIrradianceParams);

	rtvParams.Texture2DArray.ArraySize = 1;
	for (AINT8 i = 0; i < 6; i++)
	{
		rtvIrradianceParams.Texture2DArray.FirstArraySlice = i;
		m_pIrradianceMap->CreateRenderTargetView(&m_pIrradianceRTV[i]->m_pView, &rtvIrradianceParams);
	}*/

	//BlobDX11* pCSErrors = new BlobDX11();
	//m_pSphericalCoeffShader->CreateAndCompile(L"Cubemap_SH_coeff.hlsl", "generate_spherical_coeff", pCSErrors);
	//m_pIrradianceMapShader->CreateAndCompile(L"IrradianceMap_FromSH.hlsl", "irradiance_map_SH", pCSErrors);
	//delete pCSErrors;

	m_pSkyLayout = new INPUT_LAYOUT();
	//m_pSkyLayout[0].SemanticName = "POSITION";					//m_pSkyLayout[1].SemanticName = "TEXCOORD";
	//m_pSkyLayout[0].SemanticIndex = 0;							//m_pSkyLayout[1].SemanticIndex = 0;
	//m_pSkyLayout[0].Format = TEX_R32G32B32_FLOAT;				//m_pSkyLayout[1].Format = TEX_R32G32_FLOAT;
	//m_pSkyLayout[0].InputSlot = 0;								//m_pSkyLayout[1].InputSlot = 1;
	//m_pSkyLayout[0].AlignedByteOffset = 0;						//m_pSkyLayout[1].AlignedByteOffset = 0;
	//m_pSkyLayout[0].InputSlotClass = IA_PER_VERTEX_DATA;		//m_pSkyLayout[1].InputSlotClass = IA_PER_VERTEX_DATA;
	//m_pSkyLayout[0].InstanceDataStepRate = 0;					//m_pSkyLayout[1].InstanceDataStepRate = 0;
	m_pSkyLayout->SemanticName		=	"POSITION";
	m_pSkyLayout->SemanticIndex	=	0;
	m_pSkyLayout->Format			=	TEX_R32G32B32_FLOAT;
	m_pSkyLayout->InputSlot		=	0;
	m_pSkyLayout->AlignedByteOffset =	0;
	m_pSkyLayout->InputSlotClass	=	IA_PER_VERTEX_DATA;
	m_pSkyLayout->InstanceDataStepRate =	0;

	m_pSkyShaders->VSetVertexShader(L"Source\\Shaders\\SkyBox.hlsl", "SkyBox_VS", m_pSkyLayout, 1, TOPOLOGY_TRIANGLELIST, "vs_5_0");
	m_pSkyShaders->VSetPixelShader(L"Source\\Shaders\\SkyBox.hlsl", "SkyBox_PS", "ps_5_0");

	m_pAtmoShaders->VSetVertexShader(L"Shaders\\Atmosphere\\SkyFromAtmosphere_vs.hlsl", "SkyFromAtmosphere_vs", m_pSkyLayout, 1, TOPOLOGY_TRIANGLELIST, "vs_5_0");
	m_pAtmoShaders->VSetPixelShader(L"Shaders\\Atmosphere\\SkyFromAtmosphere_ps.hlsl", "SkyFromAtmosphere_ps", "ps_5_0");

	Resource meshResource("sphere.obj");
	shared_ptr<ResHandle> pMeshes = Anubis::SafeGetHandle(&meshResource);
	std::shared_ptr<MeshResourceExtraData> pData = static_pointer_cast<MeshResourceExtraData>(pMeshes->GetExtra());

	m_pSphereMesh = pData->m_pMeshes[0];

	return true;
}

///////////////////////////////////////////////
//Updating
///////////////////////////////////////////////
AVOID ForwardPlusRenderer::VPushMesh(Mesh * pMesh)
{
	m_queue.VAdd(pMesh);
}

AVOID ForwardPlusRenderer::VPushLight(Light * pLight)
{
	m_lights.push_back(pLight);
}

AVOID ForwardPlusRenderer::VUpdate(const AUINT32 deltaMilliseconds)
{
	//Firstly, sort render queue
	m_queue.VSort();
}

///////////////////////////////////////////////////
//Rendering
///////////////////////////////////////////////////
AVOID ForwardPlusRenderer::PrepareForZPrepass(CameraPtr pCamera)
{
	m_pDepthEnableStencilDisableStandard->Set(0xFF);
	AllDisabledBackCullingRasterizer()->Set();

	//SetRenderTargetView();
	//m_pLinearZRTV->Set();
	m_pLinearZRTV->Set(m_pDepthDSV);

	//SetDepthStencilView(m_pDepthDSV);

	//set shaders
	m_pZPrepassShaders->VBind();
}

AVOID ForwardPlusRenderer::NullLightCounter()
{
	float f = 0.0;
	m_psbLightCounter->UpdateSubresource(0, nullptr, &f, 0, 0);
}

AVOID ForwardPlusRenderer::LightCulling(CameraPtr pCamera)
{
	CameraCullingData data;
	data.View = pCamera->GetView();
	data.View.Transpose();
	data.ViewProj = pCamera->GetViewProjection();
	data.ViewProj.Transpose();
	data.posANDfov = pCamera->GetPosition();
	data.posANDfov.w = pCamera->GetFrustum().GetFOV();
	data.camersFar = 500;
	data.cameraNear = 1.0f;
	data.viewWidth = SCREEN_WIDTH;
	data.viewHeight = SCREEN_HEIGHT;
	data.dir = pCamera->GetDir();

	m_pcbCameraCulling->UpdateSubresource(0, nullptr, &data, 0, 0);
	m_pcbCameraCulling->Set(0, ST_Compute);

	struct culling
	{
		AUINT32 m_numLights;
		AUINT32 m_pad1;
		AUINT32 m_pad2;
		AUINT32 m_pad3;
	};
	culling culling_data;
	culling_data.m_numLights = m_iNumLights;

	m_pcbCullingData->UpdateSubresource(0, nullptr, &culling_data, 0, 0);
	m_pcbCullingData->Set(1, ST_Compute);

	m_pLightCullingShader->Set();
	m_pLinearZSRV->Set(0, ST_Compute);
	m_pLightGeometrySRV->Set(1, ST_Compute);
	m_pLightIdxUAV->Set(0, nullptr, ST_Compute);
	m_pLowerBoundUAV->Set(1, nullptr, ST_Compute);
	m_pHigherBoundUAV->Set(2, nullptr, ST_Compute);
	m_pLightCounterUAV->Set(3, nullptr, ST_Compute);

	D3D11DeviceContext()->Dispatch(ceil((float)SCREEN_WIDTH / 16.0), ceil((float)SCREEN_HEIGHT / 16.0), 1);
	//D3D11DeviceContext()->Dispatch(1, 1, 1);

	UnbindShaderResourceViews(0, 2, ST_Compute);
	UnbindUnorderedAccessViews(0, 4);

	m_pLightIdxSRV->Set(1, ST_Pixel);
}

AVOID ForwardPlusRenderer::PrepareForShadingPass(CameraPtr pCamera)
{
	m_pDepthEnableStencilDisableStandard->Set(0xFF);
	//SetDepthStencilView(m_pDepthDSV);
	AllDisabledBackCullingRasterizer()->Set();

	//SetRenderTargetView();
	SetRenderTargetView(m_pDepthDSV);
	m_pFinalShadingShaders->VBind();

	m_pLightsSRV->Set(0, ST_Pixel);
	m_pLightIdxSRV->Set(1, ST_Pixel);
	m_pLowerBoundSRV->Set(2, ST_Pixel);
	m_pHigherBoundSRV->Set(3, ST_Pixel);
	m_pLightCounterSRV->Set(4, ST_Pixel);

	m_pcbCameraPos->UpdateSubresource(0, nullptr, &pCamera->GetPosition(), 0, 0);
	m_pcbCameraPos->Set(0, ST_Pixel);
}

AVOID ForwardPlusRenderer::VRender()
{
	/*** Render Scene For Each Camera ***/
 	//for (CameraPtr pCamera : m_cameras)
	for (Cameras::iterator it = m_cameras.begin(); it != m_cameras.end(); it++)
	{
		CameraPtr pCamera = (*it);
		//Keep track of current view and projection matrices
		Mat4x4 view			= pCamera->GetView();
		Mat4x4 projection	= pCamera->GetProjection();
		Mat4x4 viewProj		= view * projection; //calculate view * projection matrix

		// ===================================================== //
		//						Z Prepass						 //
		// ===================================================== //
		PrepareForZPrepass(pCamera);
		//m_pDepthEnableStencilDisableStandard->Set(1);
		Mesh * pMesh;
		m_queue.Reset();
		while (pMesh = m_queue.Next())
		{
			pMesh->VRenderZPass(this, view, viewProj);
		} ;
		ClearDepthStencilView(true, false, 1.0f, 0xFF, m_pDepthDSV);
		UnbindRenderTargetViews(1);

		// ===================================================== //
		//						Light Culling					 //
		// ===================================================== //
		UpdateLightBuffers();
		LightCulling(pCamera);

		// ===================================================== //
		//						Final Shading Pass				 //
		// ===================================================== //
		VRenderSky(pCamera, viewProj);

		PrepareForShadingPass(pCamera);
		m_queue.Reset();
		while (pMesh = m_queue.Next())
		{
			//set states needed for rendering current mesh
			pMesh->VPreRender(this, view, viewProj); 

			//finally render it
			pMesh->VRender(this);

			//remove previous states
			pMesh->VPostRender(this);
		} ;
		
		UnbindShaderResourceViews(0, 5, ST_Pixel);

		//clear meshes queue
		m_queue.Clear();
		ClearDepthStencilView(true, false, 1.0f, 0xFF, m_pDepthDSV);
		//for (Light * pLight : m_lights)
		//for (LightList::iterator lit = m_lights.begin(); lit != m_lights.end(); lit++)
		//{
		//}

		//clean lights
		while(!m_lights.empty())
		{
			m_lights.pop_back();
		}

		VFinishPass();
	}
}

AVOID ForwardPlusRenderer::VGenerateShadowMaps()
{
}

AVOID ForwardPlusRenderer::VGenerateIrradianceMap()
{
}

AVOID ForwardPlusRenderer::VRenderSky(CameraPtr pCamera, const Mat4x4 & viewproj)
{
	SetRenderTargetView();

	m_pSkyShaders->VBind();
	//m_pAtmoShaders->VBind(); 

	Mat4x4 trans;
	trans.CreateTranslation(pCamera->GetPosition());

	Mat4x4 scale;
	scale.CreateScaling(1, 1, 1);

	Mat4x4 worldViewProj = trans * viewproj;
	//Mat4x4 worldViewProj = scale * viewproj;
	worldViewProj.Transpose();

	m_pcbWVP->UpdateSubresource(0, nullptr, &worldViewProj, 0, 0);
	m_pcbWVP->Set(0, ST_Vertex);
	struct WorldData
	{
		Mat4x4 wvp;
		//Vec cameraPos;
	};
	WorldData worldData;
	worldData.wvp = worldViewProj;
	//worldData.cameraPos = pCamera->GetPosition();
	//m_pcbAtmoWorld->UpdateSubresource(0, nullptr, &worldData, 0, 0);
	//m_pcbAtmoWorld->Set(1, ST_Vertex);

	m_pSphereMesh->SetPositionBuffer();
	m_pSphereMesh->SetIndexBuffer();

	this->NoCullingStandardRasterizer()->Set();
	this->m_pDepthDisableStencilDisable->Set(0xFF);

	m_pSkySRV->Set(0, ST_Pixel);
	m_pSphereMesh->VRender(this);

	UnbindRenderTargetViews(1);
	UnbindShaderResourceViews(0, 1, ST_Pixel);
}

AVOID ForwardPlusRenderer::VFinishPass()
{
	m_pLinearZRTV->Clear();
	NullLightCounter();
}

AVOID ForwardPlusRenderer::UpdateLightBuffers()
{
	m_iNumLights = m_lights.size();

	ZeroMemory(m_pLightGeometryData, sizeof(LightGeometry) * m_iNumLights);
	ZeroMemory(m_pLightParams, sizeof(LightParams) * m_iNumLights);

	D3D11_MAPPED_SUBRESOURCE geometrySubresource;
	D3D11_MAPPED_SUBRESOURCE paramsSubresource;
	ZeroMemory(&geometrySubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&paramsSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	AUINT32 index = 0;
	for (LightList::iterator lit = m_lights.begin(); lit != m_lights.end(); lit++)
	{
		Light* pLight = (*lit);
		Vec pos = pLight->GetData()->m_pos * pLight->GetWorldTransform();
		m_pLightGeometryData[index].posANDrang = pos;
		m_pLightGeometryData[index].posANDrang.w = pLight->GetRange();

		m_pLightParams[index].color = pLight->GetData()->m_color;
		m_pLightParams[index].dirANDrange = pLight->GetData()->m_dir;
		m_pLightParams[index].dirANDrange.w = pLight->GetRange();
		m_pLightParams[index].pos = pos;
		m_pLightParams[index].spotAngles = Vector(0.8, 1.2, 0.0f, 0.0f);

		index++;
	}

	D3D11DeviceContext()->Map(m_psbLightGeometry->GetResourcePointer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &geometrySubresource);
	D3D11DeviceContext()->Map(m_psbLights->GetResourcePointer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &paramsSubresource);

	memcpy(geometrySubresource.pData, m_pLightGeometryData, sizeof(LightGeometry)*m_iNumLights);
	memcpy(paramsSubresource.pData, m_pLightParams, sizeof(LightParams)*m_iNumLights);

	D3D11DeviceContext()->Unmap(m_psbLightGeometry->GetResourcePointer(), 0);
	D3D11DeviceContext()->Unmap(m_psbLights->GetResourcePointer(), 0);

	//m_psbLightGeometry->UpdateSubresource(0, nullptr, m_pLightGeometryData, 0, 0);
	//m_psbLights->UpdateSubresource(0, nullptr, m_pLightParams, 0, 0);
}