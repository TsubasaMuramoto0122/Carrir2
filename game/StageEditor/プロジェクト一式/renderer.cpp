//=============================================================================
// renderer.cppの作成
// Author:村元翼
//=============================================================================
#include "renderer.h"
#include "scene.h"
#include "manager.h"
#include "camera.h"
#include "DirectInput.h"
#include "game.h"
#include "meshfield.h"
#include "model_spawner.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CRenderer::CRenderer()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CRenderer::~CRenderer()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CRenderer::Init(HWND hWnd, bool bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;

	// Direct3Dオブジェクトの作成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!m_pD3D)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferCount = 1;									// バックバッファの数
	d3dpp.BackBufferWidth = SCREEN_WIDTH;						// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;						// カラーモードの指定
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// 映像信号に同期してフリップする
	d3dpp.EnableAutoDepthStencil = TRUE;						// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// デプスバッファとして16bitを使う
	d3dpp.Windowed = bWindow;									// ウィンドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// インターバル

	// デバイスの生成
	// ディスプレイアダプタを表すためのデバイスを作成
	// 描画と頂点処理をハードウェアで行なう
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &m_pD3DDevice)))
	{
		// 上記の設定が失敗したら
		// 描画をハードウェアで行い、頂点処理はCPUで行なう
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &m_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// 描画と頂点処理をCPUで行なう
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp, &m_pD3DDevice)))
			{
				// 生成失敗
				return E_FAIL;
			}
		}
	}

	// レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// カリングを行わない
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファを使用
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う(透明度を可能にするかの設定)
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャ縮小フィルタモードを設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャ拡大フィルタモードを設定

	// テクスチャステージステートの設定
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// デバッグ情報表示用フォントの生成
	D3DXCreateFont(m_pD3DDevice, 36, 14, 0, 0, FALSE, SHIFTJIS_CHARSET,
	OUT_STRING_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳゴシック", &m_pFont);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CRenderer::Uninit(void)
{
	// デバッグ情報表示用フォントの破棄
	if (m_pFont)
	{
		m_pFont->Release();
		m_pFont = nullptr;
	}

	// デバイスの破棄
	if (m_pD3DDevice)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = nullptr;
	}

	// Direct3Dオブジェクトの破棄
	if (m_pD3D)
	{
		m_pD3D->Release();
		m_pD3D = nullptr;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CRenderer::Update(void)
{
	//---------------------------------
	// 全ての更新処理
	//---------------------------------
	CScene::UpdateAll();
}

//=============================================================================
// 描画処理
//=============================================================================
void CRenderer::Draw(void)
{
	// バックバッファ＆Ｚバッファのクリア
	m_pD3DDevice->Clear
	(
		0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 255, 255, 0), 1.0f, 0
	);

	// Direct3Dによる描画の開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{
		//---------------------------------
		// 全ての描画処理
		//---------------------------------
		CScene::DrawAll();

		// FPS表示
		DrawFPS();
		DrawTXT();

		// Direct3Dによる描画の終了
		m_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

}
//=============================================================================
// Zテストの処理
//=============================================================================
void CRenderer::SetZTest(bool bUse, LPDIRECT3DDEVICE9 device)
{
	if (bUse)
	{
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}

	else
	{
		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

//=============================================================================
// アルファテストの処理
//=============================================================================
void CRenderer::SetAlphaTest(bool bUse, LPDIRECT3DDEVICE9 device)
{
	if (bUse)
	{
		device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		device->SetRenderState(D3DRS_ALPHAREF, 0x00);
	}

	else
	{
		device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_ALPHAREF, 0x00);
	}
}

//=============================================================================
// 加算合成(透過版)の処理
//=============================================================================
void CRenderer::SetAddSynthesis(bool bUse, LPDIRECT3DDEVICE9 device)
{
	if (bUse)
	{
		// αブレンディングを加算合成に設定
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	}
	else
	{
		// αブレンディングを通常合成に設定
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}

//=============================================================================
// 減算合成の処理
//=============================================================================
void CRenderer::SetSubSynthesis(bool bUse, LPDIRECT3DDEVICE9 device)
{
	if (bUse)
	{
		//減算合成の設定
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	else
	{
		//通常合成に戻す
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}

//=============================================================================
// FPS表示
//=============================================================================
void CRenderer::DrawFPS(void)
{
	// mainからFPSを取得
	int nCntFPS = GetFPS();

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	char str[32];

	wsprintf(str, "FPS:%d\n", nCntFPS);

	// テキスト描画
	m_pFont->DrawText(NULL, str, -1, &rect, DT_RIGHT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));
}

//=============================================================================
// テキスト表示
//=============================================================================
void CRenderer::DrawTXT(void)
{
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	char aStr[1024];

	CCamera *pCamera = CManager::GetInstance()->GetCamera(0);
	CGame *pGame = CManager::GetInstance()->GetGame();

	// sprintfは<stdio.h>が必要&wsprintfと違ってfloatを表示できる
	int nNum = sprintf(&aStr[0], "[オブジェクト生成位置     :(%.1f ：%.1f : %.1f)]\n", pCamera->GetPosR().x, pCamera->GetPosR().y, pCamera->GetPosR().z);

	// 使われた文字数の次のaStrのアドレスを渡す
	nNum += sprintf(&aStr[nNum], "[カメラの向き             :(%.2f : %.2f : %.2f)]\n", pCamera->GetRot().x, pCamera->GetRot().y, pCamera->GetRot().z);

	nNum += sprintf(&aStr[nNum], "[モデルの種類数     :  %d]\n", pGame->GetMaxSelect());
	nNum += sprintf(&aStr[nNum], "[モデルの種類       : (%d : %d)]\n", pGame->GetSelect(), pGame->GetMaxSelect() - 1);
	nNum += sprintf(&aStr[nNum], "[モデルの生成数     :  %d]\n", pGame->GetNumModel());
	nNum += sprintf(&aStr[nNum], "[フィールドの生成数 :  %d]\n\n", pGame->GetNumField());
	nNum += sprintf(&aStr[nNum], "[全てのオブジェクトの生成数 :  %d]\n\n", pGame->GetNumField() + pGame->GetNumModel());

	if (pGame->GetFieldPrev())
	{
		D3DXVECTOR3 size = pGame->GetFieldPrev()->GetSize();
		D3DXVECTOR3 speed = pCamera->GetSpeed2();

		nNum += sprintf(&aStr[nNum], "[フィールドの大きさ  : (%.1f : %.1f : %.1f)]\n", size.x, size.y, size.z);
		nNum += sprintf(&aStr[nNum], "[フィールド移動量    : (%.1f : %.1f : %.1f)]\n", speed.x, speed.y, speed.z);

		nNum += sprintf(&aStr[nNum], "[Ｘメッシュ数             :  %d]\n", pGame->GetFieldPrev()->GetLine());
		nNum += sprintf(&aStr[nNum], "[Ｚメッシュ数             :  %d]\n\n", pGame->GetFieldPrev()->GetVertical());

		nNum += sprintf(&aStr[nNum], "[ＯかＬ       : Ｘ方向のメッシュ数切り替え]\n");
		nNum += sprintf(&aStr[nNum], "[ＩかＫ       : Ｚ方向のメッシュ数切り替え]\n\n");

	}

	if (pGame->GetModelPrev())
	{
		D3DXVECTOR3 size = pGame->GetModelPrev()->GetSize();
		nNum += sprintf(&aStr[nNum], "[モデルの大きさ  : (%.1f : %.1f : %.1f)]\n", size.x, size.y, size.z);
		nNum += sprintf(&aStr[nNum], "[モデル移動量             :  %.1f]\n\n", pCamera->GetSpeed());
		nNum += sprintf(&aStr[nNum], "[ＯかＬ       : 移動量の変更         ]\n");
		nNum += sprintf(&aStr[nNum], "[ＺかＣ       : モデルの種類切り替え      ]\n\n");
	}

	if (pGame->GetInputMode())
	{
		D3DXVECTOR3 pos = pGame->GetInputPos();
		nNum += sprintf(&aStr[nNum], "[座標の入力 : (%.1f : %.1f : %.1f)        ]\n", pos.x, pos.y, pos.z);
		nNum += sprintf(&aStr[nNum], "[最大５桁                                  ]\n");
		nNum += sprintf(&aStr[nNum], "[左右矢印キーでＸＹＺ切り替え             ]\n");
		nNum += sprintf(&aStr[nNum], "[ENTERで座標決定                          ]\n\n");
	}

	else
	{
		nNum += sprintf(&aStr[nNum], "[BACKSPACE    : 一つ前のオブジェクトを消す]\n");
		nNum += sprintf(&aStr[nNum], "[１か２       : オブジェクトの種類切り替え]\n");
		nNum += sprintf(&aStr[nNum], "[Ｆ３         : セーブ                    ]\n");
		nNum += sprintf(&aStr[nNum], "[Ｆ４         : ロード                    ]\n");

	}

	nNum += sprintf(&aStr[nNum], "[Ｆ１         : 入力モードの切り替え]\n");

	m_pFont->DrawText(NULL, aStr, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));
}