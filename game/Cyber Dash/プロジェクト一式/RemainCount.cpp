//=============================================================================
//
// 死亡回数カウントのクラス [RemainCount.cpp]
// Author : 村元翼
//
//============================================================================
#include "RemainCount.h"
#include "number.h"
#include "fade.h"
#include "Ui.h"
#include "StringManager.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_DEATH (999)	// 死亡回数限界値

//=============================================================================
// 静的メンバ変数の初期化
//=============================================================================
int		CRemainCount::m_nRemain = 0;			// 残機
bool	CRemainCount::m_bChange = false;		// 残機を変化させるか

//=============================================================================
// コンストラクタ
//=============================================================================
CRemainCount::CRemainCount(OBJTYPE nPriority) : CScene(nPriority)
{
	memset(m_apNumber, 0, sizeof(m_apNumber));			// 数字クラス
	m_pos = { 0.0f,0.0f,0.0f };							// 位置
	m_size = { 0.0f,0.0f,0.0f };						// 大きさ
	m_col = { 0.0f,0.0f,0.0f,0.0f };					// カラー
	m_bFade = false;									// フェードするか
	m_bChange = false;									// カウントする処理を行うか
	m_nFrame = 0;										// フレーム
}

//=============================================================================
// デストラクタ
//=============================================================================
CRemainCount::~CRemainCount()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CRemainCount::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// 位置・大きさの設定・取得
	m_pos = pos;
	m_size = size;
	CScene::SetPos(m_pos);
	CScene::SetSize(m_size);

	// 残機は100よりは超えない
	if (m_nRemain > MAX_DEATH)
	{
		m_nRemain = MAX_DEATH;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CRemainCount::Uninit(void)
{
	// 2Dポリゴンの破棄
	for (int nCnt = 0; nCnt < MAX_REMAIN_DIGIT; nCnt++)
	{
		if (m_apNumber[nCnt])
		{
			m_apNumber[nCnt]->Uninit();
			m_apNumber[nCnt] = nullptr;
		}
	}

	// オブジェクトの破棄
	Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void CRemainCount::Update(void)
{
	// 残機が減る
	if (m_bChange)
	{
		RemainChange();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CRemainCount::Draw(void)
{
	// 2Dポリゴンの描画
	for (int nCnt = 0; nCnt < MAX_REMAIN_DIGIT; nCnt++)
	{
		if (m_apNumber[nCnt])
		{
			m_apNumber[nCnt]->Draw();
		}
	}
}

//=============================================================================
// 時間の生成
//=============================================================================
CRemainCount *CRemainCount::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// 時間のインスタンス生成
	CRemainCount *pRemainCount = new CRemainCount;

	// 時間の準備
	if (pRemainCount)
	{
		pRemainCount->Init(pos, size);			// 初期化
	}

	return pRemainCount;
}

//=============================================================================
// 時間の生成
//=============================================================================
void CRemainCount::RemainChange(void)
{
	// 死亡回数文字表示
	if (m_nFrame == 0)
	{
		// 文字列生成
		CStringManager::Create({ m_pos.x - (m_size.x * 1.5f), m_pos.y - (m_size.y / 2), 0.0f }, m_size.y, (int)m_size.y, "死亡回数", "HGP創英ﾌﾟﾚｾﾞﾝｽEB");

		// 変化前の残機
		for (int nCnt = 0; nCnt < MAX_REMAIN_DIGIT; nCnt++)
		{
			if (!m_apNumber[nCnt])
			{
				m_apNumber[nCnt] = CNumber::Create({ m_pos.x + (m_size.x * nCnt), m_pos.y, 0.0f }, { m_size.x, m_size.y, 0.0f });
				m_apNumber[nCnt]->SetNumber((m_nRemain) % (int)pow(10, MAX_REMAIN_DIGIT - nCnt) / (int)pow(10, (MAX_REMAIN_DIGIT - 1) - nCnt));
			}
		}
		m_nRemain++;	// 死亡回数カウントを増やす
	}

	// 死亡回数カウントを表示
	else if (m_nFrame == COUNT_CHANGE_FRAME)
	{
		// 変化後の残機
		for (int nCnt = 0; nCnt < MAX_REMAIN_DIGIT; nCnt++)
		{
			if (m_apNumber[nCnt])
			{
				m_apNumber[nCnt]->SetNumber(m_nRemain % (int)pow(10, MAX_REMAIN_DIGIT - nCnt) / (int)pow(10, (MAX_REMAIN_DIGIT - 1) - nCnt));
			}
		}
	}

	// ゲームをやりなおす
	else if (m_nFrame == COUNT_CHANGE_FRAME * 2)
	{
		// フェードの取得
		CFade *pFade = CManager::GetInstance()->GetFade();

		CManager::GetInstance()->SetStop(true);
		m_bChange = false;

		pFade->SetFade(CManager::MODE::GAME);
	}

	m_nFrame++; // フレームを進める
}
