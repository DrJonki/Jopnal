// JOPNAL DEFAULT VERTEX UBERSHADER
//
// Jopnal license applies

//////////////////////////////////////////////

// Vertex attributes
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in vec3 a_Normal;
//layout(location = 3) in vec3 a_Tangent;
//layout(location = 4) in vec3 a_BiTangent;
layout(location = 5) in vec4 a_Color;

// Matrices
#ifndef JMAT_ENVIRONMENT_RECORD
    uniform mat4 u_PMatrix; // Perspective
    uniform mat4 u_VMatrix; // View
#endif
uniform mat4 u_MMatrix; // Model
uniform mat3 u_NMatrix; // Normal (is transpose(inverse(u_MMatrix)))

// Vertex attributes to fragment/geometry shader
#ifdef JMAT_ENVIRONMENT_RECORD
    #define OUTVERT_NAME inVert
    out VertexData
#else
    #define OUTVERT_NAME outVert
    out FragVertexData
#endif
{
    vec3 Position;
    vec2 TexCoords;
    vec3 Normal;
    //vec3 Tangent;
    //vec3 BiTangent;
    vec4 Color;

} OUTVERT_NAME;

// Fragment position to fragment shader
// If this shader used to record an environment map, geometry shader will
// take care of the fragment position
#ifndef JMAT_ENVIRONMENT_RECORD
    out vec3 vgf_FragPosition;
#endif

void main()
{
    vec4 pos = 
        
    #if !defined(JMAT_SKYBOX) && !defined(JMAT_SKYSPHERE)
        u_MMatrix * 
    #endif
        vec4(a_Position, 1.0);

    // Assign attributes
    OUTVERT_NAME.Position = pos.xyz;
    OUTVERT_NAME.TexCoords = a_TexCoords;
    OUTVERT_NAME.Normal = u_NMatrix * a_Normal;
    OUTVERT_NAME.Color = a_Color;

    // Calculate and assign fragment position, not used when recording environment map
    #ifndef JMAT_ENVIRONMENT_RECORD
        vgf_FragPosition = pos.xyz;
    #endif

    // Calculate and assign position
    gl_Position = (

    #ifndef JMAT_ENVIRONMENT_RECORD

        u_PMatrix * 
        
        #if defined(JMAT_SKYBOX) || defined(JMAT_SKYSPHERE)
            mat4(mat3(u_VMatrix))
        #else
            u_VMatrix
        #endif
        *

    #endif
        
    pos)
    
    #if (defined(JMAT_SKYBOX) || defined(JMAT_SKYSPHERE)) && !defined(JMAT_ENVIRONMENT_RECORD)
        .xyww
    #endif
    ;
}