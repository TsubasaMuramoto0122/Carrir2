//==============================================================
// スコアの作成
// Author:村元翼
//==============================================================
#include "main.h"
#include "manager.h"
#include "keyboard.h"
#include "scene.h"
#include "scene2D.h"
#include "score.h"
#include "texture.h"
#include "number.h"
#include "playdata.h"

//==============================================================
// マクロ定義
//==============================================================
#define NUMBER_INTERVAL (50.0f)

//==============================================================
// コンストラクタ
//==============================================================
CScore::CScore(OBJTYPE nPriority) : CScene(nPriority)
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// サイズ
	m_nAddScore = ADD_SCORE;				// 加算するスコアの初期化
	m_nScore = 0;							// スコアの初期化
}

//==============================================================
// デストラクタ
//==============================================================
CScore::~CScore()
{

}

//==============================================================
// スコアの生成
//==============================================================
CScore *CScore::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// インスタンス生成
	CScore *pScore = nullptr;
	if (pScore = new CScore())
	{
		pScore->Init(pos, size);	// 初期化
	}

	return pScore;
}

//==============================================================
// 初期化
//==============================================================
HRESULT CScore::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// ナンバー生成
	for (int nCnt = 0; nCnt < MAX_SCORE; nCnt++)
	{
		m_apNumber[nCnt] = CNumber::Create(D3DXVECTOR3(pos.x + nCnt * NUMBER_INTERVAL, pos.y, pos.z), size);
		m_apNumber[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_NUMBER"));
		m_apNumber[nCnt]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		m_nAll++;
	}

	// 位置・サイズの設定・取得
	m_pos = pos;
	m_size = size;
	SetPos(m_pos);
	SetSize(m_size);

	return S_OK;
}

//==============================================================
// 終了
//==============================================================
void CScore::Uninit()
{
	// ナンバー破棄
	for (int nCnt = 0; nCnt < MAX_SCORE; nCnt++)
	{
		if (m_apNumber[nCnt])
		{
			m_apNumber[nCnt]->Uninit();
			m_apNumber[nCnt] = nullptr;
		}
	}

	CManager::GetPlayData()->SetScore(m_nScore);	// プレイデータにスコアを設定
	m_nAll = 0;										// 数字の数をリセット
	Release();										// このオブジェクトを破棄
}

//==============================================================
// 更新
//==============================================================
void CScore::Update()
{
#if _DEBUG
	// デバッグ用スコア加算
	CInputkeyboard *pKey = CManager::GetInstance()->GetKeyboard();
	if (pKey->GetTrigger(DIK_0))
	{
		AddScore(m_nAddScore);	// スコアの加算
	}
#endif

	// 数字の更新関数
	SetScore();
}

//==============================================================
// 描画
//==============================================================
void CScore::Draw()
{
	for (int nCnt = 0; nCnt < MAX_SCORE; nCnt++)
	{
		if (m_apNumber[nCnt])
		{
			m_apNumber[nCnt]->Draw();
		}
	}
}

//==============================================================
// スコアの設定
//==============================================================
void CScore::SetScore(int nScore)
{
	m_nScore = nScore;
}

//==============================================================
// スコアの設定
//==============================================================
void CScore::SetScore()
{
	for (int nCnt = 0, nDecrement = 1; nCnt < m_nAll; nCnt++, nDecrement *= 10)
	{
		m_apNumber[nCnt]->SetNumber(m_nScore % ((int)pow(10, m_nAll) / nDecrement) / ((int)pow(10, m_nAll - 1) / nDecrement));
	}
}

//==============================================================
// スコア位置設定処理
//==============================================================
void CScore::SetScorePos(D3DXVECTOR3 pos)
{
	// 位置設定
	m_pos = pos;
	for (int nCntNumber = 0; nCntNumber < MAX_SCORE; nCntNumber++)
	{
		// 小数点をつけるため、4つ目の数字からずらす
		m_apNumber[nCntNumber]->SetPos(D3DXVECTOR3(pos.x - (m_size.x * MAX_SCORE / 2) + (m_size.x * nCntNumber) + (m_size.x / 2.0f), pos.y, 0.0f), m_size,false);
	}
	// 位置を設定
	SetPos(m_pos);
}

