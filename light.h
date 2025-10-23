//=============================================================================
//
// ���C�g���� [light.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _LIGHT_H_// ���̃}�N����`������Ă��Ȃ�������
#define _LIGHT_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "objectX.h"

//*****************************************************************************
// ���C�g�N���X
//*****************************************************************************
class CLight
{
public:
	CLight();
	~CLight();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static int AddLight(D3DLIGHTTYPE type, const D3DXCOLOR& diffuse, const D3DXVECTOR3& direction, const D3DXVECTOR3& position);
	void DeleteLight(int index); // ���C�g�폜

private:
    static constexpr int MAX_LIGHTS = 32; // ���C�g�̍ő吔

    typedef struct
    {
        D3DLIGHT9 light;
        D3DXVECTOR3 direction;
        D3DXVECTOR3 position;
        bool enabled;
    }LightInfo;

    static LightInfo m_lights[MAX_LIGHTS];
    static int m_lightCount;        // ���C�g�J�E���g
    static CObjectX* m_pObjectX;             // X�t�@�C���I�u�W�F�N�g�ւ̃|�C���^
};

#endif
