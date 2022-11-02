//==================================================
// 入力系デバイスの管理
// Author:村元翼
//==================================================
#include "InputManager.h"
#include "manager.h"
#include "keyboard.h"
#include "XInput.h"
#include "DirectInput.h"
#include "mouse.h"
#include <assert.h>

//==================================================
// コンストラクタ
//==================================================
CInputManager::CInputManager()
{

}

//==================================================
// デストラクタ
//==================================================
CInputManager::~CInputManager()
{

}

//==================================================
// 初期化
//==================================================
HRESULT CInputManager::Init()
{
	return S_OK;
}

//==================================================
// 入力系クラスのプレス情報
//==================================================
bool CInputManager::PressAnyAction(const PRESS &press)
{
	// 入力デバイスの取得
	CMouse *pMouse = CManager::GetInstance()->GetMouse();
	CInputkeyboard *pKey = CManager::GetInstance()->GetKeyboard();
	CXInput *pXPad = CManager::GetInstance()->GetXInput();
	CDInput *pDPad = CManager::GetInstance()->GetDInput();

	switch (press)
	{
	case PRESS_ENTER:	// 決定・ポーズなどに使う
		return	pKey->GetPress(DIK_RETURN) || pXPad->GetButtonPress(XINPUT_GAMEPAD_START) || pDPad->GetPress(pDPad->BUTTON_START);

	case PRESS_SPACE:	// キャンセル・ジャンプなど
		return pMouse->GetPress(CMouse::MOUSE_LEFT) || pKey->GetPress(DIK_SPACE) || pXPad->GetButtonPress(XINPUT_GAMEPAD_A) || pDPad->GetPress(CDInput::BUTTON_A);

	case PRESS_ATTACK:	// 攻撃など
		return pKey->GetPress(DIK_K) || pXPad->GetButtonPress(XINPUT_GAMEPAD_B) || pDPad->GetPress(CDInput::BUTTON_B);

	case PRESS_UP:		// 上昇など
		return	pKey->GetPress(DIK_UP) || pXPad->GetButtonPress(XINPUT_GAMEPAD_DPAD_UP) || pDPad->GetGamepad().lY <= -DINPUT_STICK_RANGE ||
				pKey->GetPress(DIK_W)  || pXPad->GetGamePad()->m_state.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

	case PRESS_DOWN:	// 下降など
		return	pKey->GetPress(DIK_DOWN) || pXPad->GetButtonPress(XINPUT_GAMEPAD_DPAD_DOWN) || pDPad->GetGamepad().lY >= DINPUT_STICK_RANGE ||
				pKey->GetPress(DIK_S)	 || pXPad->GetGamePad()->m_state.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

	case PRESS_LEFT:	// 左行動
		return  pKey->GetPress(DIK_A)	 || pXPad->GetButtonPress(XINPUT_GAMEPAD_DPAD_LEFT) || pDPad->GetGamepad().lX <= -DINPUT_STICK_RANGE ||
				pKey->GetPress(DIK_LEFT) || pXPad->GetGamePad()->m_state.Gamepad.sThumbLX <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

	case PRESS_RIGHT:	// 右行動
		return  pKey->GetPress(DIK_D)	  || pXPad->GetButtonPress(XINPUT_GAMEPAD_DPAD_RIGHT) || pDPad->GetGamepad().lX >= DINPUT_STICK_RANGE ||
				pKey->GetPress(DIK_RIGHT) || pXPad->GetGamePad()->m_state.Gamepad.sThumbLX >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

	case PRESS_R_UP:	// 上回転など
		return pXPad->GetGamePad()->m_state.Gamepad.sThumbRY <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || pDPad->GetGamepad().lRy >= DINPUT_STICK_RANGE;

	case PRESS_R_DOWN:	// 下回転など
		return pXPad->GetGamePad()->m_state.Gamepad.sThumbRY >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || pDPad->GetGamepad().lRy <= -DINPUT_STICK_RANGE;

	case PRESS_R_RIGHT: // 右回転など
		return pXPad->GetGamePad()->m_state.Gamepad.sThumbRX <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || pDPad->GetGamepad().lRx <= -DINPUT_STICK_RANGE;

	case PRESS_R_LEFT:	// 左回転など
		return pXPad->GetGamePad()->m_state.Gamepad.sThumbRX >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || pDPad->GetGamepad().lRx >= DINPUT_STICK_RANGE;

	default:
		assert(press < PRESS_ENTER || press > PRESS_MAX);	// 構造体に存在しない番号を選択した場合エラー
		break;
	}

	return false;
}

//==================================================
// 入力系クラスのトリガー情報
//==================================================
bool CInputManager::TriggerAnyAction(const TRIGGER &trigger)
{

	CMouse *pMouse = CManager::GetInstance()->GetMouse();
	CInputkeyboard *pKey = CManager::GetInstance()->GetKeyboard();
	CXInput *pXPad = CManager::GetInstance()->GetXInput();
	CDInput *pDPad = CManager::GetInstance()->GetDInput();

	switch (trigger)
	{
	case TRIGGER_ENTER:	// 決定・ポーズなどに使う
		return	pKey->GetTrigger(DIK_RETURN) || pXPad->GetButtonTrigger(XINPUT_GAMEPAD_START) || pDPad->GetTrigger(pDPad->BUTTON_START);

	case TRIGGER_SPACE:	// キャンセル・ジャンプなど
		return  pKey->GetTrigger(DIK_SPACE) || pXPad->GetButtonTrigger(XINPUT_GAMEPAD_A) || pDPad->GetTrigger(CDInput::BUTTON_A) || pMouse->GetTrigger(CMouse::MOUSE_LEFT);

	case TRIGGER_ATTACK:// 攻撃など
		return	pKey->GetTrigger(DIK_K) || pXPad->GetButtonTrigger(XINPUT_GAMEPAD_B) || pDPad->GetTrigger(CDInput::BUTTON_B);

	case TRIGGER_UP:	// 上昇など
		return	pKey->GetTrigger(DIK_UP) || pXPad->GetButtonTrigger(XINPUT_GAMEPAD_DPAD_UP) ||
				pKey->GetTrigger(DIK_W);

	case TRIGGER_DOWN:	// 下降など
		return	pKey->GetTrigger(DIK_DOWN) || pXPad->GetButtonTrigger(XINPUT_GAMEPAD_DPAD_DOWN)||
				pKey->GetTrigger(DIK_S);

	case TRIGGER_LEFT:	// 左行動
		return  pKey->GetTrigger(DIK_A) || pXPad->GetButtonTrigger(XINPUT_GAMEPAD_DPAD_LEFT) ||
				pKey->GetTrigger(DIK_LEFT);

	case TRIGGER_RIGHT:	// 右行動
		return  pKey->GetTrigger(DIK_D) || pXPad->GetButtonTrigger(XINPUT_GAMEPAD_DPAD_RIGHT) ||
				pKey->GetTrigger(DIK_RIGHT);

	default:
		assert(trigger < TRIGGER_ENTER || trigger > TRIGGER_MAX);	// 構造体に存在しない番号を選択した場合エラー
		break;
	}

	return false;
}