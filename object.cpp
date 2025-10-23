//=============================================================================
//
// �I�u�W�F�N�g���� [object.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"
#include "camera.h"
#include "manager.h"
#include "game.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CObject* CObject::m_apTop[MAX_OBJ_PRIORITY] = {};
CObject* CObject::m_apCur[MAX_OBJ_PRIORITY] = {};
int CObject::m_nNumAll = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CObject::CObject(int nPriority)
{
	m_nPriority = 0;
	m_type = TYPE_NONE;

	// �I�u�W�F�N�g(����)�����X�g�ɒǉ�
	m_pPrev = m_apCur[nPriority];

	if (m_pPrev != nullptr)
	{
		m_pPrev->m_pNext = this;
	}

	m_pNext = nullptr;

	if (m_apTop[nPriority] == nullptr)
	{
		m_apTop[nPriority] = this;
	}

	m_apCur[nPriority] = this;

	m_nNumAll++;
	m_nPriority = nPriority;
	m_bDeath = false;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CObject::~CObject()
{
	// ���͂Ȃ�
}
//=============================================================================
// �S�ẴI�u�W�F�N�g�̔j��
//=============================================================================
void CObject::ReleaseAll(void)
{
	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;// ���̃I�u�W�F�N�g��ۑ�

			// �I������
			pObject->Uninit();

			// �I�u�W�F�N�g�̃��X�g�폜�Ɣj��
			pObject->Destroy();

			pObject = pObjectNext;// ���̃I�u�W�F�N�g����
		}
	}
}
//=============================================================================
// �S�ẴI�u�W�F�N�g�̍X�V����
//=============================================================================
void CObject::UpdateAll(void)
{
	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;// ���̃I�u�W�F�N�g��ۑ�

			if (!pObject->m_bDeath)
			{
				// �X�V����
				pObject->Update();
			}

			pObject = pObjectNext;// ���̃I�u�W�F�N�g����
		}
	}

	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;// ���̃I�u�W�F�N�g��ۑ�

			if (pObject->m_bDeath)
			{
				// �I�u�W�F�N�g�̃��X�g�폜�Ɣj��
				pObject->Destroy();
			}

			pObject = pObjectNext;// ���̃I�u�W�F�N�g����
		}
	}

}
//=============================================================================
// �S�ẴI�u�W�F�N�g�̕`�揈��
//=============================================================================
void CObject::DrawAll(void)
{
	// �J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	// �J�����̐ݒ�
	pCamera->SetCamera();

	// �u���b�N�̓����蔻��̕`��
	CGame::GetBlockManager()->Draw();

	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;// ���̃I�u�W�F�N�g��ۑ�

			// �`�揈��
			pObject->Draw();

			pObject = pObjectNext;// ���̃I�u�W�F�N�g����
		}
	}
}
//=============================================================================
// �I�u�W�F�N�g�̔j���t���O�ݒ�
//=============================================================================
void CObject::Release(void)
{
	m_bDeath = true;
}
//=============================================================================
// �I�u�W�F�N�g�̑����̎擾
//=============================================================================
int CObject::GetNumObject(void)
{
	return m_nNumAll;
}
//=============================================================================
// �^�C�v�̐ݒ菈��
//=============================================================================
void CObject::SetType(TYPE type)
{
	m_type = type;
}
//=============================================================================
// �^�C�v�̎擾
//=============================================================================
CObject::TYPE CObject::GetType(void)
{
	return m_type;
}
//=============================================================================
// �I�u�W�F�N�g�̃��X�g�폜�Ɣj��
//=============================================================================
void CObject::Destroy(void)
{
	int nPriority = this->m_nPriority;

	if (m_pPrev != nullptr && m_pNext != nullptr)
	{// �Ԃ��������Ƃ�
		m_pNext->m_pPrev = m_pPrev;
		m_pPrev->m_pNext = m_pNext;
	}
	else if (m_pPrev == nullptr && m_pNext != nullptr)
	{// �擪���������Ƃ�
		m_pNext->m_pPrev = nullptr;
		m_apTop[nPriority] = m_pNext;
	}
	else if (m_pPrev != nullptr && m_pNext == nullptr)
	{// �Ō�����������Ƃ�
		m_pPrev->m_pNext = nullptr;
		m_apCur[nPriority] = m_pPrev;
	}
	else if (m_pPrev == nullptr && m_pNext == nullptr)
	{// �B��̗v�f�������ꍇ
		m_apTop[nPriority] = nullptr;
		m_apCur[nPriority] = nullptr;
	}

	m_pPrev = nullptr;
	m_pNext = nullptr;

	m_nNumAll--;

	delete this;
}