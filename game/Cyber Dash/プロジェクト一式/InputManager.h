//==================================================
// 入力系デバイスの管理
// Author:村元翼
//==================================================
#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_
#include "main.h"

//==================================================
// クラス定義
//==================================================
class CInputManager
{
public:
	//------------------------------------
	// 入力情報列挙
	//------------------------------------
	typedef enum
	{
		PRESS_ENTER = 0,
		PRESS_SPACE,
		PRESS_ATTACK,
		PRESS_UP,
		PRESS_DOWN,
		PRESS_RIGHT,
		PRESS_LEFT,
		PRESS_R_UP,
		PRESS_R_DOWN,
		PRESS_R_RIGHT,
		PRESS_R_LEFT,
		PRESS_MAX

	}PRESS;

	typedef enum
	{
		TRIGGER_ENTER = 0,
		TRIGGER_SPACE,
		TRIGGER_ATTACK,
		TRIGGER_UP,
		TRIGGER_DOWN,
		TRIGGER_RIGHT,
		TRIGGER_LEFT,
		TRIGGER_MAX

	}TRIGGER;

	CInputManager();	// コンストラクタ
	~CInputManager();	// デストラクタ
	HRESULT Init(void);	// 初期化

	static bool PressAnyAction(const PRESS &press);			// プレス情報
	static bool TriggerAnyAction(const TRIGGER &trigger);	// トリガー情報

};

#endif