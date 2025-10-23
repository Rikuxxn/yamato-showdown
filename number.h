//=============================================================================
//
// �i���o�[���� [number.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _NUMBER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _NUMBER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************

//*****************************************************************************
// �i���o�[�N���X
//*****************************************************************************
class CNumber
{
public:
	CNumber();
	~CNumber();

	static CNumber* Create(float fposX, float fposY, float fWidth, float fHeight);
	HRESULT Init(float fposX, float fposY, float fWidth, float fHeight);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDigit(int digit);
	void SetPos(D3DXVECTOR3 pos);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DCOLOR m_col;							// �F
	int m_digit;							// ��
	float m_fWidth;							// ��
	float m_fHeight;						// ����
};

#endif