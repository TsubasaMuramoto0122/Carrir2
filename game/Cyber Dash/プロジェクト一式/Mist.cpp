//========================================
// ミストの生成
// Author:村元翼
//========================================
#include "Mist.h"
#include "FadePolygon.h"

//========================================
// マクロ定義
//========================================
#define MAX_INIT_FADE_SIZE (3.0f)	// 最大初期フェードポリゴンサイズ

//========================================
// コンストラクタ
//========================================
CMist::CMist(int nMaxFrame,int nMaxCount, OBJTYPE nPriority) : CScene(nPriority),
															   m_nMaxFrame(nMaxFrame),	// 最大フレーム初期化
															   m_nMaxCount(nMaxCount)	// 最大カウント初期化
{
	m_RangeMin = { 0.0f,0.0f,0.0f };		// 範囲最小
	m_RangeMax = { 0.0f,0.0f,0.0f };		// 範囲最大
	m_nFrame = m_nMaxFrame;					// フレーム
	m_nCount = 0;							// カウント
}

//========================================
// デストラクタ
//========================================
CMist::~CMist() 
{

}

//========================================
// オブジェクト生成
//========================================
CMist *CMist::Create(D3DXVECTOR3 RangeMin, D3DXVECTOR3 RangeMax, int nMaxFrame, int nMaxCount,bool bInfinite)
{
	// オブジェクト生成
	CMist *pMist = nullptr;
	if (pMist = new CMist(nMaxFrame, nMaxCount))	// 最大値をコンストラクタで初期化
	{
		pMist->m_bInfinite = bInfinite;		// 無限生成判定
		pMist->Init(RangeMin, RangeMax);	// 初期化
	}

	return pMist;
}

//========================================
// 初期化
//========================================
HRESULT CMist::Init(D3DXVECTOR3 RangeMin, D3DXVECTOR3 RangeMax)
{
	m_RangeMin = RangeMin;	// 最小範囲の取得
	m_RangeMax = RangeMax;	// 最大範囲の取得
	return S_OK;
}

//========================================
// 終了
//========================================
void CMist::Uninit(void)
{
	// このオブジェクトを破棄
	Release();
}

//========================================
// 更新
//========================================
void CMist::Update(void)
{
	// 最大カウント数まで生成したら終了
	if (m_nCount > m_nMaxCount)
	{
		Uninit();
	}

	// フレームカウント
	m_nFrame++;

	// フェードするポリゴンを指定範囲内に生成
	if (m_nFrame > m_nMaxFrame)
	{
		mt19937_64	  mt(m_Randseed());											// 疑似乱数を作り出す
		uniform_real_distribution<float> DistX(m_RangeMin.x, m_RangeMax.x);		// 指定した範囲内の数値を等確率で返す(最小値以上、最大値"未満")
		uniform_real_distribution<float> DistY(m_RangeMin.y, m_RangeMax.y);		
		uniform_real_distribution<float> DistInitSize(1.0f, MAX_INIT_FADE_SIZE);

		CFadePolygon::Create({ DistX(mt),DistY(mt),0.0f }, { 1.0f,1.0f,0.0f }, DistInitSize(mt));
		m_nFrame = 0;	// フレームリセット

		// 無限判定off
		if (!m_bInfinite)
		{
			m_nCount++;	// 生成数をカウント
		}
	}
}

//========================================
// 描画
//========================================
void CMist::Draw(void)
{

}
