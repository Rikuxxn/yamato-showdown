//=============================================================================
//
// 3D�f�o�b�O�\������ [debugproc3D.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _DEBUGPROC3D_H_// ���̃}�N����`������Ă��Ȃ�������
#define _DEBUGPROC3D_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************

//*****************************************************************************
// 3D�f�o�b�O�N���X
//*****************************************************************************
class CDebugProc3D
{
public:
	CDebugProc3D();
	~CDebugProc3D();

	void Init(void);
	void Uninit(void);

	//*****************************************************************************
	// line�`��֐�
	//*****************************************************************************
	static void DrawLine3D(const D3DXVECTOR3& start, const D3DXVECTOR3& end, D3DXCOLOR color);
	static void DrawCapsuleCollider(btCapsuleShape* capsule, const btTransform& transform, D3DXCOLOR color);
	static void DrawBlockCollider(btRigidBody* rigidBody, D3DXCOLOR color);

private:
	static LPD3DXLINE m_pLine;   // ���C���`��p�I�u�W�F�N�g

};

#endif