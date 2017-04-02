cd ..\Shader
fxc /Fo psDepthStencil.cso			/T ps_4_0 /E main psDepthStencil.fx
fxc /Fo psShadowmap.cso 			/T ps_4_0 /E main psShadowmap.fx
fxc /Fo psSkybox.cso 				/T ps_4_0 /E main psSkybox.fx
fxc /Fo psSSR.cso				/T ps_4_0 /E main psSSR.fx
fxc /Fo psSSRBlend.cso 				/T ps_4_0 /E main psSSRBlend.fx
fxc /Fo psTexture.cso 				/T ps_4_0 /E main psTexture.fx
fxc /Fo psTexWithOutLight.cso			/T ps_4_0 /E main psTexWithOutLight.fx
fxc /Fo psTutorial.cso 				/T ps_4_0 /E main psTutorial.fx
fxc /Fo psGaussianBlurX.cso			/T ps_4_0 /E main psGaussianBlurX.fx
fxc /Fo psGaussianBlurY.cso			/T ps_4_0 /E main psGaussianBlurY.fx
fxc /Fo vsDepthStencil.cso 			/T vs_4_0 /E main vsDepthStencil.fx
fxc /Fo vsScreenRect.cso 			/T vs_4_0 /E main vsScreenRect.fx
fxc /Fo vsSSR.cso 				/T vs_4_0 /E main vsSSR.fx
fxc /Fo vsShadowmap.cso 			/T vs_4_0 /E main vsShadowmap.fx
fxc /Fo vsSkybox.cso 				/T vs_4_0 /E main vsSkybox.fx
fxc /Fo vsTexture.cso 				/T vs_4_0 /E main vsTexture.fx
fxc /Fo vsTutorial.cso 				/T vs_4_0 /E main vsTutorial.fx
copy /Y "*.cso" "..\bin\Shader\*.cso"
DEL "*.cso"
pause>nul