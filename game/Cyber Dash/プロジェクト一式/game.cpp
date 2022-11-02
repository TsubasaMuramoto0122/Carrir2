//=========================================================
// ゲームシーンの処理
// Author:村元翼
//=========================================================
#include "game.h"
#include "texture.h"
#include "manager.h"
#include "fade.h"
#include "meshfield.h"
#include "meshcylinder.h"
#include "meshsphere.h"
#include "stage.h"
#include "score.h"
#include "Scene3D.h"
#include "field.h"
#include "keyboard.h"
#include "XInput.h"
#include "DirectInput.h"
#include "LoadX.h"
#include "Ui.h"
#include "TextureFont.h"
#include "Goal.h"
#include "RemainCount.h"
#include "StringManager.h"
#include "ClearGauge.h"
#include "player.h"
#include "JumpCircle.h"
#include "sound.h"
#include "playdata.h"

//=========================================================
// マクロ定義
//=========================================================
#define START_FRAME		(60)									 // スタートフレーム
#define FONT_SIZE		(32)									 // フォントサイズ
#define UI_SIZE			(D3DXVECTOR3(1.0f,1.0f,0.0f))			 // UIサイズ
#define SKY_SIZE		(D3DXVECTOR3(6000.0f,20000.0f,6000.0f))	 // 空のサイズ
#define CYLINDER_ROT	(0.003f)								 // シリンダー回転量

//=========================================================
// 静的メンバ変数の初期化
//=========================================================
CRemainCount *CGame::m_pRemainCount = nullptr;			// 死亡カウント
CScore		 *CGame::m_pScore = nullptr;				// スコア
CStage		 *CGame::m_pStage = nullptr;				// ステージ
bool		  CGame::m_bRetry = false;					// リトライ

//=========================================================
// コンストラクタ
//=========================================================
CGame::CGame()
{
	m_nFrame		= 0;							// フレーム
	m_CameraRot		= { 0.0f,0.0f,0.0f };			// カメラ回転量
	m_pMeshCylinder = nullptr;						// シリンダー
	m_pMeshField	= nullptr;						// フィールド
	m_pMeshSphere	= nullptr;						// スフィア
	m_pMeshWall		= nullptr;						// ウォール
	m_pStage		= nullptr;						// ステージ
	m_pUI			= nullptr;						// UI
	m_bOnce			= false;						// 一度のみ通らせる変数
}

//=========================================================
// デストラクタ
//=========================================================
CGame::~CGame()
{

}

//=========================================================
// 初期化
//=========================================================
HRESULT CGame::Init(void)
{
	// ストップ解除
	CManager::GetInstance()->SetStop(false);

	// 空生成
	m_pMeshCylinder = CMeshCylinder::Create({ 0.0f,0.0f,0.0f }, SKY_SIZE, { 0.0f,0.0f,0.0f }, 12, 12,CMeshCylinder::CYLINDER_TYPE::STAND_INSIDE);
	m_pMeshCylinder->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_CYBER"));

	// ステージ生成	
	if (m_pStage = new CStage)
	{
		m_pStage->Init();									// 初期化
		m_pStage->SetStage("data/TEXT/StageSet000.txt");	// モデル設定のファイル名
	}
	

	// 残機カウント生成
	m_pRemainCount = CRemainCount::Create(SCREEN_CENTER, REMAIN_SIZE);

	// クリアゲージ
	CClearGauge::Create(GAUGE_POS, GAUGE_SIZE);

	// テクスチャフォント
#if _DEBUG
	CStringManager::Create(FONT_POS, (float)FONT_SIZE, FONT_SIZE, "Ｑでカメラ切り替え","ＭＳゴシック");
	CStringManager::Create({ FONT_POS.x,FONT_POS.y + FONT_SIZE,FONT_POS.z }, (float)FONT_SIZE, FONT_SIZE, "テクスチャフォント" );
#endif

	return S_OK;
}

//=========================================================
// 終了
//=========================================================
void CGame::Uninit(void)
{
	// ステージ終了
	if (m_pStage)
	{
		m_pStage->Uninit();
		delete m_pStage;
		m_pStage = nullptr;
	}

	// 死亡カウント終了
	if (m_pRemainCount)
	{
		CPlayData *pPlayData = CManager::GetInstance()->GetPlayData();
		pPlayData->SetScore(m_pRemainCount->GetRemain());
		m_pRemainCount->Uninit();
		m_pRemainCount = nullptr;
	}
}

//=========================================================
// 更新
//=========================================================
void CGame::Update(void)
{
	// スフィア回転
	if (m_pMeshSphere)
	{
		m_pMeshSphere->AddRot({ 0.0f,-CYLINDER_ROT,0.0f });
		m_pMeshSphere->CMesh::SetPos(m_pMeshSphere,m_pStage->GetPlayer()->GetPos(), m_pMeshSphere->GetSize());
	}

	// 背景シリンダーの回転・移動
	if (m_pMeshCylinder)
	{
		m_pMeshCylinder->AddRot({ 0.0f,-CYLINDER_ROT,0.0f });
		if (!m_pStage->GetPlayer()->GetDeath())
		{
			m_pMeshCylinder->CMesh::SetPos(m_pMeshCylinder, { m_pStage->GetPlayer()->GetPos().x,0.0f, m_pStage->GetPlayer()->GetPos().z }, m_pMeshCylinder->GetSize());
		}
	}

#if (1)
	if (!m_bStart && !m_bEnd)
	{
		// 最初は画面をストップ
		if (m_nFrame == 0)
		{
			CManager::GetInstance()->SetStop(true);
		}

		// フレームカウント
		m_nFrame++;

		// スタート
		if (m_nFrame == START_FRAME)
		{
			m_pUI = CUi::Create(SCREEN_CENTER, { UI_SIZE.x * 3,UI_SIZE.y,0.0f }, CUi::UI_TYPE_ZOOM);
			m_pUI->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_GAMESTART"));
		}
		else if (m_nFrame == START_FRAME * 2)
		{
			SetUiDelete();
			m_nFrame = 0;
			m_bStart = true;
			CManager::GetInstance()->SetStop(false);
		}
		
	}
#endif
}
//=========================================================
// 描画
//=========================================================
void CGame::Draw(void)
{

}

//=========================================================
// UI削除
//=========================================================
void CGame::SetUiDelete(void)
{
	if (m_pUI)
	{
		m_pUI->Uninit();
		delete m_pUI;
		m_pUI = nullptr;
	}
}