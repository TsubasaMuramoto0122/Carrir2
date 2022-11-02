//===========================================
// ゴール地点のモデル
// Author:村元翼
//===========================================
#include "Goal.h"
#include "model.h"
#include "manager.h"
#include "renderer.h"
#include "fade.h"
#include "Ui.h"
#include "ClearGauge.h"

//===========================================
// コンストラクタ
//===========================================
CGoal::CGoal(OBJTYPE nPriority) : CModel_Spawner(nPriority)
{
	m_scale = { 0.0f,0.0f,0.0f };	// スケール
	m_pUI = nullptr;				// UI
	m_nFrame = 0;					// フレーム
}

//===========================================
// デストラクタ
//===========================================
CGoal::~CGoal()
{

}

//===========================================
// 生成関数
//===========================================
CGoal *CGoal::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nXType)
{
	// デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// インスタンス生成
	CGoal *pGoal = nullptr;

	// NULLチェック
	if (pGoal = new CGoal())
	{
		pGoal->m_scale = scale;		// スケール
		pGoal->Init(pos, rot);		// 初期化

		if (!pGoal->m_pModel)
		{
			// モデルの生成
			pGoal->m_pModel = CModel::Create({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, scale, nXType);
			// 親モデルをNULLに設定
			pGoal->m_pModel->SetParent(NULL);
			// モデルタイプの設定
			pGoal->CScene::SetModelType(CScene::MODTYPE_GOAL);
			// シングルモデルサイズの設定
			pGoal->SetSize(pGoal->m_pModel->GetSize());
		}
	}

	return pGoal;
}


//===========================================
// 初期化関数
//===========================================
HRESULT CGoal::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// モデル初期化
	CModel_Spawner::Init(pos, rot);
	return S_OK;
}

//===========================================
// 終了関数
//===========================================
void CGoal::Uninit(void)
{
	// モデル終了
	CModel_Spawner::Uninit();

	// UI削除
	if(m_pUI)
	{ 
		m_pUI->Uninit();
		delete m_pUI;
		m_pUI = nullptr;
	}
}

//===========================================
// 更新関数
//===========================================
void CGoal::Update(void)
{
	// モデル更新
	if (m_pModel)
	{
		m_pModel->Update();

		if (m_pModel->GetPlayerHitBool() || CClearGauge::GetClear())		// ゴールに当たるとリザルトにいく
		{
			// ゴールメッセージの表示
			if (m_nFrame == 0)
			{
				m_pUI = CUi::Create(SCREEN_CENTER, { 3.0f, 1.0f, 0.0f }, CUi::UI_TYPE_ZOOM);
				m_pUI->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_END"));
			}

			// リザルトへ行く
			if (m_nFrame == COUNT_CHANGE_FRAME)
			{
				CManager::GetInstance()->SetStop(true);
				CManager::GetInstance()->GetFade()->SetFade(CManager::MODE::RESULT);
				
			}

			// フレームを進める
			m_nFrame++;
		}
	}
}

//===========================================
// 描画関数
//===========================================
void CGoal::Draw(void)
{
	// デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 現在のマテリアルを取得
	D3DXMATRIX mtxRotModel, mtxTransModel;	// 計算用マトリックス
	D3DXMATRIX mtxParent;					// 親のマトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRotModel, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRotModel);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTransModel, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTransModel);

	// 各パーツのワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// モデルの描画
	if (m_pModel)
	{
		m_pModel->Draw();
	}
}