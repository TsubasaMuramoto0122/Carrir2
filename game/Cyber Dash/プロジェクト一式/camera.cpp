//=============================================================================
// カメラの作成
// Author:村元翼
//=============================================================================
#include "camera.h"
#include "renderer.h"
#include "manager.h"
#include "keyboard.h"
#include "player.h"
#include "XInput.h"
#include "DirectInput.h"
#include "mouse.h"
#include "InputManager.h"

//==============================================================================
// マクロ定義
//==============================================================================
#define CAMERA_DISTANCE_MIN		(50.0f)		// カメラ距離最小
#define CAMERA_DISTANCE_MAX		(5000.0f)	// カメラ距離最大
#define CAMERA_MOVE_SPEED		(10.0f)		// カメラ移動速度
#define CAMERA_ROT_SPEED		(0.04f)		// カメラ回転速度(PAD)
#define MOUSE_ROT_SPEED			(0.01f)		// カメラ回転速度(MOUSE)
#define LONG_SPEED				(10.0f)		// カメラ遠近移動速度
#define POSR_Y_MOVE				(2.0f)		// カメラ注視点移動速度
#define POSR_Y_MOVEHEIGHT		(150.0f)	// カメラ注視点が動き始める高さ
#define _OX_EPSILON_			(0.000001f) // 誤差

//=============================================================================
// コンストラクタ
//=============================================================================
CCamera::CCamera()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CCamera::~CCamera()
{

}
//================================================
// カメラの生成処理
//================================================
CCamera *CCamera::Create(D3DXVECTOR3 m_pos, D3DXVECTOR3 rot, float viewportX, float viewportY, float viewportWidth, float viewporHeight, int nID)
{
	// オブジェクトの生成
	CCamera *pCamera = nullptr;
	if (pCamera = new CCamera)
	{
		pCamera->Init();	// 初期化
	}

	return pCamera;
}

//=============================================================================
// カメラの初期化処理
//=============================================================================
HRESULT CCamera::Init(void)
{
	m_fLong = CAMERA_DISTANCE;						// 視点注視点の距離
	m_rot	= CAMERA_ROT;							// カメラ回転
	m_posV	= D3DXVECTOR3(0.0f, 200.0f, -m_fLong);	// カメラ視点
	m_posR	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// カメラ注視点
	m_vecU	= D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// カメラベクトル
	return S_OK;
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void CCamera::Uninit(void)
{

}

//=============================================================================
// カメラの更新処理
//=============================================================================
void CCamera::Update(void)
{
	// カメラの移動
	MoveCamera();
	// 回転限界
	RotLimit();
}

//================================================
// カメラの設定
//================================================
void CCamera::SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice;										// デバイスのポインタ
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイス取得

	// ビューポートの設定
	//pDevice->SetViewport(&m_viewport);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH
	(
		&m_mtxProjection,
		D3DXToRadian(50.0f),							// カメラの画角
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,		// 画面比率!!!ビューポートを使うときビューポートの幅高さを使う
		10.0f,											// 手前の描画制限
		10000.0f										// 奥の描画制限
	);							

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH
	(
		&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU
	);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}

//===============================================================
// カメラの移動
//===============================================================
void CCamera::MoveCamera(void)
{
	// 入力デバイスの取得
	CInputkeyboard *pKey = CManager::GetInstance()->GetKeyboard();
	CDInput *pDInput = CManager::GetInstance()->GetDInput();
	CMouse *pMouse = CManager::GetInstance()->GetMouse();

	if (CInputManager::PressAnyAction(CInputManager::PRESS_R_DOWN))
	{	// 下へ回転
		m_rot.x += -CAMERA_ROT_SPEED;
		if (m_rot.x < 0.0f + D3DX_PI / 2)
		{
			m_rot.x = 0.0f + D3DX_PI / 2 + _OX_EPSILON_;
		}
	}
	if (CInputManager::PressAnyAction(CInputManager::PRESS_R_UP))
	{	// 上へ回転
		m_rot.x += CAMERA_ROT_SPEED;
		if (m_rot.x > D3DX_PI)
		{
			m_rot.x = D3DX_PI - _OX_EPSILON_;
		}
	}

	// カメラを近づける
	if (pDInput->GetPress(pDInput->BUTTON_R2))
	{
		m_fLong -= LONG_SPEED;
		if (m_fLong <= CAMERA_DISTANCE_MIN)
		{
			m_fLong = CAMERA_DISTANCE_MIN;
		}
	}

	// カメラを遠ざける
	else if (pDInput->GetPress(pDInput->BUTTON_L2))
	{
		m_fLong += LONG_SPEED;
		if (m_fLong >= CAMERA_DISTANCE_MAX)
		{
			m_fLong = CAMERA_DISTANCE_MAX;
		}
	}
}

//=============================================================================
// プレイヤーにカメラが追従する処理
//=============================================================================
void CCamera::SetPlayerCamera(CPlayer *pPlayer)
{
	// NULLチェック
	if (pPlayer)
	{
		//----------------------------------------------------------
		// カメラ注視点のY座標の追従を行う処理
		//----------------------------------------------------------
		float fLong = pPlayer->GetPos().y - m_posR.y;
		float fMove = (fLong / 30.0f);
		m_posR.y += fMove;					// Y座標を動かす(追従する)

		// 注視点の位置をプレイヤーの位置とする(y座標は例外)
		m_posR.x = pPlayer->GetPos().x;
		m_posR.z = pPlayer->GetPos().z;

		// 視点の場所をプレイヤーを元に移動(極座標回転)
		m_posV.x = pPlayer->GetPos().x - m_fLong * sinf(m_rot.x) * sinf(m_rot.y);
		m_posV.z = pPlayer->GetPos().z - m_fLong * sinf(m_rot.x) * cosf(m_rot.y);
		m_posV.y = pPlayer->GetPos().y - m_fLong * cosf(m_rot.x);
	}
}

//=============================================================================
// プレイヤーにカメラが追従する処理
//=============================================================================
void CCamera::SetPlayerCamera2(CPlayer *pPlayer)
{
	// NULLチェック
	if (pPlayer)
	{
		// 注視点の位置をプレイヤーの位置とする(y座標は例外)
		m_posR.x = pPlayer->GetPos().x;
		m_posR.z = pPlayer->GetPos().z;

		// 視点の場所をプレイヤーを元に移動(極座標回転)
		m_posV.x = pPlayer->GetPos().x - m_fLong * sinf(m_rot.x) * sinf(m_rot.y);
		m_posV.z = pPlayer->GetPos().z - m_fLong * sinf(m_rot.x) * cosf(m_rot.y);
		//m_posV.y = pPlayer->GetPos().y - m_fLong * cosf(m_rot.x);
	}
}


//=============================================================================
// プレイヤーにカメラが追従していない時の処理
//=============================================================================
void CCamera::NoFollowingPlayer(void)
{
	//****************************//
	//		   視点の移動		  //
	//****************************//
	if (CInputManager::PressAnyAction(CInputManager::PRESS_RIGHT))		// 右
	{
		m_posR.x += CAMERA_MOVE_SPEED * cosf(-m_rot.y);
		m_posR.z += CAMERA_MOVE_SPEED * sinf(-m_rot.y);
		m_posV.x += CAMERA_MOVE_SPEED * cosf(-m_rot.y);
		m_posV.z += CAMERA_MOVE_SPEED * sinf(-m_rot.y);
	}

	else if (CInputManager::PressAnyAction(CInputManager::PRESS_LEFT))	// 左
	{
		m_posR.x -= CAMERA_MOVE_SPEED * cosf(-m_rot.y);
		m_posR.z -= CAMERA_MOVE_SPEED * sinf(-m_rot.y);
		m_posV.x -= CAMERA_MOVE_SPEED * cosf(-m_rot.y);
		m_posV.z -= CAMERA_MOVE_SPEED * sinf(-m_rot.y);
	}

	if (CInputManager::PressAnyAction(CInputManager::PRESS_UP))			// 奥
	{
		m_posR.x += CAMERA_MOVE_SPEED * sinf(m_rot.y);
		m_posR.z += CAMERA_MOVE_SPEED * cosf(m_rot.y);
		m_posV.x += CAMERA_MOVE_SPEED * sinf(m_rot.y);
		m_posV.z += CAMERA_MOVE_SPEED * cosf(m_rot.y);
	}
	else if (CInputManager::PressAnyAction(CInputManager::PRESS_DOWN))	// 手前
	{
		m_posR.x -= CAMERA_MOVE_SPEED * sinf(m_rot.y);
		m_posR.z -= CAMERA_MOVE_SPEED * cosf(m_rot.y);
		m_posV.x -= CAMERA_MOVE_SPEED * sinf(m_rot.y);
		m_posV.z -= CAMERA_MOVE_SPEED * cosf(m_rot.y);
	}

	//****************************//
	//	　	マウスでの回転		  //
	//****************************//
	CMouse *pMouse = CManager::GetInstance()->GetMouse();
	float CameraPosRX = (float)pMouse->GetMousePos().lX;	// マウスカーソルの横軸
	float CameraPosRZ = (float)pMouse->GetMousePos().lY;	// マウスカーソルの縦軸
	float MouseWheel = (float)pMouse->GetMousePos().lZ;		// マウスホイールの回転

	// マウスの位置固定&カーソルを非表示にする
	SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	if (CameraPosRZ > 0.0f || CameraPosRZ < -0.0f)
	{
		m_rot.x += (MOUSE_ROT_SPEED / 3) * CameraPosRZ;
	}

	if (CameraPosRX > 0.0f || CameraPosRX < -0.0f)
	{
		m_rot.y += (MOUSE_ROT_SPEED / 3) * CameraPosRX;
	}

	//****************************//
	//	　	マウスでのズーム	  //
	//****************************//
	// カメラを近づける
	if (MouseWheel > 0.0f)
	{
		m_fLong -= LONG_SPEED * LONG_SPEED;
		if (m_fLong <= CAMERA_DISTANCE_MIN)
		{
			m_fLong = CAMERA_DISTANCE_MIN;
		}
	}

	// カメラを遠ざける
	else if (MouseWheel < 0.0f)
	{
		m_fLong += LONG_SPEED * LONG_SPEED;
		if (m_fLong >= CAMERA_DISTANCE_MAX)
		{
			m_fLong = CAMERA_DISTANCE_MAX;
		}
	}

#if(1)
	//++++++++++++++++++++++++++++++//
	//			視点の回転			//
	//++++++++++++++++++++++++++++++//
	if (CInputManager::PressAnyAction(CInputManager::PRESS_R_LEFT))
	{	// 左へ回転
		m_rot.y += CAMERA_ROT_SPEED;
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y = -D3DX_PI;
		}
	}
	if (CInputManager::PressAnyAction(CInputManager::PRESS_R_RIGHT))
	{	// 右へ回転
		m_rot.y += -CAMERA_ROT_SPEED;
		if (m_rot.y < -D3DX_PI)
		{
			m_rot.y = D3DX_PI;
		}
	}
#endif

	// 視点の場所を注視点を元に移動(極座標回転)
	m_posV.x = m_posR.x - m_fLong * sinf(m_rot.x) * sinf(m_rot.y);
	m_posV.z = m_posR.z - m_fLong * sinf(m_rot.x) * cosf(m_rot.y);
	m_posV.y = m_posR.y - m_fLong * cosf(m_rot.x);
}

//=============================================================================
// カメラ回転限界
//=============================================================================
void CCamera::RotLimit(void)
{
	//++++++++++++++++++++++++++++++//
	//			横回転			　　//
	//++++++++++++++++++++++++++++++//
	if (m_rot.y < -D3DX_PI)
	{
		m_rot.y = D3DX_PI;
	}
	else if (m_rot.y > D3DX_PI)
	{
		m_rot.y = -D3DX_PI;
	}

	//++++++++++++++++++++++++++++++//
	//			縦回転			　　//
	//++++++++++++++++++++++++++++++//
	if (m_rot.x < 0.0f)
	{
		m_rot.x = _OX_EPSILON_;
	}
	else if (m_rot.x > D3DX_PI)
	{
		m_rot.x = D3DX_PI - _OX_EPSILON_;
	}
}