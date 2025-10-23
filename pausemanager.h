//=============================================================================
//
// �|�[�Y�}�l�[�W���[���� [pausemanager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PAUSEMANAGER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _PAUSEMANAGER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "pause.h"

//*****************************************************************************
// �|�[�Y�}�l�[�W���[�N���X
//*****************************************************************************
class CPauseManager
{
public:
    CPauseManager();
    ~CPauseManager();

    void Init(void);                    // ������
    void Uninit(void);                  // �I��
    void Update(void);                  // �X�V�i���͏����j
    void Draw(void);                    // �`��
  
private:
    LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// �w�i�p���_�o�b�t�@�ւ̃|�C���^
    std::vector<CPause*> m_Items;       // �|�[�Y����
    int m_SelectedIndex;                // �I�������C���f�b�N�X
    bool m_inputLock;

    int GetMouseOverIndex(void) const;
};

#endif