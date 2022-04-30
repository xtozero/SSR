cd "%~dp0"

set IncludePath=Public
set RawAssetPath=../../RawAssets/Shaders

mkdir "%RawAssetPath%/Atmosphere"
mkdir "%RawAssetPath%/Cloud"
mkdir "%RawAssetPath%/Common"
mkdir "%RawAssetPath%/Shadow"

echo %IncludePath%

rem fxc /Fo CS_Experimental.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/CS_Experimental.fx
fxc /Fo %RawAssetPath%/Atmosphere/CS_Transmittance.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/Atmosphere/CS_Transmittance.fx
fxc /Fo %RawAssetPath%/Atmosphere/CS_Irradiance1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Atmosphere/CS_Irradiance1.fx
fxc /Fo %RawAssetPath%/Atmosphere/CS_Inscatter1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Atmosphere/CS_Inscatter1.fx
fxc /Fo %RawAssetPath%/Atmosphere/CS_CopyIrradiance.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/Atmosphere/CS_CopyIrradiance.fx
fxc /Fo %RawAssetPath%/Atmosphere/CS_CopyInscatter1.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/Atmosphere/CS_CopyInscatter1.fx
fxc /Fo %RawAssetPath%/Atmosphere/CS_InscatterS.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Atmosphere/CS_InscatterS.fx
fxc /Fo %RawAssetPath%/Atmosphere/CS_IrradianceN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Atmosphere/CS_IrradianceN.fx
fxc /Fo %RawAssetPath%/Atmosphere/CS_InscatterN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Atmosphere/CS_InscatterN.fx
fxc /Fo %RawAssetPath%/Atmosphere/CS_CopyInscatterN.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/Atmosphere/CS_CopyInscatterN.fx
fxc /Fo %RawAssetPath%/Common/CS_DistributionCopy.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/Common/CS_DistributionCopy.fx
rem fxc /Fo CS_WorleyNoise.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Noise/CS_WorleyNoise.fx
rem fxc /Fo CS_NormalizeNoise.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/Noise/CS_NormalizeNoise.fx
rem fxc /Fo CS_PerlinNoise.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Noise/CS_PerlinNoise.fx
rem fxc /Fo CS_FbmNoise.cso						/Zi /T cs_5_0 /I %IncludePath% /E main Private/Noise/CS_FbmNoise.fx
rem fxc /Fo CS_GatherNoise.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Cloud/CS_GatherNoise.fx
fxc /Fo %RawAssetPath%/Cloud/CS_WeatherMap.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Cloud/CS_WeatherMap.fx
fxc /Fo %RawAssetPath%/Cloud/CS_PerlinWorley.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Cloud/CS_PerlinWorley.fx
fxc /Fo %RawAssetPath%/Cloud/CS_Worley.cso						/Zi /T cs_5_0 /I %IncludePath% /E main Private/Cloud/CS_Worley.fx

fxc /Fo %RawAssetPath%/Shadow/GS_CascadedShadowmap.cso			/Zi /T gs_5_0 /I %IncludePath% /E main Private/Shadow/GS_CascadedShadowmap.fx

fxc /Fo %RawAssetPath%/Shadow/PS_CascadedShadowmap.cso			/Zi /T ps_5_0 /I %IncludePath% /E main Private/Shadow/PS_CascadedShadowmap.fx
rem fxc /Fo PS_DepthStencil.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDepthStencil.fx
fxc /Fo %RawAssetPath%/Shadow/PS_DrawCascadeShadow.cso			/Zi	/T ps_5_0 /I %IncludePath% /E main Private/Shadow/PS_DrawCascadeShadow.fx
fxc /Fo %RawAssetPath%/PS_DepthWrite.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/PS_DepthWrite.fx
fxc /Fo %RawAssetPath%/PS_Skybox.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/PS_Skybox.fx
rem fxc /Fo PS_SSR.cso							/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSR.fx
rem fxc /Fo PS_DdaSSR.cso						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDdaSSR.fx
rem fxc /Fo PS_DdaSsrMs.cso						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDdaSsrMs.fx
rem fxc /Fo PS_SSRBlend.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSRBlend.fx
rem fxc /Fo PS_SSRBlendMs.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSRBlendMs.fx
rem fxc /Fo PS_Texture.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTexture.fx
rem fxc /Fo PS_TexWithOutLight.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTexWithOutLight.fx
fxc /Fo %RawAssetPath%/PS_Tutorial.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/PS_Tutorial.fx
rem fxc /Fo PS_GaussianBlurX.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psGaussianBlurX.fx
rem fxc /Fo PS_GaussianBlurY.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psGaussianBlurY.fx
rem fxc /Fo PS_DrawBackfaceDepth.cso			/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDrawBackfaceDepth.fx
rem fxc /Fo PS_DrawUI.cso						/Zi /T ps_5_0 /I %IncludePath% /E psMain Private/drawUI.fx
rem fxc /Fo PS_DebugOverlay.cso					/Zi /T ps_5_0 /I %IncludePath% /E psMain Private/debugOverlay.fx
fxc /Fo %RawAssetPath%/Atmosphere/PS_DrawAtmosphere.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/Atmosphere/PS_DrawAtmosphere.fx
fxc /Fo %RawAssetPath%/Cloud/PS_DrawVolumetricCloud.cso			/Zi /T ps_5_0 /I %IncludePath% /E main Private/Cloud/PS_DrawVolumetricCloud.fx

fxc /Fo %RawAssetPath%/Shadow/VS_CascadedShadowmap.cso			/Zi /T vs_5_0 /I %IncludePath% /E main Private/Shadow/VS_CascadedShadowmap.fx
rem fxc /Fo VS_DepthStencil.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDepthStencil.fx
fxc /Fo %RawAssetPath%/VS_DepthWrite.cso					/Zi /T vs_5_0 /I %IncludePath% /E main Private/VS_DepthWrite.fx
fxc /Fo %RawAssetPath%/Common/VS_FullScreenQuad.cso			/Zi /T vs_5_0 /I %IncludePath% /E main Private/Common/VS_FullScreenQuad.fx
rem fxc /Fo VS_ScreenRect.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsScreenRect.fx
rem fxc /Fo VS_SSR.cso 							/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsSSR.fx
fxc /Fo %RawAssetPath%/VS_Skybox.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/VS_Skybox.fx
rem fxc /Fo VS_Texture.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsTexture.fx
fxc /Fo %RawAssetPath%/VS_Tutorial.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/VS_Tutorial.fx
rem fxc /Fo VS_DrawBackfaceDepth.cso			/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDrawBackfaceDepth.fx
rem fxc /Fo VS_DrawUI.cso						/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/drawUI.fx
rem fxc /Fo VS_DebugOverlay.cso					/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/debugOverlay.fx
rem fxc /Fo %RawAssetPath%/Atmosphere/VS_DrawAtmosphere.cso				/Zi /T vs_5_0 /I %IncludePath% /E main Private/Atmosphere/VS_DrawAtmosphere.fx
pause>nul