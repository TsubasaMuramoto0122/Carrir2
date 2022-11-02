//=============================================================
// プレイヤーの作成
// Author:村元翼
//=============================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "scene.h"

//=============================================================
// 前方宣言
//=============================================================
class CModel;
class CShadow;
class CBillboard;

//=============================================================
// クラス定義
//=============================================================
class CPlayer : public CScene
{
public:
	//------------------------------------
	// プレイヤー状態構造体
	//------------------------------------
	enum class PLAYER_STATE
	{
		BALL = 0,		// ボール
		CUBE,			// キューブ
		AIRSHIP,		// 飛行機
		UFO,			// UFO
		MAX
	};

	CPlayer(OBJTYPE nPriority = CScene::OBJTYPE_PLAYER);		// コンストラクタ
	~CPlayer();													// デストラクタ

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot);				// 初期化
	void Uninit(void);											// 終了
	void Update(void);											// 更新
	void Draw(void);											// 描画

	// オブジェクト生成
	static CPlayer *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, PLAYER_STATE nType, bool bMove = true, bool bCameraMove = true);

	//---------------------------------------------------
	// 移動系関数
	//---------------------------------------------------
	void Move(void);																				// 移動
	void Inertia(D3DXVECTOR3 &speed);																// 慣性
	void Gravity(D3DXVECTOR3& pos, float& fGravity, const float& fGravitySpeed, bool &bJump);		// 重力
	void Action(void);																				// アクション
	void JumpAction(const float& fJump);															// ジャンプアクション
	void SpeedAndRotLimit(D3DXVECTOR3 &speed, D3DXVECTOR3 &rot, const float fMaxSpeed);				// 移動・回転限界																			// 爆弾持ち上げ関数
	void Quaternion(void);																			// クォータニオン回転関数
	void ModelDelete(void);																			// モデル破棄関数
	void PosYLimit(float& PosY);																	// 位置Y限界関数
	void SetReverse(void);																			// 重力反転関数
	void Fall(const D3DXVECTOR3& pos, const D3DXVECTOR3& posOld);									// 落下判定関数

	//---------------------------------------------------
	// 設定・取得関数
	//---------------------------------------------------
	D3DXVECTOR3 GetPos(void)					{ return m_pos; }
	D3DXVECTOR3 GetRot(void)					{ return m_rot; }
	D3DXVECTOR3 GetSpeed(void)					{ return m_Speed; }
	bool GetJump()								{ return m_bJump; }
	bool GetLock()								{ return m_bJumpLock; }
	float GetBuoyancy()							{ return m_fBuoyancy; }
	PLAYER_STATE GetState()						{ return m_state; }
	bool GetReverse()							{ return m_bReverse; }
	void SetUninit()							{ m_bUninit = true; }
	void SetSpeed(D3DXVECTOR3 speed)			{ m_Speed = speed; }
	void SetJump(bool bJump)					{ m_bJump = bJump; }
	void SetGravity(float fGravity,bool bJump)	{ m_fGravity = fGravity , m_bJump = bJump; }
	void SetPlayerHit(bool bHit)				{ m_bHit = bHit; }
	void SetLock(bool bLock)					{ m_bJumpLock = bLock; }
	void SetMoveBool(bool bMove)				{ m_bMove = bMove; }
	void SetMoveCameraBool(bool bCameraMove)	{ m_bCameraMove = bCameraMove; }
	void SetState(PLAYER_STATE state);

private:
	const static float	m_fMaxSpeed;					// 最大スピード
	const static float	m_fMaxGravity;					// 重力最高速度
	static float		m_fFallSpeed;					// 落下速度
	static float		m_fGravitySpeed;				// 重力加速度
	static float		m_fJump;						// ジャンプ力
	static float		m_fHop;							// ジャンプ力(UFO時)
	static float		m_fCubeRotate;					// 回転量(キューブ時)
	static float		m_fBallRotate;					// 回転量(ボール時)
	static float		m_fBuoyancy;					// 浮力

	CBillboard			*m_pBillboard;					// ビルボードクラス
	CShadow				*m_pShadow;						// シャドウクラス
	CModel				*m_pModel;						// モデルクラス
	D3DXVECTOR3			m_pos,m_posOld;					// 位置・最後の位置
	D3DXVECTOR3			m_rot;							// 回転
	D3DXVECTOR3			m_size;							// サイズ
	D3DXVECTOR3			m_Speed;						// 速度			
	float				m_fGravity;						// 重力
	float				m_fAngle;						// 回転する角度
	int					m_nType;						// タイプ
	bool				m_bReverse;						// 重力反転したか
	bool				m_bJumpLock;					// ジャンプロック判定
	bool				m_bJump;						// ジャンプ判定
	bool				m_bSwitch;						// カメラ追従切り替え
	bool				m_bUninit;						// 終了判定
	bool				m_bHit;							// プレイヤーが対象に当たっているか
	bool				m_bMove;						// 移動可能状態の判定
	bool				m_bCameraMove;					// カメラ移動可能か
	D3DXMATRIX			m_mtxWorld;						// ワールドマトリックス
	D3DXMATRIX			m_mtxRot;						// クォータニオンの計算に使うマトリックス
	PLAYER_STATE		m_state;						// プレイヤーの状態
};
#endif 