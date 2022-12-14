//===========================================
// ゴール地点のモデル
// Author:村元翼
//===========================================
#ifndef _GOAL_H_
#define _GOAL_H_
#include "main.h"
#include "model_spawner.h"

//===========================================
// 前方宣言
//===========================================
class CUi;

//===========================================
// クラスの定義
//===========================================
class CGoal : public CModel_Spawner
{
public:
	CGoal(OBJTYPE nPriority = OBJTYPE_MODEL);			// コンストラクタ
	~CGoal();											// デストラクタ

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot);		// 初期化
	void Uninit(void);									// 終了
	void Update(void);									// 更新
	void Draw(void);									// 描画

	// オブジェクト生成
	static CGoal *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nXType);

	// 取得関数
	D3DXVECTOR3 GetScale() { return m_scale; }

private:
	D3DXVECTOR3 m_scale;			// スケール
	CUi			*m_pUI;				// 終了UI
	int			m_nFrame;			// フレーム数

};
#endif