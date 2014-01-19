
#include "../Deferred/GBuffer.h"

namespace Anubis
{
	struct GBuffer2D : public GBuffer
	{
		/***
			== Textures
						***/
		Texture2DPtr	m_pPosTex;
		Texture2DPtr	m_pNormalTex;
		Texture2DPtr	m_pDepthTex;
		Texture2DPtr	m_pDiffuseTex;
		Texture2DPtr	m_pSpecularTex;

		/***
			== ShaderResource Views
									***/
		/* ShaderResourceView &	m_pPosSRV;
		ShaderResourceView &	m_pNormalSRV;
		ShaderResourceView &	m_pDiffuseSRV;
		ShaderResourceView &	m_pSpecularSRV; */
		ShaderResourceViewList	m_SRVList;

		/***
			== RenderTarget Views
									***/
		/*RenderTargetView &	m_pPosRTV;
		RenderTargetView &	m_pNormalRTV;
		RenderTargetView &	m_pDiffuseRTV;
		RenderTargetView &	m_pSpecularRTV; */
		RenderTargetViewList	m_RTVList;

		/***
			== DepthStencil View
									***/
		DepthStencilView*	m_pDepthStencilView;

	public:

		/***
			Constructor, Destructor
		***/
		GBuffer2D();
		~GBuffer2D();

		/***
			Initilize
		***/
		AVIRTUAL ABOOL VInitialize();

		/***
			Bind GBuffer for reading and writing
		***/
		AVIRTUAL AVOID BindForWriting(DepthStencilView * pView);
		AVIRTUAL AVOID BindForReading(AUINT16 slot) const;

		AVIRTUAL AVOID BindPositionTex(AUINT16 slot, ShaderType shaderType) const;
		AVIRTUAL AVOID BindNormalTex(AUINT16 slot, ShaderType shaderType) const;
		AVIRTUAL AVOID BindDepthTex(AUINT16 slot, ShaderType shaderType) const;
		AVIRTUAL AVOID BindDiffuseTex(AUINT16 slot, ShaderType shaderType) const;
		AVIRTUAL AVOID BindSpecularTex(AUINT16 slot, ShaderType shaderType) const;

		AVIRTUAL AVOID BindRenderTarget(const AUINT8 index);

		/***
			Unbind buffer resources from the pipeline
		***/
		AVIRTUAL AVOID UnbindFromWriting() const;
		AVIRTUAL AVOID UnbindFromReading(AUINT16 slot) const;

		/***
			Clear buffer after rendering
		***/
		AVIRTUAL AVOID Clear();
	};
}