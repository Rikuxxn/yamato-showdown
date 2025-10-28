//=============================================================================
//
// �u���b�N�}�l�[�W���[���� [blockmanager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKMANAGER_H_
#define _BLOCKMANAGER_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "block.h"
#include "debugproc3D.h"
#include <cassert>

//*****************************************************************************
// �u���b�N�}�l�[�W���[�N���X
//*****************************************************************************
class CBlockManager
{
public:
	CBlockManager();
	~CBlockManager();

    static CBlock* CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos);
    void Init(void);
    void Uninit(void);// �I������
    void CleanupDeadBlocks(void);// �폜�\�񂪂���u���b�N�̍폜
    void Update(void);
    void Draw(void);
    void UpdateInfo(void); // ImGui�ł̑���֐��������ŌĂԗp
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
    // ImGui�ł̑���֐�
    //*****************************************************************************
    void UpdateTransform(CBlock* selectedBlock);
    void PickBlockFromMouseClick(void);
    void UpdateCollider(CBlock* selectedBlock);

    //*****************************************************************************
    // getter�֐�
    //*****************************************************************************
    bool GetUpdateCollider(void) { return m_autoUpdateColliderSize; }
    static const char* GetFilePathFromType(CBlock::TYPE type);
    static std::vector<CBlock*>& GetAllBlocks(void) { return m_blocks; }
    int GetSelectedIdx(void) { return m_selectedIdx; }
    int GetPrevSelectedIdx(void) { return m_prevSelectedIdx; }
    static CBlock* GetSelectedBlock(void) { return m_selectedBlock; }

    // ����̃^�C�v�̃u���b�N���擾
    static const std::vector<CBlock*>& GetBlocksByType(CBlock::TYPE type)
    {
        return m_blocksByType[type]; // ���݂��Ȃ��ꍇ�͋�vector���Ԃ�
    }

    // --- ����̌^���܂Ƃ߂Ď擾����e���v���[�g�֐� ---
    template<typename T>
    static std::vector<T*> GetBlocksOfType(void)
    {
        std::vector<T*> result;
        CBlock::TYPE type = T::GetStaticType(); // �e�N���X��static TYPE��`
        for (CBlock* block : m_blocksByType[type])
        {
            result.push_back(static_cast<T*>(block));
        }

        return result;
    }

private:
    static std::vector<CBlock*> m_blocks;   // �u���b�N���
    static std::unordered_map<CBlock::TYPE, std::vector<CBlock*>> m_blocksByType;
    static CBlock* m_draggingBlock;         // �h���b�O���̃u���b�N���
    static int m_selectedIdx;               // �I�𒆂̃C���f�b�N�X
    int m_prevSelectedIdx;                  // �O��̑I�𒆂̃C���f�b�N�X
    bool m_hasConsumedPayload ;             // �y�C���[�h�����ς݃t���O
    CDebugProc3D* m_pDebug3D;			    // 3D�f�o�b�O�\���ւ̃|�C���^
    bool m_autoUpdateColliderSize;
    static std::unordered_map<CBlock::TYPE, std::string> s_FilePathMap;
    static CBlock* m_selectedBlock;         // �I�𒆂̃u���b�N
    bool m_isDragging;                      // �h���b�O�����ǂ���

    LPDIRECT3DTEXTURE9 m_pThumbnailRT;
    LPDIRECT3DSURFACE9  m_pThumbnailZ;

    std::vector<IDirect3DTexture9*> m_thumbnailTextures;
    bool m_thumbnailsGenerated = false;                 // ��x�������t���O

    float m_thumbWidth = 100.0f;
    float m_thumbHeight = 100.0f;
};

#endif
