//=============================================================================
//
// �}�l�[�W���[���� [manager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MANAGER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _MANAGER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "texture.h"
#include "camera.h"
#include "light.h"
#include "scene.h"
#include "fade.h"
#include "pausemanager.h"

//*****************************************************************************
// �}�l�[�W���[�N���X
//*****************************************************************************
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CRenderer* GetRenderer(void) { return m_pRenderer; }
	static CInputKeyboard* GetInputKeyboard(void) { return m_pInputKeyboard; }
	static CInputJoypad* GetInputJoypad(void) { return m_pInputJoypad; }
	static CInputMouse* GetInputMouse(void) { return m_pInputMouse; }
	static CSound* GetSound(void) { return m_pSound; }
	static CTexture* GetTexture(void) { return m_pTexture; }
	static CCamera* GetCamera(void) { return m_pCamera; }
	static CLight* GetLight(void) { return m_pLight; }
	static CFade* GetFade(void) { return m_pFade; }
	static btDiscreteDynamicsWorld* GetPhysicsWorld(void) { return m_pDynamicsWorld.get(); }
	int GetFPS(int fps) { return m_fps = fps; };
	int GetFPSCnt(void) { return m_fps; }

	static void SetMode(CScene::MODE mode);
	static CScene::MODE GetMode(void);

private:
	static CRenderer* m_pRenderer;				// �����_���[�ւ̃|�C���^
	static CInputKeyboard* m_pInputKeyboard;	// �L�[�{�[�h�ւ̃|�C���^
	static CInputJoypad* m_pInputJoypad;		// �W���C�p�b�h�ւ̃|�C���^
	static CInputMouse* m_pInputMouse;			// �}�E�X�ւ̃|�C���^
	static CSound* m_pSound;					// �T�E���h�ւ̃|�C���^
	static CTexture* m_pTexture;				// �e�N�X�`���ւ̃|�C���^
	static CCamera* m_pCamera;					// �J�����ւ̃|�C���^
	static CLight* m_pLight;					// ���C�g�ւ̃|�C���^
	int m_fps;									// FPS�l

	std::unique_ptr<btBroadphaseInterface> m_pBroadphase;						// �Փ˔���̃N���X�ւ̃|�C���^
	std::unique_ptr<btDefaultCollisionConfiguration> m_pCollisionConfiguration;// �Փˌ��o�̐ݒ���Ǘ�����N���X�ւ̃|�C���^
	std::unique_ptr<btCollisionDispatcher> m_pDispatcher;						// ���ۂɏՓ˔��菈�������s����N���X�ւ̃|�C���^
	std::unique_ptr<btSequentialImpulseConstraintSolver> m_pSolver;				// �����V�~�����[�V�����̐���\���o�[�ւ̃|�C���^
	static std::unique_ptr<btDiscreteDynamicsWorld> m_pDynamicsWorld;			// �������E���Ǘ�����N���X�ւ̃|�C���^

	static CFade* m_pFade;
	static CScene* m_pScene;
};

#endif