#include "pch.h"
#include "Object.h"
#include "Material.h"
#include <typeinfo>

Object::Object(Material* pMat, const Elite::FPoint3& pos, bool partOfMesh)
	:m_Position{ pos }
	, m_pMaterial{ pMat }
	, m_PartOfMesh{ partOfMesh }
{
}

Object::~Object()
{
	if (!m_PartOfMesh)
	{
		delete m_pMaterial;
	}
}
