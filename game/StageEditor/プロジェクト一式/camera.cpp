//=============================================================================
// カメラの作成
// Author:村元翼
//=============================================================================
#include "camera.h"
#include "renderer.h"
#include "manager.h"
#include "keyboard.h"
#include "XInput.h"
#include "DirectInput.h"
#include "mouse.h"
#include "game.h"
#include "meshfield.h"

//==============================================================================
// マクロ定義
//==============================================================================
#define CAMERA_DISTANCE			(500.0f)	// カメラ距離
#define CAMERA_DISTANCE_MIN		(10.0f)		// カメラ距離最小
#define CAMERA_DISTANCE_MAX		(10000.0f)	// カメラ距離最大
#define CAMERA_SPEED			(0.04f)		// カメラ移動速度
#define MOUSE_ROT_SPEED			(0.01f)		// カメラ回転速度(MOUSE)
#define LONG_SPEED				(10.0f)		// カメラ遠近移動速度
#define POSR_Y_MOVE				(2.0f)		// カメラ注視点移動速度
#define POSR_Y_MOVEHEIGHT		(150.0f)	// カメラ注視点が動き始める高さ
#define _OX_EPSILON_			(0.000001f) // 誤差

//=============================================================================
// 静的メンバ変数の初期化
//=============================================================================
const float CCamera::m_fMoveAmt[AMOUNT_MAX] =
{
	1.0f,
	2.0f,
	5.0f,
	10.0f,
	20.0f,
	50.0f,
	100.0f
};

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
//=============================================================================
// カメラの初期化処理
//=============================================================================
HRESULT CCamera::Init(void)
{
	m_nAmtID = 0;
	m_fSpeed = m_fMoveAmt[3];					// 初期移動量(モデル選択時)
	m_fSpeedX = 0.0f;								// 初期移動量X(フィールド選択時)
	m_fSpeedY = 50.0f;								// 初期移動量Y(フィールド選択時)
	m_fSpeedZ = 0.0f;								// 初期移動量Z(フィールド選択時)
	m_fLong = CAMERA_DISTANCE;						// 視点注視点の距離
	m_posV = D3DXVECTOR3(0.0f, 100.0f, -m_fLong);	// カメラ視点
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// カメラ注視点
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);			// カメラベクトル
	m_rot = D3DXVECTOR3(2.0f, 0.0f, 0.0f);			// カメラ回転
	m_bPosR_Move = false;
	m_bFieldMove = false;
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
}

//================================================
// カメラの生成処理
//================================================
CCamera *CCamera::Create(D3DXVECTOR3 m_pos, D3DXVECTOR3 rot, float viewportX, float viewportY, float viewportWidth, float viewporHeight, int nID)
{
	//インスタンスの生成
	CCamera *pCamera = nullptr;
	if (!pCamera)
	{
		pCamera = new CCamera;
		if (pCamera)
		{
			pCamera->Init();	// 初期化
		}
	}
	return pCamera;
}

//================================================
// カメラの設定
//================================================
void CCamera::SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイス取得

	//ビューポートの設定
	//pDevice->SetViewport(&m_viewport);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
								D3DXToRadian(50.0f),							//カメラの画角
								(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,		//画面比率!!!ビューポートを使うときビューポートの幅高さを使う
								10.0f,											//手前の描画制限
								10000.0f);										//奥の描画制限

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

	//++++++++++++++++++++++++++++++//
	//			視点の回転			//
	//++++++++++++++++++++++++++++++//
	if (CInput::PressAnyAction(CInput::ACTION_R_LEFT))
	{	// 左へ回転
		m_rot.y += CAMERA_SPEED;
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y = -D3DX_PI;
		}
	}
	if (CInput::PressAnyAction(CInput::ACTION_R_RIGHT))
	{	// 右へ回転
		m_rot.y += -CAMERA_SPEED;
		if (m_rot.y < -D3DX_PI)
		{
			m_rot.y = D3DX_PI;
		}
	}

	if (CInput::PressAnyAction(CInput::ACTION_R_DOWN))
	{	// 下へ回転
		m_rot.x += -CAMERA_SPEED;
		if (m_rot.x < 0.0f + D3DX_PI / 2)
		{
			m_rot.x = 0.0f + D3DX_PI / 2 + _OX_EPSILON_;
		}
	}
	if (CInput::PressAnyAction(CInput::ACTION_R_UP))
	{	// 上へ回転
		m_rot.x += CAMERA_SPEED;
		if (m_rot.x > D3DX_PI)
		{
			m_rot.x = D3DX_PI - _OX_EPSILON_;
		}
	}

	//****************************//
	//	　	GamePadでのズーム	  //
	//****************************//
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

	//****************************//
	//	　	マウスでの回転		  //
	//****************************//
	CMouse *pMouse = CManager::GetInstance()->GetMouse();
	float CameraPosRX = (float)pMouse->GetMousePos().lX;	// マウスカーソルの横軸
	float CameraPosRZ = (float)pMouse->GetMousePos().lY;	// マウスカーソルの縦軸
	float MouseWheel = (float)pMouse->GetMousePos().lZ;		// マウスホイールの回転

	if (pMouse->GetPress(CMouse::MOUSE_RIGHT))
	{
		// マウスの位置固定&カーソルを非表示にする
		SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		//ShowCursor(false);

		// 横回転
		if (CameraPosRZ > 0.0f || CameraPosRZ < -0.0f)
		{
			m_rot.x += (MOUSE_ROT_SPEED / 3) * CameraPosRZ;
		}

		// 縦回転
		if (CameraPosRX > 0.0f || CameraPosRX < -0.0f)
		{
			m_rot.y += (MOUSE_ROT_SPEED / 3) * CameraPosRX;
		}
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

	//++++++++++++++++++++++++++++//
	//		カメラ回転限界		  //
	//++++++++++++++++++++++++++++//
	// 横回転
	if (m_rot.y < -D3DX_PI)
	{
		m_rot.y = D3DX_PI;
	}
	else if (m_rot.y > D3DX_PI)
	{
		m_rot.y = -D3DX_PI;
	}

	// 縦回転
	if (m_rot.x <  D3DXToRadian(90.0f))
	{
		m_rot.x = D3DXToRadian(90.0f);
	}
	else if (m_rot.x > D3DXToRadian(170.0f))
	{
		m_rot.x = D3DXToRadian(170.0f);
	}

	//++++++++++++++++++++++++++++//
	//		カメラ移動量変更	  //
	//++++++++++++++++++++++++++++//
	// フィールドを置くときの移動
	CMeshField *pMeshField = CManager::GetGame()->GetFieldPrev();
	if (pMeshField)
	{
		if (!m_bFieldMove)
		{
			m_bFieldMove = true;

			// 整数2桁以下を四捨五入してキリの良い数値にする
			m_posR.x = roundf(m_posR.x / 100) * 100;	
			m_posR.z = roundf(m_posR.z / 100) * 100;
		}

		m_fSpeedX = pMeshField->GetSize().x;
		m_fSpeedZ = pMeshField->GetSize().z;

		//++++++++++++++++++++++++++++//
		//		  カメラの移動	      //
		//++++++++++++++++++++++++++++//
		// 左移動
		if (pKey->GetTrigger(DIK_A))
		{
			m_posV.x -= m_fSpeedX;
			m_posR.x -= m_fSpeedX;
		}

		// 右移動
		else if (pKey->GetTrigger(DIK_D))
		{
			m_posV.x += m_fSpeedX;
			m_posR.x += m_fSpeedX;
		}

		// 奥移動
		if (pKey->GetTrigger(DIK_W))
		{
			m_posV.z += m_fSpeedZ;
			m_posR.z += m_fSpeedZ;
		}

		// 手前移動
		else if (pKey->GetTrigger(DIK_S))
		{
			m_posV.z -= m_fSpeedZ;
			m_posR.z -= m_fSpeedZ;
		}

		// 上移動
		if (pKey->GetTrigger(DIK_T))
		{
			m_posR.y += m_fSpeed;
			m_posV.y += m_fSpeed;
		}

		// 下移動
		else if (pKey->GetTrigger(DIK_G))
		{
			m_posR.y -= m_fSpeed;
			m_posV.y -= m_fSpeed;
		}
	}

	// モデルを置くときの移動
	else
	{
		if (m_bFieldMove)
		{
			m_bFieldMove = false;
			m_fSpeed = m_fMoveAmt[0];
		}

		//++++++++++++++++++++++++++++//
		//		  カメラの移動	      //
		//++++++++++++++++++++++++++++//
		// 左移動
		if (pKey->GetPress(DIK_A))
		{
			m_posV.x -= m_fSpeed;
			m_posR.x -= m_fSpeed;
		}

		// 右移動
		else if (pKey->GetPress(DIK_D))
		{
			m_posV.x += m_fSpeed;
			m_posR.x += m_fSpeed;
		}

		// 奥移動
		if (pKey->GetPress(DIK_W))
		{
			m_posV.z += m_fSpeed;
			m_posR.z += m_fSpeed;
		}

		// 手前移動
		else if (pKey->GetPress(DIK_S))
		{
			m_posV.z -= m_fSpeed;
			m_posR.z -= m_fSpeed;
		}

		// 上移動
		if (pKey->GetPress(DIK_T))
		{
			m_posR.y += m_fSpeed;
			m_posV.y += m_fSpeed;
		}

		// 下移動
		else if (pKey->GetPress(DIK_G))
		{
			m_posR.y -= m_fSpeed;
			m_posV.y -= m_fSpeed;
		}


		if ((pKey->GetTrigger(DIK_L) || pKey->GetTrigger(DIK_O)))
		{
			if (pKey->GetTrigger(DIK_L))
			{
				m_nAmtID++;
				if (m_nAmtID >= AMOUNT_MAX)
				{
					m_nAmtID = 0;
				}
			}

			else if (pKey->GetTrigger(DIK_O))
			{
				m_nAmtID--;
				if (m_nAmtID < AMOUNT_00)
				{
					m_nAmtID = AMOUNT_MAX - 1;
				}
			}

			// 移動量変更
			m_fSpeed = m_fMoveAmt[m_nAmtID];
		}
	}

	// 視点の場所を注視点を元に移動(極座標回転)
	m_posV.x = m_posR.x - m_fLong * sinf(m_rot.x) * sinf(m_rot.y);
	m_posV.z = m_posR.z - m_fLong * sinf(m_rot.x) * cosf(m_rot.y);
	m_posV.y = m_posR.y - m_fLong * cosf(m_rot.x);
}