if NOT "%1" == "" cd %1
cd ..\..\Program
mkdir Shaders
cd ..\Source\Shaders

set IncludePath=Public

echo %IncludePath%

fxc /Fo CS_Experimental.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/csExperimental.fx
fxc /Fo CS_Transmittance.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csTransmittance.fx
fxc /Fo CS_Irradiance1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csIrradiance1.fx
fxc /Fo CS_Inscatter1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csInscatter1.fx
fxc /Fo CS_CopyIrradiance.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csCopyIrradiance.fx
fxc /Fo CS_CopyInscatter1.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csCopyInscatter1.fx
fxc /Fo CS_InscatterS.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csInscatterS.fx
fxc /Fo CS_IrradianceN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csIrradianceN.fx
fxc /Fo CS_InscatterN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csInscatterN.fx
fxc /Fo CS_CopyInscatterN.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csCopyInscatterN.fx

fxc /Fo GS_CascadedShadowmap.cso				/Zi /T gs_5_0 /I %IncludePath% /E main Private/gsCascadedShadowmap.fx

fxc /Fo PS_CascadedShadowmap.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psCascadedShadowmap.fx
fxc /Fo PS_DepthStencil.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDepthStencil.fx
fxc /Fo PS_Shadowmap.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psShadowmap.fx
fxc /Fo PS_Skybox.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSkybox.fx
fxc /Fo PS_SSR.cso							/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSR.fx
fxc /Fo PS_DdaSSR.cso						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDdaSSR.fx
fxc /Fo PS_DdaSsrMs.cso						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDdaSsrMs.fx
fxc /Fo PS_SSRBlend.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSRBlend.fx
fxc /Fo PS_SSRBlendMs.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSRBlendMs.fx
fxc /Fo PS_Texture.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTexture.fx
fxc /Fo PS_TexWithOutLight.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTexWithOutLight.fx
fxc /Fo PS_Tutorial.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTutorial.fx
fxc /Fo PS_GaussianBlurX.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psGaussianBlurX.fx
fxc /Fo PS_GaussianBlurY.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psGaussianBlurY.fx
fxc /Fo PS_DrawBackfaceDepth.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDrawBackfaceDepth.fx
fxc /Fo PS_DrawUI.cso						/Zi /T ps_5_0 /I %IncludePath% /E psMain Private/drawUI.fx
fxc /Fo PS_DebugOverlay.cso					/Zi /T ps_5_0 /I %IncludePath% /E psMain Private/debugOverlay.fx
fxc /Fo PS_DrawAtmosphere.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/psDrawAtmosphere.fx

fxc /Fo VS_CascadedShadowmap.cso				/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsCascadedShadowmap.fx
fxc /Fo VS_DepthStencil.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDepthStencil.fx
fxc /Fo VS_ScreenRect.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsScreenRect.fx
fxc /Fo VS_SSR.cso 							/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsSSR.fx
fxc /Fo VS_Shadowmap.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsShadowmap.fx
fxc /Fo VS_Skybox.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsSkybox.fx
fxc /Fo VS_Texture.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsTexture.fx
fxc /Fo VS_Tutorial.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsTutorial.fx
fxc /Fo VS_DrawBackfaceDepth.cso				/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDrawBackfaceDepth.fx
fxc /Fo VS_DrawUI.cso						/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/drawUI.fx
fxc /Fo VS_DebugOverlay.cso					/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/debugOverlay.fx
fxc /Fo VS_DrawAtmosphere.cso				/Zi /T vs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/vsDrawAtmosphere.fx
copy /Y "*.cso" "..\..\RawAssets\Shaders\*.cso"
DEL "*.cso"
pause>nul