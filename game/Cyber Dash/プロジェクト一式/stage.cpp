//========================================
// ステージ
// Author:村元翼
//========================================
#include "stage.h"
#include "model.h"
#include "texture.h"
#include "model_spawner.h"
#include "player.h"
#include "meshfield.h"
#include "meshcylinder.h"
#include "LoadX.h"
#include "manager.h"
#include "meshsphere.h"
#include "Goal.h"
#include "Obstacle.h"
#include "Gate.h"
#include "JumpCircle.h"

//========================================
// マクロ定義
//========================================
#define PLAYER_POS (D3DXVECTOR3(-1000.0f, 50.0f, 0.0f))
#define DEBUG_POS (D3DXVECTOR3(16600.0f, 50.0f, 0.0f))

//========================================
// 静的メンバ変数の初期化
//========================================
D3DXVECTOR3 CStage::m_StartPos	= { 0.0f,0.0f,0.0f };
D3DXVECTOR3 CStage::m_GoalPos	= { 0.0f,0.0f,0.0f };
CGoal	*CStage::m_pGoal		= nullptr;
CPlayer *CStage::m_pPlayer		= nullptr;

//========================================
// コンストラクタ
//========================================
CStage::CStage()
{
	m_StartPos = PLAYER_POS;
}

//------------------------------------
// デストラクタ
//------------------------------------
CStage::~CStage()
{
}

//------------------------------------
// ステージ初期化
//------------------------------------
HRESULT CStage::Init(void)
{
	return S_OK;
}

//--------------------------------------------
// 終了
//--------------------------------------------
void CStage::Uninit(void)
{
	if (m_pGoal)
	{
		m_pGoal->Uninit();
		m_pGoal = nullptr;
	}

	if (m_pPlayer)
	{
		m_pPlayer = nullptr;
	}
}

//--------------------------------------------
// 更新
//--------------------------------------------
void CStage::Update(void)
{

}

//------------------------------------
// ステージの読み込み
//------------------------------------
void CStage::SetStage(const char *aStageFileName)
{
	// ファイル読み込み
	FILE *pFile;
	// NULLチェック
	if (pFile = fopen(aStageFileName, "r"))
	{
		// 読み込みに使う変数
		char aStr[6][255] = { NULL };		

		// 取得に使うローカル変数
		int nFunc;							// モデルに付ける機能
		int nTexType, nMeshType, nObjType;	// メッシュとテクスチャとオブジェクトのタイプ
		int nBlockX, nBlockY;				// 横分割数・縦分割数
		string sType;						// モデルの名前(ModelLoad000.txtで付けた名前)
		D3DXVECTOR3 pos, rot, scale, size;	// 位置・回転・スケール・サイズ
		bool bInSide = false;				// 内側に描画するかの判定

		CLoadX *pLoadX = CManager::GetInstance()->GetLoadX();

		// END_SCRIPTが呼ばれるまでループする
		// １単語を読み込む
		while(fscanf(pFile, "%s", &aStr[0]))
		{
			//--------------------------------------------------------------
			// メッシュ設置処理
			//--------------------------------------------------------------
			while (strcmp(aStr[0], "MESH_SET") == 0)
			{
				fscanf(pFile, "%s", &aStr[1]);				// 文字列読み込み
				if (strcmp(aStr[1], "MESH_TYPE") == 0)		// メッシュタイプ取得
				{
					fscanf(pFile, "%*s%d", &nMeshType);
				}
				if (strcmp(aStr[1], "TEX_TYPE") == 0)		// テクスチャタイプ取得
				{
					fscanf(pFile, "%*s%d", &nTexType);
				}
				if (strcmp(aStr[1], "POS") == 0)			// 位置取得
				{
					fscanf(pFile, "%*s%f %f %f", &pos.x, &pos.y, &pos.z);
				}
				if (strcmp(aStr[1], "ROT") == 0)			// 回転取得
				{
					fscanf(pFile, "%*s%f %f %f", &rot.x, &rot.y, &rot.z);
				}
				if (strcmp(aStr[1], "SIZE") == 0)			// サイズ取得
				{
					fscanf(pFile, "%*s%f %f %f", &size.x, &size.y, &size.z);
				}
				if (strcmp(aStr[1], "BLOCK") == 0)			// 分割数取得
				{
					fscanf(pFile, "%*s%d %d", &nBlockX, &nBlockY);
				}
				if (strcmp(aStr[1], "SIDE") == 0)			// 描画サイド取得
				{
					fscanf(pFile, "%*s%d", (int*)&bInSide);
				}
				if (strcmp(aStr[1], "END_MESH_SET") == 0)	// メッシュの生成
				{
					switch (nMeshType)
					{
					case CScene::MESH_FIELD:
						CMeshField::Create(pos, D3DXVECTOR3(size.x, 0.0f, size.y), rot, nBlockX, nBlockY);
						break;

					case CScene::MESH_WALL:
						CMeshField::Create(pos, D3DXVECTOR3(size.x, size.y, 0.0f), rot, nBlockX, nBlockY);
						break;

					case CScene::MESH_CYLINDER:
						CMeshCylinder::Create(pos, size, rot, nBlockX, nBlockY,CMeshCylinder::CYLINDER_TYPE::STAND);
						break;

					case CScene::MESH_SPHERE:
						CMeshSphere::Create(pos, size, rot, nBlockX, nBlockY,bInSide,CScene::OBJTYPE_MESH);
						break;

					default:
						break;
					}
					break;
				}
			}

			//--------------------------------------------------------------
			// オブジェクト設置処理
			//--------------------------------------------------------------
			while (strcmp(aStr[0], "MODEL_SET") == 0)
			{
				if (pLoadX->GetNumAll() > 0)	// サイズチェック
				{
					fscanf(pFile, "%s", &aStr[1]);					// 文字列読み込み
					if (strcmp(aStr[1], "FUNC") == 0)				// モデル機能取得
					{
						fscanf(pFile, "%*s%d", &nFunc);
					}
					if (strcmp(aStr[1], "TYPE") == 0)				// タイプ取得
					{
						fscanf(pFile, "%*s%s", sType.c_str());
					}
					if (strcmp(aStr[1], "POS") == 0)				// 位置取得
					{
						fscanf(pFile, "%*s%f %f %f", &pos.x, &pos.y, &pos.z);
					}
					if (strcmp(aStr[1], "ROT") == 0)				// 回転取得
					{
						fscanf(pFile, "%*s%f %f %f", &rot.x, &rot.y, &rot.z);
					}
					if (strcmp(aStr[1], "SCALE") == 0)				// スケール取得
					{
						fscanf(pFile, "%*s%f %f %f", &scale.x, &scale.y, &scale.z);
					}
					if (strcmp(aStr[1], "END_MODEL_SET") == 0)		// オブジェクトの生成
					{
						int nNumX = pLoadX->GetNum(sType.c_str());

						D3DXVECTOR3 rad;
						rad.x = D3DXToRadian(rot.x);
						rad.y = D3DXToRadian(rot.y);
						rad.z = D3DXToRadian(rot.z);

						switch (static_cast<LOADMODEL>(nFunc))
						{
						case LOADMODEL::NORMAL:			// 通常モデル
							CModel_Spawner::Create(	pos, rad, scale, nNumX);
							break;

						case LOADMODEL::GOAL:			// ゴールモデル
							if (!m_pGoal)
							{
								m_pGoal = CGoal::Create(pos, rad, scale, nNumX);
								m_GoalPos = pos;
							}
							break;

						case LOADMODEL::NEEDLE:			// 針(障害物)モデル
							CObstacle::Create(pos, rad, scale, nNumX,CObstacle::OBSTACLE::NEEDLE);
							break;

						case LOADMODEL::OBSCUBE:		// キューブ(障害物)モデル
							CObstacle::Create(pos, rad, scale, nNumX, CObstacle::OBSTACLE::CUBE);
							break;

						case LOADMODEL::GATE01:			// ゲート1モデル
							CGate::Create(pos, rad, scale, nNumX, CGate::GATE_TYPE::A);
							break;

						case LOADMODEL::GATE02:			// ゲート2モデル
							CGate::Create(pos, rad, scale, nNumX, CGate::GATE_TYPE::B);
							break;

						case LOADMODEL::GATE03:			// ゲート3モデル
							CGate::Create(pos, rad, scale, nNumX, CGate::GATE_TYPE::C);
							break;

						case LOADMODEL::GATE04:			// ゲート4モデル
							CGate::Create(pos, rad, scale, nNumX, CGate::GATE_TYPE::D);
							break;

						default :						// 通常モデル
							CModel_Spawner::Create(pos, rad, scale, nNumX);
							break;
						}
						break;
					}
				}

				else
				{
					printf("読み込み済みのXファイルが存在しません");
				}
			}
			
			//--------------------------------------------------------------
			// プレイヤー設置処理
			//--------------------------------------------------------------
			while (strcmp(aStr[0], "PLAYER_SET") == 0)
			{
				if (pLoadX->GetNumAll() > 0)	// サイズチェック
				{
					fscanf(pFile, "%s", &aStr[1]);							// 文字列読み込み
					if (strcmp(aStr[1], "END_PLAYER_SET") == 0)				// オブジェクトの生成
					{
						// プレイヤーの読み込み
						m_pPlayer = CPlayer::Create(m_StartPos, { 0.0f,0.0f,0.0f }, CPlayer::PLAYER_STATE::CUBE);
						break;
					}
				}

				else
				{
					printf("読み込み済みのXファイルが存在しません");
				}
			}

			//--------------------------------------------------------------
			// ジャンプサークル生成処理
			//--------------------------------------------------------------
			while (strcmp(aStr[0], "OBJECT_SET") == 0)
			{
				fscanf(pFile, "%s", &aStr[1]);			// 文字列読み込み
				if (strcmp(aStr[1], "TYPE") == 0)		// オブジェクトタイプ取得
				{
					fscanf(pFile, "%*s%d", &nObjType);
				}

				if (strcmp(aStr[1], "POS") == 0)		// 位置取得
				{
					fscanf(pFile, "%*s%f %f %f", &pos.x, &pos.y, &pos.z);
				}

				if (strcmp(aStr[1], "SIZE") == 0)		// サイズ取得
				{
					fscanf(pFile, "%*s%f %f %f", &size.x, &size.y, &size.z);
				}

				if (strcmp(aStr[1], "END_OBJECT_SET") == 0)
				{
					CJumpCircle::Create(pos, size, static_cast<CJumpCircle::CIRCLE_TYPE>(nObjType));	// ジャンプサークルの生成
					break;
				}
			}
			
			// 読み込みと配置終了
			if (strcmp(aStr[0], "END_SCRIPT") == 0)
			{
				break;
			}
		}
	}

	else
	{
		// エラー
		perror("ファイルが開けませんでした");
	}

	// ファイルを閉じる
	fclose(pFile);
}