//==================================
// メッシュフィールドの作成
// Author: 村元翼
//==================================
#ifndef _MESHFIELD_H
#define _MESHFIELD_H
#include "main.h"
#include "scene.h"
#include "Scene3D.h"
#include "mesh.h"

//==============================================
// マクロ定義
//==============================================
#define MESH_VTX (4)

//------------------------------------------------------------------------------
// クラスの定義
//------------------------------------------------------------------------------
class CMeshField : public CScene, public CMesh
{
public:
	//--------------------------------------------
	// コンストラクタ・デストラクタ
	//--------------------------------------------
	CMeshField(OBJTYPE nPriority = OBJTYPE_MESH);
	~CMeshField();

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);	// 初期化
	void Uninit(void);									// 終了
	void Update(void);									// 更新
	void Draw(void);									// 描画

	static CMeshField *Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot, int nLine, int nVertical);	// メッシュフィールドの生成
	void CollisionManager(void);																				// 当たり判定管理関数
	bool LineCollisionMesh(CScene *pScene, const int *pnVtx);													// 線とメッシュの当たり判定
	bool ShadowCollisionMesh(CScene *pScene, const int *pnVtx);													// 影との当たり判定
	void ProcessByObjtype(CScene *pScene,D3DXVECTOR3 &pos);														// タイプ別処理
};
#endif