//========================================
// 爆風エフェクト(メッシュ球)の作成
// Author : 村元翼
//========================================
#include "effect_blast.h"
#include "model.h"
#include "manager.h"
#include "renderer.h"
#include "effect.h"
#include "LoadX.h"
#include "meshsphere.h"
#include "score.h"
#include "game.h"

//===========================================
// マクロ定義
//===========================================
#define BLAST_SCALE_SPEED		(1.1f)	// 爆風スケール速度
#define BLAST_ROLLING_SPEED		(0.5f)	// 爆風回転速度
#define EXPLOSION_SCORE			(100)	// 爆破スコア
#define START_TRANSPARENT_FRAME (60)	// 透明になり始めるフレーム数
#define ALPHA_SUB				(0.05f) // α値減少量

//=============================================================================
// コンストラクタ
//=============================================================================
CBlast::CBlast(OBJTYPE nPriority) : CScene(nPriority)
{
	memset(&m_pSphere, 0, sizeof(m_pSphere));				// 球クラス
	m_pos			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
	m_rot			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 回転
	m_size			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// サイズ
	m_col			= D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);	// 色
	m_nFrame		= 0;									// フレーム数
	m_scale			= 0.0f;									// スケール
	m_fScalingSpeed = BLAST_SCALE_SPEED;					// スケール変更速度
}

//=============================================================================
// デストラクタ
//=============================================================================
CBlast::~CBlast()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CBlast::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	m_pos = pos;		// 位置
	m_size = size;		// サイズ
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CBlast::Uninit(void)
{
	// メッシュ球破棄
	if (m_pSphere)
	{
		// 死亡判定
		if (!m_pSphere->GetDeath())
		{
			m_pSphere->Uninit();
			m_pSphere = nullptr;
		}
	}

	// オブジェクトの破棄
	Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void CBlast::Update(void)
{
	// 爆発中の処理
	if (m_pSphere)
	{
		Spread();	// 爆風広がり
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CBlast::Draw(void)
{

}

//=============================================================================
// 爆風の広がり方の処理
//=============================================================================
void CBlast::Spread(void)
{
	// ネクスト減少量
	float Nextsub = m_fScalingSpeed;

	// フレームカウント
	m_nFrame++;
	if (m_nFrame >= START_TRANSPARENT_FRAME)
	{
		m_col.a -= ALPHA_SUB;	// α値を下げる
	}
	else
	{
		m_scale += m_fScalingSpeed;			// サイズを大きくする
		m_fScalingSpeed -= Nextsub * 0.1f;	// 大きさを変更するスピードを10/1にする
	}

	m_rot.y += BLAST_ROLLING_SPEED;			// 回転する

	// 回転・大きさ・カラーを設定
	m_pSphere->SetRot(m_rot);
	m_pSphere->SetScale({ m_scale,m_scale ,m_scale });
	m_pSphere->CMesh::SetCol(m_pSphere,m_col);

	// 完全に透明になるまで終了しない
	if (m_col.a <= 0.0f)
	{
		Uninit();
	}
}

//=============================================================================
// インスタンス生成処理
//=============================================================================
CBlast *CBlast::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col, int nLine, int nVertical)
{
	// オブジェクト生成
	CBlast *pBlast = nullptr;
	if (pBlast = new CBlast())
	{
		pBlast->m_col = col;					// カラー
		pBlast->Init(pos, size);				// 初期化

		if (!pBlast->m_pSphere)
		{
			// 爆弾モデルの生成
			pBlast->m_pSphere = CMeshSphere::Create(pos, size, { 0.0f,0.0f,0.0f }, nLine, nVertical, false, CScene::OBJTYPE_EFFECTMESH);
		}

	}
	return pBlast;
}