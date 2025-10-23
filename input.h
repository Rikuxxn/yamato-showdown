//=============================================================================
//
// ���͏��� [input.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _INPUT_H_// ���̃}�N����`������Ă��Ȃ�������
#define _INPUT_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define NUM_KEY_MAX (256)	// �L�[�̍ő吔


//*****************************************************************************
// ���̓N���X
//*****************************************************************************
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(HINSTANCE hInstance);
	virtual void Uninit(void);
	virtual void Update(void) = 0;
	LPDIRECTINPUTDEVICE8 GetDevice(void) { return m_pDevice; }
protected:
	static LPDIRECTINPUT8 m_pInput;		// DirectInput�I�u�W�F�N�g�ւ̃|�C���^
	LPDIRECTINPUTDEVICE8 m_pDevice;		// ���̓f�o�C�X�ւ̃|�C���^

private:
};

//*****************************************************************************
// �L�[�{�[�h�N���X
//*****************************************************************************
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetAnyKeyTrigger(void);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);
private:
	BYTE m_aKeyState[NUM_KEY_MAX];				// �L�[�{�[�h�̃v���X���
	BYTE m_aOldState[NUM_KEY_MAX];				// �L�[�{�[�h�̑O��̃v���X���
};

//*****************************************************************************
// �W���C�p�b�h�N���X
//*****************************************************************************
class CInputJoypad
{
public:
	CInputJoypad();
	~CInputJoypad();

	//�L�[�̎��
	typedef enum
	{
		JOYKEY_UP = 0,	// �\���L�[��
		JOYKEY_DOWN,	// �\���L�[��
		JOYKEY_LEFT,	// �\���L�[��
		JOYKEY_RIGHT,	// �\���L�[�E
		JOYKEY_START,	// �X�^�[�g�{�^��
		JOYKEY_BACK,	// �o�b�N�{�^��
		JOYKEY_LS,		// ���X�e�B�b�N��������
		JOYKEY_RS,		// �E�X�e�B�b�N��������
		JOYKEY_LB,		// ���{�^��
		JOYKEY_RB,		// �E�{�^��
		JOYKEY_LT,		// ���g���K�[
		JOYKEY_RT,		// �E�g���K�[
		JOYKEY_A,		// A�{�^��
		JOYKEY_B,		// B�{�^��
		JOYKEY_X,		// X�{�^��
		JOYKEY_Y,		// Y�{�^��
		JOYKEY_MAX
	}JOYKEY;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	bool GetTrigger(JOYKEY Key);
	bool GetPress(JOYKEY Key);
	bool GetTriggerL2(void);
	bool GetTriggerR2(void);
	bool GetPressL2(void);
	bool GetPressR2(void);
	bool GetAnyPress(void);
	bool GetAnyTrigger(void);
	bool GetStick(void);
	static XINPUT_STATE* GetStickAngle(void);
private:
	static XINPUT_STATE m_joyKeyState;			// �W���C�p�b�h�̃v���X���
	XINPUT_STATE m_joyKeyStateTrigger;			// �W���C�p�b�h�̃g���K�[���
	XINPUT_STATE m_joyKeyStateRelease;			// �W���C�p�b�h�̃����[�X���
	XINPUT_STATE m_aOldJoyKeyState;				// �W���C�p�b�h�̑O��̏��
	bool m_joyKeyFlag[JOYKEY_MAX];
};

//*****************************************************************************
// �}�E�X�N���X
//*****************************************************************************
class CInputMouse : public CInput
{
public:
	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int button);
	bool GetTrigger(int button);
	bool GetRelease(int button);
	bool GetMouseState(DIMOUSESTATE* mouseState);
	static int GetWheel(void);
	static void SetCursorVisibility(bool visible);
	D3DXVECTOR3 GetGroundHitPosition(void); // �n��Y=0�Ƃ̌����_���擾

private:
	static DIMOUSESTATE m_mouseState;			// �}�E�X�̏��
};

#endif