//===========================================================================
// メッシュスフィアの作成
// Author: 村元翼
//===========================================================================
#ifndef _MESHSPHERE_H_
#define _MESHSPHERE_H_
#include "main.h"
#include "scene.h"
#include "Scene3D.h"
#include "mesh.h"

//===========================================================================
// マクロ定義
//===========================================================================
#define SPHERE_RADIUS	(100.0f)													// 球半径
#define SPHERE_SIZE		(D3DXVECTOR3(SPHERE_RADIUS,SPHERE_RADIUS,SPHERE_RADIUS))	// 球サイズ

//===========================================================================
// クラスの定義
//===========================================================================
class CMeshSphere : public CScene, public CMesh
{
public:
	//--------------------------------------------
	// コンストラクタ・デストラクタ
	//--------------------------------------------
	CMeshSphere(OBJTYPE nPriority = OBJTYPE_MESH);
	~CMeshSphere();

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);	// 初期化
	void Uninit(void);									// 終了
	void Update(void);									// 更新
	void Draw(void);									// 描画

	//--------------------------------------------
	// 設定・取得処理
	//--------------------------------------------
	void SetScale(D3DXVECTOR3 scale)				{ m_scale = scale; }				// スケールの設定
	D3DXVECTOR3 GetScale()							{ return m_scale; }					// スケールの取得

	// オブジェクト生成
	static CMeshSphere *Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot, int nLine, int nVertical, bool bInsideDraw, CScene::OBJTYPE objtype);

	// 当たり判定
	bool SphereCollisionSphere(float fRadius, CScene *pScene);
	bool LineCollisionMesh(CScene *pScene, const int *nVtx);

private:
	D3DXVECTOR3 m_scale;										// スケール
	bool m_bInsideDraw;											// 内側に描画するか
	CScene::OBJTYPE objtype;									// オブジェクトタイプ
};
#endif