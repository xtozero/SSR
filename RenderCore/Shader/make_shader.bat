if NOT "%1" == "" cd %1
fxc /Fo psDepthStencil.cso			/T ps_4_0 /E main psDepthStencil.fx
fxc /Fo psShadowmap.cso 			/T ps_4_0 /E main psShadowmap.fx
fxc /Fo psSkybox.cso 				/T ps_4_0 /E main psSkybox.fx
fxc /Fo psSSR.cso				/T ps_4_0 /E main psSSR.fx
fxc /Fo psDdaSSR.cso				/T ps_4_0 /E main psDdaSSR.fx
fxc /Fo psDdaSsrMs.cso				/T ps_5_0 /E main psDdaSsrMs.fx
fxc /Fo psSSRBlend.cso 				/T ps_4_0 /E main psSSRBlend.fx
fxc /Fo psSSRBlendMs.cso 			/T ps_5_0 /E main psSSRBlendMs.fx
fxc /Fo psTexture.cso 				/T ps_4_0 /E main psTexture.fx
fxc /Fo psTexWithOutLight.cso			/T ps_4_0 /E main psTexWithOutLight.fx
fxc /Fo psTutorial.cso 				/T ps_4_0 /E main psTutorial.fx
fxc /Fo psGaussianBlurX.cso			/T ps_4_0 /E main psGaussianBlurX.fx
fxc /Fo psGaussianBlurY.cso			/T ps_4_0 /E main psGaussianBlurY.fx
fxc /Fo psDrawBackfaceDepth.cso			/T ps_4_0 /E main psDrawBackfaceDepth.fx
fxc /Fo psDrawUI.cso				/T ps_4_0 /E psMain drawUI.fx
fxc /Fo vsDepthStencil.cso 			/T vs_4_0 /E main vsDepthStencil.fx
fxc /Fo vsScreenRect.cso 			/T vs_4_0 /E main vsScreenRect.fx
fxc /Fo vsSSR.cso 				/T vs_4_0 /E main vsSSR.fx
fxc /Fo vsShadowmap.cso 			/T vs_4_0 /E main vsShadowmap.fx
fxc /Fo vsSkybox.cso 				/T vs_4_0 /E main vsSkybox.fx
fxc /Fo vsTexture.cso 				/T vs_4_0 /E main vsTexture.fx
fxc /Fo vsTutorial.cso 				/T vs_4_0 /E main vsTutorial.fx
fxc /Fo vsDrawBackfaceDepth.cso			/T vs_4_0 /E main vsDrawBackfaceDepth.fx
fxc /Fo vsDrawUI.cso				/T vs_4_0 /E vsMain drawUI.fx
copy /Y "*.cso" "..\..\bin\Shader\*.cso"
DEL "*.cso"
pause>nul