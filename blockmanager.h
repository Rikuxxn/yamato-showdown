//=============================================================================
//
// ブロックマネージャー処理 [blockmanager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKMANAGER_H_
#define _BLOCKMANAGER_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "block.h"
#include "debugproc3D.h"
#include <cassert>

//*****************************************************************************
// ブロックマネージャークラス
//*****************************************************************************
class CBlockManager
{
public:
	CBlockManager();
	~CBlockManager();

    static CBlock* CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos);
    void Init(void);
    void Uninit(void);// 終了処理
    void CleanupDeadBlocks(void);// 削除予約があるブロックの削除
    void Update(void);
    void Draw(void);
    void UpdateInfo(void); // ImGuiでの操作関数をここで呼ぶ用
    void SaveToJson(const char* filename);
    void LoadFromJson(const char* filename);
    void LoadConfig(const std::string& filename);
    void GenerateRandomMap(int seed);
    void FillFloor(int GRID_X, int GRID_Z, float AREA_SIZE);

    void ReleaseThumbnailRenderTarget(void);

    HRESULT InitThumbnailRenderTarget(LPDIRECT3DDEVICE9 device);
    IDirect3DTexture9* RenderThumbnail(CBlock* pBlock);
    void GenerateThumbnailsForResources(void);


    IDirect3DTexture9* GetThumbnailTexture(size_t index)
    {
        assert(index < m_thumbnailTextures.size());
        return m_thumbnailTextures[index];
    }

     //*****************************************************************************
    // ImGuiでの操作関数
    //*****************************************************************************
    void UpdateTransform(CBlock* selectedBlock);
    void PickBlockFromMouseClick(void);
    void UpdateCollider(CBlock* selectedBlock);

    //*****************************************************************************
    // getter関数
    //*****************************************************************************
    bool GetUpdateCollider(void) { return m_autoUpdateColliderSize; }
    static const char* GetFilePathFromType(CBlock::TYPE type);
    static std::vector<CBlock*>& GetAllBlocks(void);
    int GetSelectedIdx(void) { return m_selectedIdx; }
    int GetPrevSelectedIdx(void) { return m_prevSelectedIdx; }
    static CBlock* GetSelectedBlock(void) { return m_selectedBlock; }

private:
    static std::vector<CBlock*> m_blocks;   // ブロック情報
    static CBlock* m_draggingBlock;         // ドラッグ中のブロック情報
    static int m_selectedIdx;               // 選択中のインデックス
    int m_prevSelectedIdx;                  // 前回の選択中のインデックス
    bool m_hasConsumedPayload ;             // ペイロード生成済みフラグ
    CDebugProc3D* m_pDebug3D;			    // 3Dデバッグ表示へのポインタ
    bool m_autoUpdateColliderSize;
    static std::unordered_map<CBlock::TYPE, std::string> s_FilePathMap;
    static CBlock* m_selectedBlock;         // 選択中のブロック
    bool m_isDragging;                      // ドラッグ中かどうか

    LPDIRECT3DTEXTURE9 m_pThumbnailRT;
    LPDIRECT3DSURFACE9  m_pThumbnailZ;

    std::vector<IDirect3DTexture9*> m_thumbnailTextures;
    bool m_thumbnailsGenerated = false;                 // 一度だけ作るフラグ

    float m_thumbWidth = 100.0f;
    float m_thumbHeight = 100.0f;
};

#endif
