//===========================================
// ゴール地点のモデル
// Author:村元翼
//===========================================
#include "Gate.h"
#include "model.h"
#include "manager.h"
#include "renderer.h"
#include "fade.h"
#include "Ui.h"
#include "ClearGauge.h"
#include "player.h"

//===========================================
// コンストラクタ
//===========================================
CGate::CGate(OBJTYPE nPriority) : CModel_Spawner(nPriority)
{
	m_scale = { 0.0f,0.0f,0.0f };	// スケール
	m_Type = GATE_TYPE::A;			// ゲートのタイプ
	m_bDoOnce = false;				// 一度のみ通らせる変数
}

//===========================================
// デストラクタ
//===========================================
CGate::~CGate()
{

}

//===========================================
// オブジェクト生成関数
//===========================================
CGate *CGate::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nXType, GATE_TYPE Type)
{
	// デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// オブジェクト生成
	CGate *pGate = nullptr;

	// NULLチェック
	if (pGate = new CGate())
	{
		pGate->m_Type = Type;		// ゲートタイプ
		pGate->m_scale = scale;		// スケール
		pGate->Init(pos, rot);		// 初期化

		if (!pGate->m_pModel)
		{
			// モデルの生成
			pGate->m_pModel = CModel::Create({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, scale, nXType, true, false);
			// 親モデルをNULLに設定
			pGate->m_pModel->SetParent(NULL);
			// モデルタイプの設定
			pGate->CScene::SetModelType(CScene::MODTYPE_GATE);
			// シングルモデルサイズの設定
			pGate->SetSize(pGate->m_pModel->GetSize());

			// タイプ別処理
			switch (pGate->m_Type)
			{
			case GATE_TYPE::A:
				break;

			case GATE_TYPE::B:
				pGate->m_pModel->SetEmissive(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
				break;

			case GATE_TYPE::C:
				pGate->m_pModel->SetEmissive(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
				break;

			case GATE_TYPE::D:
				pGate->m_pModel->SetEmissive(D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
				break;

			default:
				break;

			}
		}
	}

	return pGate;
}

//===========================================
// 初期化関数
//===========================================
HRESULT CGate::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// モデル初期化
	CModel_Spawner::Init(pos, rot);
	return S_OK;
}

//===========================================
// 終了関数
//===========================================
void CGate::Uninit(void)
{
	// モデル終了
	CModel_Spawner::Uninit();
}

//===========================================
// 更新関数
//===========================================
void CGate::Update(void)
{
	// モデル更新
	if (m_pModel)
	{
		m_pModel->Update();

#if(1)
		// ここでゲートのタイプによってプレイヤーの姿が変わる
		if (m_pModel->GetPlayerHitBool() && !m_bDoOnce)
		{
			CPlayer *pPlayer = (CPlayer*)CScene::GetScene(CScene::OBJTYPE_PLAYER);

			// タイプ別処理
			switch (m_Type)
			{
			case GATE_TYPE::A:
				pPlayer->SetState(CPlayer::PLAYER_STATE::AIRSHIP);
				break;

			case GATE_TYPE::B:
				pPlayer->SetState(CPlayer::PLAYER_STATE::CUBE);
				break;

			case GATE_TYPE::C:
				pPlayer->SetState(CPlayer::PLAYER_STATE::BALL);
				break;

			case GATE_TYPE::D:
				pPlayer->SetState(CPlayer::PLAYER_STATE::UFO);
				break;

			default:
				pPlayer->SetState(CPlayer::PLAYER_STATE::AIRSHIP);
				break;
			}

			m_bDoOnce = true;
		}
#endif
	}
}

//===========================================
// 描画関数
//===========================================
void CGate::Draw(void)
{
	// デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス
	D3DXMATRIX mtxParent;				// 親のマトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// 各パーツのワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// モデルの描画
	if (m_pModel)
	{
		m_pModel->Draw();
	}
}
