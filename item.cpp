//=============================================================================
//
// 項目処理 [item.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "item.h"
#include "renderer.h"
#include "manager.h"
#include "itemselect.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CItem::CItem(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(m_szPath, 0, sizeof(m_szPath));
	m_pVtxBuff = nullptr;
	m_pos = INIT_VEC3;
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_nIdxTexture = 0;
	m_isSelected = false;
}
//=============================================================================
// デストラクタ
//=============================================================================
CItem::~CItem()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CItem* CItem::Create(ITEM type, D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	CItem* pItem = nullptr;

	switch (type)
	{
	case ITEM_ID_PLAY:
		pItem = new CPlay;
		pItem->SetPath("data/TEXTURE/play_button.png");
		break;
	case ITEM_ID_CREDITS:
		pItem = new CCredits;
		pItem->SetPath("data/TEXTURE/credits_button.png");
		break;
	case ITEM_ID_EXIT:
		pItem = new CExit;
		pItem->SetPath("data/TEXTURE/exit_button.png");
		break;
	default:
		pItem = new CItem;
		break;
	}

	pItem->SetPos(pos);
	pItem->m_fWidth = fWidth;
	pItem->m_fHeight = fHeight;
	pItem->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// 初期化処理
	pItem->Init();

	return pItem;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CItem::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	m_nIdxTexture = CManager::GetTexture()->RegisterDynamic(m_szPath);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CItem::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CItem::Update(void)
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 選択項目の色の切り替え
	if (IsMouseOver() || m_isSelected)
	{
		m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 描画処理
//=============================================================================
void CItem::Draw(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//=============================================
	// 項目
	//=============================================
	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
//=============================================================================
// マウスカーソルの判定処理
//=============================================================================
bool CItem::IsMouseOver(void)
{
	// マウスカーソルの位置を取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// ウィンドウハンドルを取得
	HWND hwnd = GetActiveWindow();

	// スクリーン座標をクライアント座標に変換
	ScreenToClient(hwnd, &cursorPos);

	// クライアントサイズを取得
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	float left = m_pos.x - m_fWidth;
	float right = m_pos.x + m_fWidth;
	float top = m_pos.y - m_fHeight;
	float bottom = m_pos.y + m_fHeight;

	return (cursorPos.x >= left && cursorPos.x <= right &&
		cursorPos.y >= top && cursorPos.y <= bottom);
}
//=============================================================================
// ステージIDを返す処理
//=============================================================================
CItem::ITEM CItem::GetItemId(void) const
{
	if (dynamic_cast<const CPlay*>(this)) return ITEM_ID_PLAY;
	if (dynamic_cast<const CCredits*>(this)) return ITEM_ID_CREDITS;
	if (dynamic_cast<const CExit*>(this)) return ITEM_ID_EXIT;

	return ITEM_MAX; // 想定外
}


//=============================================================================
// プレイ項目のコンストラクタ
//=============================================================================
CPlay::CPlay()
{
	// 値のクリア

}
//=============================================================================
// プレイ項目のデストラクタ
//=============================================================================
CPlay::~CPlay()
{
	// なし
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CPlay::Execute(void)
{
	// ゲーム画面に移行
	CManager::GetFade()->SetFade(CScene::MODE_GAME);
}


//=============================================================================
// クレジット項目のコンストラクタ
//=============================================================================
CCredits::CCredits()
{
	// 値のクリア

}
//=============================================================================
// クレジット項目のデストラクタ
//=============================================================================
CCredits::~CCredits()
{
	// なし
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CCredits::Execute(void)
{
	// ゲーム画面に移行
	CManager::GetFade()->SetFade(CScene::MODE_GAME);
}


//=============================================================================
// やめる項目のコンストラクタ
//=============================================================================
CExit::CExit()
{
	// 値のクリア

}
//=============================================================================
// やめる項目のデストラクタ
//=============================================================================
CExit::~CExit()
{
	// なし
}
//=============================================================================
// 選択時の処理
//=============================================================================
void CExit::Execute(void)
{
	//ウィンドウを破棄する
	PostQuitMessage(0);
}
