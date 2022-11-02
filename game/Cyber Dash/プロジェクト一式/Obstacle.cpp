//===========================================
// 障害物モデル
// Author:村元翼
//===========================================
#include "Obstacle.h"
#include "model.h"
#include "manager.h"
#include "renderer.h"
#include "player.h"
#include "fade.h"
#include "Ui.h"
#include "RemainCount.h"
#include "effect_blast.h"
#include "sound.h"

//===========================================
// コンストラクタ
//===========================================
CObstacle::CObstacle(OBJTYPE nPriority) : CModel_Spawner(nPriority)
{
	m_pUI = nullptr;		// UI
	m_nFrame = 0;			// フレーム
	m_bDoOnce = false;		// 一度のみ通らせる変数
	m_bRetry = false;		// リトライ判定
}

//===========================================
// デストラクタ
//===========================================
CObstacle::~CObstacle()
{

}

//===========================================
// オブジェクト生成
//===========================================
CObstacle *CObstacle::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nXType, OBSTACLE obstype)
{
	// デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// オブジェクト生成
	CObstacle *pObstacle = nullptr;
	if (pObstacle = new CObstacle())
	{
		pObstacle->m_ObsType = obstype;	// 障害物タイプ
		pObstacle->Init(pos, rot);		// 初期化

		if (!pObstacle->m_pModel)
		{
			// モデルの生成
			pObstacle->m_pModel = CModel::Create({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, scale, nXType);
			// 親モデルをNULLに設定
			pObstacle->m_pModel->SetParent(NULL);
			// モデルタイプの設定
			pObstacle->CScene::SetModelType(CScene::MODTYPE_OBSCUBE);
			// シングルモデルサイズの設定
			pObstacle->SetSize(pObstacle->m_pModel->GetSize());
		}
	}

	return pObstacle;
}

//===========================================
// 初期化関数
//===========================================
HRESULT CObstacle::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// モデル初期化
	CModel_Spawner::Init(pos, rot);
	return S_OK;
}


//===========================================
// 終了関数
//===========================================
void CObstacle::Uninit()
{
	// モデル終了
	CModel_Spawner::Uninit();

	// UI削除
	if (m_pUI)
	{
		m_pUI->Uninit();
		delete m_pUI;
		m_pUI = nullptr;
	}
}

//===========================================
// 更新関数
//===========================================
void CObstacle::Update()
{
	// モデル更新
	if (m_pModel)
	{
		m_pModel->Update();

		// タイプ別処理
		switch(m_ObsType)
		{
			// どこから当たっても通る
		case OBSTACLE::NEEDLE:
			if (m_pModel->GetPlayerHitBool() && !m_bDoOnce)
			{
				m_bDoOnce = true;
				DeletePlayer();
				CManager::GetInstance()->GetSound()->PlaySound(CSound::SOUND_LABEL_SE_DEATH);
			}
			break;

			// 左から当たった時のみ通る
		case OBSTACLE::CUBE:
			if (m_pModel->GetPlayerHitBool()  && !m_bDoOnce && m_pModel->GetHitFromLeft())
			{
				m_bDoOnce = true;
				DeletePlayer();
				CManager::GetInstance()->GetSound()->PlaySound(CSound::SOUND_LABEL_SE_DEATH);
			}
			break;
		}
	}
}

//===========================================
// 描画関数
//===========================================
void CObstacle::Draw()
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

//===========================================
// プレイヤー消去関数
//===========================================
void CObstacle::DeletePlayer(void)
{
	// プレイヤーを取得
	CPlayer *pPlayer = (CPlayer*)CScene::GetScene(OBJTYPE_PLAYER);
	if (pPlayer)
	{
		float fRad = pPlayer->GetSize().y;														// プレイヤーサイズYを取得
		pPlayer->SetUninit();																	// プレイヤーの終了
		CBlast::Create(pPlayer->GetPos(), { fRad,fRad,fRad }, { 0.0f,1.0f,1.0f,1.0f }, 20, 20);	// 爆発生成
		CRemainCount::SetRemainChange();														// 残機を減らす
	}
}