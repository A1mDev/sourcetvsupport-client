#ifndef _INCLUDE_WRAPPERS_VSP_H_
#define _INCLUDE_WRAPPERS_VSP_H_

#include "vsp_client.h"
#include "l4d2_gamedata.h"
#include "ehandle.h"

class C_BaseEntity;
typedef CHandle<C_BaseEntity> EHANDLE; // The client's version of EHANDLE.

class C_BaseEntity
{
public:
	inline EHANDLE &GetMoveParent()
	{
		return *(EHANDLE*)((byte*)(this) + OFF_CBASEENTITY_MOVEPARENT); // C_BaseEntity::m_pMoveParent
	}

	inline EHANDLE &GetNetworkMoveParent()
	{
		return *(EHANDLE*)((byte*)(this) + OFF_CBASEENTITY_NETWORK_MOVEPARENT); // C_BaseEntity::m_hNetworkMoveParent
	}

	inline bool IsServerEntity() const
	{
		return (GetIndex() != -1);
	}

	inline int GetIndex() const
	{
		// C_BaseEntity::index
		return *(int*)((byte*)(this) + OFF_CBASEENTITY_INDEX);
	}

	/*inline const Vector& GetAbsOrigin() const
	{
		const_cast<C_BaseEntity*>(this)->CalcAbsolutePosition();
		return m_vecAbsOrigin;
	}

	inline const QAngle& GetAbsAngles() const
	{
		const_cast<C_BaseEntity*>(this)->CalcAbsolutePosition();
		return m_angAbsRotation;
	}

	inline const Vector& C_BaseEntity::GetAbsVelocity() const
	{
		const_cast<C_BaseEntity*>(this)->CalcAbsoluteVelocity();
		return m_vecAbsVelocity;
	}

	void CalcAbsolutePosition()
	{
		
	}

	void CalcAbsoluteVelocity()
	{

	}

	void SetAbsOrigin(const Vector& origin)
	{

	}

	void SetAbsAngles(const QAngle& angles)
	{

	}

	void SetAbsVelocity(const Vector& vecVelocity)
	{

	}
	*/
};

#endif // _INCLUDE_WRAPPERS_VSP_H_