//===============================================
// フェード用ポリゴン
// Author:村元翼
//===============================================
#include "main.h"
#include "scene2D.h"

//===============================================
// マクロ定義
//===============================================
#define INIT_FADE_SIZE (1.5f)	// 何もサイズを指定しなかったときの初期フェードサイズ

//===============================================
// クラスの定義
//===============================================
class CFadePolygon : public CScene2D
{
public:

	CFadePolygon(float fFadeSize,OBJTYPE nPriority = OBJTYPE_POLYGON2D);	// コンストラクタ
	~CFadePolygon();														// デストラクタ

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);						// 初期化
	void Uninit(void);														// 終了
	void Update(void);														// 更新
	void Draw(void);														// 描画

	// オブジェクト生成関数
	static CFadePolygon *Create(D3DXVECTOR3 pos, D3DXVECTOR3 size,float fInitFadeSize = INIT_FADE_SIZE);

	// フェード関数
	void Fade(float& fAlpha);

private:
	const static float	m_fFadeInSpeed;		// フェードイン速度
	const static float	m_fFadeOutSpeed;	// フェードアウト速度
	const float			m_fInitFadeSize;	// 初期のフェードサイズ

	CScene2D		*m_pScene2D;			// 2Dポリゴンクラス
	D3DXVECTOR3		m_pos;					// 位置
	D3DXVECTOR3		m_size;					// 大きさ
	float			m_fAlpha;				// 透明度(α値)
	float			m_fFadeSize;			// フェードサイズ
	bool			m_bFadeIn;				// フェードインを行うかの判定

};