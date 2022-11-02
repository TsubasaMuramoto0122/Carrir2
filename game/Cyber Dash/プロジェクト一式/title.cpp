//============================================
// タイトルの処理
// Author:村元翼
//============================================
#include "title.h"
#include "texture.h"
#include "manager.h"
#include "inputManager.h"
#include "fade.h"
#include "bg.h"
#include "Ui.h"
#include "sound.h"
#include "StringManager.h"
#include "control_blink.h"
#include "Mist.h"
#include "keyboard.h"

//===============================================
// マクロ定義
//==============================================
#define PRESS_ENTER_SIZE	(D3DXVECTOR3(600.0f, 150.0f, 0.0f))														// PressEnterUIのサイズ
#define PRESS_ENTER_POS		(D3DXVECTOR3(SCREEN_WIDTH / 2, 550.0f, 0.0f))											// PressEnterUIの位置

#define TITLE_LOGO_SIZE		(D3DXVECTOR3(1200.0f, 350.0f, 0.0f))													// タイトルロゴのサイズ
#define TITLE_LOGO_POS		(D3DXVECTOR3(SCREEN_WIDTH / 2, 200.0f, 0.0f))											// タイトルロゴの位置

#define TUTORIAL_SIZE		(D3DXVECTOR3(1000.0f,600.0f,0.0f))														// チュートリアルサイズ

#define SCROLL_UI_SIZE		(D3DXVECTOR3(TUTORIAL_SIZE.x,TUTORIAL_SIZE.y / 20.0f,0.0f))								// スクロールUIのサイズ
#define SCROLL_SPEED		(0.005f)																				// スクロールスピード
#define SCROLL_UI_INDENT	(100.0f)																				// スクロールUI同士の隙間
#define SCROLL_UI_SPLIT		(D3DXVECTOR2(10.0f,1.0f))																// スクロールテクスチャ分割数

#define KEY_EXP_POS			(D3DXVECTOR3(SCREEN_CENTER.x - (SCREEN_CENTER.x / 1.8f),SCREEN_CENTER.y + 130.0f,0.0f))	// キー説明の位置								
#define KEY_EXP_FONTNAME	("HG創英角ﾎﾟｯﾌﾟ体")																		// キー説明のフォントサイズ

#define MIST_INTERVAL		(2)																						// 背景に使うポリゴンの生成インターバル
#define PRESSSTART_BLINK	(5)																						// スタートを押したときの点滅速度
#define PRESSSTART_INTERVAL (100)																					// スタートを押してから次の画面までの時間間隔				

//===============================================
// 静的メンバ変数の初期化
//===============================================
const CTitle::SELECT_PARAM CTitle::m_SelectParam[TITLE_SELECT_MAX] =
{
	{ { 300.0f,500.0f,0.0f }			,{ 300.0f,150.0f,0.0f },"TEX_TYPE_TITLE_SELECT00",CUi::UI_TYPE_NONE },	// セレクト01
	{ { SCREEN_WIDTH / 2,600.0f,0.0f }	,{ 300.0f,150.0f,0.0f },"TEX_TYPE_TITLE_SELECT03",CUi::UI_TYPE_NONE },	// セレクト02(現在未完成)
	{ { 980.0f,500.0f,0.0f }			,{ 300.0f,150.0f,0.0f },"TEX_TYPE_TITLE_SELECT02",CUi::UI_TYPE_NONE }	// セレクト03
};

//============================================
// コンストラクタ
//============================================
CTitle::CTitle()
{
	// 変数初期化
	memset(m_pSelectUI, 0, sizeof(m_pSelectUI));  // 選択UI
	memset(m_pScrollUI, 0, sizeof(m_pScrollUI));  // スクロールUI
	m_pBg			= nullptr;					  // 背景
	m_pTutorial		= nullptr;					  // チュートリアルUI
	m_pTuto			= nullptr;					  // チュートリアルUI
	m_pPressEnter	= nullptr;					  // pressenterUI
	m_pTitleLogo	= nullptr;					  // タイトルロゴUI
	m_nSelTuto		= 0;						  // チュートリアルの選択UI
	m_Page			= PAGE_01;					  // ページ構造体
	m_TutoPage		= TUTORIAL_PAGE_01;			  // チュートリアルページ構造体
	m_nSelectKey	= 0;						  // 選択キー
	m_nSelect		= 0;						  // 選択肢
	m_nFrame		= 0;						  // フレーム
	m_bOPRLock		= false;					  // 操作ロック判定
	m_bUnlock		= false;					  // アンロック判定
	m_bLock			= false;					  // ロック判定
}

//============================================
// デストラクタ
//============================================
CTitle::~CTitle()
{

}

//============================================
// 初期化
//============================================
HRESULT CTitle::Init(void)
{
	// テクスチャクラスの取得
	CTexture *pTex = CManager::GetInstance()->GetTexture();

	// タイトル背景の生成
	CMist::Create({ 0.0f,0.0f,0.0f }, { SCREEN_WIDTH ,SCREEN_HEIGHT / 2,0.0f }, MIST_INTERVAL, 1, true);

	// PressStartUIの生成
	if (!m_pPressEnter)
	{
		m_pPressEnter = CUi::Create(PRESS_ENTER_POS, PRESS_ENTER_SIZE, CUi::UI_TYPE_BLINK);
		m_pPressEnter->BindTexture(pTex->GetTexture("TEX_TYPE_PRESS_START"));
	}

	// タイトルUIの生成
	if (!m_pTitleLogo)
	{
		m_pTitleLogo = CUi::Create(TITLE_LOGO_POS, TITLE_LOGO_SIZE);
		m_pTitleLogo->BindTexture(pTex->GetTexture("TEX_TYPE_TITLE"));
	}

	return S_OK;
}

//============================================
// 終了
//============================================
void CTitle::Uninit(void)
{
	//-------------------------------
	// オブジェクト終了処理
	//-------------------------------
	// 選択肢UIの削除
	for (int nCnt = 0; nCnt < TITLE_SELECT_MAX; nCnt++)
	{
		if (m_pSelectUI[nCnt])
		{
			m_pSelectUI[nCnt]->Uninit();
			m_pSelectUI[nCnt] = nullptr;
		}
	}

	// チュートリアルUIの破棄
	if (m_pTutorial)
	{
		m_pTutorial->Uninit();
		m_pTutorial = nullptr;
	}

	// スクロールUIの破棄
	for (int nCnt = 0; nCnt < MAX_SCROLL_UI; nCnt++)
	{
		if (m_pScrollUI[nCnt])
		{
			m_pScrollUI[nCnt]->Uninit();
			m_pScrollUI[nCnt] = nullptr;
		}
	}

	// PressEnterの削除
	if (m_pPressEnter)
	{
		m_pPressEnter->Uninit();
		m_pPressEnter = nullptr;
	}

	// タイトルロゴの削除
	if (m_pTitleLogo)
	{
		m_pTitleLogo->Uninit();
		m_pTitleLogo = nullptr;
	}

	// タイトル背景の削除
	if (m_pBg)
	{
		m_pBg->Uninit();
		m_pBg = nullptr;
	}

	// チュートリアルUI
	if (m_pTuto)
	{
		m_pTuto->Uninit();
		m_pTuto = nullptr;
	}
}
//--------------------------------------------
//更新
//--------------------------------------------
void CTitle::Update(void)
{
	// クラス情報取得
	CTexture *pTex = CManager::GetInstance()->GetTexture();
	CSound *pSound = CManager::GetInstance()->GetSound();

	//-------------------------------------------------------
	// ページ別処理
	//-------------------------------------------------------
	switch (m_Page)
	{
		//---------------------------------------
		// ページ1
		//---------------------------------------
	case PAGE_01:
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ENTER) && !m_bUnlock)
		{
			// 決定音
			pSound->PlaySound(CSound::SOUND_LABEL_SE_DECIDE);

			// 点滅を早くする
			CControlBlink *pBlink = (CControlBlink*)m_pPressEnter->GetControl();
			pBlink->SetInterval(PRESSSTART_BLINK);

			// スタートを押した後の処理を通す
			m_bUnlock = true;
		}

		// スタートを押してから少し間を置く
		if (m_bUnlock)
		{
			m_nFrame++;
			if (m_nFrame >= PRESSSTART_INTERVAL)
			{

				// ページ2に移行
				m_Page = PAGE_02;

				// プレススタートを破棄
				if (m_pPressEnter)
				{
					m_pPressEnter->Uninit();
					m_pPressEnter = nullptr;
				}

				// 選択肢の生成
				for (int nCnt = 0; nCnt < TITLE_SELECT_MAX; nCnt++)
				{
					// 選択肢UIの生成
					if (!m_pSelectUI[nCnt])
					{
						m_pSelectUI[nCnt] = CUi::Create(m_SelectParam[nCnt].pos, m_SelectParam[nCnt].size, m_SelectParam[nCnt].UiType);
						m_pSelectUI[nCnt]->BindTexture(pTex->GetTexture(m_SelectParam[nCnt].Tex));
						m_pSelectUI[nCnt]->SetCol(NOT_SELECT_COLOR);
					}
				}
				m_pSelectUI[m_nSelect]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// カラーの設定
			}
		}
		break;

		//---------------------------------------
		// ページ2
		//---------------------------------------
	case PAGE_02:
		// 決定を押す
		if (!m_bOPRLock)
		{
			if (m_bLock)
			{
				TutoAppear();	// ゲーム説明表示
			}

			if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ENTER) || CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ATTACK))
			{
				// 決定音
				pSound->PlaySound(CSound::SOUND_LABEL_SE_DECIDE);

				switch (m_nSelect)
				{
				case TITLE_SELECT_01:
					// ゲームモードへ移行
					m_bLock = true;
					break;

				case TITLE_SELECT_02:
					if (m_bUnlock)
					{
						//CFade::SetFade(CManager::MODE::TUTORIAL);
						m_bUnlock = false;
					}
					break;

				case TITLE_SELECT_03:
					// ゲームを終了
					PostQuitMessage(0);
					break;
				}
			}

			// 選択を左へ移動させる
			else if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_LEFT) || CInputManager::TriggerAnyAction(CInputManager::TRIGGER_RIGHT))
			{
				if (!m_bLock)
				{
					// 現在選択されている選択肢の明るさを下げる
					m_pSelectUI[m_nSelect]->SetCol(NOT_SELECT_COLOR);

					// カーソル移動処理
					if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_LEFT))
					{
						CursorMove(m_nSelect, TITLE_SELECT_03, TITLE_SELECT_01, false);

						// チュートリアル画面は未完成なので飛ばす
						if (m_nSelect == TITLE_SELECT_02)
						{
							m_nSelect--;
						}
					}

					// 選択を右へ移動させる
					else if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_RIGHT))
					{
						CursorMove(m_nSelect, TITLE_SELECT_01, TITLE_SELECT_03, true);

						// チュートリアル画面は未完成なので飛ばす
						if (m_nSelect == TITLE_SELECT_02)
						{
							m_nSelect++;
						}
					}

					// 移動後の選択肢のカラーを明るさを最大にする
					m_pSelectUI[m_nSelect]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
		}
		break;

		//---------------------------------------
		// ページ3
		//---------------------------------------
	case PAGE_03:
		// チュートリアルUIの生成
		if (!m_pTutorial)
		{
			m_pTutorial = CUi::Create(SCREEN_CENTER, TUTORIAL_SIZE);
			m_pTutorial->BindTexture(pTex->GetTexture("TEX_TYPE_TUTORIAL01"));
		}

		TutorialDisplay(&m_pTutorial, m_pScrollUI, pTex);	// チュートリアル表示関数
		break;
	}
}

//============================================
// 描画
//============================================
void CTitle::Draw(void)
{

}

//============================================
// チュートリアル画面表示処理
//============================================
void CTitle::TutorialDisplay(CUi **pTutoUI, CUi **pScr, const CTexture *pTex)
{
	switch (m_TutoPage)
	{
		//---------------------------------
		// ページ1
		//---------------------------------
	case TUTORIAL_PAGE_01:
		// 次のページへ
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ENTER) || CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ATTACK))
		{
			// 決定音
			CManager::GetInstance()->GetSound()->PlaySound(CSound::SOUND_LABEL_SE_DECIDE);

			// ページ2へ
			m_TutoPage = TUTORIAL_PAGE_02;

			// テクスチャ変更
			pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_TUTORIAL02"));

			// スクロールUIの生成
			for (int nCnt = 0; nCnt < MAX_SCROLL_UI; nCnt++)
			{
				pScr[nCnt] = CUi::Create({ SCREEN_CENTER.x,SCREEN_CENTER.y + ((nCnt + 1) * SCROLL_UI_INDENT),0.0f }, SCROLL_UI_SIZE / 1.14f);
				pScr[nCnt]->BindTexture(pTex->GetTexture("TEX_TYPE_CAUTION"));
				pScr[nCnt]->SetTex({ 0.0f,0.0f }, SCROLL_UI_SPLIT);
			}

			KeyExpCreate(&m_pKeyExp, "ＡかＤまたは十字ボタン横でキーの説明が出ます");							// WASD説明

		}
		break;

		//---------------------------------
		// ページ2
		//---------------------------------
	case TUTORIAL_PAGE_02:

		// どのキー説明か切り替える
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_LEFT) || CInputManager::TriggerAnyAction(CInputManager::TRIGGER_RIGHT))
		{
			if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_LEFT))
			{
				CursorMove(m_nSelectKey, OPR_INSTR_06, OPR_INSTR_01, false);
			}

			else if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_RIGHT))
			{
				CursorMove(m_nSelectKey, OPR_INSTR_01, OPR_INSTR_06, true);
			}

			// テクスチャ変更
			switch (m_nSelectKey)
			{
			case OPR_INSTR_01:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_KEY_SELECT01"));	// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "ＷＡＳＤ：移動＆選択肢切り替え");			// WASD説明
				break;

			case OPR_INSTR_02:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_KEY_SELECT02"));	// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "ＳＰＡＣＥ：ジャンプ");					// SPACEキー説明
				break;

			case OPR_INSTR_03:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_KEY_SELECT04"));	// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "Ｐキー：ポーズ画面を開く＆閉じる");		// ポーズ説明
				break;

			case OPR_INSTR_04:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_KEY_SELECT03"));	// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "ＥＮＴＥＲ：決定");						// エンター説明
				break;

			case OPR_INSTR_05:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_KEY_SELECT05"));	// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "左クリック：爆弾を持つ＆投げる");			// 左クリック説明
				break;

			case OPR_INSTR_06:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_KEY_SELECT06"));	// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "マウスカーソル：視点を動かす");			// マウスカーソル移動説明
				break;
			}
		}

		// 次のページへ
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ENTER) || CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ATTACK))
		{
			// 決定音
			CManager::GetInstance()->GetSound()->PlaySound(CSound::SOUND_LABEL_SE_DECIDE);

			// ページ3へ
			m_TutoPage = TUTORIAL_PAGE_03;

			// テクスチャ変更
			pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_TUTORIAL03"));

			m_nSelectKey = OPR_INSTR_07;
			KeyExpCreate(&m_pKeyExp, "ＡかＤまたは十字ボタン横でボタンの説明が出ます");

		}
		break;

		//---------------------------------
		// ページ3
		//---------------------------------
	case TUTORIAL_PAGE_03:
		// どのキー説明か切り替える
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_LEFT) || CInputManager::TriggerAnyAction(CInputManager::TRIGGER_RIGHT))
		{
			if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_LEFT))
			{
				CursorMove(m_nSelectKey, OPR_INSTR_MAX - 1, OPR_INSTR_07, false);
			}

			else if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_RIGHT))
			{
				CursorMove(m_nSelectKey, OPR_INSTR_07, OPR_INSTR_MAX - 1, true);
			}

			// テクスチャ変更
			switch (m_nSelectKey)
			{
			case OPR_INSTR_07:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_PAD_SELECT01"));			// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "左スティック：移動");								// 移動説明
				break;

			case OPR_INSTR_08:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_PAD_SELECT06"));			// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "十字ボタン：選択肢切り替え");						// 選択切り替え説明
				break;

			case OPR_INSTR_09:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_PAD_SELECT02"));			// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "右スティック：視点を動かす");						// 視点移動説明
				break;

			case OPR_INSTR_10:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_PAD_SELECT03"));			// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "ＳＴＡＲＴボタン：ポーズ画面を開く＆閉じる");		// ポーズ説明
				break;

			case OPR_INSTR_11:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_PAD_SELECT04"));			// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "Ｂボタン：ジャンプ");								// ジャンプ説明
				break;

			case OPR_INSTR_12:
				pTutoUI[0]->BindTexture(pTex->GetTexture("TEX_TYPE_PAD_SELECT05"));			// テクスチャ取得
				KeyExpCreate(&m_pKeyExp, "Ａボタン：決定・爆弾を持つ＆投げる");				// 爆弾を持つ説明
				break;
			}
		}

		// チュートリアルを終える
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ENTER) || CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ATTACK))
		{
			// キー説明文字列破棄
			if (m_pKeyExp)
			{
				m_pKeyExp->Uninit();
				m_pKeyExp = nullptr;
			}

			// チュートリアルUIの破棄
			if (pTutoUI[0])
			{
				pTutoUI[0]->Uninit();
				pTutoUI[0] = nullptr;
			}

			// スクロールUIの破棄
			for (int nCnt = 0; nCnt < MAX_SCROLL_UI; nCnt++)
			{
				if (pScr[nCnt])
				{
					pScr[nCnt]->Uninit();
					pScr[nCnt] = nullptr;
				}
			}

			// タイトルページ2に戻る
			m_Page = PAGE_02;
			m_TutoPage = TUTORIAL_PAGE_01;
		}
		break;
	}

	// スクロール用UIのスクロール処理
	for (int nCnt = 0; nCnt < MAX_SCROLL_UI; nCnt++)
	{
		if (pScr[nCnt])
		{
			switch (nCnt)
			{
			case 0:
				pScr[nCnt]->SetTex(SCROLL_SPEED, 0.0f);
				break;

			case 1:
				pScr[nCnt]->SetTex(-SCROLL_SPEED, 0.0f);
				break;
			}
		}
	}

}

//============================================
// カーソル移動処理
//============================================
void CTitle::CursorMove(int &nNowSel, const int &nFirst, const int &nLast, const bool &bMinus)
{
	// カーソル音
	CManager::GetInstance()->GetSound()->PlaySound(CSound::SOUND_LABEL_SE_CURSOR);

	if (!bMinus)
	{
		// 選択肢をずらす
		if (nNowSel > nLast)
		{
			nNowSel--;
		}
		else
		{
			nNowSel = nFirst;
		}
	}

	else
	{
		// 選択肢をずらす
		if (nNowSel < nLast)
		{
			nNowSel++;
		}
		else
		{
			nNowSel = nFirst;
		}
	}
}

//============================================
// キー説明文字列生成関数
//============================================
void CTitle::KeyExpCreate(CStringManager **ppKeyExp, const char *cStr, const int& nFontSize)
{
	// 文字列が残っているなら破棄してから生成
	if (ppKeyExp[0])
	{
		ppKeyExp[0]->Uninit();
		delete ppKeyExp[0];
		ppKeyExp[0] = nullptr;
	}

	// 文字列生成
	if (!ppKeyExp[0])
	{
		ppKeyExp[0] = CStringManager::Create(KEY_EXP_POS, static_cast<float>(nFontSize), nFontSize, cStr, KEY_EXP_FONTNAME);
	}
}

//============================================
// チュートリアル生成関数
//============================================
void CTitle::TutoAppear(void)
{
	// キーボード取得
	CInputkeyboard *pKey = CManager::GetInstance()->GetKeyboard();

	// 選択切り替え
	if (m_pTuto)
	{
		if (pKey->GetTrigger(DIK_BACKSPACE))
		{
			if (m_nSelTuto > 0)
			{
				m_nSelTuto--;
			}
		}

		if (pKey->GetTrigger(DIK_RETURN))
		{
			if (m_nSelTuto < 2)
			{
				m_nSelTuto++;
			}
			else
			{
				if (CManager::SetModeBool(CManager::MODE::GAME))
				{
					CSound *pSound = CManager::GetInstance()->GetSound();
					pSound->PlaySound(CSound::SOUND_LABEL_SE_START);
					m_bOPRLock = true;	// 操作をロック
				}
			}
		}

		// 画像切り替え
		if (pKey->GetTrigger(DIK_RETURN) || pKey->GetTrigger(DIK_BACKSPACE))
		{
			switch (m_nSelTuto)
			{
			case 0:
				m_pTuto->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_TUTORIAL_SELECT00"));
				break;

			case 1:
				m_pTuto->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_TUTORIAL_SELECT01"));
				break;

			case 2:
				m_pTuto->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_TUTORIAL_SELECT02"));
				break;
			}
		}
	}

	// 始めに生成するときはこっち
	else
	{
		m_pTuto = 0;
		m_pTuto = CUi::Create(SCREEN_CENTER, { 1000.0f,600.0f,0.0f }, CUi::UI_TYPE_NONE);
		m_pTuto->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_TUTORIAL_SELECT00"));
	}
}