//=============================================================================
//
// テキスト表示のクラス [TextureFont.h]
// Author : 村元翼
//
//=============================================================================
#ifndef _TEXTUREFONT_H_
#define _TEXTUREFONT_H_
#include "scene.h"
#include "scene2D.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define FONT_POS (D3DXVECTOR3(50.0f,600.0f,0.0f))		// フォントの位置

//=============================================================================
// クラス定義
//=============================================================================
class CFont : public CScene2D
{
public:
	CFont(OBJTYPE nPriority = OBJTYPE_NUMBER);					// コンストラクタ
	~CFont();													// デストラクタ

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);			// 初期化処理
	void Uninit(void);											// 終了処理
	void Update(void);											// 更新処理
	void Draw(void);											// 描画処理

	// オブジェクト生成
	static CFont *Create(D3DXVECTOR3 pos, int nfontSize,const char* str, const char* font = "ＭＳ 明朝");

private:
	int m_nFontSize;		// フォントサイズ
	const char* m_str;		// 文字
	const char* m_font;     // フォント名
	D3DXVECTOR3 m_pos;		// 位置
};
#endif
