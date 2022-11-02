//=============================================================================
// プレイヤーの処理
// Author:村元翼
//=============================================================================
#include "main.h"
#include "manager.h"
#include "LoadX.h"
#include "renderer.h"
#include "player.h"
#include "model.h"
#include "keyboard.h"
#include "camera.h"
#include "Scene3D.h"
#include "XInput.h"
#include "DirectInput.h"
#include "effect.h"
#include "model_spawner.h"
#include "meshfield.h"
#include "shadow.h"
#include "mouse.h"
#include "InputManager.h"
#include "sound.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define PLAYER_SHADOWSIZE	(D3DXVECTOR3(m_size.x * 1.5f,0.0f,m_size.z * 1.5f))	// プレイヤー影サイズ
#define MOVE_EFFECTSIZE		(D3DXVECTOR3(10.0f,10.0f,0.0f))						// 移動エフェクトサイズ
#define JUMP_EFFECT_NUM		(10)												// ジャンプしたときの一度に出すエフェクト量
#define MARK_SIZE			(D3DXVECTOR3(10.0f,40.0f,0.0f))						// 目印のサイズ
#define INERTIA				(0.1f)												// 慣性
#define ACCELERATION		(0.2f)												// 加速力	
#define POS_Y_LIMIT			(1000.0f)											// 縦の移動限界値
#define REVERSE				(-1)												// 値を反転させる

//=============================================================================
// 静的メンバ変数の初期化
//=============================================================================
const float CPlayer::m_fMaxSpeed		= 5.0f;		// 最大速度
const float CPlayer::m_fMaxGravity		= 12.0f;	// 重力最高速度
float		CPlayer::m_fFallSpeed		= 0.4f;		// 落下速度
float		CPlayer::m_fGravitySpeed	= 0.6f;		// 重力の強さ
float		CPlayer::m_fJump			= 11.0f;	// ジャンプ力
float		CPlayer::m_fHop				= 8.0f;		// ジャンプ力(UFO時)
float		CPlayer::m_fCubeRotate		= 0.1f;		// 回転量(キューブ時)
float		CPlayer::m_fBallRotate		= 0.3f;		// 回転量(ボール時)
float		CPlayer::m_fBuoyancy		= 0.3f;		// 浮力

//=============================================================================
// コンストラクタ
//=============================================================================
CPlayer::CPlayer(OBJTYPE nPriority) : CScene(nPriority)
{
	m_pModel		= nullptr;							  // モデル
	m_pBillboard	= nullptr;							  // ビルボード
	m_pos			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	  // 位置
	m_posOld		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	  // 最後に更新した位置
	m_Speed			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	  // 速度
	m_fGravity		= 0.0f;								  // 重力
	m_fAngle		= 0.0f;								  // 角度
	m_bJumpLock		= false;							  // ジャンプロック
	m_bReverse		= false;							  // 反転の判定
	m_bJump			= false;							  // ジャンプの判定
	m_bUninit		= false;							  // 終了判定
	m_bSwitch		= false;							  // 切り替え判定
	m_bMove			= false;							  // 移動判定
	m_bHit			= false;							  // ヒット判定
	m_bCameraMove	= false;							  // カメラ移動判定
	m_state			= PLAYER_STATE::MAX;				  // プレイヤーの状態
}

//=============================================================================
// デストラクタ
//=============================================================================
CPlayer::~CPlayer()
{

}

//=============================================================================
// オブジェクト生成
//=============================================================================
CPlayer *CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, PLAYER_STATE nType, bool bMove, bool bCameraMove)
{
	// オブジェクト生成
	CPlayer *pPlayer = nullptr;
	if (pPlayer = new CPlayer)
	{
		// モデルの生成
		if (!pPlayer->m_pModel)
		{
			// 状態別モデル生成
			pPlayer->SetState(nType);
			// モデルタイプの設定
			pPlayer->CScene::SetModelType(CScene::MODTYPE_PLAYER);
		}

		pPlayer->m_bCameraMove = bCameraMove;	// カメラ移動可能か
		pPlayer->m_bMove = bMove;				// 移動可能か
		pPlayer->Init(pos, rot);				// 初期化
	}

	return pPlayer;
}

//=============================================================================
// 初期化
//=============================================================================
HRESULT CPlayer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 位置・回転取得
	m_pos = pos;
	m_rot = rot;

	// サイズ取得
	m_size = m_pModel->GetSize();

	// 影の設定
	if (!m_pShadow)
	{
		m_pShadow = CShadow::Create({m_pos.x , m_pos.y - (m_size.y / 2),m_pos.z}, PLAYER_SHADOWSIZE, { 0.0f,0.0f,0.0f });
	}

	// 位置・回転設定
	CScene::SetPos(m_pos);
	CScene::SetSize(m_size);

	// 回転用マトリックスの初期化
	D3DXMatrixIdentity(&m_mtxRot);

	// カメラの取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera(0);
	pCamera->SetPlayerCamera(this);

	return S_OK;
}

//=============================================================================
// 終了
//=============================================================================
void CPlayer::Uninit()
{
	// プレイヤーモデル破棄
	ModelDelete();

	// 影の破棄
	if (m_pShadow)
	{
		m_pShadow->Uninit();
		m_pShadow = nullptr;
	}

	// オブジェクト破棄
	Release();

	// ゲームサウンド停止
	CManager::GetInstance()->GetSound()->StopSound(CSound::SOUND_LABEL_BGM_GAME);
	
	if (m_bReverse)
	{
		SetReverse();			// 重力反転
	}
}

//=============================================================================
// 更新
//=============================================================================
void CPlayer::Update()
{
	// シーンに保存した位置を取得
	 m_pos = CScene::GetPos();

	 // 現在の位置を最後の位置にする
	 m_posOld = m_pos;
	 CScene::SetPosOld(m_posOld);

	 if (m_bCameraMove)
	 {
		 // カメラの取得
		 CCamera *pCamera = CManager::GetInstance()->GetCamera(0);

#ifdef _DEBUG
		 //-------------------------------------------
		 // カメラ追従関数の呼び出し
		 //-------------------------------------------
		 if (pCamera)
		 {
			 if (m_bSwitch)
			 {
				 switch (m_state)
				 {
					 // 飛行船専用カメラ移動
				 case PLAYER_STATE::AIRSHIP:
					 pCamera->SetPlayerCamera2(this);
					 break;

					 // プレイヤー追従カメラ移動
				 default:
					 pCamera->SetPlayerCamera(this);
					 break;
				 }
			 }
			 else
			 {
				 // カメラ自由移動
				 pCamera->NoFollowingPlayer();
			 }

			 //-------------------------------------------
			 // カメラ追従切り替え
			 //-------------------------------------------
			 if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_Q))
			 {
				 m_bSwitch = !m_bSwitch;
				 if (m_bSwitch)
				 {
					 pCamera->SetLong(CAMERA_DISTANCE);
					 pCamera->SetRot(CAMERA_ROT);
				 }
				 else
				 {

				 }
			 }
		 }

#else
		 switch (m_state)
		 {
			 // 飛行船専用カメラ移動
		 case PLAYER_STATE::AIRSHIP:
			 pCamera->SetPlayerCamera2(this);
			 break;

			 // プレイヤー追従カメラ移動
		 default:
			 pCamera->SetPlayerCamera(this);
			 break;
		 }
#endif
	 }

	if (m_bMove)
	{
		 //------------------------------------------
		 // プレイヤー制御系関数
		 //------------------------------------------
		 Move();		// 移動
		 Action();		// アクション
	}

	// シーンに位置を設定する
	CScene::SetPos(m_pos);

	if (m_pModel)	// プレイヤーモデル更新
	{
		m_pModel->Update();
	}

	if (m_pShadow)	// シャドウの位置設定
	{
		m_pShadow->CScene::SetPosOld({ m_pos.x ,m_pos.y, m_pos.z });
	}

	// 終了判定
	if (m_bUninit)
	{
		Uninit();
	}

	// ヒット判定OFF
	m_bHit = false;
}

//==========================================================================================================
// 描画
//==========================================================================================================
void CPlayer::Draw()
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice(); // デバイスのポインタ
	D3DXMATRIX mtxRotModel, mtxTransModel;											 // 計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);
	
	// クォータニオンで生成した回転行列を掛ける
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxRot, &m_mtxWorld);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTransModel, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTransModel);

	// 各パーツのワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// モデルの描画
	m_pModel->Draw();
}

//==========================================================================================================
// 移動処理
//==========================================================================================================
void CPlayer::Move(void)
{
#ifndef _DEBUG
	m_Speed.x += ACCELERATION;	// 移動
#endif

	//=============================================================================
	// 移動する(座標更新)
	//=============================================================================
	if (m_Speed.x != 0.0f)
	{
		m_pos.x += m_Speed.x;

		// 移動のエフェクト
		CEffect::Create(m_pos, MOVE_EFFECTSIZE, { 1.0f, 1.0f, 1.0f ,1.0f }, 0.1f, 1);
	}

	//=============================================================================
	// 状態別移動処理
	//=============================================================================
	switch (m_state)
	{
	//----------------------------------
	// ボール
	//----------------------------------
	case PLAYER_STATE::BALL:
		m_fAngle -= m_fBallRotate;					// 常に回転させる
		Quaternion();								// クォータニオン回転

		// ジャンプ
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_SPACE) && !m_bJump && !m_bJumpLock)	// Aボタン
		{
			JumpAction(m_fGravity);	// ジャンプ
			SetReverse();			// 重力反転
		}

		// 重力限界
		if (m_fGravity <= -m_fMaxGravity)
		{
			m_fGravity = -m_fMaxGravity;
		}
		else if (m_fGravity >= m_fMaxGravity)
		{
			m_fGravity = m_fMaxGravity;
		}

		Gravity(m_pos, m_fGravity, m_fGravitySpeed, m_bJump);		// 重力
		Fall(m_pos, m_posOld);

		break;

	//----------------------------------
	// キューブ
	//----------------------------------
	case PLAYER_STATE::CUBE:
		if (m_bJump)
		{
			m_fAngle -= m_fCubeRotate;				// ジャンプ時に回転
			Quaternion();							// クォータニオン回転
		}
		else
		{
			D3DXMatrixIdentity(&m_mtxRot);			// 着地時に回転初期化
		}

		// ジャンプ
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_SPACE) && !m_bJump && !m_bJumpLock)	// Aボタン
		{
			JumpAction(m_fJump);										// ジャンプ
		}

		Gravity(m_pos, m_fGravity, m_fGravitySpeed, m_bJump);			// 重力
		Fall(m_pos, m_posOld);

		break;

	//----------------------------------
	// 飛行船
	//----------------------------------
	case PLAYER_STATE::AIRSHIP:
		m_fAngle = atan2(m_Speed.y, m_Speed.x);	// 縦横の移動量から角度を求める
		Quaternion();							// クォータニオン回転

		if (CInputManager::PressAnyAction(CInputManager::PRESS_SPACE))
		{
			// 浮遊エフェクト
			CEffect::Create(m_pos, MOVE_EFFECTSIZE, { 0.0f,1.0f,0.0f,1.0f }, 0.05f, 2, "");
			m_Speed.y += m_fBuoyancy;		// 上昇する
		}
		else
		{
			if (m_bHit)
			{
				m_Speed.y = 0.0f;			// 天井か床に乗っているときはスピードを0にする
			}
			else
			{
				m_Speed.y -= m_fBuoyancy;	// 落ちる
			}
		}

		m_fGravity = m_Speed.y;										// 重力にスピードを代入
		Gravity(m_pos, m_fGravity, m_fGravitySpeed, m_bJump);		// 重力関数

		break;

	//----------------------------------
	// ＵＦＯ
	//----------------------------------
	case PLAYER_STATE::UFO:
		// ジャンプ
		if (CInputManager::TriggerAnyAction(CInputManager::TRIGGER_SPACE) && !m_bJumpLock)	// Aボタン
		{
			JumpAction(m_fHop);									// 跳ねる
		}

		Gravity(m_pos, m_fGravity, m_fFallSpeed, m_bJump);		// 重力
		break;
	}

	Inertia(m_Speed);								// 慣性
	SpeedAndRotLimit(m_Speed, m_rot, m_fMaxSpeed);	// 速度と回転限界
	PosYLimit(m_pos.y);								// 高さ限界
}

//==========================================================================================================
// プレイヤーの慣性
//==========================================================================================================
void CPlayer::Inertia(D3DXVECTOR3 &speed)
{
	//------------------------------------------------
	// 左右入力していない状態
	//------------------------------------------------
	// 0に戻り続ける処理
	if (speed.x > 0.0f)
	{
		speed.x -= INERTIA;

		if (speed.x <= 0.0f)
		{
			speed.x = 0.0f;
		}
	}

	else if (speed.x < 0.0f)
	{
		speed.x += INERTIA;

		if (speed.x >= 0.0f)
		{
			speed.x = 0.0f;
		}
	}

	//------------------------------------------------
	// 上下入力していない状態
	//------------------------------------------------
	// 0に戻り続ける処理
	if (speed.z > 0.0f)
	{
		speed.z -= INERTIA;

		if (speed.z <= 0.0f)
		{
			speed.z = 0.0f;
		}
	}

	else if (speed.z < 0.0f)
	{
		speed.z += INERTIA;

		if (speed.z >= 0.0f)
		{
			speed.z = 0.0f;
		}
	}
}

//==========================================================================================================
// クォータニオン回転処理関数
//==========================================================================================================
void CPlayer::Quaternion(void)
{
	D3DXVECTOR3 moveVec = { m_pos - m_posOld };			// 移動ベクトル

	// 軸ベクトル(移動ベクトルと垂直のベクトル)
	D3DXVECTOR3 Axis = { 0.0f,0.0f,0.0f };
	Axis.x = -moveVec.z;
	Axis.z = moveVec.x;

	if (m_Speed.x != 0.0f || m_Speed.z != 0.0f)
	{
		D3DXQUATERNION QuaAns = { 0.0f,0.0f,0.0f,1.0f };					// 単位クォータニオン
		D3DXQuaternionRotationAxis(&QuaAns, &Axis, m_fAngle);				// 軸ベクトルと角度からクォータニオンを求める
		D3DXMatrixRotationQuaternion(&m_mtxRot, &QuaAns);					// クォータニオンを回転行列に変換
	}
}

//==========================================================================================================
// 速度 ＆ 回転の限界
//==========================================================================================================
void CPlayer::SpeedAndRotLimit(D3DXVECTOR3 &speed, D3DXVECTOR3 &rot,const float fMaxSpeed)
{
	//==========================================================================================================
	// 回転限界
	//==========================================================================================================
	if (rot.x < -D3DX_PI)
	{
		rot.x = D3DX_PI;
	}
	else if (rot.x > D3DX_PI)
	{
		rot.x = -D3DX_PI;
	}
	if (rot.z < -D3DX_PI)
	{
		rot.z = D3DX_PI;
	}
	else if (rot.z > D3DX_PI)
	{
		rot.z = -D3DX_PI;
	}

	if (m_fAngle > D3DX_PI)
	{
		m_fAngle = -D3DX_PI;
	}
	else if (m_fAngle < -D3DX_PI)
	{
		m_fAngle = D3DX_PI;
	}

	//==========================================================================================================
	// 速度限界
	//==========================================================================================================
	if (speed.x >= fMaxSpeed)
	{
		speed.x = fMaxSpeed;
	}
	else if (speed.x <= -fMaxSpeed)
	{
		speed.x = -fMaxSpeed;
	}
	if (speed.y >= fMaxSpeed)
	{
		speed.y = fMaxSpeed;
	}
	else if (speed.y <= -fMaxSpeed)
	{
		speed.y = -fMaxSpeed;
	}
	if (speed.z >= fMaxSpeed)
	{
		speed.z = fMaxSpeed;
	}
	else if (speed.z <= -fMaxSpeed)
	{
		speed.z = -fMaxSpeed;
	}
}

//==========================================================================================================
// プレイヤーアクション(デバッグ用)
//==========================================================================================================
void CPlayer::Action(void)
{
#ifdef _DEBUG
	// 入力デバイスの取得
	CInputkeyboard *pKey = CManager::GetInstance()->GetKeyboard();

	// モード切替
	if (pKey->GetTrigger(DIK_1))
	{
		SetState(PLAYER_STATE::BALL);
	}
	else if (pKey->GetTrigger(DIK_2))
	{
		SetState(PLAYER_STATE::CUBE);
	}
	else if (pKey->GetTrigger(DIK_3))
	{
		SetState(PLAYER_STATE::AIRSHIP);
	}
	else if (pKey->GetTrigger(DIK_4))
	{
		SetState(PLAYER_STATE::UFO);
	}
#endif
}

//==========================================================================================================
// ジャンプ関数
//==========================================================================================================
void CPlayer::JumpAction(const float& fJump)
{
	m_fGravity = fJump;
	m_bJump = true;

	// ジャンプエフェクト
	if (m_bReverse)
	{
		for (int nCnt = 0; nCnt < JUMP_EFFECT_NUM; nCnt++)
		{
			CEffect::Create(m_pos, MOVE_EFFECTSIZE, { 0.0f,1.0f,0.0f,1.0f }, 0.05f, 4, "");
		}
	}

	else
	{
		for (int nCnt = 0; nCnt < JUMP_EFFECT_NUM; nCnt++)
		{
			CEffect::Create(m_pos, MOVE_EFFECTSIZE, { 0.0f,1.0f,0.0f,1.0f }, 0.05f, 2, "");
		}
	}
}

//==========================================================================================================
// プレイヤー状態設定関数
//==========================================================================================================
void CPlayer::SetState(PLAYER_STATE state)
{
	if (m_state != state)	// 現在の状態と同じ状態なら通らない(姿が変わらないので処理が無駄)
	{
		// モデル終了
		ModelDelete();

		// カメラ取得
		CCamera *pCamera = CManager::GetInstance()->GetCamera(0);

		// モデルを切り替える
		if (!m_pModel)
		{
			char *pStr = nullptr;

			switch (state)
			{
			case PLAYER_STATE::BALL:
				pStr = "PLAYER_ONE";
				m_bJump = true;
				break;

			case PLAYER_STATE::CUBE:
				pStr = "PLAYER_TWO";
				m_bJump = true;
				break;

			case PLAYER_STATE::AIRSHIP:
				pStr = "PLAYER_THREE";
				m_Speed.y = m_fGravity;

				// 移動可能か
				if (m_bMove)
				{
					pCamera->SetPosVY(200.0f);	// カメラ視点のY固定
					pCamera->SetPosRY(200.0f);	// カメラ注視点のY固定
				}

				break;

			case PLAYER_STATE::UFO:
				pStr = "PLAYER_FOUR";
				D3DXMatrixIdentity(&m_mtxRot);
				if (m_bReverse)
				{
					m_fAngle = D3DXToRadian(180.0f);		// 反転させる
					Quaternion();							// クォータニオン回転
				}
				break;
			}

			// Xファイルの種類設定
			CLoadX *pLoad = CManager::GetInstance()->GetLoadX();
			m_nType = pLoad->GetNum(pStr);

			// モデル生成
			m_pModel = CModel::Create({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, m_nType, false);
			// プレイヤーサイズの設定
			CScene::SetSize(m_pModel->GetSize());
		}

		m_state = state;	// 状態設定
	}
}

//==========================================================================================================
// 重力の処理(プレイヤーに重力を減らす値(初期値は減らしたい正の数)を持たして、第三引数に入れて)
//==========================================================================================================
void CPlayer::Gravity(D3DXVECTOR3& pos, float& fGravity, const float& fGravitySpeed, bool& bJump)
{
	// 重力加算
	fGravity -= fGravitySpeed;
	pos.y += fGravity;

	// 床についたかどうか
	if (pos.y <= 0.0f)
	{
		// 位置を固定する
		pos.y = 0.0f;
		fGravity = 0.0f;
		bJump = false;
	}
}

//==========================================================================================================
// モデル終了処理
//==========================================================================================================
void CPlayer::ModelDelete(void)
{
	if (m_pModel)
	{
		m_pModel->Uninit();
		delete m_pModel;
		m_pModel = nullptr;
	}
}

//==========================================================================================================
// 位置Y限界
//==========================================================================================================
void CPlayer::PosYLimit(float& PosY)
{
	if (PosY > POS_Y_LIMIT)
	{
		switch (m_state)
		{
		case PLAYER_STATE::BALL:
			if(m_fGravity > 0.0f)
			m_bJump = false;
			break;

		case PLAYER_STATE::CUBE:
			if (m_fGravity > 0.0f)
			m_bJump = false;
			break;

		case PLAYER_STATE::AIRSHIP:
			m_bHit = true;
			break;

		case PLAYER_STATE::UFO:
			break;
		}

		PosY = POS_Y_LIMIT;
		m_fGravity = 0.0f;
	}
}

//==========================================================================================================
// 重力反転処理
//==========================================================================================================
void CPlayer::SetReverse(void)
{
	m_bReverse = !m_bReverse;	// 重力反転
	m_fJump			*= REVERSE;
	m_fHop			*= REVERSE;
	m_fGravitySpeed *= REVERSE;
	m_fFallSpeed	*= REVERSE;
	m_fCubeRotate	*= REVERSE;
	m_fBallRotate	*= REVERSE;
	m_fBuoyancy		*= REVERSE;
}

//==========================================================================================================
// 落下判定
//==========================================================================================================
void CPlayer::Fall(const D3DXVECTOR3& pos, const D3DXVECTOR3& posOld)
{
	//落下判定
	float moveY = pos.y - posOld.y;

	if (moveY != 0.0f)
	{
		m_bJump = true;
	}
}
