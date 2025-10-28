//=============================================================================
//
// �����蔻�菈�� [collisionUtils.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "collisionUtils.h"

// ���O��Ԃ̎g�p
using namespace std;

//=============================================================================
// �V�����_�[��AABB�̓����蔻��(���̋�����Ԃ�)
//=============================================================================
bool CCollision::CheckCylinderAABBCollisionWithHitDistance(
	const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight, const D3DXVECTOR3& cylinderDir,
	const D3DXVECTOR3& aabbMin, const D3DXVECTOR3& aabbMax,
	float* outHitDistance)
{
	// �V�����_�[�������̃x�N�g��
	D3DXVECTOR3 axis = cylinderDir;

	// AABB��8���_���擾
	D3DXVECTOR3 verts[8] =
	{
		{aabbMin.x, aabbMin.y, aabbMin.z},
		{aabbMax.x, aabbMin.y, aabbMin.z},
		{aabbMin.x, aabbMax.y, aabbMin.z},
		{aabbMax.x, aabbMax.y, aabbMin.z},
		{aabbMin.x, aabbMin.y, aabbMax.z},
		{aabbMax.x, aabbMin.y, aabbMax.z},
		{aabbMin.x, aabbMax.y, aabbMax.z},
		{aabbMax.x, aabbMax.y, aabbMax.z},
	};

	// ���_�Q���������Ɏˉe���čŏ��ő�����߂�i�������͈̔́j
	float minProj = FLT_MAX;
	float maxProj = -FLT_MAX;
	for (int i = 0; i < 8; i++)
	{
		D3DXVECTOR3 v = verts[i] - cylinderCenter;
		float proj = D3DXVec3Dot(&v, &axis);
		if (proj < minProj) minProj = proj;
		if (proj > maxProj) maxProj = proj;
	}

	// �������̋�������
	float halfHeight = cylinderHeight * 0.5f;

	if (maxProj < -halfHeight || minProj > halfHeight)
	{
		// �������ŏd�Ȃ�Ȃ�
		return false;
	}

	// �������ɔ���Ă��镔���̋�����clamp���Ĕ��莲��̓_���擾
	float clampProj = std::max(-halfHeight, std::min(0.5f * (minProj + maxProj), halfHeight));
	D3DXVECTOR3 closestPointOnAxis = cylinderCenter + axis * clampProj;

	// ���ɐ�����2�̃x�N�g�����v�Z�i�V�����_�[���ɐ����j
	D3DXVECTOR3 up = axis;
	D3DXVECTOR3 right, forward;

	// axis�ɒ�������x�N�g����1���߂�
	if (fabs(up.x) < 0.9f)
	{
		right = D3DXVECTOR3(1, 0, 0);
	}
	else
	{
		right = D3DXVECTOR3(0, 1, 0);
	}

	D3DXVec3Cross(&right, &right, &up);
	D3DXVec3Normalize(&right, &right);

	D3DXVec3Cross(&forward, &up, &right);
	D3DXVec3Normalize(&forward, &forward);

	// AABB�̒��_��f�ʍ��W�n�ɕϊ����āA�ŏ��ő�����߂�i2D�����`�̋��E�j
	float minX = FLT_MAX, maxX = -FLT_MAX;
	float minY = FLT_MAX, maxY = -FLT_MAX;
	for (int i = 0; i < 8; i++)
	{
		D3DXVECTOR3 v = verts[i] - closestPointOnAxis;
		float x = D3DXVec3Dot(&v, &right);
		float y = D3DXVec3Dot(&v, &forward);
		if (x < minX) minX = x;
		if (x > maxX) maxX = x;
		if (y < minY) minY = y;
		if (y > maxY) maxY = y;
	}

	// ��`�̍ŋߐړ_���~�̒��S�ɓ��e�i�N�����v�j
	float closestX = std::max(minX, std::min(0.0f, maxX));
	float closestY = std::max(minY, std::min(0.0f, maxY));

	// ���S����̋���
	float distX = closestX;
	float distY = closestY;

	float distSq = distX * distX + distY * distY;

	if (distSq <= cylinderRadius * cylinderRadius)
	{
		if (outHitDistance)
		{
			// ���ˈʒu���狁�߂�
			D3DXVECTOR3 startPos = cylinderCenter + axis * halfHeight;	// ��[
			D3DXVECTOR3 hitPos = cylinderCenter + axis * clampProj;		// ���������ʒu
			D3DXVECTOR3 disPos = hitPos - startPos;
			*outHitDistance = D3DXVec3Length(&disPos);
		}

		// �����蔻�肠��
		return true;
	}

	return false;
}
//=============================================================================
// �V�����_�[�ƃJ�v�Z���̓����蔻��
//=============================================================================
bool CCollision::CheckCapsuleCylinderCollision_Dir(
	const D3DXVECTOR3& capsuleCenter, float capsuleRadius, float capsuleHeight,
	const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight,
	const D3DXVECTOR3& cylinderDir, bool flag)
{

	float collisionCylinderRadius;
	float collisionCapsuleRadius;

	// flag�͎g��Ȃ����false�ł���
	if (flag)
	{
		// ==== ���a���k�� ====
		collisionCylinderRadius = cylinderRadius * 0.40f; // �~����50%
		collisionCapsuleRadius = capsuleRadius * 0.85f;   // �J�v�Z����90%
	}
	else
	{
		collisionCylinderRadius = cylinderRadius;
		collisionCapsuleRadius = capsuleRadius;
	}

	// �x�N�g����
	D3DXVECTOR3 delta = capsuleCenter - cylinderCenter;

	// �������Ɏˉe
	float proj = D3DXVec3Dot(&delta, &cylinderDir);

	// ���㋗������
	float halfHeightSum = (capsuleHeight * 0.5f) + (cylinderHeight * 0.5f);
	float axisDist = 0.0f;

	if (proj > halfHeightSum)
	{
		axisDist = proj - halfHeightSum;
	}
	else if (proj < -halfHeightSum)
	{
		axisDist = -halfHeightSum - proj;
	}
	else
	{
		axisDist = 0.0f; // ����͏d�Ȃ��Ă���
	}

	// ������ʂ̋���
	D3DXVECTOR3 closestPointOnAxis = cylinderCenter + cylinderDir * proj;
	D3DXVECTOR3 diff = capsuleCenter - closestPointOnAxis;
	float radialDist = D3DXVec3Length(&diff);

	// ==== ���� ====
	if (axisDist <= collisionCapsuleRadius + collisionCylinderRadius &&
		radialDist <= collisionCapsuleRadius + collisionCylinderRadius)
	{
		return true;
	}

	return false;
}
//=============================================================================
// AABB���m�̓����蔻��
//=============================================================================
bool CCollision::CheckCollisionAABB(D3DXVECTOR3 pos1, D3DXVECTOR3 modelSize1, D3DXVECTOR3 scale1, 
	D3DXVECTOR3 pos2, D3DXVECTOR3 modelSize2, D3DXVECTOR3 scale2)
{
	//=========================================================================
	// �u���b�N1�̏��
	//=========================================================================

	// AABB���擾
	D3DXVECTOR3 Pos_1 = pos1;				// �u���b�N�̈ʒu
	D3DXVECTOR3 ModelSize_1 = modelSize1;	// �u���b�N�̌��̃T�C�Y�i���S���_�j
	D3DXVECTOR3 Scale_1 = scale1;			// �u���b�N�̊g�嗦

	// ���T�C�Y�Ɋg�嗦��K�p����
	D3DXVECTOR3 Size_1;
	Size_1.x = ModelSize_1.x * Scale_1.x;
	Size_1.y = ModelSize_1.y * Scale_1.y;
	Size_1.z = ModelSize_1.z * Scale_1.z;

	// �ŏ��l�ƍő�l�����߂�
	D3DXVECTOR3 Min_1 = Pos_1 - Size_1 * 0.5f;
	D3DXVECTOR3 Max_1 = Pos_1 + Size_1 * 0.5f;

	//=========================================================================
	// �u���b�N2�̏��
	//=========================================================================

	// AABB���擾
	D3DXVECTOR3 Pos_2 = pos2;				// �u���b�N�̈ʒu
	D3DXVECTOR3 ModelSize_2 = modelSize2;	// �u���b�N�̌��̃T�C�Y�i���S���_�j
	D3DXVECTOR3 Scale_2 = scale2;			// �u���b�N�̊g�嗦

	// ���T�C�Y�Ɋg�嗦��K�p����
	D3DXVECTOR3 Size_2;
	Size_2.x = ModelSize_2.x * Scale_2.x;
	Size_2.y = ModelSize_2.y * Scale_2.y;
	Size_2.z = ModelSize_2.z * Scale_2.z;

	// �ŏ��l�ƍő�l�����߂�
	D3DXVECTOR3 Min_2 = Pos_2 - Size_2 * 0.5f;
	D3DXVECTOR3 Max_2 = Pos_2 + Size_2 * 0.5f;

	//=========================================================================
	// AABB���m�̌����`�F�b�N
	//=========================================================================
	bool isOverlap =
		Min_1.x <= Max_2.x && Max_1.x >= Min_2.x &&
		Min_1.y <= Max_2.y && Max_1.y >= Min_2.y &&
		Min_1.z <= Max_2.z && Max_1.z >= Min_2.z;

	if (isOverlap)
	{// �������Ă���
		return true;
	}

	return false;
}
//=============================================================================
// AABB�ƃJ�v�Z���̓����蔻��(�J�v�Z����AABB�ϊ�)
//=============================================================================
bool CCollision::CheckCapsuleAABBCollision(D3DXVECTOR3 pos, D3DXVECTOR3 modelSize, D3DXVECTOR3 scale, 
	D3DXVECTOR3 capsulePos, float Radius, float Height)
{
	//=========================================================================
	// �u���b�N1�̏��
	//=========================================================================

	// AABB ���擾
	D3DXVECTOR3 Pos = pos;				// �u���b�N�̈ʒu
	D3DXVECTOR3 ModelSize = modelSize;	// �u���b�N�̌��̃T�C�Y�i���S���_�j
	D3DXVECTOR3 Scale = scale;			// �u���b�N�̊g�嗦

	// ���T�C�Y�Ɋg�嗦��K�p����
	D3DXVECTOR3 Size;
	Size.x = modelSize.x * scale.x;
	Size.y = modelSize.y * scale.y;
	Size.z = modelSize.z * scale.z;

	D3DXVECTOR3 Min = Pos - Size * 0.5f;
	D3DXVECTOR3 Max = Pos + Size * 0.5f;

	//=========================================================================
	// �J�v�Z���R���C�_�[�̏��
	//=========================================================================

	// �J�v�Z���R���C�_�[�̃T�C�Y����AABB�T�C�Y���v�Z
	D3DXVECTOR3 pSize;
	pSize.x = Radius * 2.0f;
	pSize.z = Radius * 2.0f;
	pSize.y = Height + Radius * 2.0f;

	// �ŏ��l�ƍő�l�����߂�
	D3DXVECTOR3 pMin = capsulePos - pSize * 0.5f;
	D3DXVECTOR3 pMax = capsulePos + pSize * 0.5f;

	//=========================================================================
	// �����`�F�b�N
	//=========================================================================
	bool isOverlap =
		Min.x <= pMax.x && Max.x >= pMin.x &&
		Min.y <= pMax.y && Max.y >= pMin.y &&
		Min.z <= pMax.z && Max.z >= pMin.z;

	if (isOverlap)
	{// �������Ă���
		return true;
	}

	return false;
}
//=============================================================================
// OBB���m�̓����蔻��
//=============================================================================
bool CCollision::CheckCollisionOBB(const CBoxCollider* obb1, const CBoxCollider* obb2)
{
	// �eOBB�̉�]�s��
	const D3DXMATRIX& R1 = obb1->GetRotation();
	const D3DXMATRIX& R2 = obb2->GetRotation();

	D3DXVECTOR3 Axis1[3] =
	{
		{R1._11,R1._12,R1._13},// X��
		{R1._21,R1._22,R1._23},// Y��
		{R1._31,R1._32,R1._33},// Z��
	};

	D3DXVECTOR3 Axis2[3] =
	{
		{R2._11,R2._12,R2._13},// X��
		{R2._21,R2._22,R2._23},// Y��
		{R2._31,R2._32,R2._33},// Z��
	};

	// 15�̎��Ń`�F�b�N (3+3+9)
	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		if (!OverlapOnAxis(obb1, obb2, Axis1[nCnt])) return false;
		if (!OverlapOnAxis(obb1, obb2, Axis2[nCnt])) return false;
	}

	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		for (int nCnt2 = 0; nCnt2 < 3; nCnt2++)
		{
			D3DXVECTOR3 axis;
			D3DXVec3Cross(&axis, &Axis1[nCnt], &Axis2[nCnt2]);
			if (D3DXVec3LengthSq(&axis) > 1e-6f) // �قڃ[���x�N�g���͏��O
			{
				if (!OverlapOnAxis(obb1, obb2, axis))
				{
					return false;
				}
			}
		}
	}

	return true; // �S�ďd�Ȃ��Ă���
}
//=============================================================================
// OBB�ƃJ�v�Z���̓����蔻��
//=============================================================================
bool CCollision::CheckCapsuleOBBCollision(const CCapsuleCollider* capsule, const CBoxCollider* obb)
{
	// �J�v�Z�����S���̒[�_�i���[���h���W�j
	D3DXVECTOR3 capCenter = capsule->GetPosition();
	float h = capsule->GetHeight() * 0.5f;

	D3DXVECTOR3 capA = capCenter + D3DXVECTOR3(0, h, 0);
	D3DXVECTOR3 capB = capCenter - D3DXVECTOR3(0, h, 0);

	// OBB�̃��[�J����Ԃ֕ϊ�
	D3DXMATRIX invRot;
	D3DXMatrixTranspose(&invRot, &obb->GetRotation()); // ��]�s��̋t = �]�u
	D3DXVECTOR3 obbCenter = obb->GetPosition();
	D3DXVECTOR3 halfSize = obb->GetSize() * 0.5f;

	D3DXVECTOR3 localA = capA - obbCenter;
	D3DXVECTOR3 localB = capB - obbCenter;
	D3DXVec3TransformNormal(&localA, &localA, &invRot);
	D3DXVec3TransformNormal(&localB, &localB, &invRot);

	// �J�v�Z��������AABB�̍ŋߐړ_�����߂�
	D3DXVECTOR3 closest = ClosestPointSegmentAABB(localA, localB, -halfSize, halfSize);

	// �ŋߐړ_�ƃJ�v�Z�������̋���
	float distSq = DistanceSqPointSegment(closest, localA, localB);

	return distSq <= (capsule->GetRadius() * capsule->GetRadius());
}
//=============================================================================
// �����ƃJ�v�Z���̓����蔻��
//=============================================================================
bool CCollision::IntersectSegmentCapsule(
	const D3DXVECTOR3& segA,
	const D3DXVECTOR3& segB,
	const D3DXVECTOR3& capsuleBottom,
	const D3DXVECTOR3& capsuleTop,
	float radius)
{
	// �ŒZ������2�_���v�Z
	D3DXVECTOR3 p1, p2;
	float distSqr = DistanceSqSegmentSegment(segA, segB, capsuleBottom, capsuleTop, &p1, &p2);

	return distSqr <= radius * radius;
}

//=============================================================================
// �_�Ɛ����̍ŋߐڋ�����Ԃ��֐�
//=============================================================================
float CCollision::DistanceSqPointSegment(const D3DXVECTOR3& p, const D3DXVECTOR3& a, const D3DXVECTOR3& b)
{
	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ap = p - a;
	float t = D3DXVec3Dot(&ap, &ab) / D3DXVec3Dot(&ab, &ab);
	t = max(0.0f, min(1.0f, t));
	D3DXVECTOR3 closest = a + ab * t;
	D3DXVECTOR3 dis = p - closest;
	return D3DXVec3LengthSq(&dis);
}
//=============================================================================
// ������AABB�̍ŋߐړ_
//=============================================================================
D3DXVECTOR3 CCollision::ClosestPointSegmentAABB(const D3DXVECTOR3& a, const D3DXVECTOR3& b,
	const D3DXVECTOR3& minVal, const D3DXVECTOR3& maxVal)
{
	// �����̒��_������āAAABB�ɃN�����v����(�Ȉ�)
	D3DXVECTOR3 mid = (a + b) * 0.5f;
	D3DXVECTOR3 result = mid;
	result.x = max(minVal.x, min(result.x, maxVal.x));
	result.y = max(minVal.y, min(result.y, maxVal.y));
	result.z = max(minVal.z, min(result.z, maxVal.z));
	return result;
}
//=============================================================================
// OBB �� axis �ɓ��e���� min/max ��Ԃ��֐�
//=============================================================================
void CCollision::ProjectOBB(const CBoxCollider* obb, const D3DXVECTOR3& axis,
	float& outMin, float& outMax)
{
	// ���K��
	D3DXVECTOR3 nAxis;
	D3DXVec3Normalize(&nAxis, &axis);

	// OBB���
	D3DXVECTOR3 center = obb->GetPosition();
	D3DXVECTOR3 half = obb->GetSize() * 0.5f;
	const D3DXMATRIX& R = obb->GetRotation();

	// ���[�J���������o��
	D3DXVECTOR3 obbAxis[3] =
	{
		{ R._11, R._12, R._13 },// X��
		{ R._21, R._22, R._23 },// Y��
		{ R._31, R._32, R._33 }	// Z��
	};

	// ���S�𓊉e
	float centerProj = D3DXVec3Dot(&center, &nAxis);

	// �e�������̊�^ = (���a * cos��)
	float r = 0.0f;
	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		float e = D3DXVec3Dot(&obbAxis[nCnt], &nAxis);
		r += fabsf(e) * half[nCnt]; // half[i] = (x, y, z)
	}

	outMin = centerProj - r;
	outMax = centerProj + r;
}

//=============================================================================
// 2��OBB���w�莲��ŏd�Ȃ邩���肷��֐�
//=============================================================================
bool CCollision::OverlapOnAxis(const CBoxCollider* obb1, const CBoxCollider* obb2,
	const D3DXVECTOR3& axis)
{
	float min1, max1;
	float min2, max2;

	ProjectOBB(obb1, axis, min1, max1);
	ProjectOBB(obb2, axis, min2, max2);

	return !(max1 < min2 || max2 < min1);
}
//=============================================================================
// 2�̐��� (p1-q1, p2-q2) �Ԃ̍ŒZ�����̓����v�Z���鏈��
// outClosestPoint1, outClosestPoint2 �͍ŋߐړ_��Ԃ�
//=============================================================================
float CCollision::DistanceSqSegmentSegment(
	const D3DXVECTOR3& p1, const D3DXVECTOR3& q1,// ����1�̒[�_
	const D3DXVECTOR3& p2, const D3DXVECTOR3& q2,// ����2�̒[�_
	D3DXVECTOR3* outClosestPoint1,
	D3DXVECTOR3* outClosestPoint2)
{
	D3DXVECTOR3 d1 = q1 - p1;	// ����1�̃x�N�g��
	D3DXVECTOR3 d2 = q2 - p2;	// ����2�̃x�N�g��
	D3DXVECTOR3 r = p1 - p2;	// ����1�̎n�_�Ɛ���2�̎n�_�̍�

	float a = D3DXVec3Dot(&d1, &d1);	// ����1�̒�����2��
	float e = D3DXVec3Dot(&d2, &d2);	// ����2�̒�����2��
	float f = D3DXVec3Dot(&d2, &r);		// ����2�Ɛ���1�n�_�̍��̓���

	float s, t;

	const float EPS = 1e-6f;// �������l�i�������قړ_�̏ꍇ�̔���p�j

	// �����̐������قړ_�̏ꍇ
	if (a <= EPS && e <= EPS)
	{
		// �����̐������_
		s = t = 0.0f;
		if (outClosestPoint1)
		{
			*outClosestPoint1 = p1;
		}
		if (outClosestPoint2)
		{
			*outClosestPoint2 = p2;
		}

		D3DXVECTOR3 distance = p1 - p2;
		return D3DXVec3LengthSq(&distance);// ����2�_�Ԃ̋�����2���Ԃ�
	}

	// ����1���قړ_�̏ꍇ
	if (a <= EPS)
	{
		s = 0.0f;
		t = f / e;// ����2��Ő���1�_�ɍł��߂��_�̃p�����[�^
		t = max(0.0f, min(1.0f, t));// �����͈͂ɃN�����v
	}
	else
	{
		float c = D3DXVec3Dot(&d1, &r);// ����1�Ɛ���1�n�_������2���̓���

		// ����2���قړ_�̏ꍇ
		if (e <= EPS)
		{
			t = 0.0f;
			s = -c / a;// ����1��Ő���2�_�ɍł��߂��_�̃p�����[�^
			s = max(0.0f, min(1.0f, s));// �����͈͂ɃN�����v
		}
		else
		{
			// ��������
			float b = D3DXVec3Dot(&d1, &d2);// �����Ԃ̕����x�N�g���̓���
			float denom = a * e - b * b;

			if (denom != 0.0f)
			{
				s = (b * f - c * e) / denom;
			}
			else
			{
				s = 0.0f; // ���s�ȏꍇ��0�Œ�
			}

			s = max(0.0f, min(1.0f, s));// �����͈͂ɃN�����v

			t = (b * s + f) / e;

			// ����2�͈̔͊O�Ȃ�Ē���
			if (t < 0.0f)
			{
				t = 0.0f;
				s = max(0.0f, min(1.0f, -c / a));
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = max(0.0f, min(1.0f, (b - c) / a));
			}
		}
	}

	// �ŋߐړ_���v�Z
	D3DXVECTOR3 c1 = p1 + d1 * s;
	D3DXVECTOR3 c2 = p2 + d2 * t;

	if (outClosestPoint1)
	{
		*outClosestPoint1 = c1;// ����1�̍ŋߐړ_
	}
	if (outClosestPoint2)
	{
		*outClosestPoint2 = c2;// ����2�̍ŋߐړ_
	}

	// �ŋߐړ_�Ԃ̋�����2���Ԃ�
	D3DXVECTOR3 diff = c1 - c2;
	return D3DXVec3LengthSq(&diff);
}