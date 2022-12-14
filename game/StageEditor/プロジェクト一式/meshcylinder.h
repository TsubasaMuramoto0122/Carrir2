//==================================
// メッシュフィールドの作成
// Author: 村元翼
//==================================
#ifndef _MESHCYLINDER_H_
#define _MESHCYLINDER_H_
#include "main.h"
#include "scene.h"
#include "Scene3D.h"
#include "texture.h"

//------------------------------------------------------------------------------
//クラスの定義
//------------------------------------------------------------------------------
class CMeshCylinder : public CScene
{
public:
	//--------------------------------------------
	// コンストラクタ・デストラクタ
	//--------------------------------------------
	CMeshCylinder(OBJTYPE nPriority = OBJTYPE_MESH);
	~CMeshCylinder();

	//--------------------------------------------
	// メソッド
	//--------------------------------------------
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//--------------------------------------------
	// 設定・取得関数
	//--------------------------------------------
	void BindTexture(LPDIRECT3DTEXTURE9 pTexture) { m_pTexture = pTexture; }		// テクスチャ設定処理
	void SetPos(D3DXVECTOR3 pos, D3DXVECTOR3 scale);								// 頂点座標の設定
	void SetTex(int nAnim, int nPartU);												// テクスチャの設定
	void SetTex(float fSpeedX, float fSpeedY);										// テクスチャの設定
	void SetCol(D3DXCOLOR col);														// カラーの設定
	D3DXMATRIX	GetMatrix()				{ return m_mtxWorld; }						// ワールドマトリックスの取得
	D3DXVECTOR3 GetLocalVtx(int nID)	{ return m_vtxLocal[nID]; }					// ローカル頂点座標の取得

	static CMeshCylinder *Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot, int nLine, int nVertical);	// 生成

private:
	vector<D3DXVECTOR3> m_vtxLocal;								// ローカル頂点座標
	vector<D3DXVECTOR3> m_vtxWorld;								// ワールド頂点座標
	vector<D3DXMATRIX>  m_mtxVec;								// ワールド頂点マトリックス
	D3DXVECTOR3 m_pos;											// 位置
	D3DXVECTOR3 m_rot;											// 回転
	D3DXVECTOR3 m_size;											// サイズ
	int m_nLine;												// 横のポリゴン数
	int m_nVertical;											// 縦のポリゴン数

protected:

	LPDIRECT3DTEXTURE9		m_pTexture = NULL;				// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff = NULL;				// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9	m_pVtxIndexBuff = nullptr;		// 頂点インデックスバッファへのポインタ
	D3DXMATRIX				m_mtxWorld;
};


#endif

