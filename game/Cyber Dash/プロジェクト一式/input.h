//======================================================
// 入力処理
// Author:村元翼
//======================================================
#ifndef _INPUT_H_
#define _INPUT_H_
#include "main.h"

//======================================================
// 入力処理のクラス
//======================================================
class CInput
{
public:
	CInput();												   // コンストラクタ
	virtual ~CInput();										   // デストラクタ
	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);	   // 初期化
	virtual void Uninit(void);								   // 終了
	virtual void Update(void) = 0;							   // 更新

protected:
	static LPDIRECTINPUT8			m_pInput;		// DirectInputオブジェクトのポインタ
	static LPDIRECTINPUTDEVICE8		m_pJoystick;	// ジョイスティック用デバイス
	LPDIRECTINPUTDEVICE8			m_pDevice;		// 入力デバイスへのポインタ
};

#endif
