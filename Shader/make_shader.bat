cd ..\Shader
fxc /Fo psDepthStencil.cso			/T ps_4_0 /E main psDepthStencil.fx
fxc /Fo psShadowmap.cso 			/T ps_4_0 /E main psShadowmap.fx
fxc /Fo psSkybox.cso 				/T ps_4_0 /E main psSkybox.fx
fxc /Fo psTexture.cso 				/T ps_4_0 /E main psTexture.fx
fxc /Fo psTexWithOutLight.cso 		/T ps_4_0 /E main psTexWithOutLight.fx
fxc /Fo psTutorial.cso 				/T ps_4_0 /E main psTutorial.fx
fxc /Fo vsDepthStencil.cso 			/T vs_4_0 /E main vsDepthStencil.fx
fxc /Fo vsShadowmap.cso 			/T vs_4_0 /E main vsShadowmap.fx
fxc /Fo vsSkybox.cso 				/T vs_5_0 /E main vsSkybox.fx
fxc /Fo vsTexture.cso 				/T vs_4_0 /E main vsTexture.fx
fxc /Fo vsTutorial.cso 				/T vs_4_0 /E main vsTutorial.fx
pause>nul