//=============================================================================
// モデルクラス処理
// Author : 村元翼
//=============================================================================
#include "model.h"
#include "Scene3D.h"
#include "scene.h"
#include "input.h"
#include "renderer.h"
#include "manager.h"
#include "LoadX.h"
#include <assert.h>

//=============================================================================
// マクロ定義
//=============================================================================
#define RANGE_COLLISION (100000.0f)	// 当たり判定範囲
#define IKD_EPSIRON		(0.00001f)	// 誤差

//=============================================================================
// コンストラクタ
//=============================================================================
CModel::CModel()
{
	memset(m_vtx, NULL, sizeof(m_vtx));				// 頂点
	m_pMesh			= nullptr;						// メッシュ（頂点情報）へのポインタ
	m_nNumVtx		= 0;							// 頂点の数
	m_vtxMin		= { 0.0f, 0.0f, 0.0f };			// モデルの最小値
	m_vtxMax		= { 0.0f, 0.0f, 0.0f };			// モデルの最大値
	m_pos			= { 0.0f, 0.0f, 0.0f };			// モデルの位置（オフセット）
	m_rot			= { 0.0f, 0.0f, 0.0f };			// モデルの向き
	m_size			= { 0.0f, 0.0f, 0.0f };			// モデルの大きさ
	m_scale			= { 0.0f, 0.0f, 0.0f };			// モデルの規模
	m_SaveEmissive	= { 0.0f, 0.0f, 0.0f,1.0f };	// 発光色保存
	m_bDraw			= false;						// 描画するか					
	m_bDoOnce		= false;						// 一回のみ通る処理
}

//=============================================================================
// デストラクタ
//=============================================================================
CModel::~CModel()
{

}

//=============================================================================
// インスタンス生成処理
//=============================================================================
CModel *CModel::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nXType, bool bCollision)
{
	//インスタンス生成
	CModel *pModel = nullptr;
	if (!pModel)
	{
		pModel = new CModel;
		if (pModel)
		{
			pModel->m_pos = pos;							// 位置
			pModel->m_rot = rot;							// 回転
			pModel->m_scale = scale;						// 規模
			pModel->m_bCollision = bCollision;				// 当たり判定をつけるか
			pModel->Init(nXType);							// 初期化
		}
	}
	return pModel;
}


//=============================================================================
// モデルの初期化処理
//=============================================================================
void CModel::Init(int nXType)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイスのポインタ

	// Xファイルデータ取得
	m_nXType = nXType;
	CLoadX::XData pXData = CManager::GetInstance()->GetLoadX()->GetXData(m_nXType);
	m_pBuffMat		= pXData.m_pBuffMat;
	m_pMesh			= pXData.m_pMesh;
	m_nNumMat		= pXData.m_nNumMat;
	m_pMat			= pXData.m_pMat;
	m_pTexture		= pXData.m_pTex;
	m_SaveEmissive	= pXData.m_pMat->MatD3D.Emissive;

	// 頂点数を取得
	m_nNumVtx = m_pMesh->GetNumVertices();
	// 頂点フォーマットのサイズを取得
	m_sizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());
	// 頂点バッファをロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&m_pVtxBuff);
	//モデルの大きさを測る
	for (int nCnt = 0; nCnt < m_nNumVtx; nCnt++)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)m_pVtxBuff;	// 頂点座標の代入
		
		// xの最大値の比較
		if (m_vtxMax.x <= vtx.x)
		{
			m_vtxMax.x = vtx.x;
		}
		// xの最小値の比較
		if (m_vtxMin.x >= vtx.x)
		{
			m_vtxMin.x = vtx.x;
		}
		// yの最大値の比較
		if (m_vtxMax.y <= vtx.y)
		{
			m_vtxMax.y = vtx.y;
		}
		// yの最小値の比較
		if (m_vtxMin.y >= vtx.y)
		{
			m_vtxMin.y = vtx.y;
		}
		// zの最大値の比較
		if (m_vtxMax.z <= vtx.z)
		{
			m_vtxMax.z = vtx.z;
		}
		// zの最小値の比較
		if (m_vtxMin.z >= vtx.z)
		{
			m_vtxMin.z = vtx.z;
		}

		// xyzの最大値と最小値を引き、サイズにする
		m_size.x = m_vtxMax.x - m_vtxMin.x;
		m_size.y = m_vtxMax.y - m_vtxMin.y;
		m_size.z = m_vtxMax.z - m_vtxMin.z;

		m_pVtxBuff += m_sizeFVF;	// 頂点フォーマットのサイズ分ポインタを進める
	}

	// 頂点バッファをアンロック
	m_pMesh->UnlockVertexBuffer();

	// 8つの頂点情報の保存
	// 左奥（上面）
	m_vtx[0].vtx.x = m_vtxMin.x;
	m_vtx[0].vtx.y = m_vtxMax.y;
	m_vtx[0].vtx.z = m_vtxMax.z;

	// 右奥（上面）
	m_vtx[1].vtx.x = m_vtxMax.x;
	m_vtx[1].vtx.y = m_vtxMax.y;
	m_vtx[1].vtx.z = m_vtxMax.z;
		
	// 左前（上面）
	m_vtx[2].vtx.x = m_vtxMin.x;
	m_vtx[2].vtx.y = m_vtxMax.y;
	m_vtx[2].vtx.z = m_vtxMin.z;

	// 右前（上面）
	m_vtx[3].vtx.x = m_vtxMax.x;
	m_vtx[3].vtx.y = m_vtxMax.y;
	m_vtx[3].vtx.z = m_vtxMin.z;
		
	// 左奥（下面）
	m_vtx[4].vtx.x = m_vtxMin.x;
	m_vtx[4].vtx.y = m_vtxMin.y;
	m_vtx[4].vtx.z = m_vtxMax.z;
	
	// 右奥（下面）
	m_vtx[5].vtx.x = m_vtxMax.x;
	m_vtx[5].vtx.y = m_vtxMin.y;
	m_vtx[5].vtx.z = m_vtxMax.z;

	// 左前（下面）
	m_vtx[6].vtx.x = m_vtxMin.x;
	m_vtx[6].vtx.y = m_vtxMin.y;
	m_vtx[6].vtx.z = m_vtxMin.z;

	// 右前（下面）
	m_vtx[7].vtx.x = m_vtxMax.x;
	m_vtx[7].vtx.y = m_vtxMin.y;
	m_vtx[7].vtx.z = m_vtxMin.z;

	//　変数初期化
	m_SaveEmissive = { 0.0f,0.0f,0.0f,1.0f };
}

//=============================================================================
// モデルの終了処理
//=============================================================================
void CModel::Uninit(void)
{
 
}

//=============================================================================
// モデルの更新処理
//=============================================================================
void CModel::Update(void)
{

}

//=============================================================================
// モデルの描画処理
//=============================================================================
void CModel::Draw(void)
{
	//========================================================================
	// モデル本体のワールド座標設定
	//========================================================================
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイスのポインタ

	// 現在のマテリアルを取得
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;				// 計算用マトリックス
	D3DXMATRIX mtxParent;								// 親のマトリックス
	D3DMATERIAL9 Matdef;

	// 各パーツのワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// サイズ変更
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// マテリアルの取得
	pDevice->GetMaterial(&Matdef);

	// 書くパーツの親のマトリックスを設定
	if (m_pParent)
	{
		mtxParent = m_pParent->GetMatrix();
	}

	else
	{
		// 最新のマトリックスを取得
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 算出した各パーツのワールドマトリックスと親のマトリックスを掛け合わせる
	D3DXMatrixMultiply
	(
		&m_mtxWorld,
		&m_mtxWorld,
		&mtxParent
	);

	// 各パーツのワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	if (!m_bDraw)
	{
		// マテリアルデータ編のポインタを取得
		m_pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)m_nNumMat; nCntMat++)
		{
			 m_pMat[nCntMat].MatD3D.Emissive = m_SaveEmissive;

			// マテリアルの設定
			pDevice->SetMaterial(&m_pMat[nCntMat].MatD3D);

			// テクスチャの設定
			if (m_pTexture.at(nCntMat))
			{
				pDevice->SetTexture(0, m_pTexture[nCntMat]);
			}
			else
			{
				pDevice->SetTexture(0, NULL);
			}

			// モデル（パーツ）の描画
			m_pMesh->DrawSubset(nCntMat);
		}
		// 保存していたマテリアルを戻す
		pDevice->SetMaterial(&Matdef);
	}

	//========================================================================
	// 各モデルの頂点のワールド座標設定
	//========================================================================
	D3DXMATRIX vtxParent;	// 各モデルの頂点の親マトリックス
	D3DXMATRIX vtxTrans;	// 計算用マトリックス

	// 配列サイズを設定
	for (int nCnt = 0; nCnt < MAX_MODEL_VTX; nCnt++)
	{
		// 各頂点のワールドマトリックスの初期化
		D3DXMatrixIdentity(&m_vtx[nCnt].mtxWorld);

		// 位置を反映
		D3DXMatrixTranslation(&vtxTrans, m_vtx[nCnt].vtx.x, m_vtx[nCnt].vtx.y, m_vtx[nCnt].vtx.z);
		D3DXMatrixMultiply(&m_vtx[nCnt].mtxWorld, &m_vtx[nCnt].mtxWorld, &vtxTrans);

		//*****************************************************************************
		// 親子関係付け処理
		//*****************************************************************************
		// 各頂点の親のマトリックスを設定
		if (m_mtxWorld)
		{
			vtxParent = m_mtxWorld;
		}

		else
		{
			// 最新のマトリックスを取得
			pDevice->GetTransform(D3DTS_WORLD, &vtxParent);
		}

		// 算出した各頂点のワールドマトリックスと親のマトリックスを掛け合わせる
		D3DXMatrixMultiply
		(
			&m_vtx[nCnt].mtxWorld,
			&m_vtx[nCnt].mtxWorld,
			&vtxParent
		);
		//****************************************************************************
		// ここまで

		// 各頂点のワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_vtx[nCnt].mtxWorld);

		// 各頂点のワールド座標の保存
		m_vtx[nCnt].vtxWorld.x = m_vtx[nCnt].mtxWorld._41;
		m_vtx[nCnt].vtxWorld.y = m_vtx[nCnt].mtxWorld._42;
		m_vtx[nCnt].vtxWorld.z = m_vtx[nCnt].mtxWorld._43;
	}
}