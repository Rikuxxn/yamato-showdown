//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SOUND_H_// ���̃}�N����`������Ă��Ȃ�������
#define _SOUND_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*************************************************************************
// �T�E���h�N���X
//*************************************************************************
class CSound
{
public:
	CSound();
	~CSound();

	//*************************************************************************
	// �T�E���h�ꗗ
	//*************************************************************************
	typedef enum
	{
		SOUND_LABEL_GAMEBGM = 0,	// �Q�[��BGM
		SOUND_LABEL_WATER,
		SOUND_LABEL_WATERRISE,
		SOUND_LABEL_HIT,
		SOUND_LABEL_FIRE,
		SOUND_LABEL_ROCKHIT,
		SOUND_LABEL_SWITCH,
		SOUND_LABEL_PAUSE,
		SOUND_LABEL_SELECT,
		SOUND_LABEL_ENTER,
		SOUND_LABEL_ROLL,
		SOUND_LABEL_MASK,
		SOUND_LABEL_INSPIRATION,
		SOUND_LABEL_TIMER,
		SOUND_LABEL_TREASURE,
		SOUND_LABEL_SWING,
		SOUND_LABEL_GET,
		SOUND_LABEL_RANK,
		SOUND_LABEL_GAMEBGM2,
		SOUND_LABEL_SHINE,
		SOUND_LABEL_DOOR,
		SOUND_LABEL_WATERWHEEL,
		SOUND_LABEL_WATERFLOW,
		SOUND_LABEL_FLOATING,
		SOUND_LABEL_RELEASE,
		SOUND_LABEL_MAX,
	} SOUND_LABEL;

	HRESULT Init(HWND hWnd);
	void Uninit(void);

	void PauseAll(void);
	void ResumeAll(void);

	void Stop(int instanceId);			// �C���f�b�N�X�w��ł̒�~(3D�T�E���h)
	void StopByLabel(SOUND_LABEL label);// ���x���w��ł̒�~(2D�T�E���h)
	void Stop(void);
	int Play(SOUND_LABEL label);
	int Play3D(SOUND_LABEL label,D3DXVECTOR3 soundPos,float minDistance,float maxDistance);

	void UpdateListener(D3DXVECTOR3 pos);
	void UpdateSoundPosition(int instanceId, D3DXVECTOR3 pos);

private:
	// �ő哯���Đ���
	static constexpr int MAX_SIMULTANEOUS_PLAY = 2;

	// ��̃T�E���h�Đ��C���X�^���X
	struct SoundInstance
	{
		int id = -1;
		IXAudio2SourceVoice* pSourceVoice = nullptr;
		X3DAUDIO_EMITTER emitter = {};
		SOUND_LABEL label;
		bool active = false;
		float minDistance = 0.0f, maxDistance = 0.0f;						// �ŏ������ƍő勗���i���������͈̔́j
	};

	struct SoundData
	{
		BYTE* pAudioData = nullptr;
		DWORD audioBytes = 0;
		WAVEFORMATEXTENSIBLE wfx = {};
	};

	struct SOUNDINFO
	{
		const char* pFilename;
		int loopCount;
	};

private:
	IXAudio2* m_pXAudio2;									// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice* m_pMasteringVoice;				// �}�X�^�[�{�C�X

	// �T�E���h�̏��
	SOUNDINFO m_aSoundInfo[SOUND_LABEL_MAX] =
	{
		{"data/BGM/gameBGM.wav", -1},			// �Q�[��BGM(�X�e�[�W1)
		{"data/SE/water.wav", 0},				// ����SE
		{"data/SE/waterrise.wav", 0},			// ���ʏ㏸SE
		{"data/SE/hit.wav", 0},					// �v���C���[�q�b�gSE
		{"data/SE/fire.wav", -1},				// ��SE(���[�v)
		{"data/SE/rock_hit.wav", 0},			// ��Փ�SE
		{"data/SE/switch.wav", 0},				// �X�C�b�`SE
		{"data/SE/menu.wav", 0},				// �|�[�YSE
		{"data/SE/select.wav", 0},				// �I��SE
		{"data/SE/enter.wav", 0},				// ����SE
		{"data/SE/roll.wav", -1},				// ��]����SE(���[�v)
		{"data/SE/mask.wav", -1},				// ����SE(���[�v)
		{"data/SE/Inspiration.wav", 0},			// �Ђ�߂�SE
		{"data/SE/timer.wav", 0},				// �^�C�}�[SE
		{"data/SE/treasure.wav", -1},			// ���SE(���[�v)
		{"data/SE/swing.wav", 0},				// ��SE
		{"data/SE/get.wav", 0},					// ����SE
		{"data/SE/resultrank.wav", 0},			// ���U���g�����N�\��SE
		{"data/BGM/gameBGM2.wav", -1},			// �Q�[��BGM2(�X�e�[�W2)
		{"data/SE/shines.wav", 0},				// �h�A���J���Ƃ��̑��SE
		{"data/SE/door.wav", 0},				// �h�ASE
		{"data/SE/waterWheel.wav", -1},			// ����SE(���[�v)
		{"data/SE/waterrise2.wav", -1},			// ����SE(���[�v)
		{"data/SE/magic_floating.wav", -1},		// ���VSE(���[�v)
		{"data/SE/magic_release.wav", 0},		// ����SE
	};

	SoundData m_SoundData[SOUND_LABEL_MAX];

	// �C���X�^���X�Ǘ�
	std::vector<SoundInstance> m_Instances;
	int m_nextInstanceId;

	X3DAUDIO_HANDLE m_X3DInstance;							// X3DAudio �C���X�^���X
	X3DAUDIO_LISTENER m_Listener;							// ���X�i�[�i�v���C���[�̈ʒu�j

private:
	HRESULT LoadWave(SOUND_LABEL label);
	void CalculateCustomPanning(SoundInstance& inst, FLOAT32* matrix);
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

};

#endif