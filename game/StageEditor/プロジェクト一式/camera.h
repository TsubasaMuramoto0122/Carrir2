//======================================
// カメラの作成
// Author:村元翼
//======================================
#ifndef _CAMERA_H
#define _CAMERA_H
#include "main.h"

//==============================================================================
// マクロ定義
//==============================================================================
#define MAX_CAMERA		(1)		// カメラの最大数

//==============================================================================
// クラスの定義
//==============================================================================
class CCamera
{
public:
	//--------------------------------------------------
	// 移動量の列挙
	//--------------------------------------------------
	typedef enum 
	{
		AMOUNT_00 = 0,
		AMOUNT_01,
		AMOUNT_02,
		AMOUNT_03,
		AMOUNT_04,
		AMOUNT_05,
		AMOUNT_06,

		AMOUNT_MAX

	}MOVE_AMOUNT;

	CCamera();						// コンストラクタ
	~CCamera();						// デストラクタ

	HRESULT Init(void);				// 初期化
	void Uninit(void);				// 終了
	void Update(void);				// 更新
	void SetCamera(void);			// 設定

	// 生成
	static CCamera *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float viewportX, float viewportY, float viewportWidth, float viewporHeight, int nID);

	//--------------------------------------------------
	// 設定・取得関数
	//--------------------------------------------------
	void SetRot(D3DXVECTOR3 rot)	{ m_rot = rot; }								// 回転設定
	void SetPosR(D3DXVECTOR3 posR)	{ m_posR = posR; }								// 注視点の位置設定
	D3DXVECTOR3 GetPosV(void)		{ return m_posV; }								// 視点
	D3DXVECTOR3 GetPosR(void)		{ return m_posR; }								// 注視点
	D3DXVECTOR3 GetRot(void)		{ return m_rot; }								// 回転
	float		GetLong(void)		{ return m_fLong; }								// 視点注視点の距離
	float		GetSpeed()			{ return m_fSpeed; }							// 移動スピード
	D3DXVECTOR3	GetSpeed2()			{ return{ m_fSpeedX,m_fSpeedY,m_fSpeedZ }; }	// 移動量

	void MoveCamera(void);									// 移動関数

private:
	const static float m_fMoveAmt[AMOUNT_MAX];		// カメラ移動量の変更
	D3DXVECTOR3 m_posV;								// 現在の視点
	D3DXVECTOR3 m_posR;								// 現在の注視点
	D3DXVECTOR3 m_vecU;								// 上方向ベクトル
	D3DXVECTOR3 m_rot;								// カメラの向き
	int			m_nAmtID;							// 移動量の番号
	float		m_fLong;							// 視点と注視点の間の距離
	float		m_fSpeedX;							// カメラ移動速度X
	float		m_fSpeedY;							// カメラ移動速度Y
	float		m_fSpeedZ;							// カメラ移動速度Z
	float		m_fSpeed;							// カメラ移動速度

	bool		m_bPosR_Move;						// 注視点が動くか動かないか判定
	bool		m_bFieldMove;						// カメラ移動判定(メッシュフィールドモード)
	D3DXMATRIX m_mtxProjection;						// プロジェクションマトリックス
	D3DXMATRIX m_mtxView;							// ビューマトリックス
};

#endif
