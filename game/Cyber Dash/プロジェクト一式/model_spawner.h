//===========================================
// モデルを配置するための処理
// Author:村元翼
//===========================================
#ifndef _MODEL_SPAWNER_H_
#define _MODEL_SPAWNER_H_
#include "main.h"
#include "scene.h"

//===========================================
// 前方宣言
//===========================================
class CModel;

//===========================================
// クラスの定義
//===========================================
class CModel_Spawner : public CScene
{
public:
	CModel_Spawner(OBJTYPE nPriority = OBJTYPE_MODEL);	// コンストラクタ
	~CModel_Spawner();									// デストラクタ

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot);		// 初期化
	void Uninit(void);									// 終了
	void Update(void);									// 更新
	void Draw(void);									// 描画

	// オブジェクト生成
	static CModel_Spawner *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int nXType);//rotは角度を入れて

	//-----------------------------------------------
	// 設定・取得処理
	//-----------------------------------------------
	CModel *GetModel(void)			{ return m_pModel; }	// モデル情報の取得
	void SetRot(float fRot)			{ m_rot.y = fRot; }		// Y軸回転情報の設定
	void SetSRot(D3DXVECTOR3 rot)	{ m_rot = rot; }		// 回転情報の設定
	void SetSPos(D3DXVECTOR3 pos)	{ m_pos = pos; }		// 位置情報の設定
	void SetBoolRot(bool bRot)		{ m_MoveRot = bRot; }	// 回転するか否かの設定

protected:
	D3DXVECTOR3				m_pos;					// モデルの位置（オフセット）
	D3DXVECTOR3				m_rot;					// 向き
	D3DXMATRIX				m_mtxWorld;				// ワールドマトリックス
	CModel					*m_pModel;				// モデルクラス

private:
	float					m_fRot;					// 回転
	bool					m_MoveRot;				// 回転するモデルになるかの判定
	D3DXMATERIAL			*m_pMat;				// マテリアルへのポインタ
};
#endif