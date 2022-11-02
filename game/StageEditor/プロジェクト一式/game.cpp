//=========================================================
// ステージ制作画面の処理
// Author:村元翼
//=========================================================
#include "game.h"
#include "fade.h"
#include "manager.h"
#include "meshfield.h"
#include "meshcylinder.h"
#include "meshsphere.h"
#include "timercount.h"
#include "Scene3D.h"
#include "keyboard.h"
#include "XInput.h"
#include "DirectInput.h"
#include "LoadX.h"
#include "Ui.h"
#include "mouse.h"
#include "StringManager.h"
#include "model_spawner.h"
#include "model.h"

//=========================================================
// マクロ定義
//=========================================================
#define COUNT_DOWN_FRAME	(60)										// カウントダウンのフレーム
#define SKY_SIZE			(D3DXVECTOR3(10000.0f,10000.0f,10000.0f))	// 空サイズ
#define UI_SIZE				(D3DXVECTOR3(1.0f,1.0f,0.0f))				// UIサイズ
#define NUMBER_SIZE			(D3DXVECTOR3(50.0f, 30.0f, 0.0f))			// スコアサイズ
#define DELETE_FRAME		(90)										// メッセージが消えるまでのフレーム
#define GAME_TIME			(60)										// ゲームタイム
#define FILENAME			("data/SAVE/SaveData000.txt")				// 読み込みテキスト
#define PREVIEW_COLOR		(D3DXCOLOR(1.0f,1.0f,1.0f,0.5f))			// プレビューカラー
#define BASE_FIELD_SIZE		(D3DXVECTOR3(50.0f,0.0f,50.0f))				// 初期フィールドサイズ
#define MAX_MULTI			(10)										// 最大倍率
#define MAX_DIGIT			(10000)										// 最大桁
#define DIGIT_SHIFT_UP		(10)										// 桁上がり
#define DIGIT_SHIFT_DOWN	(0.1f)										// 桁下がり

//=========================================================
// 静的メンバ変数の初期化
//=========================================================
int CGame::m_nMultiX = 1;
int CGame::m_nMultiZ = 1;
float CGame::m_fEvenX = 0.0f;
float CGame::m_fEvenZ = 0.0f;
bool CGame::m_bInputMode = false;

//=========================================================
// コンストラクタ
//=========================================================
CGame::CGame()
{
	CLoadX *pLoadX = CManager::GetInstance()->GetLoadX();	// モデルXファイルクラスの取得
	m_nSelectMax = pLoadX->GetNumAll();						// 最大選択数
	m_nSelect = 0;											// 選択番号
	m_nObjType = 0;											// オブジェクトタイプ番号
	m_nPosMode = INPUT_POSX;								// 入力する座標の選択番号
	m_Inputpos = { 0.0f,0.0f,0.0f };						// 入力する座標		
	m_nFrame = 0;											// フレーム
	m_pUI = nullptr;										// UIクラス
	m_pString = nullptr;									// 文字列クラス
	m_pField = nullptr;										// メッシュフィールドクラス
	m_pModelPrev = nullptr;									// モデルプレビュークラス
	m_pFieldPrev = nullptr;									// メッシュフィールドプレビュークラス
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
	// メッシュフィールド
	m_pField = CMeshField::Create({ 0.0f,-100.0f,0.0f }, { 10000.0f,0.0f,10000.0f }, { 0.0f,0.0f,0.0f }, 10, 10);
	m_pField->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_FLOOR"));

	// プレビュー生成
	ObjCreate(m_nObjType);

	return S_OK;
}

//=========================================================
// 終了
//=========================================================
void CGame::Uninit(void)
{
	if (!m_vModel.empty())
	{
		// 格納されているモデルクラスを全て破棄
		for (CModel_Spawner *pDeleteModel : m_vModel)
		{
			if (pDeleteModel)
			{
				pDeleteModel->Uninit();
				pDeleteModel = nullptr;
			}
		}

		// モデルクラスコンテナを破棄
		m_vModel.clear();
	}

	if (!m_vMeshField.empty())
	{
		// コンテナに格納されているメッシュフィールドクラスを全て破棄
		for (CMeshField *pDeleteField : m_vMeshField)
		{
			if (pDeleteField)
			{
				pDeleteField->Uninit();
				pDeleteField = nullptr;
			}
		}

		// メッシュフィールドクラスコンテナを破棄
		m_vMeshField.clear();
	}

}

//=========================================================
// 更新
//=========================================================
void CGame::Update(void)
{
	//-----------------------------------------------
	// クラス取得
	//-----------------------------------------------
	CInputkeyboard *pKey = CManager::GetInstance()->GetKeyboard();
	CMouse *pMouse = CManager::GetInstance()->GetMouse();
	CCamera *pCamera = CManager::GetInstance()->GetCamera(0);

	// 生成時の座標
	D3DXVECTOR3 CreatePos = { pCamera->GetPosR().x + m_fEvenX, pCamera->GetPosR().y,pCamera->GetPosR().z + m_fEvenZ };

	// 入力モード判定
	if (!m_bInputMode)
	{
		//-----------------------------------------------
		// プレビューオブジェクトの移動
		//-----------------------------------------------
		switch (m_nObjType)
		{
		case OBJ_MODEL:
			if (m_pModelPrev)
			{
				m_pModelPrev->SetPos(pCamera->GetPosR());
			}
			break;

		case OBJ_MESHFIELD:
			if (m_pFieldPrev)
			{
				m_pFieldPrev->CScene::SetPos(CreatePos);
			}
			break;
		}

		//-----------------------------------------------
		// オブジェクトの種類切り替え
		//-----------------------------------------------
		if (pKey->GetTrigger(DIK_1) || pKey->GetTrigger(DIK_2))
		{
			// 変数リセット
			m_fEvenX = 0.0f;
			m_fEvenZ = 0.0f;
			m_nMultiX = 1;
			m_nMultiZ = 1;

			ObjDelete();				// プレビュー破棄

			SelectLoop(m_nObjType, 0, OBJ_MAX - 1, DIK_1, DIK_2);

			ObjCreate(m_nObjType);		// プレビュー生成
		}

		//-----------------------------------------------
		// オブジェクトタイプ別の切り替え処理
		//-----------------------------------------------
		switch (m_nObjType)
		{
			//---------------------------------
			// モデル切り替え
			//---------------------------------
		case OBJ_MODEL:
			if (pKey->GetTrigger(DIK_C) || pKey->GetTrigger(DIK_Z))
			{
				ObjDelete();			// オブジェクト破棄

				SelectLoop(m_nSelect, 0, m_nSelectMax - 1, DIK_Z, DIK_C);

				ObjCreate(m_nObjType);	// オブジェクト生成
			}
			break;

			//---------------------------------
			// メッシュフィールドのサイズ変更
			//---------------------------------
		case OBJ_MESHFIELD:
			if (pKey->GetTrigger(DIK_O) || pKey->GetTrigger(DIK_L) || pKey->GetTrigger(DIK_I) || pKey->GetTrigger(DIK_K))
			{
				ObjDelete();			// オブジェクト破棄

				SelectLoop(m_nMultiX, 1, MAX_MULTI, DIK_I, DIK_K);
				SelectLoop(m_nMultiZ, 1, MAX_MULTI, DIK_O, DIK_L);

				ObjCreate(m_nObjType);	// オブジェクト生成

				// メッシュ数が２倍数ならマス目に座標を合わせる変数を作る
				if (m_pFieldPrev)
				{
					MatchSquare(m_fEvenX, m_nMultiX, BASE_FIELD_SIZE.x / 2);
					MatchSquare(m_fEvenZ, m_nMultiZ, BASE_FIELD_SIZE.z / 2);
				}
			}
			break;
		}

		//-----------------------------------------------
		// 左クリックorSPACEキーで生成
		//-----------------------------------------------
		if (pMouse->GetTrigger(CMouse::MOUSE_LEFT) || pKey->GetTrigger(DIK_SPACE))
		{
			// ローカルクラス
			CModel_Spawner	*pModel = nullptr;
			CMeshField		*pField = nullptr;

			// タイプ別クラスオブジェクト生成
			switch (m_nObjType)
			{
			case OBJ_MODEL:
				pModel = CModel_Spawner::Create(CreatePos, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, m_nSelect);
				m_vModel.emplace_back(pModel);
				break;

			case OBJ_MESHFIELD:
				pField = CMeshField::Create(CreatePos, { BASE_FIELD_SIZE.x * m_nMultiX,0.0f,BASE_FIELD_SIZE.z * m_nMultiZ }, { 0.0f,0.0f,0.0f }, m_nMultiX, m_nMultiZ);
				m_vMeshField.emplace_back(pField);
				break;
			}
		}

		//-----------------------------------------------
		// BackSpaceで最後に生成したモデルを削除
		//-----------------------------------------------
		if (pKey->GetTrigger(DIK_BACKSPACE))
		{
			switch (m_nObjType)
			{
			case OBJ_MODEL:
				if (!m_vModel.empty())
				{
					CModel_Spawner *pModelSpn = m_vModel.back();
					if (pModelSpn)
					{
						pModelSpn->Uninit();
						pModelSpn = nullptr;
					}

					// モデルコンテナの最後の要素を破棄
					m_vModel.pop_back();
				}
				break;

			case OBJ_MESHFIELD:
				if (!m_vMeshField.empty())
				{
					CMeshField *pMeshField = m_vMeshField.back();
					if (pMeshField)
					{
						pMeshField->Uninit();
						pMeshField = nullptr;
					}

					// メッシュフィールドコンテナの最後の要素を破棄
					m_vMeshField.pop_back();
				}
				break;
			}
		}

		//-----------------------------------------------
		// セーブ
		//-----------------------------------------------
		if (pKey->GetTrigger(DIK_F3))
		{
			if (Save(FILENAME))
			{
				// セーブ完了メッセージの生成
				SetMessage("セーブ完了");
			}
			else
			{
				exit(EXIT_FAILURE);	// 異常終了
			}
		}

		//-----------------------------------------------
		// ロード
		//-----------------------------------------------
		if (pKey->GetTrigger(DIK_F4))
		{
			if (Load(FILENAME))
			{
				// ロード完了メッセージの生成
				SetMessage("ロード完了");
			}
		}

		// メッセージ表示時の処理
		if (m_pString)
		{
			m_nFrame++;	// フレームカウント

			// メッセージ削除
			if (m_nFrame > DELETE_FRAME)
			{
				m_pString->Uninit();
				delete m_pString;
				m_pString = nullptr;

				m_nFrame = 0;
			}
		}
	}

	// 入力モードなら
	else
	{
		SelectLoop(m_nPosMode, 0, INPUT_MAX - 1, DIK_LEFT, DIK_RIGHT);

		// モード別に座標入力を行う
		switch (m_nPosMode)
		{
		case INPUT_POSX:
			InputPos(m_Inputpos.x);
			break;

		case INPUT_POSY:
			InputPos(m_Inputpos.y);
			break;

		case INPUT_POSZ:
			InputPos(m_Inputpos.z);
			break;
		}
	}

	if (pKey->GetTrigger(DIK_F1))
	{
		// メッセージを表示
		if (!m_pString)
		{
			m_pString = CStringManager::Create({ SCREEN_WIDTH / 3,SCREEN_HEIGHT / 2, 0.0f }, 64, 64, "座標を入力してください", "ＭＳゴシック");

			// 入力位置リセット
			m_Inputpos = { 0.0f,0.0f,0.0f };
		}
		else
		{
			m_pString->Uninit();
			delete m_pString;
			m_pString = nullptr;
		}

		// 座標入力モードを切り替える
		m_bInputMode = !m_bInputMode;
	}
}

//=========================================================
// 描画
//=========================================================
void CGame::Draw(void)
{

}

//=========================================================
// メッセージの設定(生成)
//=========================================================
void CGame::SetMessage(const char* sMessage)
{
	// メッセージが他にあるなら先に破棄
	if (m_pString)
	{
		m_pString->Uninit();
		delete m_pString;
		m_pString = nullptr;
	}

	if (!m_pString)
	{
		m_pString = CStringManager::Create({ SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2, 0.0f }, 64, 64, sMessage, "ＭＳゴシック");
	}
}

//=========================================================
// マス目に合わせる関数
//=========================================================
void CGame::MatchSquare(float &fEven,const int &nMulti, const float &fShift)
{
	// 掛ける変数が2倍数なら
	if (nMulti % 2 == 0)
	{
		fEven = fShift;
	}

	// そうでないなら
	else
	{
		fEven = 0.0f;
	}
}

//=========================================================
// 選択肢の切り替え(ループ)
//=========================================================
void CGame::SelectLoop(int &nNow, const int &nMin, const int &nMax, const int &nKey1, const int nKey2)
{
	// キーボードクラス取得
	CInputkeyboard *pKey = CManager::GetInstance()->GetKeyboard();

	// 戻る
	if (pKey->GetTrigger(nKey1))
	{
		nNow--;
		if (nNow < nMin)
		{
			nNow = nMax;
		}
	}

	// 進める
	else if (pKey->GetTrigger(nKey2))
	{
		nNow++;
		if (nNow > nMax)
		{
			nNow = nMin;
		}
	}
}

//=========================================================
// 座標指定
//=========================================================
void CGame::InputPos(float &fpos)
{
	// キーボードクラス取得
	CInputkeyboard *pKey = CManager::GetInstance()->GetKeyboard();

	int nNum = 0;
	if (pKey->GetTrigger(DIK_1) || pKey->GetTrigger(DIK_2) || pKey->GetTrigger(DIK_3) || pKey->GetTrigger(DIK_4) || pKey->GetTrigger(DIK_5)||
		pKey->GetTrigger(DIK_6) || pKey->GetTrigger(DIK_7) || pKey->GetTrigger(DIK_8) || pKey->GetTrigger(DIK_9) || pKey->GetTrigger(DIK_0))
	{
		if (pKey->GetTrigger(DIK_1))
		{
			nNum = 1;
		}
		else if (pKey->GetTrigger(DIK_2))
		{
			nNum = 2;
		}
		else if (pKey->GetTrigger(DIK_3))
		{
			nNum = 3;
		}
		else if (pKey->GetTrigger(DIK_4))
		{
			nNum = 4;
		}
		else if (pKey->GetTrigger(DIK_5))
		{
			nNum = 5;
		}
		else if (pKey->GetTrigger(DIK_6))
		{
			nNum = 6;
		}
		else if (pKey->GetTrigger(DIK_7))
		{
			nNum = 7;
		}
		else if (pKey->GetTrigger(DIK_8))
		{
			nNum = 8;
		}
		else if (pKey->GetTrigger(DIK_9))
		{
			nNum = 9;
		}
		else if (pKey->GetTrigger(DIK_0))
		{
			nNum = 0;
		}

		if (fpos < MAX_DIGIT)
		{
			fpos *= DIGIT_SHIFT_UP;	// 桁をずらす
			fpos += nNum;			// 最小桁に入力した数字を入れる
		}
	}

	// 最小桁を削る
	if (pKey->GetTrigger(DIK_BACKSPACE))
	{
		if (fpos > 0.0f)
		{
			fpos *= DIGIT_SHIFT_DOWN;
			fpos = floorf(fpos);	// 小数点以下切り捨て
		}
	}

	// 入力した座標にカメラの位置を合わせる
	if (pKey->GetTrigger(DIK_RETURN))
	{
		CManager::GetInstance()->GetCamera(0)->SetPosR(m_Inputpos);

		// メッセージ破棄
		if (m_pString)
		{
			m_pString->Uninit();
			delete m_pString;
			m_pString = nullptr;
		}

		// 入力モードを終了する
		m_bInputMode = false;
	}
}

//=========================================================
// UI生成関数
//=========================================================
void CGame::SetUiCreate(const char* TexStr, float fSizeMulti)
{
	// テクスチャクラス取得
	CTexture *pTex = CManager::GetInstance()->GetTexture();

	// UI削除
	SetUiDelete();

	// UI生成
	if (!m_pUI)
	{
		m_pUI = CUi::Create(SCREEN_CENTER, { UI_SIZE.x * fSizeMulti,UI_SIZE.y,0.0f }, CUi::UI_TYPE_ZOOM);
		m_pUI->BindTexture(pTex->GetTexture(TexStr));
	}
}

//=========================================================
// UI削除関数
//=========================================================
void CGame::SetUiDelete(void)
{
	if (m_pUI)
	{
		m_pUI->Uninit();
		m_pUI = nullptr;
	}
}

//=========================================================
// オブジェクト削除
//=========================================================
void CGame::ObjDelete(void)
{
	if (m_pFieldPrev)
	{
		m_pFieldPrev->Uninit();
		m_pFieldPrev = nullptr;
	}

	if (m_pModelPrev)
	{
		m_pModelPrev->Uninit();
		m_pModelPrev = nullptr;
	}
}

//=========================================================
// オブジェクト生成
//=========================================================
void CGame::ObjCreate(int nObjtype)
{
	// カメラ取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera(0);

	// タイプ別オブジェクト生成
	switch (nObjtype)
	{
	case OBJ_MODEL:
		if (!m_pModelPrev)
		{
			m_pModelPrev = CModel_Spawner::Create({ pCamera->GetPosR() }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, m_nSelect);
			m_pModelPrev->SetCol(PREVIEW_COLOR);
		}
		break;

	case OBJ_MESHFIELD:
		if (!m_pFieldPrev)
		{
			m_pFieldPrev = CMeshField::Create({ pCamera->GetPosR() }, { BASE_FIELD_SIZE.x * m_nMultiX,0.0f,BASE_FIELD_SIZE.z * m_nMultiZ }, { 0.0f,0.0f,0.0f }, m_nMultiX, m_nMultiZ);
			m_pFieldPrev->SetCol(PREVIEW_COLOR);
		}
		break;
	}
}


//=========================================================
// セーブ処理
//=========================================================
bool CGame::Save(const char* sSaveName)
{
	// ファイル構造体
	FILE *pFile = nullptr;

	// 書き込みモードでファイルを開く
	if (pFile = fopen(sSaveName, "w"))
	{
		fprintf(pFile, "//=============================================//\n");
		fprintf(pFile, "// ステージデータ[SaveData000.txt]\n");
		fprintf(pFile, "// Author : 村元翼\n");
		fprintf(pFile, "//=============================================//\n\n");

		// モデルの配置
		if (!m_vModel.empty())
		{
			fprintf(pFile, "//======================================//\n");
			fprintf(pFile, "//			 モデル配置情報				//\n");
			fprintf(pFile, "//======================================//\n");

			// モデルの数だけループする
			for (CModel_Spawner *pModel : m_vModel)
			{
				fprintf(pFile, "MODELSET\n");
				fprintf(pFile, "	TYPE = %d\n", pModel->GetModel()->GetXType());
				fprintf(pFile, "	POS = %.2f %.2f %.2f\n", pModel->GetPos().x, pModel->GetPos().y, pModel->GetPos().z);
				fprintf(pFile, "	ROT = %.2f %.2f %.2f\n", pModel->GetRot().x, pModel->GetRot().y, pModel->GetRot().z);
				fprintf(pFile, "END_MODELSET\n\n");
			}
		}

		// メッシュフィールドの配置
		if (!m_vMeshField.empty())
		{
			fprintf(pFile, "//======================================//\n");
			fprintf(pFile, "//			 モデル配置情報				//\n");
			fprintf(pFile, "//======================================//\n");

			// メッシュフィールドの数だけループする
			for (CMeshField *pMeshField : m_vMeshField)
			{
				fprintf(pFile, "FIELDSET\n");
				fprintf(pFile, "	POS = %.2f %.2f %.2f\n", pMeshField->GetPos().x, pMeshField->GetPos().y, pMeshField->GetPos().z);
				fprintf(pFile, "	ROT = %.2f %.2f %.2f\n", pMeshField->GetRot().x, pMeshField->GetRot().y, pMeshField->GetRot().z);
				fprintf(pFile, "	SIZE = %.2f %.2f %.2f\n", pMeshField->GetSize().x, pMeshField->GetSize().y, pMeshField->GetSize().z);
				fprintf(pFile, "	BLOCK = %d %d\n", pMeshField->GetLine(), pMeshField->GetVertical());
				fprintf(pFile, "END_FIELDSET\n\n");
			}
		}

		fprintf(pFile, "END_SCRIPT");
	}

	// セーブの失敗
	else
	{
		return false;
	}

	// ファイルを閉じる
	fclose(pFile);

	return true;
}

//=========================================================
// ロード処理
//=========================================================
bool CGame::Load(const char* sLoadName)
{
	// ファイル構造体
	FILE *pFile;

	// 読み込みモードでファイルを開く
	if (pFile = fopen(sLoadName, "r"))
	{
		// コンテナクラスを破棄
		if (!m_vModel.empty() || !m_vMeshField.empty())
		{
			// 終了
			Uninit();
		}

		// NULLチェック
		if (m_vModel.empty() || m_vMeshField.empty())
		{
			// 読み込みに使う変数
			char aStr[2][64] = { NULL,NULL };

			D3DXVECTOR3 pos = { 0.0f,0.0f,0.0f };	// 位置
			D3DXVECTOR3 rot = { 0.0f,0.0f,0.0f };	// 回転
			D3DXVECTOR3 size = { 0.0f,0.0f,0.0f };	// サイズ
			int nType = 0;							// タイプ
			int nLine = 0;							// 横分割数
			int nVertical = 0;						// 縦分割数

			while (fscanf(pFile, "%s", &aStr[0]))
			{
				// モデルの読み込み
				while (strcmp(aStr[0], "MODELSET") == 0)
				{
					fscanf(pFile, "%s", &aStr[1]);					// 文字列読み込み

					if (strcmp(aStr[1], "TYPE") == 0)				// タイプ取得
					{
						fscanf(pFile, "%*s%d", &nType);
					}
					if (strcmp(aStr[1], "POS") == 0)				// 位置取得
					{
						fscanf(pFile, "%*s%f %f %f", &pos.x, &pos.y, &pos.z);
					}
					if (strcmp(aStr[1], "ROT") == 0)				// 回転取得
					{
						fscanf(pFile, "%*s%f %f %f", &rot.x, &rot.y, &rot.z);
					}
					if (strcmp(aStr[1], "END_MODELSET") == 0)		// オブジェクトの生成
					{
						CModel_Spawner *pModel = nullptr;
						pModel = CModel_Spawner::Create(pos, rot, { 1.0f,1.0f,1.0f }, nType);
						m_vModel.emplace_back(pModel);
						break;
					}
				}


				// フィールドの読み込み
				while (strcmp(aStr[0], "FIELDSET") == 0)
				{
					fscanf(pFile, "%s", &aStr[1]);					// 文字列読み込み

					if (strcmp(aStr[1], "POS") == 0)				// 位置取得
					{
						fscanf(pFile, "%*s%f %f %f", &pos.x, &pos.y, &pos.z);
					}
					if (strcmp(aStr[1], "ROT") == 0)				// 回転取得
					{
						fscanf(pFile, "%*s%f %f %f", &rot.x, &rot.y, &rot.z);
					}
					if (strcmp(aStr[1], "SIZE") == 0)				// サイズ取得
					{
						fscanf(pFile, "%*s%f %f %f", &size.x, &size.y, &size.z);
					}
					if (strcmp(aStr[1], "BLOCK") == 0)				// メッシュ数取得
					{
						fscanf(pFile, "%*s%d %d", &nLine, &nVertical);
					}
					if (strcmp(aStr[1], "END_FIELDSET") == 0)		// オブジェクトの生成
					{
						CMeshField *pField = nullptr;
						pField = CMeshField::Create(pos, size, rot, nLine, nVertical);
						m_vMeshField.emplace_back(pField);
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

	}

	// ロード失敗
	else
	{
		printf("ファイルが開けません");
		return false;
	}

	return true;
}