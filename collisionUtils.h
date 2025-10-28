//=============================================================================
//
// �����蔻�菈�� [collisionUtils.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COLLISIONUTILS_H_// ���̃}�N����`������Ă��Ȃ�������
#define _COLLISIONUTILS_H_// 2�d�C���N���[�h�h�~�̃}�N����`

// �O���錾
class CBoxCollider;
class CCapsuleCollider;
class CCylinderCollider;

//*****************************************************************************
// �����蔻��N���X
//*****************************************************************************
class CCollision
{
public:

	// �V�����_�[��AABB�̓����蔻��(���̋�����Ԃ�)
	static bool CheckCylinderAABBCollisionWithHitDistance(
		const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight, const D3DXVECTOR3& cylinderDir,
		const D3DXVECTOR3& aabbMin, const D3DXVECTOR3& aabbMax,
		float* outHitDistance);

	// �V�����_�[�ƃJ�v�Z���̓����蔻��
	static bool CheckCapsuleCylinderCollision_Dir(
		const D3DXVECTOR3& capsuleCenter, float capsuleRadius, float capsuleHeight,
		const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight,
		const D3DXVECTOR3& cylinderDir, bool flag);

	// AABB���m�̓����蔻��
	static bool CheckCollisionAABB(D3DXVECTOR3 pos1, D3DXVECTOR3 modelSize1, D3DXVECTOR3 scale1,
		D3DXVECTOR3 pos2, D3DXVECTOR3 modelSize2, D3DXVECTOR3 scale2);

	// AABB�ƃJ�v�Z���̓����蔻��(�J�v�Z����AABB�ϊ�)
	static bool CheckCapsuleAABBCollision(D3DXVECTOR3 pos, D3DXVECTOR3 modelSize, D3DXVECTOR3 scale,
		D3DXVECTOR3 capsulePos, float Radius, float Height);

	// OBB���m�̓����蔻��
	static bool CheckCollisionOBB(const CBoxCollider* obb1, const CBoxCollider* obb2);

	// �J�v�Z����OBB�̓����蔻��
	static bool CheckCapsuleOBBCollision(const CCapsuleCollider* capsule, const CBoxCollider* obb);

	// �����ƃJ�v�Z���̓����蔻��
	static bool IntersectSegmentCapsule(
		const D3DXVECTOR3& segA,
		const D3DXVECTOR3& segB,
		const D3DXVECTOR3& capsuleBottom,
		const D3DXVECTOR3& capsuleTop,
		float radius);

private:
	//*************************************************************************
	// �⏕�֐�
	//*************************************************************************

	// OBB �� axis �ɓ��e���� min/max ��Ԃ��֐�
	static void ProjectOBB(const CBoxCollider* obb, const D3DXVECTOR3& axis,
		float& outMin, float& outMax);

	// 2��OBB���w�莲��ŏd�Ȃ邩���肷��֐�
	static bool OverlapOnAxis(const CBoxCollider* obb1, const CBoxCollider* obb2,
		const D3DXVECTOR3& axis);

	static float DistanceSqPointSegment(const D3DXVECTOR3& p, const D3DXVECTOR3& a, const D3DXVECTOR3& b);

	static D3DXVECTOR3 ClosestPointSegmentAABB(const D3DXVECTOR3& a, const D3DXVECTOR3& b,
		const D3DXVECTOR3& minVal, const D3DXVECTOR3& maxVal);

	// 2�̐��� (p1-q1, p2-q2) �Ԃ̍ŒZ�����̓����v�Z���鏈��
	// outClosestPoint1, outClosestPoint2 �͍ŋߐړ_��Ԃ�
	static float DistanceSqSegmentSegment(
		const D3DXVECTOR3& p1, const D3DXVECTOR3& q1,
		const D3DXVECTOR3& p2, const D3DXVECTOR3& q2,
		D3DXVECTOR3* outClosestPoint1,
		D3DXVECTOR3* outClosestPoint2);
};


//*****************************************************************************
// �R���C�_�[�N���X
//*****************************************************************************
class CCollider
{
public:
	enum TYPE
	{
		TYPE_BOX,		// �{�b�N�X�R���C�_�[
		TYPE_CAPSULE,	// �J�v�Z���R���C�_�[
		TYPE_CYLINDER,	// �V�����_�[�R���C�_�[
	};

	CCollider(TYPE type) : m_Type(type) {}
	virtual ~CCollider() {}

	TYPE GetType(void) const { return m_Type; }

	// ���[���h�ʒu���X�V����
	virtual void SetPosition(const D3DXVECTOR3& pos) { m_Position = pos; }
	virtual const D3DXVECTOR3& GetPosition(void) const { return m_Position; }

protected:
	TYPE m_Type;
	D3DXVECTOR3 m_Position;
};

//*****************************************************************************
// �{�b�N�X�R���C�_�[(OBB)�N���X
//*****************************************************************************
class CBoxCollider : public CCollider
{
public:
	CBoxCollider(const D3DXVECTOR3& size)// size : ���T�C�Y
	: CCollider(TYPE_BOX), m_Size(size) {}

	// �ʒu�Ɖ�]�̔��f
	void UpdateTransform(const D3DXVECTOR3& pos, const D3DXVECTOR3& rotRad, const D3DXVECTOR3& scale)
	{
		// ���T�C�Y = ���f�����T�C�Y �~ �X�P�[��
		m_ScaledSize.x = m_Size.x * scale.x;
		m_ScaledSize.y = m_Size.y * scale.y;
		m_ScaledSize.z = m_Size.z * scale.z;

		// �ʒu
		m_Position = pos;

		// ��]�s��
		D3DXMatrixRotationYawPitchRoll(&m_Rotation, rotRad.y, rotRad.x, rotRad.z);
	}

	const D3DXVECTOR3& GetSize(void) const { return m_Size; }
	const D3DXMATRIX& GetRotation(void) const { return m_Rotation; }

private:
	D3DXVECTOR3 m_Size;			// ���E�����E���s��
	D3DXVECTOR3 m_ScaledSize;	// �g�嗦���f��T�C�Y
	D3DXMATRIX  m_Rotation;		// OBB�p�̉�]�s��
};

//*****************************************************************************
// �J�v�Z���R���C�_�[�N���X
//*****************************************************************************
class CCapsuleCollider : public CCollider
{
public:
	CCapsuleCollider(float radius, float height)
	: CCollider(TYPE_CAPSULE), m_fRadius(radius), m_fHeight(height) {}

	float GetRadius(void) const { return m_fRadius; }
	float GetHeight(void) const { return m_fHeight; }

private:
	float m_fRadius;
	float m_fHeight;
};

//*****************************************************************************
// �V�����_�[�R���C�_�[�N���X
//*****************************************************************************
class CCylinderCollider : public CCollider
{
public:
	CCylinderCollider(float radius, float height, const D3DXVECTOR3& dir)
	: CCollider(TYPE_CYLINDER), m_fRadius(radius), m_fHeight(height), m_Dir(dir) {}

	float GetRadius(void) const { return m_fRadius; }
	float GetHeight(void) const { return m_fHeight; }
	const D3DXVECTOR3& GetDirection(void) const { return m_Dir; }

private:
	float m_fRadius;
	float m_fHeight;
	D3DXVECTOR3 m_Dir;  // ������
};

#endif