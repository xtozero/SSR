#pragma once

#include <cassert>

namespace SLinkedList
{
	template <typename T>
	inline void Init( T*& head, T* node )
	{
		head = node;
	}

	template <typename T>
	inline void InsertAfter( T* predecessor, T* successor )
	{
		assert( predecessor != nullptr );
		assert( successor != nullptr );

		successor->m_next = predecessor->m_next;
		predecessor->m_next = successor;
	}

	template <typename T>
	inline void AddToHead( T*& head, T* node )
	{
		node->m_next = head;
		head = node;
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
	inline T* Find( T* head, T*& prev, Pred pred )
	{
		while ( head )
		{
			if ( pred( head ) )
			{
				break;
			}

			prev = head;
			head = head->m_next;
		}

		return head;
	}

	template <typename T>
	inline void Remove( T*& head, T* target )
	{
		assert( target != nullptr );

		if ( head == target )
		{
			head = target->m_next;
		}
		else
		{
			T* prev = head;
			while ( prev && prev->m_next != target )
			{
				prev = prev->m_next;
			}

			if ( prev )
			{
				prev->m_next = target->m_next;
			}
		}
	}

	template <typename T, typename Pred>
	inline void ForEach( T* head, Pred pred )
	{
		while ( head )
		{
			pred( head );
			head = head->m_next;
		}
	}
}
