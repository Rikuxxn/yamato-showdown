//=============================================================================
//
// �|�[�Y���� [pause.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PAUSE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _PAUSE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object2D.h"


//*****************************************************************************
// �|�[�Y�N���X
//*****************************************************************************
class CPause : public CObject2D
{
public:
	CPause(int nPriority = 7);
	~CPause();

	// �I�����ڂ̎��
	typedef enum
	{
		MENU_CONTINUE = 0,		// �Q�[���ɖ߂�
		MENU_RETRY,				// �Q�[������蒼��
		MENU_QUIT,				// �^�C�g����ʂɖ߂�
		MENU_MAX
	}MENU;

	static CPause* Create(MENU type, D3DXVECTOR3 pos,float fWidth,float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool IsMouseOver(void);
	virtual void Execute(void) {};

	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }

	// �I����Ԑݒ�E�擾
	void SetSelected(bool selected) { m_isSelected = selected; }
	bool IsSelected(void) const { return m_isSelected; }

private:
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X
	char m_szPath[MAX_PATH];				// �t�@�C���p�X
	bool m_isSelected;						// �I�񂾂�
};

//*****************************************************************************
// �R���e�B�j���[���ڃN���X
//*****************************************************************************
class CContinue : public CPause
{
public:
	CContinue();
	~CContinue();

	void Execute(void) override;

private:

};

//*****************************************************************************
// ���g���C���ڃN���X
//*****************************************************************************
class CRetry : public CPause
{
public:
	CRetry();
	~CRetry();

	void Execute(void) override;

private:

};

//*****************************************************************************
// �I�����ڃN���X
//*****************************************************************************
class CQuit : public CPause
{
public:
	CQuit();
	~CQuit();

	void Execute(void) override;

private:

};

#endif
