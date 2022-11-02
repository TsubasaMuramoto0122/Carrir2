//===========================================
// ジャンプサークルの作成
// Author;村元翼
//===========================================
#ifndef _JUMPCIRCLE_H_
#define _JUMPCIRCLE_H_
#include "main.h"
#include "billboard.h"

//===========================================
// クラス定義
//===========================================
class CJumpCircle : public CBillboard
{
public:
	//----------------------------------
	// 種類構造体
	//----------------------------------
	enum class CIRCLE_TYPE
	{
		JUMP = 0,	// ジャンプ
		QUICK_REV,	// 急に反転
		SLOW_REV,	// ゆっくり反転
		FALL,		// 急に落ちる
		MAX
	};

	CJumpCircle(OBJTYPE nPriority = OBJTYPE_BILLBOARD);	// コンストラクタ
	~CJumpCircle();										// デストラクタ

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);	// 初期化処理
	void Uninit(void);									// 終了処理
	void Update(void);									// 更新処理
	void Draw(void);									// 描画処理

	// オブジェクト生成
	static CJumpCircle *Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, CIRCLE_TYPE type);

	// 2D円の当たり判定
	void CircleCollision(void);

	void Jump(CPlayer *pPlayer);		// ジャンプ関数
	void Reverse(CPlayer *pPlayer);		// 重力反転関数
	void SlowRev(CPlayer *pPlayer);		// ゆっくり重力反転関数
	void Fall(CPlayer *pPlayer);		// 落ちる関数

private:
	static float m_fJumpPower;		// ジャンプ力
	CIRCLE_TYPE m_type;				// ジャンプサークルタイプ
};
#endif