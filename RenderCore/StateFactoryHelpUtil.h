#pragma once

#include "common.h"

enum D3D11_FILL_MODE;
enum D3D11_CULL_MODE;
enum D3D11_COMPARISON_FUNC;
enum D3D11_DEPTH_WRITE_MASK;
enum D3D11_STENCIL_OP;
enum D3D11_FILTER;
enum D3D11_TEXTURE_ADDRESS_MODE;

D3D11_FILL_MODE TranslateFillMode( const String& fillMode );
D3D11_CULL_MODE TranslateCullMode( const String& cullMode );
D3D11_COMPARISON_FUNC TranslateComparisonFunc( const String& key );
D3D11_DEPTH_WRITE_MASK TranslateDepthWriteMask( const String& key );
D3D11_STENCIL_OP TranslateStencilOP( const String& key );
D3D11_FILTER TranslateFilter( const String& key );
D3D11_TEXTURE_ADDRESS_MODE  TranslateTextureAddressMode( const String& key );