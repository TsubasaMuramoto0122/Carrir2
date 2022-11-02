//===============================================
// フェード用ポリゴン
// Author:村元翼
//===============================================
#include "FadePolygon.h"
#include "texture.h"
#include "manager.h"

//===============================================
// 静的メンバ変数の初期化
//===============================================
const float CFadePolygon::m_fFadeInSpeed	= 0.02f;	// フェードイン速度
const float CFadePolygon::m_fFadeOutSpeed	= 0.01f;	// フェードアウト速度

//===============================================
// コンストラクタ
//===============================================
CFadePolygon::CFadePolygon(float fFadeSize, OBJTYPE nPriority) : CScene2D(nPriority),
																 m_fInitFadeSize(fFadeSize)	// 変数初期化
{
	m_fFadeSize = INIT_FADE_SIZE;		// 初期フェードサイズ
	m_pScene2D = nullptr;				// 2Dポリゴン
	m_pos		= { 0.0f,0.0f,0.0f };	// 位置
	m_size		= { 0.0f,0.0f,0.0f };	// サイズ
	m_fAlpha	= 0.0f;					// α値
	m_bFadeIn	= false;				// フェードイン判定
}

//===============================================
// デストラクタ
//===============================================
CFadePolygon::~CFadePolygon()
{

}

//===============================================
// オブジェクト生成
//===============================================
CFadePolygon *CFadePolygon::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, float fInitFadeSize)
{
	// オブジェクト生成
	CFadePolygon *pFadePolygon = nullptr;
	if (pFadePolygon = new CFadePolygon(fInitFadeSize))
	{
		pFadePolygon->Init(pos, size);
	}

	return pFadePolygon;
}

//===============================================
// 初期化
//===============================================
HRESULT CFadePolygon::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// 位置・サイズ取得
	m_pos = pos;
	m_size = size;

	// 2Dポリゴンの生成
	if (!m_pScene2D)
	{
		m_pScene2D = CScene2D::Create(m_pos, m_size);
		m_pScene2D->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_ROUNDFRAME"));
		m_pScene2D->SetCol({ 1.0f,1.0f,1.0f,0.0f });
	}

	return S_OK;
}

//===============================================
// 終了
//===============================================
void CFadePolygon::Uninit(void)
{
	// 2Dポリゴンの破棄
	if (m_pScene2D)
	{
		m_pScene2D->Uninit();
		m_pScene2D = nullptr;
	}

	// このオブジェクトを破棄
	Release();
}

//===============================================
// 更新
//===============================================
void CFadePolygon::Update(void)
{
	// フェード処理関数
	Fade(m_fAlpha);
}

//===============================================
// 描画
//===============================================
void CFadePolygon::Draw(void)
{

}

//===============================================
// フェード
//===============================================
void CFadePolygon::Fade(float& fAlpha)
{
	if (m_pScene2D)
	{
		// カラー・サイズ設定
		m_pScene2D->SetCol({ 1.0f,1.0f,1.0f,fAlpha });
		m_pScene2D->SetPos(m_pos, m_size);

		// フェードを消す
		if (m_bFadeIn)
		{
			m_fFadeSize += m_fInitFadeSize * 0.01f;	// フェードサイズを大きく
			fAlpha -= m_fFadeInSpeed;				// α値を小さく

			// 徐々にサイズを小さく
			if (m_fFadeSize > 0.0f)
			{
				m_size.x -= m_fFadeSize;
				m_size.y -= m_fFadeSize;
			}

			// α値0以下でポリゴン破棄
			if (fAlpha <= 0.0f)
			{
				fAlpha = 0.0f;
				Uninit();
			}

		}

		// フェードを出す
		else
		{
			m_fFadeSize -= m_fInitFadeSize * 0.01f;	// フェードサイズを小さく
			fAlpha += m_fFadeOutSpeed;				// α値を大きく

			// 徐々にサイズを大きく
			if (m_fFadeSize > 0.0f)
			{
				m_size.x += m_fFadeSize;
				m_size.y += m_fFadeSize;
			}

			// フェードイン判定on
			if (fAlpha >= 1.0f)
			{
				m_fFadeSize = 0.1f;
				fAlpha = 1.0f;
				m_bFadeIn = true;
			}
		}

	}
}