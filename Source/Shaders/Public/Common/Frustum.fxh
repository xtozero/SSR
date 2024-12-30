struct Frustum
{
    float4 m_plane[6];
};

Frustum ExtractFrustum( matrix mat )
{
    Frustum frustum = (Frustum)0;

    float4 column[4] = {
        float4( mat[0][0], mat[1][0], mat[2][0], mat[3][0] ),
        float4( mat[0][1], mat[1][1], mat[2][1], mat[3][1] ),
        float4( mat[0][2], mat[1][2], mat[2][2], mat[3][2] ),
        float4( mat[0][3], mat[1][3], mat[2][3], mat[3][3] )
    };

    frustum.m_plane[0] = column[3] + column[0]; // left
    frustum.m_plane[1] = column[3] - column[0]; // right
    frustum.m_plane[2] = column[3] + column[1]; // bottom
    frustum.m_plane[3] = column[3] - column[1]; // top
    frustum.m_plane[4] = column[2];			    // near
    frustum.m_plane[5] = column[3] - column[2]; // far

    frustum.m_plane[0] /= length( frustum.m_plane[0].xyz );
    frustum.m_plane[1] /= length( frustum.m_plane[1].xyz );
    frustum.m_plane[2] /= length( frustum.m_plane[2].xyz );
    frustum.m_plane[3] /= length( frustum.m_plane[3].xyz );
    frustum.m_plane[4] /= length( frustum.m_plane[4].xyz );
    frustum.m_plane[5] /= length( frustum.m_plane[5].xyz );

    return frustum;
}

bool IsInside( Frustum frustum, float3 center, float radius )
{
    bool i0 = dot( frustum.m_plane[0].xyz, center ) + frustum.m_plane[0].w + radius >= 0;
    bool i1 = dot( frustum.m_plane[1].xyz, center ) + frustum.m_plane[1].w + radius >= 0;
    bool i2 = dot( frustum.m_plane[2].xyz, center ) + frustum.m_plane[2].w + radius >= 0;
    bool i3 = dot( frustum.m_plane[3].xyz, center ) + frustum.m_plane[3].w + radius >= 0;
    bool i4 = dot( frustum.m_plane[4].xyz, center ) + frustum.m_plane[4].w + radius >= 0;
    bool i5 = dot( frustum.m_plane[5].xyz, center ) + frustum.m_plane[5].w + radius >= 0;

    return i0 && i1 && i2 && i3 && i4 && i5;
}