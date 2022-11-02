//=====================================
// チュートリアル
// Author:村元翼
//=====================================
#include "Tutorial.h"
#include "bg.h"
#include "stage.h"
#include "camera.h"
#include "manager.h"
#include "InputManager.h"
#include "StringManager.h"

//=====================================
// チュートリアル案
// ページ1にプレイヤーの説明
// ページ2にゲートとジャンプサークルの説明
// 上半分にチュートリアルで操作をさせる場所を作り、
// 下半分に流れるメッセージを作る。
//=====================================

//=====================================
// マクロ定義
//=====================================
#define EXP_POS			(D3DXVECTOR3(SCREEN_WIDTH/2,SCREEN_HEIGHT/1.5f,0.0f))
#define EXP_FONTNAME	("HG創英角ﾎﾟｯﾌﾟ体")

//=====================================
// コンストラクタ
//=====================================
CTutorial::CTutorial()
{
	m_Exp = EXP_01;
	m_pPlayer[TYPE_MAX] = { nullptr };
	m_pExpStr = nullptr;
	m_pStage = nullptr;
}

//=====================================
// デストラクタ
//=====================================
CTutorial::~CTutorial()
{

}

//=====================================
// 初期化
//=====================================
HRESULT CTutorial::Init()
{
	CCamera *pCamera = CManager::GetInstance()->GetCamera(0);
	pCamera->SetRot({ D3DX_PI/2,0.0f,0.0f });
	pCamera->NoFollowingPlayer();

	// ステージ生成
	if (!m_pStage)
	{
		if (m_pStage = new CStage)
		{
			m_pStage->Init();
			m_pStage->SetStage("data/TEXT/TutorialSet000.txt");
		}
	}

	for (int nCnt = 0; nCnt < TYPE_MAX; nCnt++)
	{
		if (!m_pPlayer[nCnt])
		{
			bool bMove = false;
			CPlayer::PLAYER_STATE State;

			switch (nCnt)
			{
			case 0:
				State = CPlayer::PLAYER_STATE::CUBE;
				bMove = true;
				break;

			case 1:
				State = CPlayer::PLAYER_STATE::BALL;
				break;

			case 2:
				State = CPlayer::PLAYER_STATE::AIRSHIP;
				break;

			case 3:
				State = CPlayer::PLAYER_STATE::UFO;
				break;
			}

			// プレイヤー生成
			m_pPlayer[nCnt] = CPlayer::Create({ -450.0f + (300.0f * nCnt),100.0f,0.0f }, { 0.0f,0.0f,0.0f }, State, bMove,false);
		}
	}

	return S_OK;
}

//=====================================
// 終了
//=====================================
void CTutorial::Uninit()
{
	// 文字列破棄
	if (m_pExpStr)
	{
		m_pExpStr->Uninit();
		delete m_pExpStr;
		m_pExpStr = nullptr;
	}

	// ステージ破棄
	if (m_pStage)
	{
		m_pStage->Uninit();
		delete m_pStage;
		m_pStage = nullptr;
	}

	// プレイヤー破棄
	for (int nCnt = 0; nCnt < TYPE_MAX; nCnt++)
	{
		if (m_pPlayer[nCnt])
		{
			m_pPlayer[nCnt]->Uninit();
			m_pPlayer[nCnt] = nullptr;
		}
	}
}

//=====================================
// 更新
//=====================================
void CTutorial::Update()
{
	if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_RIGHT))
	{
		ExpChange(m_Exp + 1);
	}

	else if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_LEFT))
	{
		ExpChange(m_Exp - 1);
	}
}

//=====================================
// 描画
//=====================================
void CTutorial::Draw()
{

}

//=====================================
// 説明番号管理
//=====================================
void CTutorial::ExpManager(void)
{

}

//=====================================
// 説明状態変化
//=====================================
void CTutorial::ExpChange(int nExp)
{
	if (m_Exp != nExp)
	{
		m_pPlayer[m_Exp]->SetMoveBool(false);
		if (m_pPlayer[m_Exp]->GetReverse())	// 重力反転していたら元に戻す
		{
			m_pPlayer[m_Exp]->SetReverse();
		}

		if (nExp < EXP_01)
		{
			nExp = EXP_04;
		}

		else if (nExp > EXP_04)
		{
			nExp = EXP_01;
		}

		m_Exp = static_cast<CHARA_EXP>(nExp);
		m_pPlayer[m_Exp]->SetMoveBool(true);

		switch (m_Exp)
		{
		case EXP_01:
			KeyExpCreate(&m_pExpStr, "プレイヤー説明1");
			break;

		case EXP_02:
			KeyExpCreate(&m_pExpStr, "プレイヤー説明2");
			break;

		case EXP_03:
			KeyExpCreate(&m_pExpStr, "プレイヤー説明3");
			break;

		case EXP_04:
			KeyExpCreate(&m_pExpStr, "プレイヤー説明4");
			break;
		}
	}
}

//=====================================
// 文字列生成
//=====================================
void  CTutorial::KeyExpCreate(CStringManager **ppKeyExp, const char *cStr, const int& nFontSize)
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
		ppKeyExp[0] = CStringManager::Create(EXP_POS, static_cast<float>(nFontSize), nFontSize, cStr, EXP_FONTNAME);
	}
}
