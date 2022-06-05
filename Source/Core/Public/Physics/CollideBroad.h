#pragma once

#include "SizedTypes.h"

#include <stack>

template <typename RigidBody>
struct PotentialContact
{
	RigidBody* m_body[2];
};

template <typename BoundingVolume, typename RigidBody>
class BVHNode
{
public:
	bool IsLeaf() const
	{
		return m_body != nullptr;
	}

	bool Overlaps( const BVHNode<BoundingVolume, RigidBody>* other ) const;
	uint32 GetPotentialContacts( PotentialContact<RigidBody>* contacts, uint32 limit ) const;
	uint32 GetPotentialContactsWith( const BVHNode<BoundingVolume, RigidBody>* other, PotentialContact<RigidBody>* contacts, uint32 limit ) const;
	void Insert( RigidBody* body, const BoundingVolume& volume );

	BVHNode() = default;
	BVHNode( BVHNode<BoundingVolume, RigidBody>* parent, const BoundingVolume& volume, RigidBody* body );
	~BVHNode();
	BVHNode( const BVHNode& ) = default;
	BVHNode& operator=( const BVHNode& ) = default;
	BVHNode( BVHNode&& ) = default;
	BVHNode& operator=( BVHNode&& ) = default;

	BVHNode* m_parent = nullptr;
	BVHNode* m_children[2] = { nullptr, nullptr };
	BoundingVolume m_boundingVolume;
	RigidBody* m_body = nullptr;

private:
	void RecalculateBoundingVolume();
};

template <typename BoundingVolume, typename RigidBody>
bool BVHNode<BoundingVolume, RigidBody>::Overlaps( const BVHNode<BoundingVolume, RigidBody>* other ) const
{
	return ( m_boundingVolume.Overlapped( other->m_boundingVolume ) > COLLISION::OUTSIDE );
}

template <typename BoundingVolume, typename RigidBody>
uint32 BVHNode<BoundingVolume, RigidBody>::GetPotentialContacts( PotentialContact<RigidBody>* contacts, uint32 limit ) const
{
	if ( IsLeaf() || limit == 0 )
	{
		return 0;
	}

	return m_children[0]->GetPotentialContactsWith( m_children[1], contacts, limit );
}

template <typename BoundingVolume, typename RigidBody>
uint32 BVHNode<BoundingVolume, RigidBody>::GetPotentialContactsWith( const BVHNode<BoundingVolume, RigidBody>* other, PotentialContact<RigidBody>* contacts, uint32 limit ) const
{
	if ( Overlaps( other ) == false || limit == 0 )
	{
		return 0;
	}

	if ( IsLeaf() && other->IsLeaf() )
	{
		contacts->m_body[0] = m_body;
		contacts->m_body[1] = other->m_body;
		return 1;
	}

	if ( other->IsLeaf() || ( ( IsLeaf() == false ) && m_boundingVolume.Volume() >= other->m_boundingVolume.Volume() ) )
	{
		uint32 count = m_children[0]->GetPotentialContactsWith( m_children[1], contacts, limit );

		if ( limit > count )
		{
			count += m_children[0]->GetPotentialContactsWith( other, contacts + count, limit - count );
		}

		if ( limit > count )
		{
			count += m_children[1]->GetPotentialContactsWith( other, contacts + count, limit - count );
		}

		return count;
	}
	else
	{
		uint32 count = other->m_children[0]->GetPotentialContactsWith( other->m_children[1], contacts, limit );

		if ( limit > count )
		{
			count += GetPotentialContactsWith( other->m_children[0], contacts + count, limit - count );
		}

		if ( limit > count )
		{
			count += GetPotentialContactsWith( other->m_children[1], contacts + count, limit - count );
		}

		return count;
	}
}

template<typename BoundingVolume, typename RigidBody>
void BVHNode<BoundingVolume, RigidBody>::Insert( RigidBody* body, const BoundingVolume& volume )
{
	if ( IsLeaf() )
	{
		m_children[0] = new BVHNode<BoundingVolume, RigidBody>( this, m_boundingVolume, m_body );

		m_children[1] = new BVHNode<BoundingVolume, RigidBody>( this, volume, body );

		this->m_body = nullptr;

		RecalculateBoundingVolume();
	}
	else
	{
		float growth1 = CalcGrowth( m_children[0]->m_boundingVolume, volume );
		float growth2 = CalcGrowth( m_children[1]->m_boundingVolume, volume );

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

template<typename BoundingVolume, typename RigidBody>
inline BVHNode<BoundingVolume, RigidBody>::BVHNode( BVHNode<BoundingVolume, RigidBody>* parent, const BoundingVolume& volume, RigidBody* body )
	: m_parent( parent )
	, m_boundingVolume( volume )
	, m_body( body )
{}

template<typename BoundingVolume, typename RigidBody>
inline BVHNode<BoundingVolume, RigidBody>::~BVHNode()
{
	if ( m_parent )
	{
		BVHNode<BoundingVolume, RigidBody>* sibling;
		if ( m_parent->m_children[0] == this )
		{
			sibling = m_parent->m_children[1];
		}
		else
		{
			sibling = m_parent->m_children[0];
		}

		m_parent->m_boundingVolume = sibling->m_boundingVolume;
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

		m_parent->RecalculateBoundingVolume();
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

template<typename BoundingVolume, typename RigidBody>
void BVHNode<BoundingVolume, RigidBody>::RecalculateBoundingVolume()
{
	if ( IsLeaf() || m_parent == nullptr )
	{
		return;
	}

	m_boundingVolume = m_children[0]->m_boundingVolume + m_children[1]->m_boundingVolume;

	if ( m_parent )
	{
		m_parent->RecalculateBoundingVolume();
	}
}

template <typename T>
class BVHTreeIterater
{
public:
	using value_type = T;
	using reference = T&;
	using pointer = T*;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;

	using Node = typename T::NodeType;

	explicit BVHTreeIterater( Node* pNode )
	{
		m_cur = pNode;
	}
	~BVHTreeIterater() = default;
	BVHTreeIterater( const BVHTreeIterater<T>& ) = default;
	BVHTreeIterater<T>& operator=( const BVHTreeIterater<T>& ) = default;
	BVHTreeIterater( BVHTreeIterater<T>&& ) = default;
	BVHTreeIterater<T>& operator=( BVHTreeIterater<T>&& ) = default;

	BVHTreeIterater<T>& operator++( int32 )
	{
		BVHTreeIterater<T> prev = *this;
		++( *this );
		return prev;
	}

	BVHTreeIterater<T>& operator++()
	{
		if ( m_cur == nullptr )
		{
			// DoNothing
		}
		else if ( m_cur->m_children[1] )
		{
			m_cur = m_cur->m_children[1];
			while ( m_cur->m_children[0] )
			{
				m_cur = m_cur->m_children[0];
			}
		}
		else
		{
			Node* pNode = m_cur->m_parent;
			while ( pNode != nullptr && pNode->m_children[1] == m_cur )
			{
				m_cur = pNode;
				pNode = m_cur->m_parent;
			}
			m_cur = pNode;
		}

		return *this;
	}

	bool operator==( const BVHTreeIterater<T>& other ) const
	{
		return m_cur == other.m_cur;
	}

	bool operator!=( const BVHTreeIterater<T>& other ) const
	{
		return !( *this == other );
	}

	Node* operator->() const
	{
		return m_cur;
	}

	Node& operator*() const
	{
		return *m_cur;
	}

private:
	Node* m_cur = nullptr;
};

template <typename BoundingVolume, typename RigidBody>
class BVHTree
{
public:
	using NodeType = BVHNode<BoundingVolume, RigidBody>;

	void Insert( RigidBody* body, const BoundingVolume& volume );
	void Remove( RigidBody* body );
	void Clear();
	BVHNode<BoundingVolume, RigidBody>* Find( RigidBody* body );
	
	BVHTreeIterater<BVHTree<BoundingVolume, RigidBody>> begin();
	BVHTreeIterater<BVHTree<BoundingVolume, RigidBody>> end();

	size_t LeafSize() const { return m_leafSize; }

	uint32 GetPotentialContacts( PotentialContact<RigidBody>* contacts, uint32 limit ) const;

private:
	BVHNode<BoundingVolume, RigidBody> m_root;
	size_t m_leafSize = 0;
};

template<typename BoundingVolume, typename RigidBody>
void BVHTree<BoundingVolume, RigidBody>::Insert( RigidBody* body, const BoundingVolume& volume )
{
	if ( m_root.m_children[0] == nullptr )
	{
		m_root.m_children[0] = new BVHNode<BoundingVolume, RigidBody>( &m_root, volume, body );
	}
	else
	{
		m_root.m_children[0]->Insert( body, volume );
	}

	++m_leafSize;
}

template<typename BoundingVolume, typename RigidBody>
void BVHTree<BoundingVolume, RigidBody>::Remove( RigidBody* body )
{
	if ( auto found = Find( body ) )
	{
		delete found;
		--m_leafSize;
	}
}

template<typename BoundingVolume, typename RigidBody>
BVHNode<BoundingVolume, RigidBody>* BVHTree<BoundingVolume, RigidBody>::Find( RigidBody* body )
{
	if ( m_root.m_children[0] == nullptr )
	{
		return nullptr;
	}

	BVHNode<BoundingVolume, RigidBody>* found = nullptr;
	std::stack<BVHNode<BoundingVolume, RigidBody>*> visit;

	visit.push( m_root.m_children[0] );

	while ( visit.empty() == false )
	{
		BVHNode<BoundingVolume, RigidBody>* cur = visit.top();
		visit.pop();

		if ( cur->IsLeaf() && cur->m_body == body )
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

template<typename BoundingVolume, typename RigidBody>
void BVHTree<BoundingVolume, RigidBody>::Clear()
{
	for ( BVHNode<BoundingVolume, RigidBody>*& child : m_root.m_children )
	{
		if ( child != nullptr )
		{
			child->m_parent = nullptr;
			delete child;
		}
	}

	std::construct_at( &m_root );
}

template<typename BoundingVolume, typename RigidBody>
BVHTreeIterater<BVHTree<BoundingVolume, RigidBody>> BVHTree<BoundingVolume, RigidBody>::begin()
{
	BVHNode<BoundingVolume, RigidBody>* pNode = &m_root;
	while ( pNode->m_children[0] )
	{
		pNode = pNode->m_children[0];
	}

	return BVHTreeIterater<BVHTree<BoundingVolume, RigidBody>>( pNode );
}

template<typename BoundingVolume, typename RigidBody>
BVHTreeIterater<BVHTree<BoundingVolume, RigidBody>> BVHTree<BoundingVolume, RigidBody>::end()
{
	return BVHTreeIterater<BVHTree<BoundingVolume, RigidBody>>( &m_root );
}

template<typename BoundingVolume, typename RigidBody>
inline uint32 BVHTree<BoundingVolume, RigidBody>::GetPotentialContacts( PotentialContact<RigidBody>* contacts, uint32 limit ) const
{
	if ( m_root.m_children[0] == nullptr )
	{
		return 0;
	}

	return m_root.m_children[0]->GetPotentialContacts( contacts, limit );
}
