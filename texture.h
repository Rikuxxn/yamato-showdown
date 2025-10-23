//=============================================================================
//
// �e�N�X�`������ [texture.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TEXTURE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _TEXTURE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************

//*****************************************************************************
// �e�N�X�`���N���X
//*****************************************************************************
class CTexture
{
public:
	CTexture();
	~CTexture();

	HRESULT Load(void);
	void Unload(void);
	int Register(const char* pFilename);
	int RegisterDynamic(const char* pFilename);
	LPDIRECT3DTEXTURE9 GetAddress(int nIdx);

private:
	static constexpr int MAX_TEXTURE = 128;// �e�N�X�`���̍ő吔
	std::string TEXTURE[MAX_TEXTURE];
	LPDIRECT3DTEXTURE9 m_apTexture[MAX_TEXTURE];
	static int m_nNumAll;
};

#endif