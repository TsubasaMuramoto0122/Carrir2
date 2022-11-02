//===========================================
// ゲートのモデル
// Author:村元翼
//===========================================
#ifndef _GATE_H_
#define _GATE_H_
#include "main.h"
#include "model_spawner.h"

//===========================================
// 前方宣言
//===========================================
class CUi;

//===========================================
// クラスの定義
//===========================================
class CGate : public CModel_Spawner
{
public:
	//------------------------------------------
	// ゲートの種類
	//------------------------------------------
	enum class GATE_TYPE
	{
		A = 0,
		B,
		C,
		D,
		MAX
	};

	CGate(OBJTYPE nPriority = OBJTYPE_MODEL);			// コンストラクタ
	~CGate();											// デストラクタ

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot);		// 初期化
	void Uninit(void);									// 終了
	void Update(void);									// 更新
	void Draw(void);									// 描画

	// オブジェクト生成
	static CGate *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nXType, GATE_TYPE Type);

	// 取得関数
	D3DXVECTOR3 GetScale() { return m_scale; }

private:
	D3DXVECTOR3 m_scale;			// スケール
	GATE_TYPE	m_Type;				// ゲートの種類
	bool		m_bDoOnce;			// 一度のみ通る変数
};
#endif
