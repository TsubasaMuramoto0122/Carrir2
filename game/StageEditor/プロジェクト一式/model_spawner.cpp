//========================================
// モデルスポナーの処理
// Author : 村元翼
//========================================
#include "model_spawner.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "model.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CModel_Spawner::CModel_Spawner(OBJTYPE nPriority) :CScene(nPriority)
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 回転
}

//=============================================================================
// デストラクタ
//=============================================================================
CModel_Spawner::~CModel_Spawner()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CModel_Spawner::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 位置・回転の設定
	m_pos = pos;
	m_rot = rot;
	CScene::SetPos(m_pos);
	CScene::SetRot(m_rot);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CModel_Spawner::Uninit(void)
{
	// モデルクラスの破棄
	if (m_pModel)
	{
		m_pModel->Uninit();
		delete m_pModel;
		m_pModel = nullptr;
	}

	// オブジェクトの破棄
	Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void CModel_Spawner::Update(void)
{
	m_pos = CScene::GetPos();

	// モデル更新処理
	if (m_pModel)
	{
		m_pModel->Update();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CModel_Spawner::Draw(void)
{
	// デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 現在のマテリアルを取得
	D3DXMATRIX mtxRotModel, mtxTransModel;	// 計算用マトリックス
	D3DXMATRIX mtxParent;					// 親のマトリックス
	//D3DMATERIAL9 Matdef;

	// 各パーツのワールドマトリックスの初期化
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
//=============================================================================
// インスタンス生成処理
//=============================================================================
CModel_Spawner *CModel_Spawner::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nXType)
{
	// デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// インスタンス生成
	CModel_Spawner *pSpawner = nullptr;

	if (pSpawner = new CModel_Spawner())
	{
		pSpawner->Init(pos, rot);		// 初期化

		// 角度をラジアンに変換
		float rot_x = D3DXToRadian(rot.x);
		float rot_y = D3DXToRadian(rot.y);
		float rot_z = D3DXToRadian(rot.z);

		if (!pSpawner->m_pModel)
		{
			// モデルの生成
			pSpawner->m_pModel = CModel::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(rot_x, rot_y, rot_z), scale, nXType);
			// 親モデルをNULLに設定
			pSpawner->m_pModel->SetParent(NULL);
			// モデルタイプの設定
			pSpawner->CScene::SetModelType(CScene::MODTYPE_NORMAL);
			// シングルモデルサイズの設定
			pSpawner->SetSize(pSpawner->m_pModel->GetSize());
		}

	}

	return pSpawner;
}
