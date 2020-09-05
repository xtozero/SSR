#pragma once

#include "Delegate.h"
#include "Surface.h"
#include "Util.h"

#include <cassert>
#include <memory>
#include <string>

struct MeshDescription;
class Material;

class IModelLoaderHandle
{
public:
	virtual bool IsLoadingInProgress( ) const = 0;
	virtual bool IsLoadComplete( ) const = 0;

	virtual ~IModelLoaderHandle( ) = default;
};

using ModelLoaderSharedHandle = std::shared_ptr<IModelLoaderHandle>;

class IModelLoader
{
public:
	using LoadCompletionCallback = Delegate<void, void*>;

	virtual ModelLoaderSharedHandle RequestAsyncLoad( const char* pFilePath, LoadCompletionCallback completionCallback ) = 0;

	virtual ~IModelLoader( ) = default;

protected:
	IModelLoader( ) = default;
};

class ModelLoaderHandle : public IModelLoaderHandle
{
public:
	virtual bool IsLoadingInProgress( ) const override
	{
		return m_loadingInProgress;
	}

	virtual bool IsLoadComplete( ) const override
	{
		return m_loadComplete;
	}

	void ExecuteCompletionCallback( )
	{
		assert( m_loadedAsset != nullptr );
		m_loadingInProgress = false;
		m_loadComplete = true;

		if ( m_loadCompletionCallback.IsBound( ) )
		{
			m_loadCompletionCallback( m_loadedAsset );
		}
	}

	void SetLoadedAsset( void* loadedAsset )
	{
		m_loadedAsset = loadedAsset;
	}

	void BindCompletionCallback( const IModelLoader::LoadCompletionCallback& callback )
	{
		m_loadCompletionCallback = callback;
	}

	void OnStartLoading( )
	{
		m_loadingInProgress = true;
	}

private:
	IModelLoader::LoadCompletionCallback m_loadCompletionCallback;
	void* m_loadedAsset = nullptr;
	bool m_loadingInProgress = false;
	bool m_loadComplete = false;
};