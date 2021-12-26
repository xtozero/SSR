#pragma once

class ShadowInfo;

void BuildOrthoShadowProjectionMatrix( ShadowInfo& shadowInfo );
void BuildPSMProjectionMatrix( ShadowInfo& shadowInfo );
void BuildLSPSMProjectionMatrix( ShadowInfo& shadowInfo );