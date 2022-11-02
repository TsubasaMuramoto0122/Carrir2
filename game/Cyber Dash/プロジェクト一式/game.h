//============================================================
// ゲームシーンの処理
// Author:村元翼
//============================================================
#ifndef _GAME_H_
#define _GAME_H_

//============================================================
// インクルードファイル
//============================================================
#include "main.h"
#include "scene.h"
#include "manager.h"

//============================================================
// 前方宣言
//============================================================
class CBg;
class CPlayer;
class CTime;
class CScore;
class CPolygon;
class CTarget;
class CGauge;
class CMeshField;
class CMeshCylinder;
class CMeshSphere;
class CStage;
class CUi;
class CRemainCount;

//============================================================
// クラスの生成
//============================================================
class CGame
{
public:

	CGame();				// コンストラクタ
	~CGame();				// デストラクタ
	HRESULT Init(void);		// 初期化
	void Uninit(void);		// 終了
	void Update(void);		// 更新
	void Draw(void);		// 描画

	// 設定・取得関数
	static CRemainCount *GetRemainCount()	{ return m_pRemainCount; }	// 死亡回数取得
	static CScore *GetScore()				{ return m_pScore; }		// スコア取得
	void SetUiDelete(void);												// UIのみ削除するときの関数
	static CStage *GetStage()				{ return m_pStage; }		// ステージ取得	

private:
	//メンバ変数
	static CRemainCount	*m_pRemainCount;	// 死亡カウントクラス
	CUi					*m_pUI;				// UIクラス
	CMeshField			*m_pMeshField;		// メッシュフィールドクラス
	CMeshField			*m_pMeshWall;		// メッシュウォールクラス
	CMeshCylinder		*m_pMeshCylinder;	// メッシュシリンダークラス
	CMeshSphere			*m_pMeshSphere;		// メッシュスフィアクラス
	static CStage		*m_pStage;			// ステージクラス
	static CScore		*m_pScore;			// スコアクラス
	D3DXVECTOR3			m_CameraRot;		// カメラ回転
	bool				m_bOnce;			// 一度のみ通る処理
	static bool			m_bRetry;			// リトライ判定

	int m_nFrame;							// フレーム
	bool m_bStart;							// スタート判定
	bool m_bEnd;							// 終了判定
};
#endif