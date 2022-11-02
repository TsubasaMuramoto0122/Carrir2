//===========================================
// ジャンプサークルの作成
// Author;村元翼
//===========================================
#include "JumpCircle.h"
#include "texture.h"
#include "effect.h"
#include "player.h"
#include "InputManager.h"

//===========================================
// マクロ定義
//===========================================
#define JUMP_POW	(15.0f)											// ジャンプ力
#define YELLOW		(D3DXCOLOR({ 1.0f,1.0f,1.0f,1.0f }))			// 黄色
#define RED			(D3DXCOLOR({ 1.0f,0.0f,0.0f,1.0f }))			// 赤色
#define GREEN		(D3DXCOLOR({ 0.0f,1.0f,0.0f,1.0f }))			// 緑色
#define BLACK		(D3DXCOLOR({ 0.0f,0.0f,0.0f,1.0f }))			// 黒色

//===========================================
// 静的メンバ変数の初期化
//===========================================
float CJumpCircle::m_fJumpPower = JUMP_POW;		// ジャンプ力を決定

//===========================================
// コンストラクタ
//===========================================
CJumpCircle::CJumpCircle(OBJTYPE nPriority) : CBillboard(nPriority)
{

}

//===========================================
// デストラクタ
//===========================================
CJumpCircle::~CJumpCircle()
{

}

//===========================================
// 生成
//===========================================
CJumpCircle *CJumpCircle::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, CIRCLE_TYPE type)
{
	CJumpCircle *pJumpCircle = nullptr;
	if (pJumpCircle = new CJumpCircle)
	{
		pJumpCircle->m_type = type;		// タイプ
		pJumpCircle->Init(pos, size);	// 初期化
	}

	return pJumpCircle;
}

//===========================================
// 初期化
//===========================================
HRESULT CJumpCircle::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// ビルボード初期化
	CBillboard::Init(pos, size);
	BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_JUMPCIRCLE"));

	// タイプ別にカラーを変更
	switch (m_type)
	{
	case CIRCLE_TYPE::JUMP:
		m_col = YELLOW;
		break;
			
	case CIRCLE_TYPE::QUICK_REV:
		m_col = RED;
		break;

	case CIRCLE_TYPE::SLOW_REV:
		m_col = GREEN;
		break;

	case CIRCLE_TYPE::FALL:
		m_col = BLACK;
		break;
	}

	return S_OK;
}

//===========================================
// 終了
//===========================================
void CJumpCircle::Uninit(void)
{
	CBillboard::Uninit();
}

//===========================================
// 更新
//===========================================
void CJumpCircle::Update(void)
{
	CBillboard::Update();

	// エフェクト生成
	CEffect::Create(m_pos, m_size / 2, { 1.0f,1.0f,0.0f,1.0f }, 0.5f, 3);

	// 円の当たり判定
	CircleCollision();
}

//===========================================
// 描画
//===========================================
void CJumpCircle::Draw(void)
{
	CBillboard::Draw();
}

//===========================================
// 当たり判定(円と円)
//===========================================
void CJumpCircle::CircleCollision(void)
{
	CScene *pScene = CScene::GetScene(OBJTYPE_PLAYER);

	while (pScene)
	{
		CScene *pSceneNext = CScene::GetSceneNext(pScene);

		D3DXVECTOR3 pos = m_pos;			// 原点
		float Rad = m_size.y ;				// 半径

		D3DXVECTOR3 pos2 = pScene->GetPos();
		float Rad2 = pScene->GetSize().y;
		CPlayer *pPlayer = (CPlayer*)pScene;

		// 円の中にいるかを判定する
		if (powf(Rad + Rad2, 2) >= powf(pos.x - pos2.x, 2) + powf(pos.y - pos2.y, 2))
		{
			if (!pPlayer->GetLock())
			{
				pPlayer->SetLock(true);
			}

			// ジャンプボタンを押す
			if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_SPACE))
			{
				// ジャンプしたときのエフェクトを出す
				for (int nCnt = 0; nCnt < 5; nCnt++)
				{
					CEffect::Create(m_pos, m_size * 3, { 1.0f,1.0f,1.0f,1.0f }, 1.0f, 3, "TEX_TYPE_JUMPSTAR");
				}

				// 反転判定
				if (pPlayer->GetReverse())
				{
					m_fJumpPower = -JUMP_POW;
				}
				else
				{
					m_fJumpPower = JUMP_POW;
				}

				// タイプ別ジャンプ処理
				switch (m_type)
				{
				case CIRCLE_TYPE::JUMP:
					Jump(pPlayer);
					break;

				case CIRCLE_TYPE::QUICK_REV:
					Reverse(pPlayer);
					break;

				case CIRCLE_TYPE::SLOW_REV:
					SlowRev(pPlayer);
					break;

				case CIRCLE_TYPE::FALL:
					Fall(pPlayer);
					break;
				}
			}
		}

		else
		{
			if (pPlayer->GetLock())
			{
				pPlayer->SetLock(false);
			}
		}

		pScene = pSceneNext;
	}
}

//===========================================
// ジャンプの処理
//===========================================
void CJumpCircle::Jump(CPlayer *pPlayer)
{
	// プレイヤーの状態を取得
	CPlayer::PLAYER_STATE State = pPlayer->GetState();
	switch (State)
	{
	case CPlayer::PLAYER_STATE::BALL:
		pPlayer->JumpAction(m_fJumpPower);
		break;

	case CPlayer::PLAYER_STATE::CUBE:
		pPlayer->JumpAction(m_fJumpPower);
		break;

	case CPlayer::PLAYER_STATE::AIRSHIP:
		pPlayer->SetSpeed({ pPlayer->GetSpeed().x,50.0f,0.0f });
		break;

	case CPlayer::PLAYER_STATE::UFO:
		pPlayer->JumpAction(m_fJumpPower);
		break;
	}

}

//===========================================
// 重力反転
//===========================================
void CJumpCircle::Reverse(CPlayer *pPlayer)
{
	pPlayer->SetReverse();
	pPlayer->JumpAction(m_fJumpPower);
}

//===========================================
// ゆっくり重力反転
//===========================================
void CJumpCircle::SlowRev(CPlayer *pPlayer)
{
	pPlayer->SetReverse();
	pPlayer->JumpAction(-m_fJumpPower/2);

}

//===========================================
// 急に落ちる
//===========================================
void CJumpCircle::Fall(CPlayer *pPlayer)
{
	pPlayer->JumpAction(-m_fJumpPower);
}