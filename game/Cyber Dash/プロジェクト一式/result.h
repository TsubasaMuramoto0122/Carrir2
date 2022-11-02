//--------------------------------------
// リザルトの処理
// Author:村元翼
//--------------------------------------
#ifndef _RESULT_H_
#define _RESULT_H_
#include "main.h"
#include "scene.h"
#include "Ui.h"

//--------------------------------------
// マクロ定義
//--------------------------------------
#define MAX_RESULT					(4)
#define RESULT_RANKING_POSY			(80.0f)				//ランキングのposY

//--------------------------------------
// 前方宣言
//--------------------------------------
class CBg;
class CRanking;

//--------------------------------------
// リザルトクラスの生成
//--------------------------------------
class CResult
{
public:

	//------------------------------------
	// ゲームモード選択の識別番号
	//------------------------------------
	typedef enum
	{
		RESULT_SELECT_01 = 0,
		RESULT_SELECT_02,
		RESULT_SELECT_03,
		RESULT_SELECT_MAX

	}RESULT_SELECT;

	//--------------------------------------
	// 選択肢構造体
	//--------------------------------------
	typedef enum
	{
		GAME_RETRY = 0,
		TITLE,
		MAX,
	}TYPE;
										
	CResult();							// コンストラクタ
	~CResult();							// デストラクタ
	HRESULT Init(void);					// 初期化
	void Uninit(void);					// 終了
	void Update(void);					// 更新
	void Draw(void);					// 描画

	// カーソル移動
	void CursorMove(int &nNowSel, const int &nFirst, const int &nLast, const bool &bMinus);

private:
	//--------------------------------------
	// ゲーム選択UIパラメータ
	//--------------------------------------
	typedef struct
	{
		D3DXVECTOR3		pos, size;	// 位置・サイズ
		string			Tex;		// テクスチャ名
		CUi::UI_TYPE	UiType;		// UIのタイプ

	}SELECT_PARAM;

	const static SELECT_PARAM	m_SelectParam[RESULT_SELECT_MAX];	// ゲーム選択UIパラメータ
	CRanking					*m_pRanking;						// ランキングクラス
	CUi							*m_pUi;								// UI
	CUi							*m_pReturnTitle;					// タイトルに戻るUI
	CUi							*m_pSelectUI[RESULT_SELECT_MAX];	// 選択肢UIクラス
	CBg							*m_pBgResult;						// リザルト背景
	int							m_nSelectType;						// 現在の選択
	int							m_nFrame;							// フレーム数
	int							m_nSelect;							// モード選択に使う
	bool						m_bOPRLock;							// 操作ロック判定
	bool						m_bUnlock;							// アンロック判定

};
#endif