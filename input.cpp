//=============================================================================
//
// ���͏��� [input.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "input.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
LPDIRECTINPUT8 CInput::m_pInput = nullptr;		// DirectInput�I�u�W�F�N�g�ւ̃|�C���^
XINPUT_STATE CInputJoypad::m_joyKeyState = {};
DIMOUSESTATE CInputMouse::m_mouseState = {};

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CInput::CInput()
{
	// �l�̃N���A
	m_pDevice = nullptr;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CInput::~CInput()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CInput::Init(HINSTANCE hInstance)
{
	// DirectInput�I�u�W�F�N�g�̐���
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&m_pInput, NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CInput::Uninit(void)
{
	// ���̓f�o�C�X�̔j��
	if (m_pDevice != nullptr)
	{
		m_pDevice->Unacquire();// �L�[�{�[�h�ւ̃A�N�Z�X����j��
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	// DirectInput�I�u�W�F�N�g�̔j��
	if (m_pInput != nullptr)
	{
		m_pInput->Release();
		m_pInput = nullptr;
	}
}
//=============================================================================
// �L�[�{�[�h�̃R���X�g���N�^
//=============================================================================
CInputKeyboard::CInputKeyboard()
{
	// �l�̃N���A
	for (int nCnt = 0; nCnt < NUM_KEY_MAX; nCnt++)
	{
		m_aKeyState[nCnt] = {};				// �L�[�{�[�h�̃v���X���
		m_aOldState[nCnt] = {};				// �L�[�{�[�h�̑O��̃v���X���
	}
}
//=============================================================================
// �L�[�{�[�h�̃f�X�g���N�^
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{
	// �Ȃ�
}
//=============================================================================
// �L�[�{�[�h�̏���������
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	// ���͂̏�����
	CInput::Init(hInstance);

	// ���̓f�o�C�X(�L�[�{�[�h)�̐���
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	//�f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//�������[�h��ݒ�
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return S_OK;
	}

	// �L�[�{�[�h�ւ̃A�N�Z�X��
	m_pDevice->Acquire();

	return S_OK;
}
//=============================================================================
// �L�[�{�[�h�̏I������
//=============================================================================
void CInputKeyboard::Uninit(void)
{
	// ���͂̏I������
	CInput::Uninit();
}
//=============================================================================
// �L�[�{�[�h�̍X�V����
//=============================================================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX]; // �ŐV�̃L�[�{�[�h���
	memcpy(m_aOldState, m_aKeyState, sizeof(m_aKeyState)); // ����Ԃ�ۑ�

	// �L�[�{�[�h�̌��݂̏�Ԃ��擾
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		memcpy(m_aKeyState, aKeyState, sizeof(m_aKeyState)); // �ŐV�̏�Ԃ�ۑ�
	}
	else
	{
		// ���͎擾���s���A�f�o�C�X���Ď擾
		m_pDevice->Acquire();
		memset(m_aKeyState, 0, sizeof(m_aKeyState)); // ���ׂẴL�[�𖢉�����ԂɃ��Z�b�g
	}
}
//=============================================================================
// �L�[�{�[�h�̃v���X�����擾
//=============================================================================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true : false;
}
//=============================================================================
// �L�[�{�[�h�̃g���K�[�����擾
//=============================================================================
bool CInputKeyboard::GetTrigger(int nKey)
{
	bool Trigger = false;
	if (m_aKeyState[nKey] & 0x80 && !(m_aOldState[nKey] & 0x80))
	{
		Trigger = true;
	}
	return Trigger;
}
//=============================================================================
// ��������̃L�[���V���������ꂽ���`�F�b�N
//=============================================================================
bool CInputKeyboard::GetAnyKeyTrigger(void)
{
	for (int nCnt = 0; nCnt < NUM_KEY_MAX; nCnt++)
	{
		if (nCnt == DIK_F11)
		{
			continue;
		}

		if ((m_aKeyState[nCnt] & 0x80) && !(m_aOldState[nCnt] & 0x80)) // �V���������ꂽ�L�[������
		{
			return true;
		}
	}
	return false;
}
//=============================================================================
// �L�[�{�[�h�̃����[�X�����擾
//=============================================================================
bool CInputKeyboard::GetRelease(int nKey)
{
	bool Trigger = false;
	if (m_aOldState[nKey] & 0x80 && !(m_aKeyState[nKey] & 0x80))
	{
		Trigger = true;
	}
	return Trigger;
}
//=============================================================================
// �L�[�{�[�h�̃��s�[�g�����擾
//=============================================================================
bool CInputKeyboard::GetRepeat(int nKey)
{
	bool Trigger = false;
	if (m_aOldState[nKey] & 0x80 && !(m_aKeyState[nKey] & 0x80))
	{
		Trigger = true;
	}
	return Trigger;
}


//=============================================================================
// �W���C�p�b�h�̃R���X�g���N�^
//=============================================================================
CInputJoypad::CInputJoypad()
{
	// �l�̃N���A
	m_joyKeyState = {};					// �W���C�p�b�h�̃v���X���
	m_joyKeyStateTrigger = {};			// �W���C�p�b�h�̃g���K�[���
	m_joyKeyStateRelease = {};			// �W���C�p�b�h�̃����[�X���
	m_aOldJoyKeyState = {};				// �W���C�p�b�h�̑O��̏��

	for (int nCnt = 0; nCnt < JOYKEY_MAX; nCnt++)
	{
		m_joyKeyFlag[nCnt] = {};
	}
}
//=============================================================================
// �W���C�p�b�h�̃f�X�g���N�^
//=============================================================================
CInputJoypad::~CInputJoypad()
{
	// �Ȃ�
}
//=============================================================================
// �W���C�p�b�h�̏���������
//=============================================================================
HRESULT CInputJoypad::Init(void)
{
	//�������̃N���A
	memset(&m_joyKeyState, 0, sizeof(XINPUT_STATE));

	//�������̃N���A
	memset(&m_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));

	//�������̃N���A
	memset(&m_aOldJoyKeyState, 0, sizeof(XINPUT_STATE));

	//XInput�̃X�e�[�g��ݒ�(�L���ɂ���)
	XInputEnable(true);

	return S_OK;
}
//=============================================================================
// �W���C�p�b�h�̏I������
//=============================================================================
void CInputJoypad::Uninit(void)
{
	// XInput�̃X�e�[�g��ݒ�(�����ɂ���)
	XInputEnable(false);
}
//=============================================================================
// �W���C�p�b�h�̍X�V����
//=============================================================================
void CInputJoypad::Update(void)
{
	XINPUT_STATE joyKeyState;

	// �O�t���[���̏�Ԃ�ۑ�
	m_aOldJoyKeyState = m_joyKeyState;

	if (XInputGetState(0, &joyKeyState) == ERROR_SUCCESS)
	{
		WORD Button = joyKeyState.Gamepad.wButtons;
		WORD OldButton = m_joyKeyState.Gamepad.wButtons;

		// �V���������ꂽ�{�^���̔���
		m_joyKeyStateTrigger.Gamepad.wButtons = (Button & ~OldButton);
		m_joyKeyStateRelease.Gamepad.wButtons = (OldButton & ~Button);

		// L2�ER2 �̃g���K�[����
		m_joyKeyStateTrigger.Gamepad.bLeftTrigger =
			(joyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
			!(m_aOldJoyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		m_joyKeyStateTrigger.Gamepad.bRightTrigger =
			(joyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
			!(m_aOldJoyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		m_joyKeyState = joyKeyState; // �ŐV�̏�Ԃ�ۑ�
	}
	else
	{
		memset(&m_joyKeyState, 0, sizeof(XINPUT_STATE));
		memset(&m_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));
		memset(&m_joyKeyStateRelease, 0, sizeof(XINPUT_STATE));
	}
}
//=============================================================================
// �W���C�p�b�h�̃g���K�[����
//=============================================================================
bool CInputJoypad::GetTrigger(JOYKEY Key)
{
	bool joykey = false;
	if (m_joyKeyState.Gamepad.wButtons & (0x01 << Key) && !(m_aOldJoyKeyState.Gamepad.wButtons & (0x01 << Key)))
	{
		joykey = true;
	}
	return joykey;
}
//=============================================================================
// �W���C�p�b�h�̃v���X
//=============================================================================
bool CInputJoypad::GetPress(JOYKEY Key)
{
	return (m_joyKeyState.Gamepad.wButtons & (0x01 << Key)) ? true : false;
}
//=============================================================================
// L2�g���K�[�i���g���K�[�j�̃g���K�[����
//=============================================================================
bool CInputJoypad::GetTriggerL2(void)
{
	return (m_joyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		!(m_aOldJoyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

//=============================================================================
// R2�g���K�[�i�E�g���K�[�j�̃g���K�[����
//=============================================================================
bool CInputJoypad::GetTriggerR2(void)
{
	return (m_joyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		!(m_aOldJoyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}
//=============================================================================
// L2�g���K�[�i���g���K�[�j�̃v���X����
//=============================================================================
bool CInputJoypad::GetPressL2(void)
{
	return (m_joyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}
//=============================================================================
// R2�g���K�[�i�E�g���K�[�j�̃v���X����
//=============================================================================
bool CInputJoypad::GetPressR2(void)
{
	return (m_joyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}
//=============================================================================
// ��������̃{�^����������Ă��邩����i�v���X�j
//=============================================================================
bool CInputJoypad::GetAnyPress(void)
{
	return m_joyKeyState.Gamepad.wButtons != 0;
}
//=============================================================================
// ��������̃{�^����������Ă��邩����i�g���K�[�j
//=============================================================================
bool CInputJoypad::GetAnyTrigger(void)
{
	if (m_joyKeyStateTrigger.Gamepad.wButtons != 0 ||
		m_joyKeyStateTrigger.Gamepad.bLeftTrigger ||
		m_joyKeyStateTrigger.Gamepad.bRightTrigger)
	{
		return true;
	}
	return false;
}
//=============================================================================
// �X�e�B�b�N����
//=============================================================================
bool CInputJoypad::GetStick(void)
{
	bool joykey = false;
	//�^�񒆂���Ȃ�������
	if (m_joyKeyState.Gamepad.sThumbLX >= 100 ||
		m_joyKeyState.Gamepad.sThumbLX <= -100 ||
		m_joyKeyState.Gamepad.sThumbLY >= 100 ||
		m_joyKeyState.Gamepad.sThumbLY <= -100)
	{
		joykey = true;
	}
	return joykey;
}
//=============================================================================
// �X�e�B�b�N���̎擾����
//=============================================================================
XINPUT_STATE* CInputJoypad::GetStickAngle(void)
{
	return &m_joyKeyState;
}


//=============================================================================
// �}�E�X�̃R���X�g���N�^
//=============================================================================
CInputMouse::CInputMouse()
{
	// �l�̃N���A
	m_mouseState = {};
}
//=============================================================================
// �}�E�X�̃f�X�g���N�^
//=============================================================================
CInputMouse::~CInputMouse()
{
	// �Ȃ�
}
//=============================================================================
// �}�E�X�̏���������
//=============================================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	// ���͂̏���������
	CInput::Init(hInstance);

	// ���̓f�o�C�X(�L�[�{�[�h)�̐���
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	//�f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse)))
	{
		return E_FAIL;
	}

	// �E�B���h�E����A�N�e�B�u�ł��}�E�X���g�p�\�ɂ���
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd,
		DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		MessageBox(NULL, "SetCooperativeLevel �Ɏ��s���܂���", "�G���[", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// �}�E�X�ւ̃A�N�Z�X��
	m_pDevice->Acquire();

	return S_OK; // ����������
}
//=============================================================================
// �}�E�X�̏I������
//=============================================================================
void CInputMouse::Uninit(void)
{
	// ���͂̏I������
	CInput::Uninit();
}
//=============================================================================
// �}�E�X�̍X�V����
//=============================================================================
void CInputMouse::Update(void)
{
	if (m_pDevice)
	{
		HRESULT hr = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState);
		if (FAILED(hr))
		{
			if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			{
				m_pDevice->Acquire();
			}
		}
	}
}
//=============================================================================
// �}�E�X�̃v���X���
//=============================================================================
bool CInputMouse::GetPress(int button)
{
	return (m_mouseState.rgbButtons[button] & 0x80) != 0;
}
//=============================================================================
// �}�E�X�̃g���K�[���
//=============================================================================
bool CInputMouse::GetTrigger(int button)
{
	static DIMOUSESTATE oldState = {};

	bool trigger = (m_mouseState.rgbButtons[button] & 0x80) &&
		!(oldState.rgbButtons[button] & 0x80);

	oldState = m_mouseState;
	return trigger;
}
//=============================================================================
// �}�E�X�̃����[�X���
//=============================================================================
bool CInputMouse::GetRelease(int button)
{
	static DIMOUSESTATE oldState = {};

	bool release = !(m_mouseState.rgbButtons[button] & 0x80) &&
		(oldState.rgbButtons[button] & 0x80);

	oldState = m_mouseState;  // Update previous state
	return release;
}
//=============================================================================
// �}�E�X�̏��
//=============================================================================
bool CInputMouse::GetMouseState(DIMOUSESTATE* mouseState)
{
	// �}�E�X�f�o�C�X���擾
	LPDIRECTINPUTDEVICE8 pMouse = GetDevice();

	if (pMouse == nullptr)
	{
		return false;
	}

	// �}�E�X�̏�Ԃ��擾(��������������)
	HRESULT hr = pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)mouseState);

	if (FAILED(hr))
	{
		// ���̓f�o�C�X�����Z�b�g����Ă���ꍇ�A�Ď擾�����݂�
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			pMouse->Acquire();

			// �Ď擾�����݂�
			hr = pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)mouseState);
			if (FAILED(hr))
			{
				return false;
			}
		}
		else
		{
			//�G���[�̏ꍇ
			return false;
		}
	}

	return true; // ����Ɏ擾�ł����ꍇ
}
//=============================================================================
// �}�E�X�z�C�[���̃X�N���[���ʂ��擾����֐�
//=============================================================================
int CInputMouse::GetWheel(void)
{
	const int ScrollUnit = 120; // �ʏ�̃X�N���[���P��

	int wheelValue = m_mouseState.lZ;

	// �X�N���[���P�ʂɊ�Â��Đ��K��
	if (wheelValue != 0)
	{
		wheelValue /= ScrollUnit; // ���K�����ꂽ�X�N���[���ʂ�Ԃ�
	}

	return wheelValue; // ���i��X�N���[���j�A���i���X�N���[���j�A0�i�X�N���[���Ȃ��j
}
//=============================================================================
// �}�E�X�J�[�\���\������
//=============================================================================
void CInputMouse::SetCursorVisibility(bool visible)
{
	static int cursorCount = 0;

	if (visible)
	{
		// �J�[�\����\��
		while (cursorCount < 0)
		{
			ShowCursor(TRUE);
			cursorCount++;
		}
	}
	else
	{
		// �J�[�\�����\��
		while (cursorCount >= 0)
		{
			ShowCursor(FALSE);
			cursorCount--;
		}
	}
}
//=============================================================================
// �n��(Y = 0)�Ƃ̌����_���擾���鏈��
//=============================================================================
D3DXVECTOR3 CInputMouse::GetGroundHitPosition(void)
{
	// �f�o�C�X�̎擾
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	if (!pDevice)
	{
		return D3DXVECTOR3(0, 0, 0);
	}

	// �r���[�|�[�g�ƍs��擾
	D3DVIEWPORT9 viewport;
	D3DXMATRIX matProj, matView, matWorld;
	pDevice->GetViewport(&viewport);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matWorld);

	// �}�E�X���W�擾
	POINT mousePos;
	GetCursorPos(&mousePos);

	// �E�B���h�E�n���h�����擾
	HWND hwnd = GetActiveWindow();
	ScreenToClient(hwnd, &mousePos);

	// �X�N���[�����W�����[���h���W�ϊ�
	D3DXVECTOR3 nearPoint((float)mousePos.x, (float)mousePos.y, 0.0f);
	D3DXVECTOR3 farPoint((float)mousePos.x, (float)mousePos.y, 1.0f);
	D3DXVECTOR3 rayOrigin, rayDir;

	D3DXVec3Unproject(&rayOrigin, &nearPoint, &viewport, &matProj, &matView, &matWorld);
	D3DXVec3Unproject(&rayDir, &farPoint, &viewport, &matProj, &matView, &matWorld);
	rayDir -= rayOrigin;
	D3DXVec3Normalize(&rayDir, &rayDir);

	// �n��(Y = 0)�Ƃ̌�_
	if (fabsf(rayDir.y) < 1e-5f)
	{
		return D3DXVECTOR3(0, 0, 0); // �������C�΍�
	}

	float t = -rayOrigin.y / rayDir.y;

	return rayOrigin + rayDir * t;
}