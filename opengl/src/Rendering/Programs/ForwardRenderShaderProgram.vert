R"(

// The scale matrix is used to push the projected vertex into the 0.0 - 1.0 region.
const mat4 ScaleMatrix = mat4( 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0 );
                                             
in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec2 aTextureCoord;
   
uniform mat4 uPMatrix;
uniform mat4 uVMatrix;
uniform mat4 uMMatrix;
uniform mat4 uLightSourceProjectionMatrix;
uniform mat4 uLightSourceViewMatrix;
   
uniform bool uUseNormalMap;
                                             
out vec4 vWorldVertex;
out vec3 vWorldNormal;
out vec3 vWorldTangent;
out vec3 vWorldBiTangent;
out vec2 vTextureCoord;
out vec3 vViewVec;
out vec4 vPosition;

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

