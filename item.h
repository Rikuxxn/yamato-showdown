//=============================================================================
//
// 項目処理 [item.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _ITEM_H_// このマクロ定義がされていなかったら
#define _ITEM_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object.h"

//*****************************************************************************
// 項目クラス
//*****************************************************************************
class CItem : public CObject
{
public:
	CItem(int nPriority = 7);
	~CItem();

	// 選択項目の種類
	typedef enum
	{
		ITEM_ID_PLAY = 0,
		ITEM_ID_CREDITS,
		ITEM_ID_EXIT,
		ITEM_MAX
	}ITEM;

	static CItem* Create(ITEM type, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	virtual void Execute(void) {};

	bool IsMouseOver(void);
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPos(void) { return m_pos; }

	// 選択状態設定・取得
	void SetSelected(bool selected) { m_isSelected = selected; }
	bool IsSelected(void) const { return m_isSelected; }
	ITEM GetItemId(void) const;

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DCOLOR m_col;							// 色
	float m_fWidth, m_fHeight;				// サイズ
	int m_nIdxTexture;						// テクスチャインデックス
	char m_szPath[MAX_PATH];				// ファイルパス
	bool m_isSelected;						// 選択したか

};

//*****************************************************************************
// プレイ項目クラス
//*****************************************************************************
class CPlay : public CItem
{
public:
	CPlay();
	~CPlay();

	void Execute(void) override;

private:

};

//*****************************************************************************
// クレジット項目クラス
//*****************************************************************************
class CCredits : public CItem
{
public:
	CCredits();
	~CCredits();

	void Execute(void) override;

private:

};

//*****************************************************************************
// やめる項目クラス
//*****************************************************************************
class CExit : public CItem
{
public:
	CExit();
	~CExit();

	void Execute(void) override;

private:

};

#endif

