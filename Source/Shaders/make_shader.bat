cd "%~dp0"

set IncludePath=Public

echo %IncludePath%

rem fxc /Fo CS_Experimental.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/CS_Experimental.fx
fxc /Fo CS_Transmittance.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/CS_Transmittance.fx
fxc /Fo CS_Irradiance1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/CS_Irradiance1.fx
fxc /Fo CS_Inscatter1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/CS_Inscatter1.fx
fxc /Fo CS_CopyIrradiance.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/CS_CopyIrradiance.fx
fxc /Fo CS_CopyInscatter1.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/CS_CopyInscatter1.fx
fxc /Fo CS_InscatterS.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/CS_InscatterS.fx
fxc /Fo CS_IrradianceN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/CS_IrradianceN.fx
fxc /Fo CS_InscatterN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/CS_InscatterN.fx
fxc /Fo CS_CopyInscatterN.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/CS_CopyInscatterN.fx
fxc /Fo CS_DistributionCopy.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/CS_DistributionCopy.fx
rem fxc /Fo CS_WorleyNoise.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Noise/CS_WorleyNoise.fx
rem fxc /Fo CS_NormalizeNoise.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/Noise/CS_NormalizeNoise.fx
rem fxc /Fo CS_PerlinNoise.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Noise/CS_PerlinNoise.fx
rem fxc /Fo CS_FbmNoise.cso						/Zi /T cs_5_0 /I %IncludePath% /E main Private/Noise/CS_FbmNoise.fx
rem fxc /Fo CS_GatherNoise.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Cloud/CS_GatherNoise.fx
fxc /Fo CS_WeatherMap.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Cloud/CS_WeatherMap.fx
fxc /Fo CS_PerlinWorley.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/Cloud/CS_PerlinWorley.fx
fxc /Fo CS_Worley.cso						/Zi /T cs_5_0 /I %IncludePath% /E main Private/Cloud/CS_Worley.fx

fxc /Fo GS_CascadedShadowmap.cso			/Zi /T gs_5_0 /I %IncludePath% /E main Private/GS_CascadedShadowmap.fx

fxc /Fo PS_CascadedShadowmap.cso			/Zi /T ps_5_0 /I %IncludePath% /E main Private/PS_CascadedShadowmap.fx
rem fxc /Fo PS_DepthStencil.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDepthStencil.fx
fxc /Fo PS_DrawCascadeShadow.cso			/Zi	/T ps_5_0 /I %IncludePath% /E main Private/PS_DrawCascadeShadow.fx
fxc /Fo PS_DepthWrite.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/PS_DepthWrite.fx
fxc /Fo PS_Skybox.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSkybox.fx
rem fxc /Fo PS_SSR.cso							/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSR.fx
rem fxc /Fo PS_DdaSSR.cso						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDdaSSR.fx
rem fxc /Fo PS_DdaSsrMs.cso						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDdaSsrMs.fx
rem fxc /Fo PS_SSRBlend.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSRBlend.fx
rem fxc /Fo PS_SSRBlendMs.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSRBlendMs.fx
rem fxc /Fo PS_Texture.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTexture.fx
rem fxc /Fo PS_TexWithOutLight.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTexWithOutLight.fx
fxc /Fo PS_Tutorial.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTutorial.fx
rem fxc /Fo PS_GaussianBlurX.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psGaussianBlurX.fx
rem fxc /Fo PS_GaussianBlurY.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psGaussianBlurY.fx
rem fxc /Fo PS_DrawBackfaceDepth.cso			/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDrawBackfaceDepth.fx
rem fxc /Fo PS_DrawUI.cso						/Zi /T ps_5_0 /I %IncludePath% /E psMain Private/drawUI.fx
rem fxc /Fo PS_DebugOverlay.cso					/Zi /T ps_5_0 /I %IncludePath% /E psMain Private/debugOverlay.fx
fxc /Fo PS_DrawAtmosphere.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/PS_DrawAtmosphere.fx
fxc /Fo PS_DrawVolumetricCloud.cso			/Zi /T ps_5_0 /I %IncludePath% /E main Private/PS_DrawVolumetricCloud.fx

fxc /Fo VS_CascadedShadowmap.cso			/Zi /T vs_5_0 /I %IncludePath% /E main Private/VS_CascadedShadowmap.fx
rem fxc /Fo VS_DepthStencil.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDepthStencil.fx
fxc /Fo VS_DepthWrite.cso					/Zi /T vs_5_0 /I %IncludePath% /E main Private/VS_DepthWrite.fx
fxc /Fo VS_FullScreenQuad.cso			/Zi /T vs_5_0 /I %IncludePath% /E main Private/VS_FullScreenQuad.fx
rem fxc /Fo VS_ScreenRect.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsScreenRect.fx
rem fxc /Fo VS_SSR.cso 							/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsSSR.fx
fxc /Fo VS_Skybox.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsSkybox.fx
rem fxc /Fo VS_Texture.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsTexture.fx
fxc /Fo VS_Tutorial.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsTutorial.fx
rem fxc /Fo VS_DrawBackfaceDepth.cso			/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDrawBackfaceDepth.fx
rem fxc /Fo VS_DrawUI.cso						/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/drawUI.fx
rem fxc /Fo VS_DebugOverlay.cso					/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/debugOverlay.fx
fxc /Fo VS_DrawAtmosphere.cso				/Zi /T vs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/VS_DrawAtmosphere.fx
copy /Y "*.cso" "..\..\RawAssets\Shaders\*.cso"
DEL "*.cso"
pause>nul