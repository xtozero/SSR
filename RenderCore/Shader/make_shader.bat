if NOT "%1" == "" cd %1
cd ..\..\bin
mkdir Shader
cd ..\RenderCore\Shader

fxc /Fo csExperimental.cso				/Zi /T cs_4_0 /E main csExperimental.fx
fxc /Fo csTransmittance.cso				/Zi /T cs_5_0 /E main AtmoshpericScattering/csTransmittance.fx
fxc /Fo csIrradiance1.cso				/Zi /T cs_5_0 /E main AtmoshpericScattering/csIrradiance1.fx
fxc /Fo csInscatter1.cso				/Zi /T cs_5_0 /E main AtmoshpericScattering/csInscatter1.fx
fxc /Fo csCopyIrradiance.cso				/Zi /T cs_5_0 /E main AtmoshpericScattering/csCopyIrradiance.fx
fxc /Fo csCopyInscatter1.cso				/Zi /T cs_5_0 /E main AtmoshpericScattering/csCopyInscatter1.fx
fxc /Fo csInscatterS.cso				/Zi /T cs_5_0 /E main AtmoshpericScattering/csInscatterS.fx
fxc /Fo csIrradianceN.cso				/Zi /T cs_5_0 /E main AtmoshpericScattering/csIrradianceN.fx
fxc /Fo csInscatterN.cso				/Zi /T cs_5_0 /E main AtmoshpericScattering/csInscatterN.fx
fxc /Fo csCopyInscatterN.cso				/Zi /T cs_5_0 /E main AtmoshpericScattering/csCopyInscatterN.fx

fxc /Fo gsCascadedShadowmap.cso			/Zi /T gs_4_0 /E main gsCascadedShadowmap.fx

fxc /Fo psCascadedShadowmap.cso			/Zi /T ps_4_0 /E main psCascadedShadowmap.fx
fxc /Fo psDepthStencil.cso			/Zi /T ps_4_0 /E main psDepthStencil.fx
fxc /Fo psShadowmap.cso 			/Zi /T ps_4_0 /E main psShadowmap.fx
fxc /Fo psSkybox.cso 				/Zi /T ps_4_0 /E main psSkybox.fx
fxc /Fo psSSR.cso				/Zi /T ps_4_0 /E main psSSR.fx
fxc /Fo psDdaSSR.cso				/Zi /T ps_4_0 /E main psDdaSSR.fx
fxc /Fo psDdaSsrMs.cso				/Zi /T ps_5_0 /E main psDdaSsrMs.fx
fxc /Fo psSSRBlend.cso 				/Zi /T ps_4_0 /E main psSSRBlend.fx
fxc /Fo psSSRBlendMs.cso 			/Zi /T ps_5_0 /E main psSSRBlendMs.fx
fxc /Fo psTexture.cso 				/Zi /T ps_4_0 /E main psTexture.fx
fxc /Fo psTexWithOutLight.cso			/Zi /T ps_4_0 /E main psTexWithOutLight.fx
fxc /Fo psTutorial.cso 				/Zi /T ps_4_0 /E main psTutorial.fx
fxc /Fo psGaussianBlurX.cso			/Zi /T ps_4_0 /E main psGaussianBlurX.fx
fxc /Fo psGaussianBlurY.cso			/Zi /T ps_4_0 /E main psGaussianBlurY.fx
fxc /Fo psDrawBackfaceDepth.cso			/Zi /T ps_4_0 /E main psDrawBackfaceDepth.fx
fxc /Fo psDrawUI.cso				/Zi /T ps_4_0 /E psMain drawUI.fx
fxc /Fo psDebugOverlay.cso			/Zi /T ps_4_0 /E psMain debugOverlay.fx
fxc /Fo psDrawAtmosphere.cso		/Zi /T ps_4_0 /E main AtmoshpericScattering/psDrawAtmosphere.fx

fxc /Fo vsCascadedShadowmap.cso			/Zi /T vs_4_0 /E main vsCascadedShadowmap.fx
fxc /Fo vsDepthStencil.cso 			/Zi /T vs_4_0 /E main vsDepthStencil.fx
fxc /Fo vsScreenRect.cso 			/Zi /T vs_4_0 /E main vsScreenRect.fx
fxc /Fo vsSSR.cso 				/Zi /T vs_4_0 /E main vsSSR.fx
fxc /Fo vsShadowmap.cso 			/Zi /T vs_4_0 /E main vsShadowmap.fx
fxc /Fo vsSkybox.cso 				/Zi /T vs_4_0 /E main vsSkybox.fx
fxc /Fo vsTexture.cso 				/Zi /T vs_4_0 /E main vsTexture.fx
fxc /Fo vsTutorial.cso 				/Zi /T vs_4_0 /E main vsTutorial.fx
fxc /Fo vsDrawBackfaceDepth.cso			/Zi /T vs_4_0 /E main vsDrawBackfaceDepth.fx
fxc /Fo vsDrawUI.cso				/Zi /T vs_4_0 /E vsMain drawUI.fx
fxc /Fo vsDebugOverlay.cso			/Zi /T vs_4_0 /E vsMain debugOverlay.fx
fxc /Fo vsDrawAtmosphere.cso		/Zi /T vs_4_0 /E main AtmoshpericScattering/vsDrawAtmosphere.fx
copy /Y "*.cso" "..\..\bin\Shader\*.cso"
DEL "*.cso"
pause>nul