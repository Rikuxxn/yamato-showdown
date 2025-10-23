//=============================================================================
//
// �O���b�h���� [grid.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GRID_H_
#define _GRID_H_

//*****************************************************************************
// �O���b�h�N���X
//*****************************************************************************
class CGrid
{
public:
    CGrid();
    ~CGrid();

    static std::unique_ptr<CGrid>Create(void);// ���j�[�N�|�C���^�̐���
    HRESULT Init(void);
    void Draw(void);
    void AddLine(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, D3DCOLOR col);

private:
    static constexpr int SIZE       = 6;   // ���a
    static constexpr float SPACING  = 80.0f;// �}�X�Ԋu

    struct CUSTOMVERTEX
    {
        D3DXVECTOR3 position;
        D3DCOLOR color;
    };

    D3DXMATRIX m_mtxWorld;                  // ���[���h�}�g���b�N�X
    std::vector<CUSTOMVERTEX> m_vertices;   // ���_
    int m_size;                             // ���a
    float m_spacing;                        // �}�X�Ԋu
};

#endif
