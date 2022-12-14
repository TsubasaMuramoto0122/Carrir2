//=================================================
// renderer.hの作成
// Author:村元翼
//=================================================
#ifndef _RENDERER_H_
#define _RENDERER_H_

//=================================================
// インクルードファイル
//=================================================
#include "main.h"

//=================================================
// レンダラークラスの生成
//=================================================
class CRenderer
{
public:
	CRenderer();												// コンストラクタ
	~CRenderer();												// デストラクタ

	HRESULT Init(HWND hWnd, bool bWindow);						// 全体の初期化
	void Uninit(void);											// 全体の終了
	void Update(void);											// 全体の更新
	void Draw(void);											// 全体の描画

	void SetAddSynthesis(bool bUse, LPDIRECT3DDEVICE9 device);	// 加算合成
	void SetSubSynthesis(bool bUse, LPDIRECT3DDEVICE9 device);	// 減算合成
	void SetAlphaTest(bool bUse, LPDIRECT3DDEVICE9 device);		// アルファテスト
	void SetZTest(bool bUse, LPDIRECT3DDEVICE9 device);			// Zテスト

	LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice; }	// デバイス取得

private:
	void DrawFPS(void);											// FPSの描画
	void DrawTXT(void);											// デバッグ文字の描画
	LPD3DXFONT	m_pFont = nullptr;								// フォントへのポインタ

	LPDIRECT3D9				m_pD3D = nullptr;					// Direct3Dオブジェクト
	LPDIRECT3DDEVICE9		m_pD3DDevice = nullptr;				// Deviceオブジェクト(描画に必要)
};

#endif