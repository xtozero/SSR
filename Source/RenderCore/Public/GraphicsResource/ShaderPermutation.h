#pragma once

#include "FnvHash.h"
#include "SizedTypes.h"

#include <cassert>
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
}
