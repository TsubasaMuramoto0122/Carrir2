//=====================================================
// フェードの処理
// Author;村元翼
//=====================================================
#ifndef _FADE_H_
#define _FADE_H_

//=====================================================
// インクルードファイル
//=====================================================
#include "texture.h"
#include "manager.h"
#include "scene.h"

//=====================================================
// マクロ定義
//=====================================================
#define FADE_IN_SPEED (0.05f)	// フェードイン速度
#define FADE_OUT_SPEED (0.01f)	// フェードアウト速度

//=====================================================
// 前方宣言
//=====================================================
class CBg;

//=====================================================
// フェードクラス定義
//=====================================================
class CFade : public CScene
{
public:
	//----------------------------------
	// フェードの状態列挙型構造体
	//----------------------------------
	typedef enum
	{
		FADE_NONE = 0,
		FADE_IN,
		FADE_OUT,
		FADE_MAX,
	} FADE;

	CFade(OBJTYPE nPriority = OBJTYPE_FADE);			// コンストラクタ
	~CFade();											// デストラクタ
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);	// 初期化
	void Uninit(void);									// 終了
	void Update(void);									// 更新
	void Draw(void);									// 描画

	// 設定・取得関数
	static void SetFade(CManager::MODE modeNext);						
	FADE GetFade(void);			

	// オブジェクト生成処理
	static CFade *Create(CManager::MODE modeNext,
						 D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
						 D3DXVECTOR3 size = D3DXVECTOR3(0.0f, 0.0f, 0.0f));						

private:
	static CBg					*m_pBg;			// 背景オブジェクト
	static FADE					m_fade;			// フェードの状態
	static CManager::MODE		m_modeNext;		// 次の画面（モード）
	static float				m_colorAlpha;	// フェードの色

};
#endif

