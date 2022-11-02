//==============================================================================
// フィールドの作成
// Author:村元翼
//==============================================================================
#include "Scene3D.h"
#include "model.h"
#include "scene.h"
#include "renderer.h"
#include "manager.h"
#include "field.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CField::CField()
{
	// 各種初期化
	m_pTexture = nullptr;						// テクスチャポインタ
	m_pVtxBuff = nullptr;						// 頂点バッファポインタ
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// サイズ
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 回転

	// 頂点座標
	for (int nCnt = 0; nCnt < FIELD_VTX; nCnt++)
	{
		m_vtx[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_vtxRot[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	// 3Dポリゴン
	m_pScene3D = nullptr;
}

//=============================================================================
// デストラクタ
//=============================================================================
CField::~CField()
{

}

//=============================================================================
// インスタンス生成処理
//=============================================================================
CField *CField::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot)
{
	// インスタンス生成
	CField *pField = nullptr;

	if (pField = new CField())
	{
		pField->m_pos = pos;			// 位置
		pField->m_size = size;			// 大きさ
		pField->m_rot = rot;			// 回転

		pField->Init(pField->m_pos, pField->m_size);	// 初期化
	}
	return pField;
}

//=============================================================================
// ポリゴンの初期化処理
//=============================================================================
HRESULT CField::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// シーン3Dの生成とテクスチャの取得
	m_pScene3D = CScene3D::Create(pos, size, m_rot);
	m_pScene3D->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_FLOOR"));
	return S_OK;
}

//=============================================================================
// ポリゴンの終了処理
//=============================================================================
void CField::Uninit(void)
{
	m_pScene3D->Uninit();
}

//=============================================================================
// ポリゴンの更新処理
//=============================================================================
void CField::Update(void)
{
	m_pScene3D->Update();
}

//=============================================================================
// ポリゴンの描画処理
//=============================================================================
void CField::Draw(void)
{
	m_pScene3D->Draw();

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイスのポインタ
	D3DXMATRIX mtxTrans,mtxRot;															// 位置計算用マトリックス
	D3DXMATRIX mtxParent;																// 親のマトリックス

	for (int nCnt = 0; nCnt < FIELD_VTX; nCnt++)
	{
		D3DXVECTOR3 LocalVtx[FIELD_VTX];							// ローカル座標保存変数
		LocalVtx[nCnt] = m_pScene3D->GetLocalVtx(nCnt);				// ローカル座標取得

		// 各頂点のワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_vtxMtxWorld[nCnt]);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, LocalVtx[nCnt].x, LocalVtx[nCnt].y, LocalVtx[nCnt].z);
		D3DXMatrixMultiply(&m_vtxMtxWorld[nCnt], &m_vtxMtxWorld[nCnt], &mtxTrans);

		// 親子関係付け処理
		//*****************************************************************************
		// 各頂点の親のマトリックスを設定
		if (m_pScene3D->GetMatrix() != nullptr)
		{
			mtxParent = m_pScene3D->GetMatrix();
		}

		else
		{
			// 最新のマトリックスを取得
			pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
		}

		// 算出した各頂点のワールドマトリックスと親のマトリックスを掛け合わせる
		D3DXMatrixMultiply(&m_vtxMtxWorld[nCnt], &m_vtxMtxWorld[nCnt], &mtxParent);
		//****************************************************************************
		// ここまで

		// 各頂点のワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_vtxMtxWorld[nCnt]);
	}

	// 各頂点のワールド座標の保存
	for (int nCnt = 0; nCnt < FIELD_VTX; nCnt++)
	{
		m_vtx[nCnt].x = m_vtxMtxWorld[nCnt]._41;
		m_vtx[nCnt].y = m_vtxMtxWorld[nCnt]._42;
		m_vtx[nCnt].z = m_vtxMtxWorld[nCnt]._43;
	}
}

//================================================
// ワールドマトリックス設定処理
//================================================
void CField::SetVtxMtxWorld(D3DXVECTOR3 vtxPos, int nCntVtx)
{
	//デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice;
	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;						

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_vtxMtxWorld[nCntVtx]);		

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_vtxRot[nCntVtx].y, m_vtxRot[nCntVtx].x, m_vtxRot[nCntVtx].z);
	D3DXMatrixMultiply(&m_vtxMtxWorld[nCntVtx], &m_vtxMtxWorld[nCntVtx], &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, vtxPos.x, vtxPos.y, vtxPos.z);
	D3DXMatrixMultiply(&m_vtxMtxWorld[nCntVtx], &m_vtxMtxWorld[nCntVtx], &mtxTrans);

	D3DXMATRIX mtxParent;		//親のマトリックス
	mtxParent = m_mtxWorld;

	//算出した各パーツのワールドマトリックスと親のワールドマトリックスを掛け合わせる
	D3DXMatrixMultiply(&m_vtxMtxWorld[nCntVtx], &m_vtxMtxWorld[nCntVtx], &mtxParent);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_vtxMtxWorld[nCntVtx]);
}