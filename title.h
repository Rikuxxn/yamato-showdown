//=============================================================================
//
// タイトル処理 [title.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TITLE_H_// このマクロ定義がされていなかったら
#define _TITLE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "scene.h"
#include "blockmanager.h"

//*****************************************************************************
// タイトルクラス
//*****************************************************************************
class CTitle : public CScene
{
public:
	//タイトルの種類
	typedef enum
	{
		TYPE_FIRST = 0,	// タイトル
		TYPE_SECOND,	// PRESS
		TYPE_MAX
	}TYPE;

	// タイトルの状態
	typedef enum
	{
		WAIT_PRESS = 0,
		TO_GAME,
		STATE_MAX
	}State;

	CTitle();
	~CTitle();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void PressAny(void);
	void FadeOut(void);

private:

	// 各画像の構造体
	struct ImageInfo
	{
		D3DXVECTOR3 pos;
		float width;
		float height;
	};

	// 頂点の範囲を定義する構造体
	typedef struct
	{
		int start;
		int end;
	}VertexRange;

	// タイプごとに頂点範囲を設定
	VertexRange m_vertexRanges[TYPE_MAX];

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	int m_nIdxTextureTitle;					// テクスチャインデックス
	int m_nIdxTexturePress;					// テクスチャインデックス
	float m_alphaPress;						// 現在のアルファ値
	bool  m_isAlphaDown;					// 点滅用フラグ（上げる/下げる）
	bool  m_isEnterPressed;					// エンターキー押された
	CBlockManager* m_pBlockManager;			// ブロックマネージャーへのポインタ
	State m_state;							// 状態
};

#endif
