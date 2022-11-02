//==============================================================================
// シーン3Dの作成
// Author:村元翼
//==============================================================================
#include "meshcylinder.h"
#include "renderer.h"
#include "manager.h"
#include "player.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CMeshCylinder::CMeshCylinder(OBJTYPE nPriority) : CScene(nPriority)
{
	// 各種初期化
	m_Type = CYLINDER_TYPE::STAND;
}

//=============================================================================
// デストラクタ
//=============================================================================
CMeshCylinder::~CMeshCylinder()
{

}

//---------------------------------------------------------------
// オブジェクト生成処理
//---------------------------------------------------------------
CMeshCylinder *CMeshCylinder::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot, int nLine, int nVertical, CYLINDER_TYPE type)
{
	// オブジェクト生成
	CMeshCylinder *pMeshCylinder = nullptr;

	// NULLチェック
	if (pMeshCylinder = new CMeshCylinder())
	{
		pMeshCylinder->m_rotMS = rot;			// 回転
		pMeshCylinder->m_nLine = nLine;			// 横ポリゴン数
		pMeshCylinder->m_nVertical = nVertical;	// 縦ポリゴン数
		pMeshCylinder->m_Type = type;			// タイプ
		pMeshCylinder->Init(pos, size);			// 初期化
	}

	return pMeshCylinder;
}

//=============================================================================
// ポリゴンの初期化処理
//=============================================================================
HRESULT CMeshCylinder::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイスのポインタ

	// 位置・サイズ設定処理
	m_posMS = pos;
	m_sizeMS = size;
	CScene::SetPos(m_posMS);
	CScene::SetSize(m_sizeMS);

	// 頂点情報を設定
	pDevice->CreateVertexBuffer
	(
		sizeof(VERTEX_3D) * (m_nLine + 1) * (m_nVertical + 1),	// 頂点数
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL
	);

	// 頂点インデックスを設定
	pDevice->CreateIndexBuffer
	(
		(sizeof(WORD) * ((m_nVertical - 1) * (m_nLine + 3) + (m_nLine + 1) * (m_nVertical + 1))),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pVtxIndexBuff,
		NULL
	);

	// 3D頂点バッファのポインタ
	VERTEX_3D *pVtx;

	// 頂点バッファのロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標
	int nNum = 0;
	float fLineForm, fVerticalForm;
	D3DXVECTOR3 Rad = { m_sizeMS.x / 2,m_sizeMS.y / 2 ,m_sizeMS.z / 2 };

	for (int nVertical = 0; nVertical < m_nVertical + 1; nVertical++)
	{
		for (int nLine = 0; nLine < m_nLine + 1; nLine++, nNum++)
		{
			fLineForm		= -D3DX_PI / 2 + (D3DX_PI / m_nLine) * (nLine * 2);				// 円周頂点位置の計算式
			fVerticalForm	= m_sizeMS.y / 2.0f - (m_sizeMS.y / m_nVertical) * nVertical;	// 高さ(長さ)頂点位置の計算式
			switch (m_Type)
			{
			case CYLINDER_TYPE::STAND:			// 立てる
				pVtx[nNum].pos.x = cosf(fLineForm) * Rad.x;
				pVtx[nNum].pos.y = fVerticalForm;
				pVtx[nNum].pos.z = sinf(fLineForm) * Rad.z;
				break;

			case CYLINDER_TYPE::STAND_INSIDE:	// 立てる(内側に描画)
				pVtx[nNum].pos.x = sinf(fLineForm) * Rad.x;
				pVtx[nNum].pos.y = fVerticalForm;
				pVtx[nNum].pos.z = cosf(fLineForm) * Rad.z;
				break;

			case CYLINDER_TYPE::LIE:			// 寝かせる
				pVtx[nNum].pos.x = sinf(fLineForm) * Rad.x;
				pVtx[nNum].pos.y = cosf(fLineForm) * Rad.z;
				pVtx[nNum].pos.z = fVerticalForm;
				break;

			case CYLINDER_TYPE::LIE_INSIDE:		// 寝かせる(内側に描画)
				pVtx[nNum].pos.x = sinf(fLineForm) * Rad.x;
				pVtx[nNum].pos.y = cosf(fLineForm) * Rad.z;
				pVtx[nNum].pos.z = fVerticalForm;
				break;
			}

			// 法線
			pVtx[nNum].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			//カラー
			pVtx[nNum].col = D3DCOLOR_RGBA(255, 255, 255, 255);
			//テクスチャ
			pVtx[nNum].tex = D3DXVECTOR2(0.0f + (1.0f * nLine), 0.0f + (1.0f * nVertical));

			// 配列の最後に頂点の位置データを追加
			m_vtxLocal.emplace_back(pVtx[nNum].pos);
		}
	}

	// 頂点バッファのアンロック
	m_pVtxBuff->Unlock();

	// インデックス情報へのポインタ
	WORD *pIdx;

	// 頂点インデックスバッファのロック
	m_pVtxIndexBuff->Lock(0, 0, (void**)&pIdx, 0);

	// インデックスの割り当て
	for (int nCnt1 = 0; nCnt1 < m_nVertical; nCnt1++)
	{
		for (int nCnt2 = 0; nCnt2 < m_nLine + 1; nCnt2++)
		{
			// 番号割り当て
			pIdx[(nCnt2 * 2) + 0 + (m_nLine + 2) * 2 * nCnt1] = (m_nLine + 1) + nCnt2 + (m_nLine + 1) * nCnt1;
			pIdx[(nCnt2 * 2) + 1 + (m_nLine + 2) * 2 * nCnt1] = nCnt2 + (m_nLine + 1) * nCnt1;
		}
	}
	for (int nCnt3 = 0; nCnt3 < m_nVertical - 1; nCnt3++)
	{
		// 縮退ポリゴンの番号割り当て
		pIdx[((m_nLine + 1) * 2 + 0) * (nCnt3 + 1) + (2 * nCnt3)] = m_nLine + (m_nLine + 1) * nCnt3;
		pIdx[((m_nLine + 1) * 2 + 1) * (nCnt3 + 1) + (1 * nCnt3)] = m_nLine * 2 + 2 + (m_nLine + 1) * nCnt3;
	}

	// インデックスバッファのアンロック
	m_pVtxIndexBuff->Unlock();

	return S_OK;
}

//=============================================================================
// ポリゴンの終了処理
//=============================================================================
void CMeshCylinder::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// インデックスバッファの破棄
	if (m_pVtxIndexBuff)
	{
		m_pVtxIndexBuff->Release();
		m_pVtxIndexBuff = nullptr;
	}

	// オブジェクトの破棄
	Release();
}

//=============================================================================
// ポリゴンの更新処理
//=============================================================================
void CMeshCylinder::Update(void)
{
	// 位置・大きさ設定
	CMesh::SetPos(this, m_posMS, m_sizeMS);
}

//=============================================================================
// ポリゴンの描画処理
//=============================================================================
void CMeshCylinder::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice(); // デバイスのポインタ

	D3DXMATRIX mtxRot, mtxTrans, mtxScale;	// 計算用マトリックス
	D3DXMatrixIdentity(&m_mtxWorld);		// ワールドマトリックスの初期化

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rotMS.y, m_rotMS.x, m_rotMS.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_posMS.x, m_posMS.y, m_posMS.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	pDevice->SetFVF(FVF_VERTEX_3D);					// 頂点フォーマットの設定

	pDevice->SetIndices(m_pVtxIndexBuff);			// 頂点インデックスの登録

	pDevice->SetTexture(0, m_pTexture);				// テクスチャの設定

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive
	(
		D3DPT_TRIANGLESTRIP,
		0,
		0,
		((m_nLine + 1) * (m_nVertical + 1)),					// 頂点の数
		0,														// 開始する頂点のインデックス
		2 * m_nLine * m_nVertical + (m_nVertical * 4) - 4		// 描画するプリミティブ数
																// ↑1フィールドに2プリミティブ	↑縮退ポリゴンの分(Z方向に1フィールド伸ばす分4つ生成するから)
	);

	D3DXMATRIX mtxParent;	// 親のマトリックス

	// 配列サイズを設定
	m_mtxVec.resize((m_nLine + 1) * (m_nVertical + 1));
	m_vtxWorld.resize((m_nLine + 1) * (m_nVertical + 1));

	int nNum = 0;
	for (int nVertical = 0; nVertical < m_nVertical + 1; nVertical++)
	{
		for (int nLine = 0; nLine < m_nLine + 1; nLine++, nNum++)
		{
			// 各頂点のワールドマトリックスの初期化
			D3DXMatrixIdentity(&m_mtxVec[nNum]);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, m_vtxLocal[nNum].x, m_vtxLocal[nNum].y, m_vtxLocal[nNum].z);
			D3DXMatrixMultiply(&m_mtxVec[nNum], &m_mtxVec[nNum], &mtxTrans);

			//*****************************************************************************
			// 親子関係付け処理
			//*****************************************************************************
			// 各頂点の親のマトリックスを設定
			if (m_mtxWorld != nullptr)
			{
				mtxParent = m_mtxWorld;
			}

			else
			{
				// 最新のマトリックスを取得
				pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
			}

			// 算出した各頂点のワールドマトリックスと親のマトリックスを掛け合わせる
			D3DXMatrixMultiply
			(
				&m_mtxVec[nNum],
				&m_mtxVec[nNum],
				&mtxParent
			);
			//****************************************************************************
			// ここまで

			// 各頂点のワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &m_mtxVec[nNum]);

			// 各頂点のワールド座標の保存
			m_vtxWorld[nNum].x = m_mtxVec[nNum]._41;
			m_vtxWorld[nNum].y = m_mtxVec[nNum]._42;
			m_vtxWorld[nNum].z = m_mtxVec[nNum]._43;
		}
	}

}