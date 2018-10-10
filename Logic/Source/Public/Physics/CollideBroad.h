#pragma once

#include "Body.h"

struct PotentialContact
{
	RigidBody* m_body[2];
};

template <typename BoundingVolumeClass>
class BVHNode
{
public:
	bool IsLeaf( ) const
	{
		return m_body != nullptr;
	}

	bool Overlaps( const BVHNode<BoundingVolumeClass>* other );
	unsigned int GetPotentialContacts( PotentialContact* contacts, unsigned int limit ) const;
	unsigned int GetPotentialContactsWith( const BVHNode<BoundingVolumeClass>* other, PotentialContact* contacts, unsigned int limit ) const;
	void Insert( RigidBody* body, const BoundingVolumeClass& volume );

	BVHNode( ) = default;
	BVHNode( BVHNode<BoundingSphere>* parent, const BoundingVolumeClass& volume, RigidBody* body );
	~BVHNode( );
	BVHNode( const BVHNode& ) = default;
	BVHNode& operator=( const BVHNode& ) = default;
	BVHNode( BVHNode&& ) = default;
	BVHNode& operator=( BVHNode&& ) = default;

	BVHNode* m_parent = nullptr;
	BVHNode* m_children[2] = { nullptr, nullptr };
	BoundingVolumeClass m_volume;
	RigidBody* m_body = nullptr;

private:
	void RecalculateBoundingVolume( );
};

template <typename BoundingVolumeClass>
bool BVHNode<BoundingVolumeClass>::Overlaps( const BVHNode<BoundingVolumeClass>* other )
{
	return m_volume.Overlaps( other->m_volume );
}

template <typename BoundingVolumeClass>
unsigned int BVHNode<BoundingVolumeClass>::GetPotentialContacts( PotentialContact* contacts, unsigned int limit ) const
{
	if ( IsLeaf( ) || limit == 0 )
	{
		return 0;
	}

	return m_children[0]->GetPotentialContactsWith( m_children[1], contacts, limit );
}

template <typename BoundingVolumeClass>
unsigned int BVHNode<BoundingVolumeClass>::GetPotentialContactsWith( const BVHNode<BoundingVolumeClass>* other, PotentialContact* contacts, unsigned int limit ) const
{
	if ( Overlaps( other ) == false || limit == 0 )
	{
		return 0;
	}

	if ( IsLeaf( ) && other->IsLeaf( ) )
	{
		contacts->m_body[0] = m_body;
		contacts->m_body[1] = other->m_body;
		return 1;
	}

	if ( other->IsLeaf( ) || ( ( IsLeaf( ) == false ) && m_volume->GetSize( ) >= other->m_volume->GetSize( ) ) )
	{
		unsigned int count = m_children[0]->GetPotentialContactsWith( other, contacts, limits );

		if ( limit > count )
		{
			return count + m_children[1]->GetPotentialContactsWith( other, contacts + count, limits - count );
		}
		else
		{
			return count;
		}
	}
	else
	{
		unsigned int count = GetPotentialContactsWith( other->m_children[0], contacts, limit );

		if ( limit > count )
		{
			return count + GetPotentialContactsWith( other->m_children[1], contacts + count, limits - count );
		}
		else
		{
			return count;
		}
	}
}

template<typename BoundingVolumeClass>
void BVHNode<BoundingVolumeClass>::Insert( RigidBody* body, const BoundingVolumeClass& volume )
{
	if ( IsLeaf( ) )
	{
		m_children[0] = new BVHNode<BoundingVolumeClass>( this, m_volume, m_body );

		m_children[1] = new BVHNode<BoundingVolumeClass>( this, volume, body );

		this->m_body = nullptr;

		RecalculateBoundingVolume( );
	}
	else
	{
		float growth1 = m_children[0]->m_volume.CalcGrowth( volume );
		float growth2 = m_children[1]->m_volume.CalcGrowth( volume );

		if ( growth1 == 0.f )
		{
			growth1 = FLT_MAX;
		}

		if ( growth2 == 0.f )
		{
			growth2 = FLT_MAX;
		}

		if ( growth1 < growth2 )
		{
			m_children[0]->Insert( body, volume );
		}
		else
		{
			m_children[1]->Insert( body, volume );
		}
	}
}

template<typename BoundingVolumeClass>
inline BVHNode<BoundingVolumeClass>::BVHNode( BVHNode<BoundingSphere>* parent, const BoundingVolumeClass& volume, RigidBody* body ) :
	m_parent( parent ), m_volume( volume ), m_body( body )
{
}

template<typename BoundingVolumeClass>
inline BVHNode<BoundingVolumeClass>::~BVHNode( )
{
	if ( m_parent )
	{
		BVHNode<BoundingVolumeClass>* sibling;
		if ( m_parent->m_children[0] == this )
		{
			sibling = m_parent->m_children[1];
		}
		else
		{
			sibling = m_parent->m_children[0];
		}

		m_parent->m_volume = sibling->m_volume;
		m_parent->m_body = sibling->m_body;
		m_parent->m_children[0] = sibling->m_children[0];
		m_parent->m_children[1] = sibling->m_children[1];

		if ( sibling->m_children[0] )
		{
			sibling->m_children[0]->m_parent = m_parent;
		}

		if ( sibling->m_children[1] )
		{
			sibling->m_children[1]->m_parent = m_parent;
		}

		sibling->m_parent = nullptr;
		sibling->m_body = nullptr;
		sibling->m_children[0] = nullptr;
		sibling->m_children[1] = nullptr;
		delete sibling;

		m_parent->RecalculateBoundingVolume( );
	}

	if ( m_children[0] )
	{
		m_children[0]->m_parent = nullptr;
		delete m_children[0];
	}

	if ( m_children[1] )
	{
		m_children[1]->m_parent = nullptr;
		delete m_children[1];
	}
}

template<typename BoundingVolumeClass>
void BVHNode<BoundingVolumeClass>::RecalculateBoundingVolume( )
{
	if ( IsLeaf( ) )
	{
		return;
	}

	m_volume = BoundingVolumeClass( m_children[0]->m_volume, m_children[1]->m_volume );

	if ( m_parent )
	{
		m_parent->RecalculateBoundingVolume( );
	}
}


template <typename BoundingVolumeClass>
class BVHTree
{
public:
	void Insert( RigidBody* body, const BoundingVolumeClass& volume );
	BVHNode<BoundingVolumeClass>* Find( RigidBody* body );

	~BVHTree( );

private:
	BVHNode<BoundingVolumeClass>* m_root = nullptr;
};

template<typename BoundingVolumeClass>
void BVHTree<BoundingVolumeClass>::Insert( RigidBody* body, const BoundingVolumeClass& volume )
{
	if ( m_root == nullptr )
	{
		m_root = new BVHNode<BoundingVolumeClass>( nullptr, volume, body );
	}
	else
	{
		m_root->Insert( body, volume );
	}
}

template<typename BoundingVolumeClass>
inline BVHNode<BoundingVolumeClass>* BVHTree<BoundingVolumeClass>::Find( RigidBody* body )
{
	if ( m_root == nullptr )
	{
		return nullptr;
	}

	BVHNode<BoundingVolumeClass>* found = nullptr;
	std::stack<BVHNode<BoundingVolumeClass>*> visit;

	visit.push( m_root );

	while ( visit.empty( ) == false )
	{
		BVHNode<BoundingVolumeClass>* cur = visit.top( );
		visit.pop( );
		
		if ( cur->IsLeaf( ) && cur->m_body == body )
		{
			found = cur;
			break;
		}
		else
		{
			if ( cur->m_children[0] )
			{
				visit.push( cur->m_children[0] );
			}

			if ( cur->m_children[1] )
			{
				visit.push( cur->m_children[1] );
			}
		}
	}

	return found;
}

template<typename BoundingVolumeClass>
BVHTree<BoundingVolumeClass>::~BVHTree( )
{
	delete m_root;
}
