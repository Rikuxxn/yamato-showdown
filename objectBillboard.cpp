//=============================================================================
//
// ビルボード処理 [objectBillboard.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "objectBillboard.h"
#include "renderer.h"
#include "manager.h"
#include "game.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CObjectBillboard::CObjectBillboard(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(m_szPath, 0, sizeof(m_szPath));	// ファイルパス
	m_pVtxBuff		= nullptr;				// 頂点バッファへのポインタ
	m_pos			= INIT_VEC3;			// 位置
	m_col			= INIT_XCOL;			// 色
	m_mtxWorld		= {};					// ワールドマトリックス
	m_fSize			= 0.0f;					// サイズ(エフェクト半径)
	m_fHeight		= 0.0f;					// サイズ(ビルボード)
	m_nIdxTexture	= 0;					// テクスチャインデックス
}
//=============================================================================
// デストラクタ
//=============================================================================
CObjectBillboard::~CObjectBillboard()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CObjectBillboard::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの取得
	m_nIdxTexture = CManager::GetTexture()->RegisterDynamic(m_szPath);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D* pVtx = nullptr;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_fSize, +m_fSize + m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(+m_fSize, +m_fSize + m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fSize, -m_fSize - m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(+m_fSize, -m_fSize - m_fHeight, 0.0f);

	// 各頂点の法線の設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

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
void CObjectBillboard::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// オブジェクトの破棄(自分自身)
	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CObjectBillboard::Update(void)
{
	VERTEX_3D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_fSize, +m_fSize + m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(+m_fSize, +m_fSize + m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fSize, -m_fSize - m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(+m_fSize, -m_fSize - m_fHeight, 0.0f);

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
void CObjectBillboard::Draw(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライトを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);	// Zの比較方法
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);		// Zバッファに書き込まない

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ビューマトリックスの取得
	D3DXMATRIX mtxView;
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// カメラの逆行列を設定
	m_mtxWorld._11 = mtxView._11;
	m_mtxWorld._12 = mtxView._21;
	m_mtxWorld._13 = mtxView._31;
	m_mtxWorld._21 = mtxView._12;
	m_mtxWorld._22 = mtxView._22;
	m_mtxWorld._23 = mtxView._32;
	m_mtxWorld._31 = mtxView._13;
	m_mtxWorld._32 = mtxView._23;
	m_mtxWorld._33 = mtxView._33;

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// 元に戻す
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);//Zの比較方法
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);//Zバッファに書き込む

	// ライトを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}


//=============================================================================
// ヒントビルボードのコンストラクタ
//=============================================================================
CHintBillboard::CHintBillboard()
{
	// 値のクリア
	m_fAlpha	= 0.0f;			// アルファ値
	m_state		= STATE_NORMAL;	// 状態
}
//=============================================================================
// ヒントビルボードのデストラクタ
//=============================================================================
CHintBillboard::~CHintBillboard()
{
	// なし
}
//=============================================================================
// ヒントビルボードの初期化処理
//=============================================================================
HRESULT CHintBillboard::Init(void)
{
	// ビルボードの初期化処理
	CObjectBillboard::Init();

	return S_OK;
}
//=============================================================================
// ヒントビルボードの終了処理
//=============================================================================
void CHintBillboard::Uninit(void)
{
	// ビルボードの終了処理
	CObjectBillboard::Uninit();
}
//=============================================================================
// ヒントビルボードの更新処理
//=============================================================================
void CHintBillboard::Update(void)
{
	// ビルボードの更新処理
	CObjectBillboard::Update();

	if (!CGame::GetPlayer())
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 200.0f; // 表示距離

	switch (m_state)
	{
	case STATE_FADEIN:
		m_fAlpha += 0.02f;

		if (m_fAlpha > 1.0f)
		{
			m_fAlpha = 1.0f;
			m_state = STATE_NORMAL;
		}

		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));

		break;
	case STATE_NORMAL:
		if (distance < kTriggerDistance)
		{// 徐々に通常に戻す状態
			m_state = STATE_FADEIN;
		}
		else
		{// 徐々に透明にする状態
			m_state = STATE_FADEOUT;
		}
		break;
	case STATE_FADEOUT:
		m_fAlpha -= 0.02f;

		if (m_fAlpha < 0.0f)
		{
			m_fAlpha = 0.0f;
			m_state = STATE_NORMAL;
		}

		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));
		break;
	}
}
//=============================================================================
// ヒントビルボードの描画処理
//=============================================================================
void CHintBillboard::Draw(void)
{
	// ビルボードの描画処理
	CObjectBillboard::Draw();
}
