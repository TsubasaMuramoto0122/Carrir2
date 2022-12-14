//================================================
// ランキング処理
// Author:村元翼
//================================================
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "renderer.h"
#include "score.h"
#include "manager.h"
#include "ranking.h"
#include "scene.h"
#include "playdata.h"
#include "number.h"
#include "result.h"
#include "texture.h"

//================================================
// 静的メンバ変数宣言
//================================================
CScore *CRanking::m_apScore[MAX_RANKING] = {};

//================================================
// デフォルトコンストラクタ
//================================================
CRanking::CRanking()
{
	// スコア初期化
	for (int nCnt = 0; nCnt < MAX_SCORE_DATA; nCnt++)
	{
		m_nScore[nCnt] = 0;
	}
	m_nNowScore = 0;		// 現在のスコア

	// カラー
	m_col.r = 0.0f;			
	m_col.g = 0.0f;
	m_col.b = 0.0f;
	m_col.a = 0.0f;
}

//================================================
// デストラクタ
//================================================
CRanking::~CRanking()
{

}

//================================================
// 初期化処理
//================================================
HRESULT CRanking::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	for (int nCntRanking = 0; nCntRanking < MAX_RANKING; nCntRanking++)
	{
		// スコアの生成
		m_apScore[nCntRanking] = CScore::Create
		(
			D3DXVECTOR3
			(
				pos.x,
				pos.y + (float)(SCREEN_HEIGHT / (MAX_RANKING + 1)) * nCntRanking + (float)(SCREEN_HEIGHT / (MAX_RANKING + 1)),
				0.0f
			),
			size
		);

		// ランク生成
		m_apScene2D[nCntRanking] = CScene2D::Create
		(
			D3DXVECTOR3
			(
				RANKING_RANK_POLYGON_POS_X,
				pos.y + (float)(SCREEN_HEIGHT / (MAX_RANKING + 1)) * nCntRanking + (float)(SCREEN_HEIGHT / (MAX_RANKING + 1)),
				0.0f
			),
			RANKING_RANK_POLYGON_SIZE
		);

		// 順位テクスチャ
		switch (nCntRanking)
		{
		case 0:
			m_apScene2D[nCntRanking]->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_1ST"));
			break;
		case 1:
			m_apScene2D[nCntRanking]->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_2ND"));
			break;
		case 2:
			m_apScene2D[nCntRanking]->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_3RD"));
			break;
		case 3:
			m_apScene2D[nCntRanking]->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_4TH"));
			break;
		case 4:
			m_apScene2D[nCntRanking]->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_5TH"));
			break;
		}
	}

	// ロード処理
	FILE *pFile;
	if (pFile = fopen(RANKING_TEXT, "r"))
	{
		for (int nCntRanking = 0; nCntRanking < MAX_RANKING; nCntRanking++)
		{
			fscanf(pFile, "%d", &m_nScore[nCntRanking]);
		}
	}
	else
	{
		printf("ファイルが開けませんでした\n");
	}
	fclose(pFile);

	// プレイデータの取得
	CPlayData *pPlayData = nullptr;
	pPlayData = CManager::GetPlayData();

	// プレイデータに保存してある今回のスコアを変数に保存
	m_nScore[MAX_SCORE_DATA - 1] = pPlayData->GetScore();
	m_nNowScore = pPlayData->GetScore();

	// 降順ソート
	AscendingSort();

	//スコアの設定
	for (int nCntRanking = 0; nCntRanking < MAX_RANKING; nCntRanking++)
	{
		m_apScore[nCntRanking]->SetScore(m_nScore[nCntRanking]);
	}

	// セーブ処理
	if (pFile = fopen(RANKING_TEXT, "w"))
	{
		for (int nCntRanking = 0; nCntRanking < MAX_RANKING; nCntRanking++)
		{
			fprintf(pFile, "%d\n", m_apScore[nCntRanking]->GetScore());
		}
	}
	else
	{
		printf("ファイルが開けませんでした\n");
	}
	fclose(pFile);

	// カラーの設定
	m_col.r = 255.0f;
	m_col.g = 0.0f;
	m_col.b = 0.0f;
	m_col.a = 255.0f;

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CRanking::Uninit(void)
{
	// スコアと順位UIのポリゴン破棄
	for (int nCntRanking = 0; nCntRanking < MAX_RANKING; nCntRanking++)
	{
		if (m_apScene2D[nCntRanking])
		{
			m_apScene2D[nCntRanking]->Uninit();
			m_apScene2D[nCntRanking] = nullptr;
		}

		if (m_apScore[nCntRanking])
		{
			m_apScore[nCntRanking]->Uninit();
			m_apScore[nCntRanking] = nullptr;
		}
	}

	// このオブジェクトを破棄
	Release();
}

//================================================
//更新処理
//================================================
void CRanking::Update(void)
{
	for (int nCntRanking = 0; nCntRanking < MAX_RANKING; nCntRanking++)
	{
		//スコアの位置取得処理
		D3DXVECTOR3 posScore = m_apScore[nCntRanking]->GetPos();

		if (posScore.x > SCREEN_WIDTH / 2.0f)
		{
			if (nCntRanking > 0)
			{
				//スコアの位置取得処理
				D3DXVECTOR3 posScorePrev = m_apScore[nCntRanking - 1]->GetPos();
				if (posScorePrev.x <= SCREEN_WIDTH / 2.0f)
				{
					posScore.x += RANKING_MOVE_X;
				}
			}
			else
			{
				posScore.x += RANKING_MOVE_X;
			}
			if (posScore.x <= SCREEN_WIDTH / 2.0f)
			{
				posScore.x = SCREEN_WIDTH / 2.0f;
			}
			// 位置設定
			m_apScore[nCntRanking]->SetScorePos(posScore);
		}
	}

	for (int nCntRanking = 0; nCntRanking < MAX_RANKING; nCntRanking++)
	{
		if (m_nNowScore != 0 && m_nScore[nCntRanking] == m_nNowScore)
		{
			//カラーを薄くする
			m_col.a -= 255;
			if (m_col.r == 255)
			{
				m_col.r = 0;
				m_col.g = 255;
				m_col.b = 0;
			}
			else if (m_col.g == 255)
			{
				m_col.r = 0;
				m_col.g = 0;
				m_col.b = 255;
			}
			else if (m_col.b == 255)
			{
				m_col.r = 255;
				m_col.g = 0;
				m_col.b = 0;
			}

			if (m_col.a < 0)
			{
				m_col.a = 255;
			}

			for (int nCntNumber = 0; nCntNumber < MAX_SCORE; nCntNumber++)
			{
				// ナンバーを取得してカラーを設定
				m_apScore[nCntRanking]->GetNumber(nCntNumber)->SetCol(m_col);
			}
			break;
		}
	}
}

//================================================
//描画処理
//================================================
void CRanking::Draw(void)
{

}

//================================================
// オブジェクト生成処理
//================================================
CRanking* CRanking::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	// オブジェクト生成
	CRanking *pRanking = nullptr;
	if (pRanking = new CRanking)
	{
		pRanking->Init(pos, size);
	}

	return pRanking;
}

//================================================
// ランキング削除
//================================================
void CRanking::DeleteRanking(void)
{
	//セーブ処理
	FILE *pFile;
	if (pFile = fopen(RANKING_TEXT, "w"))
	{
		for (int nCntRanking = 0; nCntRanking < MAX_RANKING; nCntRanking++)
		{
			m_apScore[nCntRanking]->SetScore(0);
			fprintf(pFile, "%d\n", m_apScore[nCntRanking]->GetScore());
		}
	}
	else
	{
		printf("ファイルが開けませんでした\n");
	}
	fclose(pFile);
}

//================================================
// 昇順ソート
//================================================
void CRanking::AscendingSort(void)
{
	//ソート
	for (int nCntRanking = 0; nCntRanking < MAX_SCORE_DATA; nCntRanking++)
	{
		for (int nCntRanking2 = 0; nCntRanking2 < MAX_SCORE_DATA - 2; nCntRanking2++)
		{
			// 昇順で並べる
			if ((m_nScore[nCntRanking2] > m_nScore[nCntRanking2 + 1] && m_nScore[nCntRanking2] != 0))
			{
				int nScore = m_nScore[nCntRanking2];
				m_nScore[nCntRanking2] = m_nScore[nCntRanking2 + 1];
				m_nScore[nCntRanking2 + 1] = nScore;
			}
		}
	}
}


//================================================
// 降順ソート
//================================================
void CRanking::DescendingSort(void)
{
	//ソート
	for (int nCntRanking = 0; nCntRanking < MAX_SCORE_DATA; nCntRanking++)
	{
		for (int nCntRanking2 = 0; nCntRanking2 < MAX_SCORE_DATA - 1; nCntRanking2++)
		{
			// 降順で並べる
			if ((m_nScore[nCntRanking2] < m_nScore[nCntRanking2 + 1] && m_nScore[nCntRanking2 + 1] != 0) || m_nScore[nCntRanking2] == 0)
			{
				int nScore = m_nScore[nCntRanking2];
				m_nScore[nCntRanking2] = m_nScore[nCntRanking2 + 1];
				m_nScore[nCntRanking2 + 1] = nScore;
			}
		}
	}
}