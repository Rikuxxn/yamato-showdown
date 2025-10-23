//=============================================================================
//
// �e�N�X�`������ [texture.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "texture.h"
#include "renderer.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
int CTexture::m_nNumAll = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CTexture::CTexture()
{
	// �l�̃N���A
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		m_apTexture[nCnt] = {};
	}
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CTexture::~CTexture()
{
	// �Ȃ�
}
//=============================================================================
// �e�N�X�`���̓ǂݍ���
//=============================================================================
HRESULT CTexture::Load(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �S�Ẵe�N�X�`���̓ǂݍ���
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		if (!TEXTURE[nCnt].empty()) // �󕶎��񂶂�Ȃ���΃��[�h
		{
			HRESULT hr = D3DXCreateTextureFromFile(
				pDevice,
				TEXTURE[nCnt].c_str(),
				&m_apTexture[nCnt]);

			if (FAILED(hr))
			{
				m_apTexture[nCnt] = nullptr;
			}
		}
		else
		{
			m_apTexture[nCnt] = nullptr;
		}
	}

	return S_OK;
}
//=============================================================================
// �e�N�X�`���̔j��
//=============================================================================
void CTexture::Unload(void)
{
	// �S�Ẵe�N�X�`���̔j��
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		// �N���A
		TEXTURE[nCnt].clear();
		if (m_apTexture[nCnt] != nullptr)
		{
			m_apTexture[nCnt]->Release();
			m_apTexture[nCnt] = nullptr;
		}
	}
}
//=============================================================================
// �e�N�X�`���̎w��
//=============================================================================
int CTexture::Register(const char* pFilename)
{
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		if (!TEXTURE[nCnt].empty() && TEXTURE[nCnt] == pFilename)
		{
			return nCnt; // ��������
		}
	}

	return -1; // ������Ȃ�����
}
//=============================================================================
// �e�N�X�`���̎w��
//=============================================================================
int CTexture::RegisterDynamic(const char* pFilename)
{
	// ���łɃ��[�h�ς݂Ȃ�C���f�b�N�X�Ԃ�
	for (int nCnt = 0; nCnt < m_nNumAll; nCnt++)
	{
		if (!TEXTURE[nCnt].empty() && TEXTURE[nCnt] == pFilename)
		{
			return nCnt;
		}
	}

	// �V�����X���b�g�Ƀ��[�h
	if (m_nNumAll < MAX_TEXTURE)
	{
		// �f�o�C�X�̎擾
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		if (SUCCEEDED(D3DXCreateTextureFromFile(pDevice, pFilename, &m_apTexture[m_nNumAll])))
		{
			TEXTURE[m_nNumAll] = pFilename;  // std::string �ɑ��
			return m_nNumAll++;
		}
	}
	return -1;
}
//=============================================================================
// �e�N�X�`���̃A�h���X�擾
//=============================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_TEXTURE)
	{// �͈͊O
		return nullptr;
	}

	return m_apTexture[nIdx];
}