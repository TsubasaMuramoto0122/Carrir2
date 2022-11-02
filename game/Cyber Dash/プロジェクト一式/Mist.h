//========================================
// ミストの生成
// Author:村元翼
//========================================
#ifndef _MIST_H_
#define _MIST_H_
#include "main.h"
#include "scene.h"
#include <random>

//========================================
// マクロ定義
//========================================
#define DEFAULT_MAX_FRAME (60)		// デフォルトの最大フレーム数 
#define DEFAULT_MAX_COUNT (10)		// デフォルトの最大カウント数

//========================================
// 前方宣言
//========================================

//========================================
// クラスの定義
//========================================
class CMist : public CScene
{
public:
	CMist(int fMaxFrame,
		  int fMaxCount,
		  OBJTYPE nPriority = OBJTYPE_POLYGON2D);				// コンストラクタ

	~CMist();													// デストラクタ
	
	HRESULT Init(D3DXVECTOR3 RangeMin, D3DXVECTOR3 RangeMax);	// 初期化
	void Uninit(void);											// 終了
	void Update(void);											// 更新
	void Draw(void);											// 描画

	// 生成関数
	static CMist *Create
	(
		D3DXVECTOR3 RangeMin,
		D3DXVECTOR3 RangeMax,
		int nMaxFrame = DEFAULT_MAX_FRAME,
		int nMaxCount = DEFAULT_MAX_COUNT,
		bool bInfinite = false
	);

	// 設定関数
	void SetInfinite(bool bSwitch) { m_bInfinite = bSwitch; }

private:
	D3DXVECTOR3 m_RangeMin;			// 範囲最小値
	D3DXVECTOR3 m_RangeMax;			// 範囲最大値
	int			m_nFrame;			// フレーム
	int			m_nCount;			// カウント
	const int	m_nMaxFrame;		// フレーム最大値
	const int	m_nMaxCount;		// カウント最大値
	bool		m_bInfinite;		// 無限に生成するか
	random_device m_Randseed;		// 乱数生成器
};

#endif _MIST_H_