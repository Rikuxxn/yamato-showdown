//=============================================================================
//
// �f�o�b�O�\������ [debugproc.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _DEBUGPROC_H_// ���̃}�N����`������Ă��Ȃ�������
#define _DEBUGPROC_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************

//*****************************************************************************
// �f�o�b�O�N���X
//*****************************************************************************
class CDebugProc
{
public:
	CDebugProc();
	~CDebugProc();

	void Init(void);
	void Uninit(void);
	void UpdateInfo(void);
	static void Print(const char* fmt, ...);
	static void Draw(D3DXCOLOR col,int posX,int posY);

private:
	static LPD3DXFONT m_pFont;	// �t�H���g�ւ̃|�C���^
	static char m_aStr[1024];	// ��������i�[����o�b�t�@

};
#endif