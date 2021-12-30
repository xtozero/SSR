#pragma once

class ShadowInfo;

void BuildOrthoShadowProjectionMatrix( ShadowInfo& shadowInfo );
void BuildPSMProjectionMatrix( ShadowInfo& shadowInfo );
void BuildLSPSMProjectionMatrix( ShadowInfo& shadowInfo );
void CalculateSplitPositions( ShadowInfo& shadowInfo, float casterFar, float lamda = 0.5f );
