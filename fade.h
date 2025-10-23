//=============================================================================
//
// �t�F�[�h���� [fade.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _FADE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _FADE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object2D.h"
#include "scene.h"

//*****************************************************************************
// �t�F�[�h�N���X
//*****************************************************************************
class CFade
{
public:
	// ���
	typedef enum
	{
		FADE_NONE = 0,    // �������Ă��Ȃ����
		FADE_IN,          // �t�F�[�h�C�����
		FADE_OUT,         // �t�F�[�h�A�E�g���
		FADE_MAX
	}FADE;

	CFade();
	~CFade();

	static CFade* Create(CScene::MODE mode);
	HRESULT Init(CScene::MODE mode);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetFade(CScene::MODE modeNext);
	FADE GetFade(void) { return m_fade; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;					// �ʒu
	FADE m_fade;						// �t�F�[�h�̏��
	D3DXCOLOR m_fadeCol;				// �t�F�[�h�̐F
	CScene::MODE m_SceneNext;			// ���̉��
	float m_fWidth;						// ��
	float m_fHeight;					// ����
};

#endif