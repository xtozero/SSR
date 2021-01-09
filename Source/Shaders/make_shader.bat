if NOT "%1" == "" cd %1
cd ..\..\Program
mkdir Shaders
cd ..\Source\Shaders

set IncludePath=Public

echo %IncludePath%

fxc /Fo csExperimental.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/csExperimental.fx
fxc /Fo csTransmittance.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csTransmittance.fx
fxc /Fo csIrradiance1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csIrradiance1.fx
fxc /Fo csInscatter1.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csInscatter1.fx
fxc /Fo csCopyIrradiance.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csCopyIrradiance.fx
fxc /Fo csCopyInscatter1.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csCopyInscatter1.fx
fxc /Fo csInscatterS.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csInscatterS.fx
fxc /Fo csIrradianceN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csIrradianceN.fx
fxc /Fo csInscatterN.cso					/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csInscatterN.fx
fxc /Fo csCopyInscatterN.cso				/Zi /T cs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/csCopyInscatterN.fx

fxc /Fo gsCascadedShadowmap.cso				/Zi /T gs_5_0 /I %IncludePath% /E main Private/gsCascadedShadowmap.fx

fxc /Fo psCascadedShadowmap.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psCascadedShadowmap.fx
fxc /Fo psDepthStencil.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDepthStencil.fx
fxc /Fo psShadowmap.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psShadowmap.fx
fxc /Fo psSkybox.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSkybox.fx
fxc /Fo psSSR.cso							/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSR.fx
fxc /Fo psDdaSSR.cso						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDdaSSR.fx
fxc /Fo psDdaSsrMs.cso						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDdaSsrMs.fx
fxc /Fo psSSRBlend.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSRBlend.fx
fxc /Fo psSSRBlendMs.cso 					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psSSRBlendMs.fx
fxc /Fo psTexture.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTexture.fx
fxc /Fo psTexWithOutLight.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTexWithOutLight.fx
fxc /Fo psTutorial.cso 						/Zi /T ps_5_0 /I %IncludePath% /E main Private/psTutorial.fx
fxc /Fo psGaussianBlurX.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psGaussianBlurX.fx
fxc /Fo psGaussianBlurY.cso					/Zi /T ps_5_0 /I %IncludePath% /E main Private/psGaussianBlurY.fx
fxc /Fo psDrawBackfaceDepth.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/psDrawBackfaceDepth.fx
fxc /Fo psDrawUI.cso						/Zi /T ps_5_0 /I %IncludePath% /E psMain Private/drawUI.fx
fxc /Fo psDebugOverlay.cso					/Zi /T ps_5_0 /I %IncludePath% /E psMain Private/debugOverlay.fx
fxc /Fo psDrawAtmosphere.cso				/Zi /T ps_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/psDrawAtmosphere.fx

fxc /Fo vsCascadedShadowmap.cso				/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsCascadedShadowmap.fx
fxc /Fo vsDepthStencil.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDepthStencil.fx
fxc /Fo vsScreenRect.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsScreenRect.fx
fxc /Fo vsSSR.cso 							/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsSSR.fx
fxc /Fo vsShadowmap.cso 					/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsShadowmap.fx
fxc /Fo vsSkybox.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsSkybox.fx
fxc /Fo vsTexture.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsTexture.fx
fxc /Fo vsTutorial.cso 						/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsTutorial.fx
fxc /Fo vsDrawBackfaceDepth.cso				/Zi /T vs_5_0 /I %IncludePath% /E main Private/vsDrawBackfaceDepth.fx
fxc /Fo vsDrawUI.cso						/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/drawUI.fx
fxc /Fo vsDebugOverlay.cso					/Zi /T vs_5_0 /I %IncludePath% /E vsMain Private/debugOverlay.fx
fxc /Fo vsDrawAtmosphere.cso				/Zi /T vs_5_0 /I %IncludePath% /E main Private/AtmoshpericScattering/vsDrawAtmosphere.fx
copy /Y "*.cso" "..\..\RawAssets\Shaders\*.cso"
DEL "*.cso"
pause>nul