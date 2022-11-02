//=============================================
// リザルト画面の処理
// Author:村元翼
//=============================================
#include "result.h"
#include "title.h"
#include "texture.h"
#include "manager.h"
#include "keyboard.h"
#include "ranking.h"
#include "fade.h"
#include "bg.h"
#include "Ui.h"
#include "XInput.h"
#include "DirectInput.h"
#include "InputManager.h"
#include "sound.h"
#include "game.h"
#include "RemainCount.h"

//=============================================
// マクロ定義
//=============================================
#define RETURN_TITLE_TIME (120)

//===============================================
// 静的メンバ変数の初期化
//===============================================
const CResult::SELECT_PARAM CResult::m_SelectParam[RESULT_SELECT_MAX] =
{
	{ { SCREEN_WIDTH -200.0f,450.0f,0.0f }	,{ 200.0f,100.0f,0.0f },"TEX_TYPE_RESULT_SELECT00",CUi::UI_TYPE_NONE },	// セレクト01
	{ { SCREEN_WIDTH -200.0f,550.0f,0.0f }	,{ 200.0f,100.0f,0.0f },"TEX_TYPE_RESULT_SELECT01",CUi::UI_TYPE_NONE },	// セレクト02
	{ { SCREEN_WIDTH -200.0f,650.0f,0.0f }	,{ 200.0f,100.0f,0.0f },"TEX_TYPE_RESULT_SELECT02",CUi::UI_TYPE_NONE }	// セレクト03
};

//=============================================
// コンストラクタ
//=============================================
CResult::CResult()
{
	memset(m_pSelectUI, 0, sizeof(m_pSelectUI));	// 選択UI
	m_pBgResult = nullptr;							// リザルト背景
	m_pReturnTitle = nullptr;						// タイトルに戻るUI
	m_pUi = nullptr;								// UI
	m_nFrame = 0;									// フレーム
	m_nSelectType = 0;								// 選択タイプ
	m_nSelect = 0;									// 選択肢
	m_bOPRLock = false;								// 操作ロック判定
	m_bUnlock = false;								// アンロック判定
}

//=============================================
// デストラクタ
//=============================================
CResult::~CResult()
{
}

//=============================================
// 初期化
//=============================================
HRESULT CResult::Init(void)
{
	// 死亡カウントのリセット
	CManager::GetInstance()->GetGame()->GetRemainCount()->ResetRemain();

	// リザルトの背景生成
	m_pBgResult = CBg::Create();
	m_pBgResult->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));

	// UIの生成
	m_pUi = CUi::Create(D3DXVECTOR3(SCREEN_WIDTH / 2, 100.0f, 0.0f), D3DXVECTOR3(800.0f, 200.0f, 0.0f), CUi::UI_TYPE_NONE);
	m_pUi->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_RESULT"));

	//ランキングの生成
	m_pRanking = CRanking::Create
	(
		D3DXVECTOR3(400.0f + RANKING_POLYGON_WIDTH * MAX_SCORE, RESULT_RANKING_POSY, 0.0f),
		D3DXVECTOR3(RANKING_POLYGON_WIDTH, RANKING_POLYGON_HEIGHT, 0.0f)
	);

	// 選択肢の生成
	CTexture *pTex = CManager::GetInstance()->GetTexture();
	for (int nCnt = 0; nCnt < RESULT_SELECT_MAX; nCnt++)
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

	return S_OK;
}
//=============================================
// 終了
//=============================================
void CResult::Uninit(void)
{
	//------------------------//
	// 各種オブジェクトの破棄 //
	//------------------------//
	for (int nCnt = 0; nCnt < RESULT_SELECT_MAX; nCnt++)
	{
		if (m_pSelectUI[nCnt])
		{
			m_pSelectUI[nCnt]->Uninit();
			m_pSelectUI[nCnt] = nullptr;
		}
	}

	if (m_pUi)
	{
		m_pUi->Uninit();
		m_pUi = nullptr;
	}

	if (m_pRanking)
	{
		m_pRanking->Uninit();
		m_pRanking = nullptr;
	}

	if (m_pBgResult)
	{
		m_pBgResult->Uninit();
		m_pBgResult = nullptr;
	}
}

//=============================================
// 更新
//=============================================
void CResult::Update(void)
{
	CSound *pSound = CManager::GetInstance()->GetSound();

#ifdef _DEBUG
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_R))
	{
		m_pRanking->DeleteRanking();
	}
#endif

	// 決定を押す
	if (!m_bOPRLock)
	{
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ENTER) || CInputManager::TriggerAnyAction(CInputManager::TRIGGER_ATTACK))
		{
			// 決定音
			pSound->PlaySound(CSound::SOUND_LABEL_SE_DECIDE);

			switch (m_nSelect)
			{
			case RESULT_SELECT_01:
				// ゲームモードへ移行
				if (CManager::SetModeBool(CManager::MODE::GAME))
				{
					pSound->PlaySound(CSound::SOUND_LABEL_SE_START);
					m_bOPRLock = true;	// 操作をロック
				}
				break;

			case RESULT_SELECT_02:
					CFade::SetFade(CManager::MODE::TITLE);
					m_bOPRLock = true;	// 操作をロッ
				break;

			case RESULT_SELECT_03:
				// ゲームを終了
				PostQuitMessage(0);
				break;
			}
		}

		// 選択を左へ移動させる
		else if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_UP) || CInputManager::TriggerAnyAction(CInputManager::TRIGGER_DOWN))
		{
			// 現在選択されている選択肢の明るさを下げる
			m_pSelectUI[m_nSelect]->SetCol(NOT_SELECT_COLOR);

			// カーソル移動処理
			if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_UP))
			{
				CursorMove(m_nSelect, RESULT_SELECT_03, RESULT_SELECT_01, false);
			}

			// 選択を右へ移動させる
			else if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_DOWN))
			{
				CursorMove(m_nSelect, RESULT_SELECT_01, RESULT_SELECT_03, true);
			}

			// 移動後の選択肢のカラーを明るさを最大にする
			m_pSelectUI[m_nSelect]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}
}

//=============================================
// 描画
//=============================================
void CResult::Draw(void)
{

}

//============================================
// カーソル移動処理
//============================================
void CResult::CursorMove(int &nNowSel, const int &nFirst, const int &nLast, const bool &bMinus)
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