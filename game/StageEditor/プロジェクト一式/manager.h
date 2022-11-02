//*****************************************************************************
// manager.hの作成
// Author:村元翼
//*****************************************************************************
#ifndef _MANAGER_H_
#define _MANAGER_H_

//*****************************************************************************
// ヘッダファイル
//*****************************************************************************
#include "main.h"
#include "camera.h"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CRenderer;
class CLight;
class CTexture;
class CInputkeyboard;
class CGame;
class CFade;
class CXInput;
class CDInput;
class CLoadX;
class CMouse;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_LIGHT		(4)

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CManager
{
public:
	//----------------------------------
	// モード変更のスコープ付き列挙
	//----------------------------------
	enum class MODE : int
	{
		TITLE = 0,
		GAME,
		RESULT,
		MAX
	};

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, bool bReturn);	// 初期化
	void Uninit(void);											// 終了
	void Update(void);											// 更新
	void Draw(void);											// 描画

	static bool				SetModeBool(MODE modeNext);								// モード設定判定
	static void				SetMode(MODE mode);										// モード設定
	static CManager			*GetInstance(void);										// インスタンス取得
	static CRenderer		*GetRenderer()			{ return m_pRenderer; }			// レンダラーの取得
	static CInputkeyboard	*GetKeyboard()			{ return m_pInputKeyboard; }	// キーボードの取得
	static CXInput			*GetXInput()			{ return m_pXInput; }			// パッド(X)取得処理
	static CDInput			*GetDInput()			{ return m_pDInput; }			// パッド(D)取得処理
	static CTexture			*GetTexture()			{ return m_pTexture; }			// テクスチャの取得
	static CCamera			*GetCamera(int nNum)	{ return m_apCamera[nNum]; }	// カメラの取得
	static CLoadX			*GetLoadX()				{ return m_pLoadX; }			// X読み込みクラスの取得

	static CFade			*GetFade()				{ return m_pFade; }				// フェードの取得
	static CGame			*GetGame()				{ return m_pGame; }				// ゲームクラスの取得
	static MODE				Getmode()				{ return m_mode; }				// モードの取得
	static CMouse			*GetMouse()				{ return m_pMouse; }			// マウスの取得
					
	static HWND GethWnd()							{ return m_hWnd; }							// ウィンドウハンドルの取得

private:
	CManager();												// コンストラクタ
	~CManager();											// デストラクタ

	static CManager			*s_pManager;					// シングルトンマネージャーのクラス
	static CRenderer		*m_pRenderer;					// レンダラーのクラス
	static CTexture			*m_pTexture;					// テクスチャのクラス
	static CInputkeyboard	*m_pInputKeyboard;				// キーボードのクラス
	static CXInput			*m_pXInput;						// パッド(X)のクラス
	static CDInput			*m_pDInput;						// パッド(D)のクラス
	static CCamera			*m_apCamera[MAX_CAMERA];		// カメラのクラス
	static CLight			*m_pLight[MAX_LIGHT];			// ライトのクラス
	static CFade			*m_pFade;						// フェードクラス
	static CGame			*m_pGame;						// ゲームクラス
	static MODE				m_mode;							// モードの構造体
	static CLoadX			*m_pLoadX;						// X読み込みクラス
	static CMouse			*m_pMouse;						// マウスクラス
	static bool				m_bOnlyOnce;					// 次モード遷移の際の連続フェード防止
	static HWND				m_hWnd;							// ウィンドウハンドル
};
#endif
