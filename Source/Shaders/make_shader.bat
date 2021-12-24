cd "%~dp0"

set IncludePath=Public

echo %IncludePath%

rem fxc /Fo CS_Experimental.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/csExperimental.fx
rem fxc /Fo CS_Transmittance.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csTransmittance.fx
rem fxc /Fo CS_Irradiance1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csIrradiance1.fx
rem fxc /Fo CS_Inscatter1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csInscatter1.fx
rem fxc /Fo CS_CopyIrradiance.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csCopyIrradiance.fx
rem fxc /Fo CS_CopyInscatter1.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csCopyInscatter1.fx
rem fxc /Fo CS_InscatterS.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csInscatterS.fx
rem fxc /Fo CS_IrradianceN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csIrradianceN.fx
rem fxc /Fo CS_InscatterN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csInscatterN.fx
rem fxc /Fo CS_CopyInscatterN.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csCopyInscatterN.fx
rem fxc /Fo CS_DistributionCopy.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/CS_DistributionCopy.fx

fxc /Fo GS_CascadedShadowmap.cso			/Zi /T gs_5_0 /I %IncludePath% /E main Private/GS_CascadedShadowmap.fx

fxc /Fo PS_CascadedShadowmap.cso			/Zi /T ps_5_0 /I %IncludePath% /E main Private/PS_CascadedShadowmap.fx
rem fxc /Fo PS_DepthStencil.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDepthStencil.fx
fxc /Fo PS_DrawCascadeShadow.cso			/Zi	/T ps_5_0 /I %IncludePath% /E main Private/PS_DrawCascadeShadow.fx
fxc /Fo PS_DepthWrite.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/PS_DepthWrite.fx
rem fxc /Fo PS_Shadowmap.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psShadowmap.fx
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
rem fxc /Fo PS_DrawAtmosphere.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/psDrawAtmosphere.fx

fxc /Fo VS_CascadedShadowmap.cso			/Zi /T vs_5_0 /I %IncludePath% /E main Private/VS_CascadedShadowmap.fx
rem fxc /Fo VS_DepthStencil.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDepthStencil.fx
fxc /Fo VS_DepthWrite.cso					/Zi /T vs_5_0 /I %IncludePath% /E main Private/VS_DepthWrite.fx
fxc /Fo VS_FullScreenQuad.cso			/Zi /T vs_5_0 /I %IncludePath% /E main Private/VS_FullScreenQuad.fx
rem fxc /Fo VS_ScreenRect.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsScreenRect.fx
rem fxc /Fo VS_SSR.cso 							/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsSSR.fx
rem fxc /Fo VS_Shadowmap.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsShadowmap.fx
fxc /Fo VS_Skybox.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsSkybox.fx
rem fxc /Fo VS_Texture.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsTexture.fx
fxc /Fo VS_Tutorial.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsTutorial.fx
rem fxc /Fo VS_DrawBackfaceDepth.cso			/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDrawBackfaceDepth.fx
rem fxc /Fo VS_DrawUI.cso						/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/drawUI.fx
rem fxc /Fo VS_DebugOverlay.cso					/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/debugOverlay.fx
rem fxc /Fo VS_DrawAtmosphere.cso				/Zi /T vs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/vsDrawAtmosphere.fx
copy /Y "*.cso" "..\..\RawAssets\Shaders\*.cso"
DEL "*.cso"
pause>nul