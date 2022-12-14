//=============================================================================
// コントロール処理 [control_blink.h]
//=============================================================================
#ifndef _CONTROL_BLINK_H_
#define _CONTROL_BLINK_H_
#include "main.h"
#include "control.h"

//*****************************************************************************
//前方宣言
//*****************************************************************************
class CScene;

//*****************************************************************************
// コントロールクラス
//*****************************************************************************
class CControlBlink : public CControl
{
public:
	CControlBlink();						// コンストラクタ
	~CControlBlink();						// デストラクタ
	HRESULT Init(void);						// 初期化処理
	void Uninit(void);						// 終了処理
	void Update(CScene *pObject);			// 更新処理
	static CControlBlink *Create(void);		// 生成処理
	
	// 点滅速度変更
	void SetInterval(int nInterval) { m_nInterval = nInterval; }

private:
	int m_nInterval;				// 点滅間隔
};

#endif
