//====================================
// 入力処理
//====================================
#include "input.h"

//====================================
// マクロ定義
//====================================

//====================================
// 静的メンバ変数の初期化
//====================================
LPDIRECTINPUT8 CInput::m_pInput = nullptr;	// DirectInputオブジェクトのポインタ
LPDIRECTINPUTDEVICE8 CInput::m_pJoystick = nullptr;

//====================================
// コンストラクタ
//====================================
CInput::CInput()
{
	m_pDevice = nullptr;
}

//====================================
// デストラクタ
//====================================
CInput::~CInput()
{

}

//====================================
// 初期化処理
//====================================
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力の生成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL)))
	{
		return E_FAIL;
	}
	return S_OK;
}

//====================================
// 終了処理
//====================================
void CInput::Uninit(void)
{
	// 入力デバイスの開放
	if (m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	// 入力デバイスの開放
	if (m_pJoystick)
	{
		m_pJoystick->Unacquire();
		m_pJoystick->Release();
		m_pJoystick = nullptr;
	}

	// DirectInputオブジェクトの開放
	if (m_pInput)
	{
		m_pInput->Release();
		m_pInput = nullptr;
	}
}


