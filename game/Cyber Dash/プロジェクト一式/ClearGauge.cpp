//============================================================
// クリアゲージの作成
// Author:村元翼
//============================================================
#include "ClearGauge.h"
#include "number.h"
#include "stage.h"
#include "Goal.h"
#include "game.h"
#include "StringManager.h"
#include "Ui.h"
#include "meshfield.h"

//============================================================
// マクロ定義
//============================================================
#define GAUGE_FRAME_OFFSET		(D3DXVECTOR3(100.0f,0.0f,0.0f))
#define GAUGE_UI_SIZE			(D3DXVECTOR3(m_size.x * 8.0f,m_size.y * 2.0f,0.0f))
#define GAUGE_MAX_LENGTH		(m_GaugeUI.size.x - 30.0f)
#define GAUGE_OFFSET_X			(10.0f)
#define MESH_SPLIT_SIZE			(250.0f)
#define CLEAR_GAUGE				(100)

//============================================================
// 静的メンバ変数の初期化
//============================================================
bool CClearGauge::m_bClear = false;		// クリア判定

//============================================================
// コンストラクタ
//============================================================
CClearGauge::CClearGauge(OBJTYPE nPriority) :CScene(nPriority)
{
	memset(&m_GaugeUI, 0, sizeof(m_GaugeUI));		// ゲージUI
	memset(m_apPercent, 0, sizeof(m_apPercent));	// パーセント数字
	m_pos = { 0.0f,0.0f,0.0f };						// 位置
	m_size = { 0.0f,0.0f,0.0f };					// 大きさ
	m_ClearLength = { 0.0f,0.0f,0.0f };				// クリアゲージの長さ
	m_nPercent = 0;									// パーセント
}

//============================================================
// デストラクタ
//============================================================
CClearGauge::~CClearGauge()
{

}

//============================================================
// オブジェクト生成
//============================================================
CClearGauge *CClearGauge::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// オブジェクト生成
	CClearGauge *pClearGauge = nullptr;
	if (pClearGauge = new CClearGauge)
	{
		pClearGauge->Init(pos, size);
	}

	return pClearGauge;
}

//============================================================
// 初期化
//============================================================
HRESULT CClearGauge::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// 位置・サイズ設定
	m_pos = pos;
	m_size = size;
	CScene::SetPos(m_pos);
	CScene::SetSize(m_size);

	// UIの各変数初期化
	m_GaugeUI.posFrame	= m_pos - GAUGE_FRAME_OFFSET;
	m_GaugeUI.posGauge	= { m_GaugeUI.posFrame.x + GAUGE_OFFSET_X, m_GaugeUI.posFrame.y,0.0f };
	m_GaugeUI.size		= GAUGE_UI_SIZE;
	m_GaugeUI.MaxLength = GAUGE_MAX_LENGTH;

	// ゲージUIの生成
	if (!m_GaugeUI.pFrame && !m_GaugeUI.pGauge)
	{
		m_GaugeUI.pGauge = CUi::Create(m_pos, m_size, CUi::UI_TYPE_NONE);
		m_GaugeUI.pFrame = CUi::Create(m_pos, m_size, CUi::UI_TYPE_NONE);

		m_GaugeUI.pGauge->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_GAUGE01"));
		m_GaugeUI.pFrame->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_GAUGE_FRAME01"));

		m_GaugeUI.pGauge->SetScalePos(m_GaugeUI.posGauge, m_GaugeUI.size);
		m_GaugeUI.pFrame->SetScalePos(m_GaugeUI.posFrame, m_GaugeUI.size);
	}

	// パーセント文字を生成
	CStringManager::Create({ m_pos.x + (m_size.x * (MAX_PERCENT_DIGIT + 1)), m_pos.y - 20.0f, 0.0f }, m_size.x, (int)m_size.x, "％", "ＭＳゴシック");

	// 数字を生成
	for (int nCnt = 0; nCnt < MAX_PERCENT_DIGIT; nCnt++)
	{
		m_apPercent[nCnt] = CNumber::Create({ m_pos.x + (m_size.x * nCnt), m_pos.y, 0.0f }, m_size);
	}

	// ?@スタート地点とゴール地点の間の距離を測る
	CStage *pStage = CGame::GetStage();
	CGoal *pGoal = pStage->GetGoal();
	m_ClearLength = (pStage->GetGoalPos() - (pGoal->GetSize() * pGoal->GetScale().x)) - pStage->GetStartPos();

	// メッシュ分割数
	int nLine = 0;
	nLine = (int)(m_ClearLength.x / MESH_SPLIT_SIZE);
	if (nLine <= 0)	// 0以下はメッシュが描画されない
	{
		nLine = 1;
	}

	// スタートからゴールまでのフィールドを生成する
	CMeshField::Create({ m_ClearLength.x / 2 + pStage->GetStartPos().x ,0.0f,0.0f }, { m_ClearLength.x - (pStage->GetStartPos().x/2),0.0f,1000.0f }, { 0.0f,0.0f,0.0f }, nLine, 3);
	CMeshField::Create({ m_ClearLength.x / 2 + pStage->GetStartPos().x ,1000.0f,0.0f }, { m_ClearLength.x - (pStage->GetStartPos().x / 2),0.0f,1000.0f }, { 180.0f,0.0f,0.0f }, nLine, 3);

	return S_OK;
}

//============================================================
// 終了
//============================================================
void CClearGauge::Uninit()
{
	// 数字の破棄
	for (int nCnt = 0; nCnt < MAX_PERCENT_DIGIT; nCnt++)
	{
		if (m_apPercent[nCnt])
		{
			m_apPercent[nCnt]->Uninit();
			m_apPercent[nCnt] = nullptr;
		}
	}

	// ゲージUIの破棄
	if (m_GaugeUI.pGauge)
	{
		m_GaugeUI.pGauge->Uninit();
		m_GaugeUI.pGauge = nullptr;
	}

	// ゲージフレームUIの破棄
	if (m_GaugeUI.pFrame)
	{
		m_GaugeUI.pFrame->Uninit();
		m_GaugeUI.pFrame = nullptr;
	}

	// このオブジェクトを破棄
	Release();
}

//============================================================
// 更新
//============================================================
void CClearGauge::Update()
{
	// パーセントゲージの計算関数
	SetPercent();
	
	if (m_nPercent >= CLEAR_GAUGE)
	{
		m_bClear = true;
	}
}

//============================================================
// 描画
//============================================================
void CClearGauge::Draw()
{

}

//============================================================
// クリアゲージのパーセント管理
//============================================================
void CClearGauge::SetPercent(void)
{
	if (CScene::GetScene(OBJTYPE_PLAYER) && m_nPercent < CLEAR_GAUGE)
	{
		// プレイヤーの現在位置を取る
		D3DXVECTOR3 CurrentPos = CScene::GetScene(OBJTYPE_PLAYER)->GetPos()- CStage::GetStartPos();

		// 距離の内どの位置にいるかの割合を求める(Xの距離のみ)
		float fRatio = CurrentPos.x / m_ClearLength.x;

		// 百分率に直す
		m_nPercent = (int)(fRatio * CLEAR_GAUGE);

		// ゲージの長さを計算する
		float fGaugePer = (m_GaugeUI.MaxLength / CLEAR_GAUGE) * m_nPercent;

		// ゲージUIの長さを設定
		if (fGaugePer <= m_GaugeUI.MaxLength)
		{
			m_GaugeUI.pGauge->SetScalePos(m_GaugeUI.posGauge, { fGaugePer ,m_GaugeUI.size.y,0.0f });
		}

		// 計算後の数字を設定
		for (int nCnt = 0; nCnt < MAX_PERCENT_DIGIT; nCnt++)
		{
			if (m_apPercent[nCnt])
			{
				m_apPercent[nCnt]->SetNumber(m_nPercent % (int)pow(10, MAX_PERCENT_DIGIT - nCnt) / (int)pow(10, (MAX_PERCENT_DIGIT - 1) - nCnt));
			}
		}
	}
}