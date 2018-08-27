if NOT "%1" == "" cd %1
cd ..\..\bin
mkdir Shader
cd ..\RenderCore\Shader

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
copy /Y "*.cso" "..\..\bin\Shader\*.cso"
DEL "*.cso"
pause>nul