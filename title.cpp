//=============================================================================
//
// �^�C�g������ [title.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "title.h"
#include "input.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CTitle::CTitle() : CScene(CScene::MODE_TITLE)
{
	// �l�̃N���A
	m_pVtxBuff = NULL;		// ���_�o�b�t�@�ւ̃|�C���^
	m_nIdxTextureTitle = 0;
	m_nIdxTexturePress = 0;
	m_alphaPress = 0.0f;          // ���݂̃��l�i0.0f �` 1.0f�j
	m_isAlphaDown = false;         // �_�ŗp�t���O�i�グ��/������j
	m_isEnterPressed = false;      // �G���^�[�L�[�����ꂽ
	m_state = WAIT_PRESS;

	for (int nCnt = 0; nCnt < TYPE_MAX; nCnt++)
	{
		m_vertexRanges[nCnt] = { -1, -1 }; // ���g�p�l�ŏ�����
	}
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CTitle::~CTitle()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CTitle::Init(void)
{
	// �}�E�X�J�[�\����\������
	CManager::GetInputMouse()->SetCursorVisibility(true);

	// �u���b�N�}�l�[�W���[�̐���
	m_pBlockManager = new CBlockManager;

	// �u���b�N�}�l�[�W���[�̏�����
	m_pBlockManager->Init();

	// JSON�̓ǂݍ���
	m_pBlockManager->LoadFromJson("data/block_title.json");

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̎擾
	m_nIdxTextureTitle = CManager::GetTexture()->RegisterDynamic("data/TEXTURE/title.png");
	m_nIdxTexturePress = CManager::GetTexture()->RegisterDynamic("data/TEXTURE/press.png");

	m_vertexRanges[TYPE_FIRST] = { 0, 3 }; // �^�C�g��
	m_vertexRanges[TYPE_SECOND] = { 4, 7 }; // PRESS

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * 2,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	ImageInfo images[2] =
	{
		   { D3DXVECTOR3(420.0f, 270.0f, 0.0f), 320.0f, 220.0f },	// �^�C�g�����S
		   { D3DXVECTOR3(880.0f, 770.0f, 0.0f), 320.0f, 50.0f }		// PRESS
	};

	for (int nCnt = 0; nCnt < 2; nCnt++)
	{
		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(images[nCnt].pos.x - images[nCnt].width, images[nCnt].pos.y - images[nCnt].height, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(images[nCnt].pos.x + images[nCnt].width, images[nCnt].pos.y - images[nCnt].height, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(images[nCnt].pos.x - images[nCnt].width, images[nCnt].pos.y + images[nCnt].height, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(images[nCnt].pos.x + images[nCnt].width, images[nCnt].pos.y + images[nCnt].height, 0.0f);

		// rhw�̐ݒ�
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		//���_�J���[�̐ݒ�
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	// ���T�E���h�̍Đ�
	CManager::GetSound()->Play(CSound::SOUND_LABEL_FIRE);

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CTitle::Uninit(void)
{
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_FIRE);

	// �u���b�N�}�l�[�W���[�̔j��
	if (m_pBlockManager != nullptr)
	{
		m_pBlockManager->Uninit();

		delete m_pBlockManager;
		m_pBlockManager = nullptr;
	}

	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CTitle::Update(void)
{
	switch (m_state)
	{
	case WAIT_PRESS:
		PressAny();
		break;

	case TO_GAME:
		FadeOut();
		break;
	}

	// �A���t�@�l�𒸓_�ɓK�p
	int starts[] = 
	{
		m_vertexRanges[TYPE_FIRST].start,
		m_vertexRanges[TYPE_SECOND].start
	};
	int ends[] =
	{
		m_vertexRanges[TYPE_FIRST].end,
		m_vertexRanges[TYPE_SECOND].end
	};

	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// TYPE_SECOND�ɂ͏�ɃA���t�@�K�p
	{
		int start = m_vertexRanges[TYPE_SECOND].start;
		int end = m_vertexRanges[TYPE_SECOND].end;

		for (int n = start; n <= end; n++)
		{
			pVtx[n].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_alphaPress);
		}
	}

	// TYPE_FIRST��m_isEnterPressed��true�ɂȂ���������
	if (m_isEnterPressed)
	{
		int start = m_vertexRanges[TYPE_FIRST].start;
		int end = m_vertexRanges[TYPE_FIRST].end;

		for (int n = start; n <= end; n++)
		{
			pVtx[n].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_alphaPress);
		}
	}
	else
	{
		int start = m_vertexRanges[TYPE_FIRST].start;
		int end = m_vertexRanges[TYPE_FIRST].end;

		for (int n = start; n <= end; n++)
		{
			pVtx[n].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	// �u���b�N�}�l�[�W���[�̍X�V����
	m_pBlockManager->Update();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CTitle::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���C���f�b�N�X�z��
	int textures[2] = { m_nIdxTextureTitle, m_nIdxTexturePress };

	// �e�e�N�X�`�����Ƃɕ`��
	for (int nCnt = 0; nCnt < TYPE_MAX; nCnt++)
	{
		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, CManager::GetTexture()->GetAddress(textures[nCnt]));

		// �|���S���̕`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_vertexRanges[nCnt].start, 2);
	}
}
//=============================================================================
// ���͏���
//=============================================================================
void CTitle::PressAny(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();
	CInputMouse* pMouse = CManager::GetInputMouse();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	CFade* pFade = CManager::GetFade();

	if (pFade->GetFade() == CFade::FADE_NONE &&
		(pInputKeyboard->GetAnyKeyTrigger() || pMouse->GetPress(0) || pJoypad->GetAnyTrigger()))
	{
		m_state = TO_GAME;
		m_isEnterPressed = true;

		// �Q�[����ʂɈڍs
		CManager::GetFade()->SetFade(CScene::MODE_GAME);
	}
	else
	{
		// �_�Ń��W�b�N
		float speed = 0.01f;

		if (m_isAlphaDown)
		{
			m_alphaPress -= speed;

			if (m_alphaPress < 0.1f) // �ŏ��l
			{
				m_alphaPress = 0.1f;
				m_isAlphaDown = false;
			}
		}
		else
		{
			m_alphaPress += speed;

			if (m_alphaPress > 1.0f) // �ő�l
			{
				m_alphaPress = 1.0f;
				m_isAlphaDown = true;
			}
		}
	}
}
//=============================================================================
// �t�F�[�h�A�E�g����
//=============================================================================
void CTitle::FadeOut(void)
{
	// �t�F�[�h�A�E�g
	float fadeSpeed = 0.03f;

	m_alphaPress -= fadeSpeed;

	if (m_alphaPress < 0.0f)
	{
		m_alphaPress = 0.0f;
		m_state = TO_GAME;
	}
}
