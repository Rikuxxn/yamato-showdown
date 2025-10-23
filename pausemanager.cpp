//=============================================================================
//
// �|�[�Y�}�l�[�W���[���� [pausemanager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "pausemanager.h"
#include "manager.h"
#include "game.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CPauseManager::CPauseManager()
{
    // �l�̃N���A
    m_pVtxBuff      = nullptr;
    m_SelectedIndex = 0;
    m_inputLock        = false;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CPauseManager::~CPauseManager()
{
    // �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
void CPauseManager::Init(void)
{
    // �f�o�C�X�̎擾
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

    // ���_�o�b�t�@�̐���
    pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
        D3DUSAGE_WRITEONLY,
        FVF_VERTEX_2D,
        D3DPOOL_MANAGED,
        &m_pVtxBuff,
        NULL);

    VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

    // ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
    m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

    // ���_���W�̐ݒ�
    pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    pVtx[1].pos = D3DXVECTOR3(1920.0f, 0.0f, 0.0f);
    pVtx[2].pos = D3DXVECTOR3(0.0f, 1080.0f, 0.0f);
    pVtx[3].pos = D3DXVECTOR3(1920.0f, 1080.0f, 0.0f);

    // rhw�̐ݒ�
    pVtx[0].rhw = 1.0f;
    pVtx[1].rhw = 1.0f;
    pVtx[2].rhw = 1.0f;
    pVtx[3].rhw = 1.0f;

    // ���_�J���[�̐ݒ�
    pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
    pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
    pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
    pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);

    // �e�N�X�`�����W�̐ݒ�
    pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
    pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
    pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
    pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

    // ���_�o�b�t�@���A�����b�N����
    m_pVtxBuff->Unlock();


    // ��ɂ���
    m_Items.clear();

    // �|�[�Y�̐���
    m_Items.push_back(CPause::Create(CPause::MENU_CONTINUE, D3DXVECTOR3(860.0f, 310.0f, 0.0f), 200.0f, 60.0f));
    m_Items.push_back(CPause::Create(CPause::MENU_RETRY, D3DXVECTOR3(860.0f, 510.0f, 0.0f), 200.0f, 60.0f));
    m_Items.push_back(CPause::Create(CPause::MENU_QUIT, D3DXVECTOR3(860.0f, 710.0f, 0.0f), 200.0f, 60.0f));

    for (auto item : m_Items)
    {
        // �|�[�Y�̏���������
        item->Init();

        item->SetSelected(false);
    }

    m_SelectedIndex = 0;
    m_Items[m_SelectedIndex]->SetSelected(true);

    m_inputLock = false;
}
//=============================================================================
// �I������
//=============================================================================
void CPauseManager::Uninit(void)
{
    // ���_�o�b�t�@�̔j��
    if (m_pVtxBuff != nullptr)
    {
        m_pVtxBuff->Release();
        m_pVtxBuff = nullptr;
    }

    // ��ɂ���
    m_Items.clear();
}
//=============================================================================
// �X�V����
//=============================================================================
void CPauseManager::Update(void)
{
    if (!CGame::GetisPaused())
    {
        return;
    }

    int mouseOver = GetMouseOverIndex();

    // �}�E�X�I�[�o�[���̑I��ύX��SE
    if (mouseOver != -1 && mouseOver != m_SelectedIndex)
    {
        m_SelectedIndex = mouseOver;

        // �I��SE
        CManager::GetSound()->Play(CSound::SOUND_LABEL_SELECT);
    }

    // �Q�[���p�b�h�ł̏㉺�ړ�
    bool up = CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_UP);
    bool down = CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_DOWN);

    if ((up || down) && !m_inputLock)
    {
        // �I��SE
        CManager::GetSound()->Play(CSound::SOUND_LABEL_SELECT);

        if (up)
        {
            m_SelectedIndex--;
        }
        else
        {
            m_SelectedIndex++;
        }

        if (m_SelectedIndex < 0)
        {
            m_SelectedIndex = (int)m_Items.size() - 1;
        }
        if (m_SelectedIndex >= (int)m_Items.size())
        {
            m_SelectedIndex = 0;
        }

        m_inputLock = true;
    }

    if (!up && !down)
    {
        m_inputLock = false;
    }

    // �N���b�N�܂��̓Q�[���p�b�h�{�^���Ŏ��s
    if (CManager::GetFade()->GetFade() == CFade::FADE_NONE)
    {
        bool confirm = false;

        // �}�E�X�N���b�N
        if (mouseOver != -1 && CManager::GetInputMouse()->GetTrigger(0))
        {
            confirm = true;
        }

        // �Q�[���p�b�h
        if (CManager::GetInputJoypad()->GetTrigger(CInputJoypad::JOYKEY_A))
        {
            confirm = true;
        }

        if (confirm)
        {
            // ����SE
            CManager::GetSound()->Play(CSound::SOUND_LABEL_ENTER);

            // �e���ڂ̑I��������
            m_Items[m_SelectedIndex]->Execute();
        }
    }

    // �I����ԍX�V
    for (size_t i = 0; i < m_Items.size(); i++)
    {
        m_Items[i]->SetSelected(i == static_cast<size_t>(m_SelectedIndex));

        // �|�[�Y�̍X�V����
        m_Items[i]->Update();
    }
}
//=============================================================================
// �`�揈��
//=============================================================================
void CPauseManager::Draw(void)
{
    // �f�o�C�X�̎擾
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

    // ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
    pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

    // ���_�t�H�[�}�b�g�̐ݒ�
    pDevice->SetFVF(FVF_VERTEX_2D);

    // �e�N�X�`���̐ݒ�
    pDevice->SetTexture(0, nullptr);

    // �|���S���̕`��
    pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    for (auto item : m_Items)
    {
        if (item)
        {
            item->Draw();
        }
    }
}
//=============================================================================
// �}�E�X�I������
//=============================================================================
int CPauseManager::GetMouseOverIndex(void) const
{
    for (size_t i = 0; i < m_Items.size(); i++)
    {
        if (m_Items[i]->IsMouseOver())
        {
            return (int)i;
        }
    }

    return -1;
}


