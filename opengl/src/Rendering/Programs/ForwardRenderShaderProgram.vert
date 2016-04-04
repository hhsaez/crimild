R"(

// The scale matrix is used to push the projected vertex into the 0.0 - 1.0 region.
const mat4 ScaleMatrix = mat4( 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0 );
                                             
CRIMILD_GLSL_ATTRIBUTE vec3 aPosition;
CRIMILD_GLSL_ATTRIBUTE vec3 aNormal;
CRIMILD_GLSL_ATTRIBUTE vec3 aTangent;
CRIMILD_GLSL_ATTRIBUTE vec2 aTextureCoord;
   
uniform mat4 uPMatrix;
uniform mat4 uVMatrix;
uniform mat4 uMMatrix;
uniform mat4 uLightSourceProjectionMatrix;
uniform mat4 uLightSourceViewMatrix;
   
uniform bool uUseNormalMap;
                                             
CRIMILD_GLSL_VARYING_OUT vec4 vWorldVertex;
CRIMILD_GLSL_VARYING_OUT vec3 vWorldNormal;
CRIMILD_GLSL_VARYING_OUT vec3 vWorldTangent;
CRIMILD_GLSL_VARYING_OUT vec3 vWorldBiTangent;
CRIMILD_GLSL_VARYING_OUT vec2 vTextureCoord;
CRIMILD_GLSL_VARYING_OUT vec3 vViewVec;
CRIMILD_GLSL_VARYING_OUT vec4 vPosition;

void main ()
{
	vWorldVertex = uMMatrix * vec4( aPosition, 1.0 );
    vec4 viewVertex = uVMatrix * vWorldVertex;
    gl_Position = uPMatrix * viewVertex;
       
    vWorldNormal = normalize( mat3( uMMatrix ) * aNormal );
       
	if ( uUseNormalMap ) {
	  	vWorldTangent = normalize( mat3( uMMatrix ) * aTangent );
	   	vWorldBiTangent = cross( vWorldNormal, vWorldTangent );
	}
        
    vViewVec = normalize( -viewVertex.xyz );
       
	vTextureCoord = aTextureCoord;
        
    vPosition = ScaleMatrix * uLightSourceProjectionMatrix * uLightSourceViewMatrix * vWorldVertex;
}

)"

