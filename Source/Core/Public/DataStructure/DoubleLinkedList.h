#pragma once

#include <cassert>

namespace DLinkedList
{
	template <typename T>
	inline void Init( T*& head, T* node )
	{
		head = node;
		if ( node )
		{
			node->m_next = nullptr;
			node->m_prev = nullptr;
		}
	}

	template <typename T>
	inline void InsertAfter( T* predecessor, T* successor )
	{
		assert( predecessor != nullptr );
		assert( successor != nullptr );

		successor->m_prev = predecessor;
		successor->m_next = predecessor->m_next;
		if ( predecessor->m_next )
		{
			predecessor->m_next->m_prev = successor;
		}
		predecessor->m_next = successor;
	}

	template <typename T>
	inline void InsertBefore( T*& head, T* successor, T* predecessor )
	{
		assert( predecessor != nullptr );
		assert( successor != nullptr );

		predecessor->m_next = successor;
		predecessor->m_prev = successor->m_prev;
		if ( predecessor->m_prev )
		{
			predecessor->m_prev->m_next = predecessor;
		}
		successor->m_prev = predecessor;

		// Update head
		if ( predecessor->m_prev == nullptr )
		{
			head = predecessor;
		}
	}

	template <typename T>
	inline void AddToTail( T* head, T* node )
	{
		assert( head != nullptr );
		assert( node != nullptr );

		T* tail = head;
		while ( tail->m_next )
		{
			tail = tail->m_next;
		}

		InsertAfter( tail, node );
	}

	template <typename T, typename Pred>
	inline T* Find( T* head, Pred pred )
	{
		while ( head )
		{
			if ( pred( head ) )
			{
				break;
			}

			head = head->m_next;
		}

		return head;
	}

	template <typename T>
	inline void Remove( T*& head, T* target )
	{
		assert( target != nullptr );

		if ( target->m_prev )
		{
			target->m_prev->m_next = target->m_next;
			if ( target->m_next )
			{
				target->m_next->m_prev = target->m_prev;
			}
		}
		else
		{
			// Update head
			head = target->m_next;
			if ( head )
			{
				head->m_prev = nullptr;
			}
		}
	}

	template <typename T, typename Pred>
	inline void ForEach( T* head, Pred pred )
	{
		while ( head )
		{
			pred( head );
		}
	}
}
