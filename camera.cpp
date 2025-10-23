//=============================================================================
//
// �J�������� [camera.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "camera.h"
#include "renderer.h"
#include "manager.h"
#include "game.h"

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CCamera::CCamera()
{
	// �l�̃N���A
	m_posV					= INIT_VEC3;// ���_
	m_posVDest				= INIT_VEC3;// �ړI�̎��_
	m_posR					= INIT_VEC3;// �����_
	m_posRDest				= INIT_VEC3;// �ړI�̒����_
	m_vecU					= INIT_VEC3;// ������x�N�g��
	m_mtxProjection			= {};		// �v���W�F�N�V�����}�g���b�N�X
	m_mtxView				= {};		// �r���[�}�g���b�N�X
	m_rot					= INIT_VEC3;// ����
	m_targetPos				= INIT_VEC3;// �J�����U���p�̑Ώۈʒu
	m_shakeOffset			= INIT_VEC3;// �ŏI�I�ɉ�����I�t�Z�b�g
	m_eventShakeTime		= 0.0f;     // �c��̃C�x���g�V�F�C�N����
	m_eventShakePower		= 0.0f;     // �C�x���g�V�F�C�N�̍ő勭��
	m_eventShakeDuration	= 0.0f;		// �V�F�C�N�̌���
	m_fDistance				= 0.0f;		// ���_���璍���_�̋���
	m_nDirectionCamTimer	= 0;
	m_nTimer				= 0;
	m_isDirection			= false;
	m_isCameraShakeOn		= false;	// �J�����V�F�C�N��ON/OFF
#ifdef _DEBUG
	m_Mode = MODE_EDIT;					// �J�����̃��[�h
#else
	m_Mode = MODE_GAME;
#endif
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CCamera::~CCamera()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CCamera::Init(void)
{	
	m_posV = D3DXVECTOR3(0.0f, 80.0f, -540.0f);
	m_posR = D3DXVECTOR3(0.0f, 80.0f, 0.0f);
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);// �Œ�ł���
	m_rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);
	
#ifdef _DEBUG
	m_Mode = MODE_EDIT;									// �J�����̃��[�h

	m_fDistance = sqrtf(
		((m_posV.x - m_posR.x) * (m_posV.x - m_posR.x)) +
		((m_posV.y - m_posR.y) * (m_posV.y - m_posR.y)) +
		((m_posV.z - m_posR.z) * (m_posV.z - m_posR.z)));

#else
	m_Mode = MODE_GAME;

	m_fDistance = 220.0f;

#endif

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CCamera::Uninit(void)
{
	// �T�E���h�̒�~
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_INSPIRATION);
}
//=============================================================================
// �X�V����
//=============================================================================
void CCamera::Update(void)
{
	// �^�C�g����ʂ�������
	if (CManager::GetMode() == MODE_TITLE)
	{// �J�����̈ʒu�̐ݒ�
		m_posV = D3DXVECTOR3(-1373.5f, 331.5f, -1130.5f);
		m_posR = D3DXVECTOR3(-913.5f, 150.0f, -1700.0f);
		m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);// �Œ�ł���
		m_rot = D3DXVECTOR3(0.25f, -0.70f, 0.0f);
		m_fDistance = sqrtf(
			((m_posV.x - m_posR.x) * (m_posV.x - m_posR.x)) +
			((m_posV.y - m_posR.y) * (m_posV.y - m_posR.y)) +
			((m_posV.z - m_posR.z) * (m_posV.z - m_posR.z)));
	}

	// ���X�i�[�̈ʒu�̍X�V
	CManager::GetSound()->UpdateListener(m_posV);

#ifdef _DEBUG
	// �L�[�{�[�h�̎擾
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (m_Mode == MODE_EDIT && pInputKeyboard->GetTrigger(DIK_C) == true)
	{
		// �Q�[���J����
		m_Mode = MODE_GAME;
	}
	else if (m_Mode == MODE_GAME && pInputKeyboard->GetTrigger(DIK_C) == true)
	{
		// �G�f�B�^�[�J����
		m_Mode = MODE_EDIT;
	}
#endif
	switch (m_Mode)
	{
	case MODE_EDIT:

#ifdef _DEBUG
		if (CManager::GetMode() == MODE_TITLE)
		{
			return;
		}

		// �G�f�B�^�[�J�����̏���
		EditCamera();
#endif
		break;
	case MODE_GAME:
		// �Q�[���̃J��������
		GameCamera();
		break;
	case MODE_DIRECTION:
		// ���o�J�����̏���
		DirectionCamera(m_nTimer);
		break;
	}

	CameraShake();// �J�����V�F�C�N����

}
//=============================================================================
// �J�����̐ݒ菈��
//=============================================================================
void CCamera::SetCamera(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxView);

	// �r���[�}�g���b�N�X�̍쐬
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU);

	// �r���[�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxProjection);

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(80.0f),						// ����p
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, // �A�X�y�N�g��
		1.0f,										// �߃N���b�v��
		2500.0f);									// ���N���b�v��

	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}
//=============================================================================
// �G�f�B�^�[�J�����̏���
//=============================================================================
void CCamera::EditCamera(void)
{
	// �L�[�{�[�h�̎擾
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	// �}�E�X�̎擾
	CInputMouse* pInputMouse = CManager::GetInputMouse();

	// �}�E�X�J�[�\����\������
	pInputMouse->SetCursorVisibility(true);

	// ���݂̃J�[�\���ʒu���擾
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// �O�t���[������̃}�E�X�ړ��ʂ��擾
	static POINT prevCursorPos = { cursorPos.x, cursorPos.y };
	float deltaX = (float)(cursorPos.x - prevCursorPos.x);
	float deltaY = (float)(cursorPos.y - prevCursorPos.y);

	// ���݂̃J�[�\���ʒu��ۑ��i���̃t���[���ł̔�r�p�j
	prevCursorPos = cursorPos;

	// �}�E�X���x
	const float mouseSensitivity = 0.004f;

	deltaX *= mouseSensitivity;
	deltaY *= mouseSensitivity;

	//====================================
	// �}�E�X�z�C�[���ŃY�[���C���E�A�E�g
	//====================================
	int wheel = pInputMouse->GetWheel();
	const float zoomSpeed = 15.0f;

	if (wheel != 0)
	{
		m_fDistance -= wheel * zoomSpeed;

		// �J������������
		if (m_fDistance < 100.0f)
		{
			m_fDistance = 100.0f;
		}
		if (m_fDistance > 800.0f)
		{
			m_fDistance = 800.0f;
		}
	}

	if (pInputKeyboard->GetPress(DIK_LALT) && pInputMouse->GetPress(0)) // ���N���b�N�����Ȃ���}�E�X�ړ� �� ���_��]
	{
		m_rot.y += deltaX; // ������]
		m_rot.x += deltaY; // ������]

		//�p�x�̐��K��
		NormalizeRotY();

		// ������]�̐���
		if (m_rot.x > 1.57f)
		{
			m_rot.x = 1.57f;
		}

		if (m_rot.x < -1.57f)
		{
			m_rot.x = -1.57f;
		}

		// ���_�̍X�V�i�J�����̕�����K�p�j
		m_posV.x = m_posR.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

	}
	else if (pInputMouse->GetPress(1)) // �E�N���b�N�����Ȃ���}�E�X�ړ� �� �����_��]
	{
		m_rot.y += deltaX; // ������]
		m_rot.x += deltaY; // ������]

		//�p�x�̐��K��
		NormalizeRotY();

		// ������]�̐���
		if (m_rot.x > 1.57f)
		{
			m_rot.x = 1.57f;
		}
		if (m_rot.x < -1.57f)
		{
			m_rot.x = -1.57f;
		}

		// �����_�̍X�V
		m_posR.x = m_posV.x - sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posR.y = m_posV.y - sinf(m_rot.x) * m_fDistance;
		m_posR.z = m_posV.z - cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	}
	else
	{
		// ���͂��Ȃ��ꍇ�ł��Y�[�����f�̂��߂Ɏ��_���X�V
		m_posV.x = m_posR.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	}

	// �����_�̍X�V
	m_posR.x = m_posV.x - sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	m_posR.y = m_posV.y - sinf(m_rot.x) * m_fDistance;
	m_posR.z = m_posV.z - cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
}
//=============================================================================
// �Q�[���J�����̏���
//=============================================================================
void CCamera::GameCamera(void)
{
	if (CManager::GetMode() != MODE_GAME || m_Mode == MODE_DIRECTION)
	{
		return;
	}

	// ����������Z������
	static bool firstTime = true;

	if (firstTime)
	{
		m_fDistance = 220.0f;
		firstTime = false;
	}

	// �}�E�X�̎擾
	CInputMouse* pInputMouse = CManager::GetInputMouse();

	// �X�e�B�b�N�̎擾
	XINPUT_STATE* pStick = CInputJoypad::GetStickAngle();

	// �v���C���[�̎擾
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer != nullptr)
	{
		// �v���C���[�̈ʒu�̎擾
		D3DXVECTOR3 playerPos = pPlayer->GetPos();

		// �}�E�X�̏�Ԃ��擾
		DIMOUSESTATE mouseState;

		// �}�E�X�J�[�\�����\���ɂ���
		pInputMouse->SetCursorVisibility(false);

		// �Q�[���p�b�h�E�X�e�B�b�N�J��������
		if (pStick != nullptr)
		{
			// �E�X�e�B�b�N�̒l���擾
			float stickX = pStick->Gamepad.sThumbRX;
			float stickY = pStick->Gamepad.sThumbRY;

			// �f�b�h�]�[������
			const float DEADZONE = 10922.0f;
			if (fabsf(stickX) < DEADZONE)
			{
				stickX = 0.0f;
			}
			if (fabsf(stickY) < DEADZONE)
			{
				stickY = 0.0f;
			}

			// ���K��
			stickX /= 32768.0f;
			stickY /= 32768.0f;

			// �J������]�̍X�V
			CameraWithGamepad(stickX, stickY);
		}
		if (pInputMouse->GetMouseState(&mouseState))
		{
			// �O�t���[���̃J�[�\���ʒu���L�^����ÓI�ϐ�
			static POINT prevCursorPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

			// ���݂̃}�E�X�̈ʒu�̎擾
			POINT cursorPos;
			GetCursorPos(&cursorPos);

			// �}�E�X���x
			const float mouseSensitivity = 0.003f;

			float deltaX = (float)(cursorPos.x - prevCursorPos.x) * mouseSensitivity;
			float deltaY = (float)(cursorPos.y - prevCursorPos.y) * mouseSensitivity;

			prevCursorPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
			SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

			m_rot.y += deltaX;
			m_rot.x += deltaY;

			// �㉺��]����
			if (m_rot.x < -1.3f)
			{
				m_rot.x = -1.3f;
			}
			if (m_rot.x > 1.3f)
			{
				m_rot.x = 1.3f;
			}

			//====================================
			// �}�E�X�z�C�[���ŃY�[���C���E�A�E�g
			//====================================
			int wheel = pInputMouse->GetWheel();
			const float zoomSpeed = 2.0f;

			if (wheel != 0)
			{
				m_fDistance -= wheel * zoomSpeed;

				// �J������������
				if (m_fDistance < 100.0f)
				{
					m_fDistance = 100.0f;
				}
				if (m_fDistance > 200.0f)
				{
					m_fDistance = 200.0f;
				}
			}
		}

		// �J�����ʒu�v�Z
		m_posV.x = playerPos.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = playerPos.y + sinf(m_rot.x) * m_fDistance + 80.0f;
		m_posV.z = playerPos.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

		// �����_
		m_posR = playerPos;
		m_posR.y += 60.0f;

		// �J�����̈ʒu�␳����
		AdjustCameraPosition(playerPos);
	}
}
//=============================================================================
// �Q�[���p�b�h�̃J������]����
//=============================================================================
void CCamera::CameraWithGamepad(float stickX, float stickY)
{
	const float rotationSpeed = 0.07f; // ��]���x

	 // ���������̉�]�iY���j
	m_rot.y += stickX * rotationSpeed;

	// ���������̉�]�iX���j
	m_rot.x -= stickY * rotationSpeed;

	// ���������̉�]�𐧌� (-90�x �` 90�x)
	NormalizeRotX();

	// ���������̉�]�𐳋K�� (-�� �` ��)
	NormalizeRotY();
}
//=============================================================================
// �J�����̈ʒu�␳(�Ǌђʂ��Ȃ���)����
//=============================================================================
void CCamera::AdjustCameraPosition(const D3DXVECTOR3& playerPos)
{
	// �v���C���[�̓��ʒu�𒍎��_�Ƃ���
	D3DXVECTOR3 playerEye = playerPos;
	playerEye.y += 60.0f; // �v���C���[�̓��̍���
	m_posR = playerEye;

	// �J�����̗��z�ʒu
	D3DXVECTOR3 offsetFromPlayer;
	offsetFromPlayer.x = sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	offsetFromPlayer.y = sinf(m_rot.x) * m_fDistance + 20.0f;
	offsetFromPlayer.z = cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

	D3DXVECTOR3 idealCamPos = playerEye + offsetFromPlayer;

	// Bullet Physics �Ń��C�L���X�g
	btVector3 from(playerEye.x, playerEye.y, playerEye.z);
	btVector3 to(idealCamPos.x, idealCamPos.y, idealCamPos.z);

	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	CManager::GetPhysicsWorld()->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit())
	{
		// �ՓˑΏۂ̃��[�U�[�|�C���^����u���b�N�����
		const btCollisionObject* hitObj = rayCallback.m_collisionObject;
		void* userPtr = hitObj->getUserPointer();

		if (userPtr != nullptr)
		{
			// �Փ˓_�̏�����O�ɃJ������z�u
			btVector3 hitPoint = rayCallback.m_hitPointWorld;
			btVector3 camDir = (from - hitPoint).normalized();
			hitPoint += camDir * 10.0f;

			m_posV = D3DXVECTOR3(hitPoint.x(), hitPoint.y(), hitPoint.z());
		}
		else
		{// ���[�U�[�|�C���^�� null �̏ꍇ

			// �␳���Ă���
			btVector3 hitPoint = rayCallback.m_hitPointWorld;
			btVector3 camDir = (from - hitPoint).normalized();
			hitPoint += camDir * 10.0f;

			m_posV = D3DXVECTOR3(hitPoint.x(), hitPoint.y(), hitPoint.z());
		}
	}
	else
	{// �Փ˂Ȃ�
		// ���z�ʒu���̂܂�
		m_posV = idealCamPos;
	}

	// �����_�̓v���C���[�̓�
	m_posR = playerEye;
}
//=============================================================================
// ���o�J�����̐ݒ菈��
//=============================================================================
void CCamera::SetCamMode(int nTimer,D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 rot)
{
	m_Mode = MODE_DIRECTION;

	// �^�C�}�[�̐ݒ�
	SetTimer(nTimer);

	m_posV = posV;
	m_posR = posR;
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);// �Œ�ł���
	m_rot = rot;
	m_fDistance = sqrtf(
		((m_posV.x - m_posR.x) * (m_posV.x - m_posR.x)) +
		((m_posV.y - m_posR.y) * (m_posV.y - m_posR.y)) +
		((m_posV.z - m_posR.z) * (m_posV.z - m_posR.z)));
}
//=============================================================================
// ���o�J�����̏���
//=============================================================================
void CCamera::DirectionCamera(int nTimer)
{
	m_nDirectionCamTimer++;

	if (m_nDirectionCamTimer >= nTimer)
	{
		m_isDirection = false;
		// �J�E���^�[���Z�b�g
		m_nDirectionCamTimer = 0;

		m_fDistance = 220.0f;

		// �Ђ�߂�SE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_INSPIRATION);

		// �Q�[���J�����ɖ߂�
		m_Mode = MODE_GAME;
	}
}
//=============================================================================
// �J�����̑O���x�N�g���擾
//=============================================================================
D3DXVECTOR3 CCamera::GetForward(void) const
{
	// �J�����̉�]�p�x�iY���j����O���x�N�g�����v�Z
	float yaw = m_rot.y;

	D3DXVECTOR3 forward(-sinf(yaw), 0.0f, -cosf(yaw));

	// ���K������
	D3DXVec3Normalize(&forward, &forward);

	return forward;
}
//=============================================================================
// �p�x�̐��K�� X��
//=============================================================================
void CCamera::NormalizeRotX(void)
{
	if (m_rot.x > D3DX_PI * 0.5f)
	{
		m_rot.x = D3DX_PI * 0.5f;
	}
	if (m_rot.x < -D3DX_PI * 0.5f)
	{
		m_rot.x = -D3DX_PI * 0.5f;
	}
}
//=============================================================================
// �p�x�̐��K�� Y��
//=============================================================================
void CCamera::NormalizeRotY(void)
{
	if (m_rot.y > D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2.0f;
	}
	else if (m_rot.y < -D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2.0f;
	}
}
//=============================================================================
// �J�����V�F�C�N����
//=============================================================================
void CCamera::CameraShake(void)
{
	// --------------------------
	// �����ˑ��̏펞�U��
	// --------------------------
	if (m_isCameraShakeOn)
	{
		D3DXVECTOR3 targetpos = m_targetPos - m_posR;

		float dist = D3DXVec3Length(&targetpos);

		float maxDist = 1300.0f;// �U�����鋗��
		float ambientIntensity = 1.0f - (dist / maxDist);
		ambientIntensity = max(0.0f, min(1.0f, ambientIntensity));

		if (ambientIntensity > 0.0f)
		{
			static float t = 0.0f;
			t += 0.2f;
			m_shakeOffset.x += sinf(t * 8.0f) * 5.0f * ambientIntensity;
			m_shakeOffset.y += cosf(t * 12.0f) * 3.0f * ambientIntensity;
			m_shakeOffset.z += sinf(t * 6.0f) * 4.0f * ambientIntensity;
		}
	}

	// --------------------------
	// �C�x���g�ˑ��̐U��
	// --------------------------
	if (m_eventShakeTime > 0.0f)
	{
		float power = m_eventShakePower * (m_eventShakeTime / m_eventShakeDuration); // ����
		m_shakeOffset.x += ((rand() % 2000 - 1000) / 1000.0f) * power;
		m_shakeOffset.y += ((rand() % 2000 - 1000) / 1000.0f) * power;
		m_shakeOffset.z += ((rand() % 2000 - 1000) / 1000.0f) * power;

		m_eventShakeTime--; // �o�ߎ��ԂŌ���
	}

	if (m_Mode == MODE_DIRECTION || m_Mode == MODE_EDIT)
	{// ���o�J������������
		return;
	}

	// --------------------------
	// �J�����ʒu�ɔ��f
	// --------------------------
	m_posV += m_shakeOffset;
}
//=============================================================================
// �C�x���g���̃J�����V�F�C�N�̐ݒ�
//=============================================================================
void CCamera::TriggerEventShake(float power, float duration)
{
	m_eventShakePower = power;
	m_eventShakeDuration = duration;
	m_eventShakeTime = duration;
}
