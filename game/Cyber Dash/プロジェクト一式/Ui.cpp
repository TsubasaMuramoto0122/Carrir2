//================================================
// UI処理
// Author:村元翼
//================================================
#include "ui.h"
#include "control_blink.h"
#include "control_zoom.h"
#include "manager.h"
#include "renderer.h"

//================================================
// コンストラクタ
//================================================
CUi::CUi(OBJTYPE nPriority) : CScene2D(nPriority)
{
	m_pTexture = nullptr;					 // テクスチャへのポインタ
	m_pControl = nullptr;					 // コントロールクラス
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	 // 位置
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	 // 大きさ
	m_UiType = UI_TYPE_NONE;				 // UIタイプ
}

//================================================
// デストラクタ
//================================================
CUi::~CUi()
{

}

//================================================
// 初期化処理
//================================================
HRESULT CUi::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// 位置・大きさの設定
	m_pos = pos;
	m_size = size;
	CScene::SetPos(m_pos);
	CScene::SetSize(m_size);

	// テクスチャ取得
	CScene2D::BindTexture(CManager::GetInstance()->GetTexture()->GetTexture(""));

	// 初期化
	CScene2D::Init(m_pos, m_size);

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CUi::Uninit(void)
{
	// コントロールクラスの破棄
	if (m_pControl)
	{
		m_pControl->Uninit();
		delete m_pControl;
		m_pControl = nullptr;
	}

	// 破棄
	CScene2D::Uninit();
}

//================================================
// 更新処理
//================================================
void CUi::Update(void)
{
	// 位置・大きさ・カラー取得
	m_pos = GetPos();
	m_size = GetSize();
	m_col = GetCol();

	// 位置・大きさ設定
	if (m_pVtxBuff)
	{
		CScene::SetPos(m_pos);
		CScene::SetSize(m_size);
	}

	// コントロールクラスの更新
	if (m_pControl)
	{
		m_pControl->Update(this);
	}
}


//================================================
// 描画処理
//================================================
void CUi::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイスのポインタ
	CManager::GetInstance()->GetRenderer()->SetAlphaTest(true, pDevice);				// αテスト有効

	CScene2D::Draw();																	// 描画

	CManager::GetInstance()->GetRenderer()->SetAlphaTest(false, pDevice);				// αテスト無効
}


//================================================
// 生成処理
//================================================
CUi *CUi::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, UI_TYPE type)
{
	// オブジェクト生成
	CUi *pUi = nullptr;
	if (pUi = new CUi)
	{
		// タイプ別処理
		switch (type)
		{
		case UI_TYPE_BLINK:	// 点滅
			pUi->m_pControl = CControlBlink::Create();
			break;

		case UI_TYPE_ZOOM:	// ズーム
			pUi->m_pControl = CControlZoom::Create();
			break;
		}

		pUi->m_UiType = type;								// UIタイプ
		pUi->Init(pos, size);								// 初期化処理
		pUi->SetPos(pos, size, false);						// 位置設定
		pUi->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));		// カラー設定
	}

	return pUi;
}

//================================================
// UIのタイプ取得
//================================================
CUi::UI_TYPE CUi::GetUiType(void)
{
	return UI_TYPE();
}