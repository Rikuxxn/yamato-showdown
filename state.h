//=============================================================================
//
// ��ԏ��� [state.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STATE_H_
#define _STATE_H_

template<typename OwnerType>

// �O���錾
class StateMachine;

//*****************************************************************************
// �S�Ă̏�Ԃ̊��ƂȂ�N���X
//*****************************************************************************
template<typename OwnerType> // �e���v���[�g�����ŏ�Ԃ��Ǘ�����Ώۂ̌^���w��
class StateBase
{
protected:
	friend class StateMachine<OwnerType>; // �X�e�[�g�}�V���ȊO����Ăяo���֐��ȂǂɃA�N�Z�X�ł��Ȃ��悤��

	// ��Ԃ��J�n�����Ƃ��Ɉ�x�����Ă΂�鏈��
	virtual void OnStart(OwnerType* /*a_pOwner*/) {}// �X�e�[�g�̎��������������擾�ł���悤��

	// ��Ԃ��X�V����Ƃ��ɌĂ΂�鏈��
	virtual void OnUpdate(OwnerType* /*a_pOwner*/) {}

	// ��Ԃ��I�������Ƃ��Ɉ�x�����Ă΂�鏈��
	virtual void OnExit(OwnerType* /*a_pOwner*/) {}

private:

	// ���̏�Ԃ��Ǘ����Ă���X�e�[�g�}�V�[�����Z�b�g
	void SetMachine(StateMachine<OwnerType>* a_pMachine)
	{
		m_pMachine = a_pMachine;
	}

	// �J�n�֐����}�V������ĂԂ��߂̊֐�
	void CallStart(OwnerType* a_pOwner) // ���z�֐����}�V�������S�ɌĂԂ��߂̊֐���ǉ�
	{
		if (m_pMachine == nullptr || a_pOwner == nullptr)
		{
			return;
		}
		OnStart(a_pOwner);
	}

	// �X�V�֐����}�V������ĂԂ��߂̊֐�
	void CallUpdate(OwnerType* a_pOwner)
	{
		if (m_pMachine == nullptr || a_pOwner == nullptr)
		{
			return;
		}
		OnUpdate(a_pOwner);
	}

	// �I���֐����}�V������ĂԂ��߂̊֐�
	void CallExit(OwnerType* a_pOwner)
	{
		if (m_pMachine == nullptr || a_pOwner == nullptr)
		{
			return;
		}
		OnExit(a_pOwner);
	}

protected:

	StateMachine<OwnerType>* m_pMachine = nullptr; // ���̃X�e�[�g���Ǘ����Ă���X�e�[�g�}�V���̃|�C���^��ۑ�

	// 
	OwnerType* m_pOwner = nullptr;
};


//*****************************************************************************
// ��Ԃ��Ǘ�����N���X
//*****************************************************************************
// �e���v���[�g�����͏�Ԃ��Ǘ�����Ώۂ̌^
template<typename OwnerType> // �e���v���[�g�����ŏ�Ԃ��Ǘ�����Ώۂ̌^���w��
class StateMachine
{
public:
	StateMachine()
		:m_fnChangeState([]() {})
	{

	}

	// �X�e�[�g�}�V���̊J�n�֐�
	void Start(OwnerType* a_pOwner) // ���������ɃI�[�i�[���Z�b�g
	{
		m_pOwner = a_pOwner;
		m_fnChangeState = []() {};
	}

	// ��Ԃ�ύX����֐�
	// �������̓X�e�[�g�̃R���X�g���N�^�ɓn���l
	// �X�e�[�g�̕ύX�������֐��|�C���^�̒��ɕ����ߍX�V���I�������ɌĂ�
	template<typename StateType, typename...ArgType>// �X�e�[�g�̌^���w�肵����֐����ŃC���X�^���X���쐬���A�R���X�g���N�^�ɒl��n����
	void ChangeState(ArgType...a_args)
	{
		// �X�e�[�g�̕ύX���߂��i�[����
		m_fnChangeState = [&]() // �X�e�[�g�̕ύX���߂��֐��|�C���^�Ɋi�[����
		{
			// �I�[�i�[���Z�b�g����ĂȂ���Ή������Ȃ�
			if (m_pOwner == nullptr)
			{
				return;
			}

			// �������łɃX�e�[�g���Z�b�g����Ă���I������
			if (m_spNowState != nullptr)
			{
				m_spNowState->CallExit(m_pOwner);
				m_spNowState = nullptr;
			}

			// �V�����X�e�[�g���쐬
			m_spNowState = std::make_shared<StateType>(a_args...);
			if (m_spNowState == nullptr)
			{
				return;
			}
			// �V�����X�e�[�g�ɂ��̃}�V�[�����Z�b�g
			m_spNowState->SetMachine(this);

			// �X�e�[�g�̊J�n
			m_spNowState->CallStart(m_pOwner);
		};
	}


	// ��Ԃ̍X�V
	void Update()
	{
		// �X�e�[�g�̕ύX���߂�����Ώ�������
		m_fnChangeState(); // �X�e�[�g���X�V����O�ɃX�e�[�g�̕ύX���߂�����Ύ��s		
		m_fnChangeState = []() {};


		// �X�e�[�g���Z�b�g����Ă���X�V����
		if (m_spNowState != nullptr)
		{
			m_spNowState->CallUpdate(m_pOwner);
		}
	}

private:

	// ��Ԃ̎�����̃|�C���^
	OwnerType* m_pOwner = nullptr;

	// ���̃X�e�[�g
	std::shared_ptr<StateBase<OwnerType>> m_spNowState = nullptr;

	// �X�e�[�g�̕ύX���߂�ۑ����Ă����֐��I�u�W�F�N�g
	std::function<void()> m_fnChangeState;

};
#endif
