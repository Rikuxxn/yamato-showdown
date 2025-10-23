//=============================================================================
//
// �T�E���h���� [sound.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "sound.h"
#include "game.h"
#include "manager.h"

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CSound::CSound()
{
	// �l�̃N���A
	m_pXAudio2			= nullptr;
	m_pMasteringVoice	= nullptr;
	m_Listener			= {};					// ���X�i�[�̈ʒu
	m_nextInstanceId	= 0;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CSound::~CSound()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CSound::Init(HWND hWnd)
{
	HRESULT hr;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	hr = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2�I�u�W�F�N�g�̍쐬�Ɏ��s�I", "�x���I", MB_ICONWARNING);
		CoUninitialize();
		return hr;
	}

	XAUDIO2_DEVICE_DETAILS deviceDetails;
	hr = m_pXAudio2->GetDeviceDetails(0, &deviceDetails);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�f�o�C�X���̎擾�Ɏ��s�I", "�x���I", MB_ICONWARNING);
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
		CoUninitialize();
		return hr;
	}

	UINT32 channels = deviceDetails.OutputFormat.Format.nChannels;
	UINT32 sampleRate = deviceDetails.OutputFormat.Format.nSamplesPerSec;

	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice, channels, sampleRate);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�}�X�^�[�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
		CoUninitialize();
		return hr;
	}

	X3DAudioInitialize(deviceDetails.OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance);

	m_Listener.Position = { 0.0f, 0.0f, 0.0f };
	m_Listener.OrientFront = { 0.0f, 0.0f, 1.0f };
	m_Listener.OrientTop = { 0.0f, 1.0f, 0.0f };
	m_Listener.Velocity = { 0.0f, 0.0f, 0.0f };

	for (int i = 0; i < SOUND_LABEL_MAX; ++i)
	{
		hr = LoadWave((SOUND_LABEL)i);
		if (FAILED(hr))
		{
			char msg[128];
			sprintf_s(msg, "�T�E���h�̓ǂݍ��݂Ɏ��s: %s", m_aSoundInfo[i].pFilename);
			MessageBox(hWnd, msg, "Error", MB_OK);
			Uninit();
			return hr;
		}
	}
	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CSound::Uninit(void)
{
	// �S�Ă�SoundInstance���~�E�j��
	for (auto& inst : m_Instances)
	{
		if (inst.pSourceVoice)
		{
			inst.pSourceVoice->Stop(0);
			inst.pSourceVoice->DestroyVoice();
			inst.pSourceVoice = nullptr;
		}
	}
	m_Instances.clear();

	// �e�T�E���h�̃I�[�f�B�I�f�[�^���
	for (int i = 0; i < SOUND_LABEL_MAX; ++i)
	{
		if (m_SoundData[i].pAudioData)
		{
			free(m_SoundData[i].pAudioData);
			m_SoundData[i].pAudioData = nullptr;
		}
	}

	// �}�X�^�[�{�C�X�̔j��
	if (m_pMasteringVoice)
	{
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = nullptr;
	}

	// XAudio2�I�u�W�F�N�g�̊J��
	if (m_pXAudio2)
	{
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}

	// COM���C�u�����̏I������
	CoUninitialize();
}
//=============================================================================
// 2D�Z�O�����g�Đ�(�Đ����Ȃ��~)
//=============================================================================
int CSound::Play(SOUND_LABEL label)
{
	if (label < 0 || label >= SOUND_LABEL_MAX)
	{
		return -1;
	}

	SoundInstance inst = {};
	inst.id = m_nextInstanceId++;
	inst.label = label;
	inst.active = true;

	HRESULT hr = m_pXAudio2->CreateSourceVoice(&inst.pSourceVoice, &(m_SoundData[label].wfx.Format));
	if (FAILED(hr))
	{
		return -1;
	}

	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = m_SoundData[label].audioBytes;
	buffer.pAudioData = m_SoundData[label].pAudioData;
	if (m_aSoundInfo[label].loopCount == XAUDIO2_LOOP_INFINITE)
	{// ���[�v�̎�
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else
	{// ���[�v����Ȃ���
		buffer.LoopCount = m_aSoundInfo[label].loopCount;
		buffer.Flags = XAUDIO2_END_OF_STREAM;
	}

	hr = inst.pSourceVoice->SubmitSourceBuffer(&buffer);
	if (FAILED(hr))
	{
		inst.pSourceVoice->DestroyVoice();
		return -1;
	}

	hr = inst.pSourceVoice->Start(0);
	if (FAILED(hr))
	{
		inst.pSourceVoice->DestroyVoice();
		return -1;
	}

	m_Instances.push_back(inst);

	return inst.id;
}
//=============================================================================
// 3D�Z�O�����g�Đ�(�Đ����Ȃ��~)
//=============================================================================
int CSound::Play3D(SOUND_LABEL label, D3DXVECTOR3 soundPos, float minDistance, float maxDistance)
{
	if (label < 0 || label >= SOUND_LABEL_MAX)
	{
		return -1;
	}

	SoundInstance inst = {};
	inst.id = m_nextInstanceId++;
	inst.label = label;
	inst.active = true;

	HRESULT hr = m_pXAudio2->CreateSourceVoice(&inst.pSourceVoice, &(m_SoundData[label].wfx.Format));
	if (FAILED(hr))
	{
		return -1;
	}

	// �G�~�b�^�[�ݒ�
	inst.emitter.Position = { soundPos.x, soundPos.y, soundPos.z };
	inst.emitter.Velocity = { 0.0f, 0.0f, 0.0f };
	inst.emitter.ChannelCount = 1;
	inst.emitter.CurveDistanceScaler = 150.0f;

	// �����͈͂�ۑ�
	inst.minDistance = minDistance;
	inst.maxDistance = maxDistance;

	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = m_SoundData[label].audioBytes;
	buffer.pAudioData = m_SoundData[label].pAudioData;

	if (m_aSoundInfo[label].loopCount == XAUDIO2_LOOP_INFINITE)
	{// ���[�v�̎�
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else
	{// ���[�v����Ȃ���
		buffer.LoopCount = m_aSoundInfo[label].loopCount;
		buffer.Flags = XAUDIO2_END_OF_STREAM;
	}

	hr = inst.pSourceVoice->SubmitSourceBuffer(&buffer);
	if (FAILED(hr))
	{
		inst.pSourceVoice->DestroyVoice();
		return -1;
	}

	// 3D�p���j���O�s����v�Z
	FLOAT32 matrix[8] = {}; // �K�؂ȃT�C�Y�i�o�̓`�����l���~���̓`�����l���j
	CalculateCustomPanning(inst, matrix);

	// �p���j���O�K�p�i�o�̓`�����l��4�A����1�j
	inst.pSourceVoice->SetOutputMatrix(NULL, 1, 4, matrix);

	hr = inst.pSourceVoice->Start(0);
	if (FAILED(hr))
	{
		inst.pSourceVoice->DestroyVoice();
		return -1;
	}

	m_Instances.push_back(inst);

	return inst.id;
}
//=============================================================================
// �ꎞ��~
//=============================================================================
void CSound::PauseAll(void)
{
	for (auto& inst : m_Instances)
	{
		if (inst.pSourceVoice)
		{
			inst.pSourceVoice->Stop(0); // �t���O0�ňꎞ��~
		}
	}
}
//=============================================================================
// �ĊJ
//=============================================================================
void CSound::ResumeAll(void)
{
	for (auto& inst : m_Instances)
	{
		if (inst.pSourceVoice)
		{
			inst.pSourceVoice->Start(0); // �t���O0�ōĊJ
		}
	}
}
//=============================================================================
// �Z�O�����g��~(�C���f�b�N�X�w��)
//=============================================================================
void CSound::Stop(int instanceId)
{
	for (auto it = m_Instances.begin(); it != m_Instances.end(); it++)
	{
		if (it->id == instanceId)
		{
			if (it->pSourceVoice)
			{
				it->pSourceVoice->Stop(0);
				it->pSourceVoice->FlushSourceBuffers();
				it->pSourceVoice->DestroyVoice();
				it->pSourceVoice = nullptr;
			}
			m_Instances.erase(it);
			return;
		}
	}
}
//=============================================================================
// �Z�O�����g��~(���x���w��)
//=============================================================================
void CSound::StopByLabel(SOUND_LABEL label)
{
	for (auto it = m_Instances.begin(); it != m_Instances.end(); )
	{
		if (it->label == label)
		{
			if (it->pSourceVoice)
			{
				it->pSourceVoice->Stop(0);
				it->pSourceVoice->FlushSourceBuffers();
				it->pSourceVoice->DestroyVoice();
			}
			it = m_Instances.erase(it);
		}
		else
		{
			it++;
		}
	}
}
//=============================================================================
// �Z�O�����g��~(�S��)
//=============================================================================
void CSound::Stop(void)
{
	for (auto& inst : m_Instances)
	{
		if (inst.pSourceVoice)
		{
			inst.pSourceVoice->Stop(0);
			inst.pSourceVoice->FlushSourceBuffers();
			inst.pSourceVoice->DestroyVoice();
		}
	}
	m_Instances.clear();  // �ꊇ�ŃN���A
}
//=============================================================================
// �J�X�^���p���j���O
//=============================================================================
void CSound::CalculateCustomPanning(SoundInstance& inst, FLOAT32* matrix)
{
	// �v���C���[�̌����i�O�����x�N�g���j
	D3DXVECTOR3 front = m_Listener.OrientFront;
	D3DXVec3Normalize(&front, &front);

	// ������x�N�g���i�ʏ�� (0, 1, 0)�j
	D3DXVECTOR3 up = m_Listener.OrientTop;
	D3DXVec3Normalize(&up, &up);

	// �E�����x�N�g�� = front �~ up�i�O�ςŋ��߂�j
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &up, &front);
	D3DXVec3Normalize(&right, &right);

	// ���X�i�[ �� �����̃x�N�g��
	D3DXVECTOR3 toEmitter =
	{
		inst.emitter.Position.x - m_Listener.Position.x,
		inst.emitter.Position.y - m_Listener.Position.y,
		inst.emitter.Position.z - m_Listener.Position.z
	};

	// �������v�Z
	float distance = D3DXVec3Length(&toEmitter);

	// ���������v�Z
	float volumeScale = 1.0f - ((distance - inst.minDistance) / (inst.maxDistance - inst.minDistance));
	volumeScale = max(0.0f, min(1.0f, volumeScale));

	// �����x�N�g���𐳋K��
	D3DXVec3Normalize(&toEmitter, &toEmitter);

	// ���E�E�O��p���j���O���q
	float panLR = D3DXVec3Dot(&right, &toEmitter); // -1 (��) �` 1 (�E)
	float panFB = D3DXVec3Dot(&front, &toEmitter); // -1 (��) �` 1 (�O)

	// �����␳
	float lrWeight = panLR * 0.5f; // -0.5 �` +0.5
	float fbWeight = panFB * 0.5f + 0.5f; // 0�i�w��j�`1�i���ʁj

	// �w��ł��Œ�������c��悤�ɕ␳
	const float minVolume = 0.3f;
	fbWeight = fbWeight * (1.0f - minVolume) + minVolume;
	float backWeight = 1.0f - fbWeight;

	// ���ʐݒ�
	float frontLeft = (0.5f - lrWeight) * fbWeight * volumeScale;
	float frontRight = (0.5f + lrWeight) * fbWeight * volumeScale;
	float rearLeft = (0.5f - lrWeight) * backWeight * volumeScale;
	float rearRight = (0.5f + lrWeight) * backWeight * volumeScale;

	// ���ʃ}�g���b�N�X�ɃZ�b�g�i4ch: FL, FR, RL, RR�j
	matrix[0] = frontLeft;
	matrix[1] = frontRight;
	matrix[2] = rearLeft;
	matrix[3] = rearRight;
}
//=============================================================================
// ���X�i�[(�J����)�̍X�V����
//=============================================================================
void CSound::UpdateListener(D3DXVECTOR3 pos)
{
	// �J�����̌����x�N�g�����v�Z
	D3DXVECTOR3 forward = CManager::GetCamera()->GetForward();

	// ���K�����đ��
	D3DXVec3Normalize(&forward, &forward);

	m_Listener.OrientFront = forward;

	D3DXVECTOR3 orienttop;
	orienttop.x = 0.0f;
	orienttop.y = 1.0f;
	orienttop.z = 0.0f;

	D3DXVec3Normalize(&orienttop, &orienttop);

	m_Listener.OrientTop = orienttop;

	m_Listener.Position = { pos.x, pos.y, pos.z };				// ���X�i�[(�J����)�̈ʒu
}
//=============================================================================
// �����̈ʒu�X�V����
//=============================================================================
void CSound::UpdateSoundPosition(int instanceId, D3DXVECTOR3 pos)
{
	for (auto& inst : m_Instances)
	{
		if (inst.id == instanceId)
		{
			// �����̌��݈ʒu��K�p
			inst.emitter.Position = { pos.x, pos.y, pos.z };

			// 3D�I�[�f�B�I�v�Z
			X3DAUDIO_DSP_SETTINGS dspSettings = {};
			FLOAT32 matrix[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			dspSettings.SrcChannelCount = 1;
			dspSettings.DstChannelCount = 4;
			dspSettings.pMatrixCoefficients = matrix;

			X3DAudioCalculate(
				m_X3DInstance,
				&m_Listener,
				&inst.emitter,
				X3DAUDIO_CALCULATE_MATRIX,
				&dspSettings
			);

			// �p���j���O�l���N���b�v
			for (int nCnt = 0; nCnt < 4; nCnt++)
			{
				matrix[nCnt] = max(0.0f, min(1.0f, matrix[nCnt]));
			}

			// �J�X�^���p���j���O�v�Z
			CalculateCustomPanning(inst, matrix);

			// �o�̓}�g���N�X���X�V
			if (inst.pSourceVoice)
			{
				inst.pSourceVoice->SetOutputMatrix(NULL, 1, 4, matrix);
			}
			break;
		}
	}
}
//=============================================================================
// �`�����N�̃`�F�b�N
//=============================================================================
HRESULT CSound::CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��擪�Ɉړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�f�[�^�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// �t�@�C���^�C�v�̓ǂݍ���
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}
//=============================================================================
// �`�����N�f�[�^�̓ǂݍ���
//=============================================================================
HRESULT CSound::ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^���w��ʒu�܂ňړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// �f�[�^�̓ǂݍ���
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
//=============================================================================
// WAVE�̓ǂݍ���
//=============================================================================
HRESULT CSound::LoadWave(SOUND_LABEL label)
{
	if (label < 0 || label >= SOUND_LABEL_MAX)
	{
		return E_FAIL;
	}

	HANDLE hFile = CreateFileA(
		m_aSoundInfo[label].pFilename,
		GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING,
		0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	DWORD dwChunkSize;
	DWORD dwChunkPosition;

	// "RIFF" �`�����N�`�F�b�N
	if (FAILED(CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition)))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	// "fmt " �`�����N
	if (FAILED(CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition)))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	WAVEFORMATEXTENSIBLE wfx = {};
	if (FAILED(ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition))) 
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	// "data" �`�����N
	if (FAILED(CheckChunk(hFile, 'atad', &dwChunkSize, &dwChunkPosition))) 
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	BYTE* pDataBuffer = new BYTE[dwChunkSize];
	if (FAILED(ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition))) 
	{
		delete[] pDataBuffer;
		CloseHandle(hFile);
		return E_FAIL;
	}

	// �\�[�X�{�C�X�쐬
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	if (FAILED(m_pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx)))
	{
		delete[] pDataBuffer;
		CloseHandle(hFile);
		return E_FAIL;
	}

	// �o�b�t�@�ݒ�
	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = dwChunkSize;
	buffer.pAudioData = pDataBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	pSourceVoice->SubmitSourceBuffer(&buffer);

	// �ێ�
	m_SoundData[label].pAudioData = pDataBuffer;	// �ǂݍ���PCM�f�[�^�̃o�b�t�@
	m_SoundData[label].audioBytes = dwChunkSize;	// �f�[�^�̃o�C�g��
	m_SoundData[label].wfx = wfx;					// �t�H�[�}�b�g���

	CloseHandle(hFile);
	return S_OK;
}
