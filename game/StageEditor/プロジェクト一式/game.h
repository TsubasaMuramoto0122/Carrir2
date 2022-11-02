//============================================================
// ステージ制作画面の処理
// Author:村元翼
//============================================================
#ifndef _GAME_H_
#define _GAME_H_
#include "main.h"

//============================================================
// 前方宣言
//============================================================
class CTime;
class CScore;
class CMeshField;
class CMeshCylinder;
class CMeshSphere;
class CStage;
class CUi;
class CTimerCount;
class CModel_Spawner;
class CStringManager;

//============================================================
// クラスの生成
//============================================================
class CGame
{
public:
	//-----------------------------
	// オブジェクトタイプ構造体
	//-----------------------------
	typedef enum
	{
		OBJ_MODEL = 0,
		OBJ_MESHFIELD,
		OBJ_MAX
	}OBJ;

	//-----------------------------
	// 入力座標のモード切替構造体
	//-----------------------------
	typedef enum
	{
		INPUT_POSX = 0,
		INPUT_POSY,
		INPUT_POSZ,
		INPUT_MAX
	}INPUT_POS;

	CGame();				// コンストラクタ
	~CGame();				// デストラクタ
	HRESULT Init(void);		// 初期化
	void Uninit(void);		// 終了
	void Update(void);		// 更新
	void Draw(void);		// 描画

	void SetUiCreate(const char* TexStr = "",float fSizeMulti = 1);		// UIの生成
	void SetUiDelete(void);												// UIの削除

	void SetMessage(const char* sMessage);																// メッセージの設定(生成)
	void MatchSquare(float &fEven,const int &nMulti, const float &fShift);								// マス目に合わせる関数
	void SelectLoop(int &nNow,const int &nMin, const int &nMax ,const int &nKey1,const int nKey2);		// 選択肢の切り替え(ループ)
	void InputPos(float &fpos);																			// 位置の入力
	void ObjDelete(void);																				// オブジェクト削除
	void ObjCreate(int nObjtype);																		// オブジェクト生成
	bool Save(const char* sSaveName);																	// セーブ関数
	bool Load(const char* sLoadName);																	// ロード関数


	//--------------------------
	// 設定・取得関数
	//--------------------------
	CModel_Spawner	*GetModelPrev()	{ return m_pModelPrev; }				// モデルプレビューの取得
	CMeshField		*GetFieldPrev()	{ return m_pFieldPrev; }				// フィールドプレビューの取得

	D3DXVECTOR3	GetInputPos()		{ return m_Inputpos; }					// 入力用座標の取得
	bool		GetInputMode()		{ return m_bInputMode; }				// 入力モードの取得
	int			GetObjtype()		{ return m_nObjType; }					// オブジェクトタイプの取得
	int			GetSelect()			{ return m_nSelect; }					// 選択肢の取得
	int			GetMaxSelect()		{ return m_nSelectMax; }				// 選択最大値の取得
	int			GetNumModel()		{ return m_vModel.size(); }				// モデル生成数の取得
	int			GetNumField()		{ return m_vMeshField.size(); }			// フィールド生成数の取得	

private:
	CUi							*m_pUI;				// UIクラス
	CMeshField					*m_pField;			// メッシュフィールドクラス
	int							m_nFrame;			// フレームカウント変数
	int							m_nObjType;			// オブジェクトの種類
	int							m_nSelectMax;		// モデル最大数
	int							m_nSelect;			// 現在選択されているモデルの番号
	int							m_nPosMode;			// 座標モード
	static int					m_nMultiX;			// フィールドサイズに掛ける倍率X
	static int					m_nMultiZ;			// フィールドサイズに掛ける倍率Z
	static float				m_fEvenX;			// メッシュ数が偶数の場合に使う変数X
	static float				m_fEvenZ;			// メッシュ数が偶数の場合に使う変数Z
	static bool					m_bInputMode;		// 入力モード判定

	D3DXVECTOR3					m_Inputpos;			// 入力位置
	CStringManager				*m_pString;			// メッセージ
	CModel_Spawner				*m_pModelPrev;		// モデルプレビュー
	CMeshField					*m_pFieldPrev;		// メッシュフィールドプレビュー
	vector<CModel_Spawner*>		m_vModel;			// モデルvectorクラス
	vector<CMeshField*>			m_vMeshField;		// メッシュフィールドvectorクラス
};
#endif