//================================================
// manager.cppの作成
// Author:村元翼
//================================================
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "texture.h"
#include "keyboard.h"
#include "game.h"
#include "fade.h"
#include "XInput.h"
#include "DirectInput.h"
#include "LoadX.h"
#include "mouse.h"

//================================================
// 静的メンバ変数の初期化
//================================================
CManager*		CManager::s_pManager				= nullptr;		// シングルトンマネージャ
CRenderer*		CManager::m_pRenderer				= nullptr;		// レンダラー
CCamera*		CManager::m_apCamera[MAX_CAMERA]	= { nullptr };	// カメラ
CLight*			CManager::m_pLight[MAX_LIGHT]		= { nullptr };	// ライト
CTexture*		CManager::m_pTexture				= nullptr;		// テクスチャ
CInputkeyboard* CManager::m_pInputKeyboard			= nullptr;		// キーボード
CXInput*		CManager::m_pXInput					= nullptr;		// XInput
CDInput*		CManager::m_pDInput					= nullptr;		// DInput
CGame*			CManager::m_pGame					= nullptr;		// ゲーム
CFade*			CManager::m_pFade					= nullptr;		// フェード
CLoadX*			CManager::m_pLoadX					= nullptr;		// X読み込みクラス
CMouse*			CManager::m_pMouse					= nullptr;		// マウスクラス
bool			CManager::m_bOnlyOnce				= false;		// 一度のみ通る判定
HWND			CManager::m_hWnd					= nullptr;		// ウィンドウハンドル
CManager::MODE	CManager::m_mode					= MODE::GAME;	// デバッグ時初期モード

//================================================
// コンストラクタ
//================================================
CManager::CManager()
{

}

//================================================
// デストラクタ
//================================================
CManager::~CManager()
{

}

//================================================
// マネージャ初期化
//================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, bool bReturn)
{
	m_hWnd = hWnd;	// ウィンドウハンドルの保存

	// レンダラーの生成
	if (!m_pRenderer)
	{
		m_pRenderer = new CRenderer;
		if (m_pRenderer)
		{
			m_pRenderer->Init(hWnd, bReturn);
		}
	}

	// X読み込みクラスの生成
	if (!m_pLoadX)
	{
		m_pLoadX = new CLoadX;

		if (m_pLoadX)
		{
			m_pLoadX->Init();
		}
	}

	// キーボードの生成
	if (!m_pInputKeyboard)
	{
		m_pInputKeyboard = new CInputkeyboard;
		if (m_pInputKeyboard)
		{
			m_pInputKeyboard->Init(hInstance, hWnd);
		}
	}

	// Xinput生成
	if (!m_pXInput)
	{
		m_pXInput = new CXInput;
	}

	// Dinput生成
	if (!m_pDInput)
	{
		m_pDInput = new CDInput;
		if (m_pDInput)
		{
			m_pDInput->Init(hInstance, hWnd);
		}
	}

	// マウス生成
	if (!m_pMouse)
	{
		if (m_pMouse = new CMouse)
		{
			m_pMouse->Init(hInstance, hWnd);
		}
	}

	// ライトの生成
	for (int nCnt = 0; nCnt < MAX_LIGHT; nCnt++)
	{
		if (!m_pLight[nCnt])
		{
			m_pLight[nCnt] = new CLight;
		}
	}

	m_pLight[0]->Init({ 0.0f, 0.0f, 0.0f	},	{ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.2f, -0.8f, -0.4f });
	m_pLight[1]->Init({ 0.0f, 0.0f, 0.0f	},	{ 1.0f, 1.0f, 1.0f, 1.0f }, { -0.2f, -0.8f, 0.4f });
	m_pLight[2]->Init({ 0.0f, 0.0f, 0.0f	},	{ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.4f, 0.0f	});
	m_pLight[3]->Init({ 0.0f, 0.0f, -100.0f },	{ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f });

	// テクスチャの生成
	if (!m_pTexture)
	{
		m_pTexture = new CTexture;
		if (m_pTexture)
		{
			m_pTexture->Init();
		}
	}

	// フェードの生成
	if (!m_pFade)
	{
		m_pFade = CFade::Create(m_mode);
	}

	// モードの設定
	SetMode(m_mode);

	return S_OK;
}

//================================================
// マネージャ終了
//================================================
void CManager::Uninit(void)
{
	//-----------------------------------------------
	// 全ての終了処理
	//-----------------------------------------------
	CScene::ReleaseAll();

	//*******************************//
	//  各種オブジェクトの終了処理   //
	//*******************************//

	// テクスチャの破棄
	if (m_pTexture)
	{
		m_pTexture->Uninit();
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	// フェードの破棄
	if (m_pFade)
	{
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = nullptr;
	}

	// ライト破棄
	for (int nCnt = 0; nCnt < MAX_LIGHT; nCnt++)
	{
		if (m_pLight[nCnt])
		{
			delete m_pLight[nCnt];
			m_pLight[nCnt] = nullptr;
		}
	}

	// カメラの破棄
	for (int nCnt = 0; nCnt < MAX_CAMERA; nCnt++)
	{
		if (m_apCamera[nCnt])
		{
			m_apCamera[nCnt]->Uninit();
			delete m_apCamera[nCnt];
			m_apCamera[nCnt] = nullptr;
		}
	}

	// レンダラー破棄
	if (m_pRenderer)
	{
		m_pRenderer->Uninit();
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}

	// キーボードの破棄
	if (m_pInputKeyboard)
	{
		m_pInputKeyboard->Uninit();
		delete m_pInputKeyboard;
		m_pInputKeyboard = nullptr;
	}

	// DInputの破棄
	if (m_pDInput)
	{
		m_pDInput->Uninit();
		delete m_pDInput;
		m_pDInput = nullptr;
	}

	// マウスの破棄
	if (m_pMouse)
	{
		m_pMouse->Uninit();
		delete m_pMouse;
		m_pMouse = nullptr;
	}

	// X読み込みクラスの破棄
	if (m_pLoadX)
	{
		m_pLoadX->Uninit();
		delete m_pLoadX;
		m_pLoadX = nullptr;
	}

	// シングルトンマネージャの破棄
	if (s_pManager)
	{
		delete s_pManager;
		s_pManager = nullptr;
	}
}

//================================================
// マネージャ更新
//================================================
void CManager::Update(void)
{
	// レンダラーの更新
	if (m_pRenderer)
	{
		m_pRenderer->Update();
	}

	//キーボードの更新
	if (m_pInputKeyboard)
	{
		m_pInputKeyboard->Update();
	}

	// ゲームパッドの更新
	if (m_pXInput)
	{
		m_pXInput->UpdateGamepad();
	}

	// DInputの更新
	if (m_pDInput)
	{
		m_pDInput->Update();
	}

	// マウスの更新
	if (m_pMouse)
	{
		m_pMouse->Update();
	}

	// カメラの更新処理
	for (int nCnt = 0; nCnt < MAX_CAMERA; nCnt++)
	{
		if (m_apCamera[nCnt])
		{
			m_apCamera[nCnt]->Update();
		}
	}

	if (m_pGame)
	{
		m_pGame->Update();
	}
}

//================================================
// マネージャ描画
//================================================
void CManager::Draw(void)
{
	// レンダラー描画
	if (m_pRenderer)
	{
		m_pRenderer->Draw();
	}

	// カメラ描画
	for (int nCnt = 0; nCnt < MAX_CAMERA; nCnt++)
	{
		if (m_apCamera[nCnt])
		{
			m_apCamera[nCnt]->SetCamera();
		}
	}
}

//================================================
// インスタンス取得処理
//================================================
CManager *CManager::GetInstance(void)
{
	if (!s_pManager)
	{
		s_pManager = new CManager;
	}
	return s_pManager;
}

//================================================
// モードのセット
//================================================
void CManager::SetMode(MODE mode)
{
	if (m_pGame)
	{
		// カメラの破棄
		for (int nCnt = 0; nCnt < MAX_CAMERA; nCnt++)
		{
			if (m_apCamera[nCnt])
			{
				m_apCamera[nCnt]->Uninit();
				delete m_apCamera[nCnt];
				m_apCamera[nCnt] = nullptr;
			}
		}

		m_pGame->Uninit();
		delete m_pGame;
		m_pGame = nullptr;
	}

	//----------------------------------
	// 全てのオブジェクトの破棄
	//----------------------------------
	CScene::ReleaseAll();

	if (!m_pGame)
	{
		m_pGame = new CGame;
		if (m_pGame)
		{
			// カメラの生成
			for (int nCnt = 0; nCnt < MAX_CAMERA; nCnt++)
			{
				m_apCamera[nCnt] = CCamera::Create(
					D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI*-0.3f, 0.0f, 0.0f),
					(float)(SCREEN_WIDTH / MAX_CAMERA * nCnt), 0.0f,
					(float)(SCREEN_WIDTH / MAX_CAMERA), (float)SCREEN_HEIGHT, nCnt);
			}
			m_pGame->Init();
		}
	}

	// 次のモードを現在のモードにする
	m_mode = mode;
	m_bOnlyOnce = false;
}

//================================================
// モードのセット判定
//================================================
bool CManager::SetModeBool(MODE modeNext)
{
	//=========================================================
	// 画面遷移(ENTERまたはSTARTボタンを押す)
	//=========================================================
	if ((CInput::PressAnyAction(CInput::ACTION_ENTER) || CInput::PressAnyAction(CInput::ACTION_ATTACK)) && !m_bOnlyOnce)
	{
		m_pFade->SetFade(modeNext);						// ゲームモードへ
		m_bOnlyOnce = true;								// ENTER連打防止
		return true;
	}

	return false;
}
