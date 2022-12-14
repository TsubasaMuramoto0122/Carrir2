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
#define MAX_CAMERA				(1)								// カメラの最大数
#define CAMERA_DISTANCE			(800.0f)						// ゲームのカメラ距離 
#define CAMERA_ROT				(D3DXVECTOR3(1.8f,0.0f,0.0f))	// ゲームのカメラ回転

//==============================================================================
// 前方宣言
//==============================================================================
class CPlayer;

//==============================================================================
// クラスの定義
//==============================================================================
class CCamera
{
public:
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
	void SetPosVY(float posVY) { m_posV.y = posVY; }
	void SetPosRY(float posRY) { m_posR.y = posRY; }
	void SetRot(D3DXVECTOR3 rot)	{ m_rot = rot; }		// 回転の設定
	void SetLong(float fLong)		{ m_fLong = fLong; }	// 視点注視点の距離の設定
	D3DXVECTOR3 GetPosV(void)		{ return m_posV; }		// 視点の取得
	D3DXVECTOR3 GetPosR(void)		{ return m_posR; }		// 注視点の取得
	D3DXVECTOR3 GetRot(void)		{ return m_rot; }		// 回転の取得
	float		GetLong(void)		{ return m_fLong; }		// 視点注視点の距離の取得

	void SetPlayerCamera(CPlayer *pPlayer);					// カメラがプレイヤーに追従する関数
	void SetPlayerCamera2(CPlayer *pPlayer);				// カメラがプレイヤーに追従する関数
	void NoFollowingPlayer(void);							// カメラがプレイヤーに追従していない時の処理
	void MoveCamera(void);									// 移動関数
	void RotLimit(void);									// 回転限界


private:
	D3DXVECTOR3 m_posV;				// 現在の視点
	D3DXVECTOR3 m_posR;				// 現在の注視点
	D3DXVECTOR3 m_vecU;				// 上方向ベクトル
	D3DXVECTOR3 m_rot;				// カメラの向き
	float		m_fLong;			// 長さ
	D3DXMATRIX m_mtxProjection;		// プロジェクションマトリックス
	D3DXMATRIX m_mtxView;			// ビューマトリックス
};

#endif
