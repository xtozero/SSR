#pragma once

#include "FnvHash.h"
#include "SizedTypes.h"

#include <cassert>
#include <memory>
#include <new>
#include <type_traits>

namespace rendercore
{
    template <int32 Start, int32 End>
    class RangeIntDimension
    {
    public:
        static constexpr int32 NumOption = End - Start + 1;

        static constexpr int32 GetValue( int32 dimensionId )
        {
            assert( Start <= Start + dimensionId && Start + dimensionId <= End );
            return Start + dimensionId;
        }

        static constexpr int32 ToDimensionId( int32 value )
        {
            int32 dimensionId = value - Start;
            assert( 0 <= dimensionId && dimensionId < NumOption );
            return dimensionId;
        }
    };

    template <int32... Values>
    class SparseIntDimension
    {
    public:
        static constexpr int32 NumOption = 0;

        static constexpr int32 GetValue( [[maybe_unused]] int32 dimensionId )
        {
            assert( false );
            return 0;
        }

        static constexpr int32 ToDimensionId( [[maybe_unused]] int32 value )
        {
            assert( false );
            return 0;
        }
    };

    template <int32 Value, int32... Values>
    class SparseIntDimension<Value, Values...>
    {
    public:
        static constexpr int32 NumOption = SparseIntDimension<Values...>::NumOption + 1;

        static constexpr int32 GetValue( int32 dimensionId )
        {
            if ( dimensionId == 0 )
            {
                return Value;
            }

            return SparseIntDimension<Values...>::GetValue( dimensionId - 1 );
        }

        static constexpr int32 ToDimensionId( int32 value )
        {
            if ( value == Value )
            {
                return 0;
            }

            return SparseIntDimension<Values...>::ToDimensionId( value ) + 1;
        }
    };

    class IShaderDefineVisitor
    {
    public:
        virtual void Visit( const char* name, const wchar_t* nameW, int32 value ) = 0;

        virtual ~IShaderDefineVisitor() = default;
    };

    class IShaderPermutation
    {
    public:
        virtual int32 GetPermutationCount() const = 0;
        virtual int32 GetDimensionCount() const = 0;

        virtual uint32 GetPermutationId() const = 0;
        virtual void SetPermutationId( int32 permutationId ) = 0;

        virtual void SetValueByName( uint32 nameHash, int32 value ) = 0;

        virtual void ForEachShaderDefine( IShaderDefineVisitor& visitor ) const = 0;

        virtual ~IShaderPermutation() = default;
    };

    template <typename... Types>
    class ShaderPermutation : public IShaderPermutation
    {
    public:
        static constexpr int32 PermutationCount = 1;

        template <typename DimensionToSet>
        void SetValue( [[maybe_unused]] int32 value )
        {
            static_assert( sizeof( DimensionToSet ) == 0, "Invalid Dimension type" );
        }

        template <typename DimensionToGet>
        int32 GetValue() const
        {
            static_assert( sizeof( DimensionToGet ) == 0, "Invalid Dimension type" );
            return {};
        }

        template <typename Lambda>
        void ForEachShaderDefine( [[maybe_unused]] Lambda&& lambda ) const
        {
        }

        virtual int32 GetPermutationCount() const override
        {
            return PermutationCount;
        }

        virtual int32 GetDimensionCount() const override
        {
            return 0;
        }

        virtual uint32 GetPermutationId() const override
        {
            return 0;
        }

        virtual void SetPermutationId( [[maybe_unused]] int32 permutationId ) override
        {
        }

        virtual void SetValueByName( [[maybe_unused]] uint32 nameHash, [[maybe_unused]] int32 value ) override
        {
        }

        virtual void ForEachShaderDefine( [[maybe_unused]] IShaderDefineVisitor& visitor ) const override
        {
        }

    protected:
        void SetPermutationIdImpl( [[maybe_unused]] int32 permutationId )
        {
        }

        void SetValueByNameImpl( [[maybe_unused]] uint32 nameHash, [[maybe_unused]] int32 value )
        {
        }
    };

    template <typename DimensionType, typename... Types>
    class ShaderPermutation<DimensionType, Types...> : public ShaderPermutation<Types...>
    {
    public:
        using Super = ShaderPermutation<Types...>;

        static constexpr int32 PermutationCount = DimensionType::NumOption * ShaderPermutation<Types
            ...>::PermutationCount;

        template <typename DimensionToSet>
        void SetValue( int32 value )
        {
            if constexpr ( std::is_same_v<DimensionToSet, DimensionType> )
            {
                m_dimensionId = DimensionToSet::ToDimensionId( value );
            }
            else
            {
                Super::template SetValue<DimensionToSet>( value );
            }
        }

        template <typename DimensionToGet>
        int32 GetValue() const
        {
            if constexpr ( std::is_same_v<DimensionToGet, DimensionType> )
            {
                return DimensionToGet::GetValue( m_dimensionId );
            }
            else
            {
                return Super::template GetValue<DimensionToGet>();
            }
        }

        template <typename Lambda> requires !std::is_base_of_v<IShaderDefineVisitor, std::remove_cvref_t<Lambda>>
        void ForEachShaderDefine( Lambda&& lambda ) const
        {
            lambda( DimensionType::Name, DimensionType::NameW, GetValue<DimensionType>() );
            Super::ForEachShaderDefine( std::forward<Lambda>( lambda ) );
        }

        virtual int32 GetPermutationCount() const override
        {
            return PermutationCount;
        }

        virtual int32 GetDimensionCount() const override
        {
            return sizeof...( Types ) + 1;
        }

        virtual uint32 GetPermutationId() const override
        {
            return GetPermutationIdImpl();
        }

        virtual void SetPermutationId( int32 permutationId ) override
        {
            SetPermutationIdImpl( permutationId );
        }

        virtual void SetValueByName( uint32 nameHash, int32 value ) override
        {
            SetValueByNameImpl( nameHash, value );
        }

        virtual void ForEachShaderDefine( IShaderDefineVisitor& visitor ) const override
        {
            ForEachShaderDefine(
                [&visitor]( const char* name, const wchar_t* nameW, int32 value )
                {
                    visitor.Visit( name, nameW, value );
                } );
        }

    protected:
        uint32 GetPermutationIdImpl() const
        {
            return DimensionType::NumOption * Super::GetPermutationId() + m_dimensionId;
        }

        void SetPermutationIdImpl( int32 permutationId )
        {
            m_dimensionId = permutationId % DimensionType::NumOption;
            Super::SetPermutationIdImpl( permutationId / DimensionType::NumOption );
        }

        void SetValueByNameImpl( uint32 nameHash, int32 value )
        {
            if ( DimensionType::NameHash == nameHash )
            {
                m_dimensionId = DimensionType::ToDimensionId( value );
            }
            else
            {
                Super::SetValueByNameImpl( nameHash, value );
            }
        }

    private:
        uint32 m_dimensionId = {};
    };

    constexpr uint32 ShaderDefineNameHash( const char* str )
    {
        return Fnv1a32( str );
    }

    #define DEFINE_DIMENSION( BaseClass, DefineName, ... ) \
        public BaseClass<__VA_ARGS__> \
        { \
        public: \
            static constexpr const char* Name = DefineName; \
            static constexpr const wchar_t* NameW = L##DefineName; \
            static constexpr const uint32 NameHash = ShaderDefineNameHash( DefineName ); \
        }

    #define DEFINE_RANGE_DIMENSION( DefineName, Start, End ) \
        DEFINE_DIMENSION( RangeIntDimension, DefineName, Start, End )

    #define DEFINE_SPARSE_DIMENSION( DefineName, ... ) \
        DEFINE_DIMENSION( SparseIntDimension, DefineName, __VA_ARGS__ )

    #define DEFINE_BOOL_DIMENSION( DefineName, ... ) \
        DEFINE_DIMENSION( RangeIntDimension, DefineName, 0, 1 )

    template <typename ShaderClass>
    concept HasShaderPermutationType = requires
    {
        typename ShaderClass::PermutationType;
    } && std::derived_from<typename ShaderClass::PermutationType, IShaderPermutation>;

    class ShaderPermutationInstance
    {
    public:
        template <typename T>
        void Assign()
        {
            static_assert( std::is_base_of_v<IShaderPermutation, T> );

            Reset();

            if ( CanInlining<T>() )
            {
                static_assert( alignof( T ) <= StackAlign );

                m_inlined = true;
                m_ptr = new ( m_stackPermutation ) T();
            }
            else
            {
                m_heapPermutation = std::make_unique<T>();
                m_ptr = m_heapPermutation.get();
            }

            m_vtable = &VTableImpl<T>;
        }

        IShaderPermutation* Get()
        {
            return reinterpret_cast<IShaderPermutation*>( m_ptr );
        }

        const IShaderPermutation* Get() const
        {
            return reinterpret_cast<const IShaderPermutation*>( m_ptr );
        }

        bool IsValid() const
        {
            return m_ptr != nullptr;
        }

        ShaderPermutationInstance() noexcept = default;
        ~ShaderPermutationInstance()
        {
            Reset();
        }

        ShaderPermutationInstance( const ShaderPermutationInstance& ) = delete;
        ShaderPermutationInstance& operator=( const ShaderPermutationInstance& ) = delete;
        ShaderPermutationInstance( ShaderPermutationInstance&& other ) noexcept
        {
            *this = std::move( other );
        }

        ShaderPermutationInstance& operator=( ShaderPermutationInstance&& other ) noexcept
        {
            Reset();

            m_vtable = other.m_vtable;
            m_inlined = other.m_inlined;

            if ( m_inlined )
            {
                m_vtable->m_move( m_stackPermutation, other.m_stackPermutation );
                m_ptr = std::launder( m_stackPermutation );
            }
            else
            {
                m_heapPermutation = std::move( other.m_heapPermutation );
                m_ptr = m_heapPermutation.get();
            }

            other.m_ptr = nullptr;
            other.m_vtable = nullptr;
            other.m_inlined = false;

            return *this;
        }

        IShaderPermutation* operator->()
        {
            return Get();
        }

        const IShaderPermutation* operator->() const
        {
            return Get();
        }

    private:
        template <typename T>
        static bool CanInlining()
        {
            return alignof( T ) <= StackAlign
                && sizeof( T ) <= StackSize
                && std::is_nothrow_move_constructible_v<T>;
        }

        void Reset()
        {
            if ( m_ptr )
            {
                if ( m_inlined && m_vtable )
                {
                    m_vtable->m_destroy( m_stackPermutation );
                }
                else
                {
                    m_heapPermutation.reset();
                }
            }

            m_ptr = nullptr;
            m_vtable = nullptr;
            m_inlined = false;
        }

        static constexpr uint32 StackAlign = 16;
        static constexpr uint32 StackSize = 64;

        template <typename T>
        static void Move( void* dest, void* src )
        {
            T* srcPtr = std::launder( reinterpret_cast<T*>( src ) );
            new ( dest ) T( std::move( *srcPtr ) );
            srcPtr->~T();
        }

        template <typename T>
        static void Destroy( void* ptr )
        {
            std::launder( reinterpret_cast<T*>( ptr ) )->~T();
        }

        struct VTable
        {
            void ( *m_move )( void*, void* );
            void ( *m_destroy )( void* );
        };

        template <typename T>
        inline static VTable VTableImpl =
        {
            .m_move = &Move<T>,
            .m_destroy = &Destroy<T>,
        };

        void* m_ptr = nullptr;
        std::unique_ptr<IShaderPermutation> m_heapPermutation = nullptr;
        alignas( StackAlign ) uint8 m_stackPermutation[StackSize] = {};
        const VTable* m_vtable = nullptr;
        bool m_inlined = false;
    };
}
