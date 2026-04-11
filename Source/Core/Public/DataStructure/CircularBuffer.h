#pragma once

#include "SizedTypes.h"

template <typename T, int32 Capacity>
class CircularBuffer
{
public:
    void Enqueue( const T& element )
    {
        assert( m_numElement < Capacity );

        int32 index = ( m_baseIndex + m_numElement ) % Capacity;
        m_elements[index] = element;
        ++m_numElement;
    }

    void EnqueueOverwrite(const T& element)
    {
        if ( IsFull() )
        {
            m_baseIndex = ( m_baseIndex + 1 ) % Capacity;
            --m_numElement;
        }
        Enqueue(element);
    }

    T Dequeue()
    {
        assert( m_numElement > 0 );

        T ret = std::move( m_elements[m_baseIndex] );
        m_baseIndex = ( m_baseIndex + 1 ) % Capacity;
        --m_numElement;

        return ret;
    }

    void Clear()
    {
        m_baseIndex = 0;
        m_numElement = 0;
    }

    int32 Size() const
    {
        return m_numElement;
    }

    bool IsEmpty() const
    {
        return m_numElement == 0;
    }

    bool IsFull() const
    {
        return m_numElement == Capacity;
    }

    const T& At( int32 index ) const
    {
        return const_cast<CircularBuffer*>( this )->At( index    );
    }

    T& At( int32 index )
    {
        assert( 0 <= index && index < m_numElement );
        int32 physicalIndex = ( m_baseIndex + index ) % Capacity;
        return m_elements[physicalIndex];
    }

    template <typename Derived, bool IsConst>
    class IteratorBase
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = ptrint;
        using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
        using reference = std::conditional_t<IsConst, const value_type&, value_type&>;

        using ContainerType = std::conditional_t<IsConst, const CircularBuffer, CircularBuffer>;

        Derived& operator++()
        {
            ++m_index;
            return static_cast<Derived&>( *this );
        }

        Derived operator++(int)
        {
            Derived temp = static_cast<Derived&>( *this );
            ++( *this );
            return temp;
        }

        reference operator*() const
        {
            return m_container->At( m_index );
        }

        friend bool operator==( const IteratorBase& lhs, const IteratorBase& rhs )
        {
            return ( lhs.m_container == rhs.m_container ) && ( lhs.m_index == rhs.m_index );
        }

        friend bool operator!=( const IteratorBase& lhs, const IteratorBase& rhs )
        {
            return !( lhs == rhs );
        }

        IteratorBase( ContainerType& container, int32 index )
            : m_container( &container )
            , m_index( index )
        {}
        IteratorBase() = default;

    protected:
        ContainerType* m_container = nullptr;
        int32 m_index = 0;
    };

    class Iterator : public IteratorBase<Iterator, false>
    {
    public:
        using IteratorBase<Iterator, false>::IteratorBase;
    };

    class ConstIterator : public IteratorBase<ConstIterator, true>
    {
    public:
        using IteratorBase<ConstIterator, true>::IteratorBase;
    };

    Iterator begin() noexcept
    {
        return Iterator( *this, 0 );
    }

    ConstIterator begin() const noexcept
    {
        return ConstIterator( *this, 0 );
    }

    Iterator end() noexcept
    {
        return Iterator( *this, Size() );
    }

    ConstIterator end() const noexcept
    {
        return ConstIterator( *this, Size() );
    }

private:
    int32 m_baseIndex = 0;
    int32 m_numElement = 0;

    T m_elements[Capacity];
};