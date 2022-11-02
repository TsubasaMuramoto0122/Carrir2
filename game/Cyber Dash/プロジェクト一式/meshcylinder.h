//==================================
// メッシュフィールドの作成
// Author: 村元翼
//==================================
#ifndef _MESHCYLINDER_H_
#define _MESHCYLINDER_H_
#include "main.h"
#include "scene.h"
#include "mesh.h"
#include "Scene3D.h"

//------------------------------------------------------------------------------
// クラスの定義(多重定義)
//------------------------------------------------------------------------------
class CMeshCylinder : public CScene ,public CMesh
{
public:
	//--------------------------------------------
	// 筒のタイプ構造体
	//--------------------------------------------
	enum class CYLINDER_TYPE
	{
		STAND = 0,
		STAND_INSIDE,
		LIE,
		LIE_INSIDE,
		MAX
	};

	//--------------------------------------------
	// コンストラクタ・デストラクタ
	//--------------------------------------------
	CMeshCylinder(OBJTYPE nPriority = OBJTYPE_MESH);
	~CMeshCylinder();

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);	// 初期化
	void Uninit(void);									// 終了
	void Update(void);									// 更新
	void Draw(void);									// 描画

	// オブジェクト生成
	static CMeshCylinder *Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot, int nLine, int nVertical, CYLINDER_TYPE type);	// シーン3Dの生成

private:
	CYLINDER_TYPE m_Type;		// 筒メッシュのタイプ
};


#endif


