//=====================================
// チュートリアル
// Author:村元翼
//=====================================
#include "main.h"
#include "player.h"

//=====================================
// マクロ定義
//=====================================
#define TYPE_MAX		(4)
#define EXP_FONTSIZE	(64)

//=====================================
// 前方宣言
//=====================================
class CStage;
class CStringManager;

//=====================================
// クラス定義
//=====================================
class CTutorial
{
public:
	// キャラクター説明構造体
	typedef enum
	{
		EXP_01 = 0,
		EXP_02,
		EXP_03,
		EXP_04,
		EXP_MAX
	}CHARA_EXP;


	CTutorial();					// コンストラクタ
	~CTutorial();					// デストラクタ
	HRESULT Init(void);				// 初期化
	void Uninit(void);				// 終了
	void Update(void);				// 更新
	void Draw(void);				// 描画

	void ExpManager(void);
	void ExpChange(int nExp);
	void KeyExpCreate(CStringManager **ppKeyExp, const char *cStr, const int& nFontSize = EXP_FONTSIZE);	// キー説明文字列生成関数

private:
	CStringManager	*m_pExpStr;				// 文字列管理クラス
	CPlayer			*m_pPlayer[TYPE_MAX];	// プレイヤークラス
	CStage			*m_pStage;				// ステージクラス
	CHARA_EXP		m_Exp;					// どのプレイヤーの説明をしているかの番号 
};