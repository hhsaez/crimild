/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/Operations/Operations_computeRT.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Random.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Triangle.hpp"
#include "Mathematics/Triangle_edges.hpp"
#include "Mathematics/Triangle_normal.hpp"
#include "Mathematics/Vector3.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/PrincipledVolumeMaterial.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations_computeBuffer.hpp"
#include "Rendering/Operations/Operations_computeImage.hpp"
#include "Rendering/StorageBuffer.hpp"
#include "Rendering/Uniforms/CallbackUniformBuffer.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

/**
 * TODO:
 * There's a problem now based on how threads are grouped. Since they're tiled,
 * some threads might do less jobs than others. If one tile is too complex, it's still
 * handled by the same thread.
 *
 * Instead, each thread (gl_GlobalInvocationID.x) processes pixels using the following formula:
 * thread_id = mod( gl_GlobalInvocationID.x + R * N, M ), where:
 * R = random value
 * N = thread pool size (gl_WorkGroupSize * gl_WorkGroupCount )
 * M = Buffer size
 * (this formula might not be correct. I'll verify it when implementing it).
 *
 * Also, make the buffer linear instead of two-dimensional. That reduces the number of
 * computation needed to calculate the thread_id.
 *
 * This should make all threads to process both complex and simpler parts of the scene.
 *
 * I'm not sure if this is cache efficient, though.
 */

const std::string PRELUDE_SRC = R"(
    layout( local_size_x = 32, local_size_y = 32 ) in;
    layout( set = 0, binding = 0, rgba32f ) uniform image2D resultImage;

    #include <isZero>
    #include <random>
    #include <reflectance>

    struct RayData {
        vec3 origin;
        vec3 direction;
        vec3 sampleColor;
        vec3 accumColor;
        vec2 uv;
        int bounces;
        int samples;
    };

    layout( set = 1, binding = 0 ) buffer RayBounceState {
        RayData rays[];
    };
    layout( set = 1, binding = 1 ) uniform Uniforms {
        uint sampleCount;
        uint maxSamples;
        uint bounces;
        uint seedStart;
        mat4 cameraInvProj;
        mat4 cameraWorld;
        vec3 cameraOrigin;
        vec3 cameraRight;
        vec3 cameraUp;
        float cameraLensRadius;
        float cameraFocusDistance;
        vec3 backgroundColor;
    };
    layout( set = 1, binding = 2 ) uniform SceneUniforms {
        int sphereCount;
        int boxCount;
        int cylinderCount;
        int triangleCount;
        int materialCount;
    } uScene;

    struct Sphere {
        mat4 invWorld;
        uint materialID;
    };
    struct Box {
        mat4 invWorld;
        uint materialID;
    };
    struct Cylinder {
        mat4 invWorld;
        uint materialID;
    };
    struct Triangle {
        vec3 p0;
        vec3 p1;
        vec3 p2;
        vec3 e0;
        vec3 e1;
        vec3 n;
        uint materialID;
    };

    struct Material {
        int type;
        vec3 albedo;
        float metallic;
        float roughness;
        float transmission;
        float indexOfRefraction;
        vec3 emissive;
        float density;
    };

    layout( set = 2, binding = 0 ) buffer Spheres {
        Sphere allSpheres[];
    };
    layout( set = 2, binding = 1 ) buffer Boxes {
        Box allBoxes[];
    };
    layout( set = 2, binding = 2 ) buffer Cylinders {
        Cylinder allCylinders[];
    };
    layout( set = 2, binding = 3 ) buffer Triangles {
        Triangle allTriangles[];
    };
    layout( set = 2, binding = 4 ) buffer Materials {
        Material allMaterials[];
    };

    struct HitRecord {
        bool hasResult;
        float t;
        uint materialID;
        vec3 point;
        vec3 normal;
        bool frontFace;
    };

    struct Ray {
        vec3 origin;
        vec3 direction;
    };

    #include <swapsHandedness>

    HitRecord setFaceNormal( Ray ray, vec3 N, mat4 invWorld, HitRecord rec ) {
        rec.frontFace = dot( ray.direction, N ) < 0;
        rec.normal = rec.frontFace ? N : -N;
        if ( swapsHandedness( invWorld ) ) {
            rec.normal = -rec.normal;
        }
        return rec;
    }

    vec3 rayAt( Ray ray, float t ) {
        return ray.origin + t * ray.direction;
    }
)";

const auto SCATTER_SRC = R"(
    struct Scattered {
        bool hasResult;
        bool isEmissive;
        Ray ray;
        vec3 attenuation;
    };

    Scattered scatter( Material material, Ray ray, HitRecord rec ) {
        Scattered scattered;
        scattered.hasResult = false;
        scattered.isEmissive = false;

        if ( material.type == 1 ) {
scattered.ray.origin = rec.point;
scattered.ray.direction = getRandomInUnitSphere();
scattered.attenuation = material.albedo;
scattered.hasResult = true;
        } else if ( material.transmission > 0 ) {
            float ratio = rec.frontFace ? ( 1.0 / material.indexOfRefraction ) : material.indexOfRefraction;
            vec3 D = normalize( ray.direction );
            vec3 N = rec.normal;
            float cosTheta = min( dot( -D, N ), 1.0 );
            float sinTheta = sqrt( 1.0 - cosTheta * cosTheta );
            bool cannotRefract = ratio * sinTheta > 1.0;
            if ( cannotRefract || reflectance( cosTheta, ratio ) > getRandom() ) {
                D = reflect( D, N );
            } else {
                D = refract( D, N, ratio );
            }
            scattered.ray.origin = rec.point;
            scattered.ray.direction = D;
            scattered.attenuation = vec3( 1.0 );
            scattered.hasResult = true;
        } else if ( material.metallic > 0 ) {
            vec3 reflected = reflect( normalize( ray.direction ), rec.normal );
            scattered.ray.origin = rec.point;
            scattered.ray.direction = reflected + material.roughness * getRandomInUnitSphere();
            scattered.attenuation = material.albedo;
            scattered.hasResult = dot( scattered.ray.direction, rec.normal ) > 0.0;
        } else if ( !isZero( material.emissive ) ) {
            scattered.isEmissive = true;
            scattered.hasResult = true;
            scattered.attenuation = material.emissive;
        } else {
            vec3 scatterDirection = rec.normal + getRandomUnitVector();
            if ( isZero( scatterDirection ) ) {
                scatterDirection = rec.normal;
            }
            scattered.ray.origin = rec.point;
            scattered.ray.direction = scatterDirection;
            scattered.attenuation = material.albedo;
            scattered.hasResult = true;
        }

        return scattered;
    }
)";

const auto HIT_SCENE_SRC = R"(
    float hitSphere( Sphere sphere, Ray worldRay, float tMin, float tMax )
    {
        Ray ray;
        ray.origin = ( sphere.invWorld * vec4( worldRay.origin, 1.0 ) ).xyz;
        ray.direction = ( sphere.invWorld * vec4( worldRay.direction, 0.0 ) ).xyz;

        vec3 CO = ray.origin;
        float a = dot( ray.direction, ray.direction );
        float b = 2.0 * dot( ray.direction, CO );
        float c = dot( CO, CO ) - 1.0;

        float d = b * b - 4.0 * a * c;
        if ( d < 0 ) {
            return tMax;
        }

        float root = sqrt( d );
        float t = ( -b - root ) / ( 2.0 * a );
        if ( t < tMin || t > tMax ) {
            t = ( -b + root ) / ( 2.0 * a );
            if ( t < tMin || t > tMax ) {
                return tMax;
            }
        }

        return t;
    }

    float hitBox( Box box, Ray worldRay, float tInMin, float tInMax ) {
        Ray ray;
        ray.origin = ( box.invWorld * vec4( worldRay.origin, 1.0 ) ).xyz;
        ray.direction = ( box.invWorld * vec4( worldRay.direction, 0.0 ) ).xyz;

        vec3 bMin = vec3( -1, -1, -1 );
        vec3 bMax = vec3( 1, 1, 1 );

        float tMin = tInMin;
        float tMax = tInMax;

        for ( int a = 0; a < 3; a++ ) {
            float invD = 1.0 / ray.direction[ a ];
            float t0 = ( bMin[ a ] - ray.origin[ a ] ) * invD;
            float t1 = ( bMax[ a ] - ray.origin[ a ] ) * invD;
            if ( invD < 0.0 ) {
                float temp = t0;
                t0 = t1;
                t1 = temp;
            }

            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if ( tMax <= tMin ) {
                return tInMax;
            }
        }

        float t = 0.0;
        bool hasResult = false;

        if ( tMin >= 0.00001 ) {
            t = tMin;
            hasResult = true;
        }

        if ( abs( tMax - tMax ) > 0.00001 && tMax >= 0.00001 ) {
            t = tMax;
            hasResult = true;
        }

        if ( hasResult && tMin > 0 && tMax > tMin && allMaterials[ box.materialID ].type == 1 ) {
            hasResult = false;
                        float d = length( ray.direction );
                        float distanceInsideBoundary = ( tMax - tMin ) * d;
                        float density = allMaterials[ box.materialID ].density;
                        float negInvDensity = -1.0 / density;
                        float hitDistance = negInvDensity * log( getRandom() );
                        if ( hitDistance <= distanceInsideBoundary ) {
                            t = tMin + hitDistance / d;
                            hasResult = true;
                        }
        }

        return hasResult ? t : tInMax;
    }

    bool checkCylinderCap( Ray R, float t )
    {
        float x = R.origin.x + t * R.direction.x;
        float z = R.origin.z + t * R.direction.z;
        return ( x * x + z * z ) <= 1;
    }

    bool intersectCylinderCaps( Cylinder C, Ray R, inout float tMin, inout float tMax )
    {
        if ( isZero( R.direction.y ) ) {
            return false;
        }

        float cylinderHeight = 1;

        bool hasResult = false;
        float t = ( -cylinderHeight - R.origin.y ) / R.direction.y;
        if ( checkCylinderCap( R, t ) ) {
            if ( t < tMin ) {
                tMax = tMin;
                tMin = t;
            } else if ( t < tMax ) {
                tMax = t;
            }
            hasResult = true;
        }

        t = ( cylinderHeight - R.origin.y ) / R.direction.y;
        if ( checkCylinderCap( R, t ) ) {
            if ( t < tMin ) {
                tMax = tMin;
                tMin = t;
            } else if ( t < tMax ) {
                tMax = t;
            }
            hasResult = true;
        }

        return hasResult;
    }

    bool hitCylinder( Cylinder cylinder, Ray worldRay, inout float tMin, inout float tMax )
    {
        Ray ray;
        ray.origin = ( cylinder.invWorld * vec4( worldRay.origin, 1 ) ).xyz;
        ray.direction = ( cylinder.invWorld * vec4( worldRay.direction, 0 ) ).xyz;

        float cylinderHeight = 1;

        float a = ( ray.direction.x * ray.direction.x ) + ( ray.direction.z * ray.direction.z );
        if ( isZero( a ) ) {
            return intersectCylinderCaps( cylinder, ray, tMin, tMax );
        }

        float b = ( 2 * ray.origin.x * ray.direction.x ) + ( 2 * ray.origin.z * ray.direction.z );
        float c = ( ray.origin.x * ray.origin.x ) + ( ray.origin.z * ray.origin.z ) - 1;

        float disc = b * b - 4 * a * c;
        if ( disc < 0 ) {
            return false;
        }

        float disc_root = sqrt( disc );

        float t0 = ( -b - disc_root ) / ( 2.0 * a );
        float t1 = ( -b + disc_root ) / ( 2.0 * a );
        if ( t0 < t1 ) {
            float t = t0;
            t0 = t1;
            t1 = t;
        }

        bool hasResult = false;

        float y0 = ray.origin.y + t0 * ray.direction.y;
        if ( y0 < cylinderHeight && y0 > -cylinderHeight ) {
            tMin = t0;
            tMax = t1;
            hasResult = true;
        }

        float y1 = ray.origin.y + t1 * ray.direction.y;
        if ( y1 < cylinderHeight && y1 > -cylinderHeight ) {
            if ( !hasResult ) {
                tMin = t1;
            }
            tMax = t1;
            hasResult = true;
        }

        if ( intersectCylinderCaps( cylinder, ray, tMin, tMax ) ) {
            hasResult = true;
        }

        if ( tMin > tMax ) {
            float t = tMin;
            tMin = tMax;
            tMax = t;
        }

        return hasResult;
    }

    HitRecord hitSpheres( Ray ray, float tMin, HitRecord hit )
    {
        float t = hit.t;
        int hitIdx = -1;

        for ( int i = 0; i < uScene.sphereCount; i++ ) {
            float candidate = hitSphere( allSpheres[ i ], ray, tMin, t );
            if ( candidate < t ) {
                t = candidate;
                hitIdx = i;
            }
        }

        if ( hitIdx >= 0 ) {
            Sphere sphere = allSpheres[ hitIdx ];

            ray.origin = ( sphere.invWorld * vec4( ray.origin, 1.0 ) ).xyz;
            ray.direction = ( sphere.invWorld * vec4( ray.direction, 0.0 ) ).xyz;

            hit.hasResult = true;
            hit.t = t;
            hit.materialID = sphere.materialID;
            vec3 P = rayAt( ray, t );
            mat4 world = inverse( sphere.invWorld );
            hit.point = ( world * vec4( P, 1.0 ) ).xyz;
            vec3 normal = normalize( ( transpose( sphere.invWorld ) * vec4( normalize( P ), 0 ) ).xyz );
            return setFaceNormal( ray, normal, sphere.invWorld, hit );
        }

        return hit;
    }

    #include <max>

    HitRecord hitBoxes( Ray ray, float tMin, HitRecord hit )
    {
        float t = hit.t;
        int hitIdx = -1;

        for ( int i = 0; i < uScene.boxCount; i++ ) {
            float candidate = hitBox( allBoxes[ i ], ray, tMin, t );
            if ( candidate < t ) {
                t = candidate;
                hitIdx = i;
            }
        }

        if ( hitIdx >= 0 ) {
            Box box = allBoxes[ hitIdx ];

            ray.origin = ( box.invWorld * vec4( ray.origin, 1.0 ) ).xyz;
            ray.direction = ( box.invWorld * vec4( ray.direction, 0.0 ) ).xyz;

            hit.hasResult = true;
            hit.t = t;
            hit.materialID = box.materialID;
            vec3 P = rayAt( ray, t );
            mat4 world = inverse( box.invWorld );
            hit.point = ( world * vec4( P, 1.0 ) ).xyz;

            int i = maxDimension( abs( P ) );
            vec3 normal = vec3( 0 );
            normal[ i ] = P[ i ] > 0 ? 1 : -1;
            normal = normalize( ( transpose( box.invWorld ) * vec4( normal, 0 ) ).xyz );

            return setFaceNormal( ray, normal, box.invWorld, hit );
        }

        return hit;
    }

    HitRecord hitCylinders( Ray ray, float tMin, HitRecord hit )
    {
        float t = hit.t;
        int hitIdx = -1;

        for ( int i = 0; i < uScene.cylinderCount; i++ ) {
            float t0 = 999999.9;
            float t1 = 999999.9;
            if ( hitCylinder( allCylinders[ i ], ray, t0, t1 ) ) {
                if ( t0 < t ) {
                    t = t0;
                    hitIdx = i;
                }

                if ( t1 < t ) {
                    t = t1;
                    hitIdx = i;
                }
            }
        }

        if ( hitIdx >= 0 ) {
            Cylinder cylinder = allCylinders[ hitIdx ];

            ray.origin = ( cylinder.invWorld * vec4( ray.origin, 1.0 ) ).xyz;
            ray.direction = ( cylinder.invWorld * vec4( ray.direction, 0.0 ) ).xyz;

            hit.hasResult = true;
            hit.t = t;
            hit.materialID = cylinder.materialID;
            vec3 P = rayAt( ray, t );
            mat4 world = inverse( cylinder.invWorld );
            hit.point = ( world * vec4( P, 1.0 ) ).xyz;
            vec3 normal = normalize( transpose( mat3( cylinder.invWorld ) ) * P );
            return setFaceNormal( ray, normal, cylinder.invWorld, hit );
        }

        return hit;
    }

    HitRecord hitTriangles( Ray ray, float tMin, HitRecord hit )
    {
        float t = hit.t;
        int hitIdx = -1;

        for ( int i = 0; i < uScene.triangleCount; i++ ) {
vec3 e0 = allTriangles[ i ].e0;
vec3 e1 = allTriangles[ i ].e1;
vec3 p0 = allTriangles[ i ].p0;

vec3 dirCrossE1 = cross( ray.direction, e1 );
float det = dot( e0, dirCrossE1 );
if ( det > 0 ) {
   float f = 1.0 / det;

   vec3 p0ToOrigin = ray.origin - p0;
   float u = f * dot( p0ToOrigin, dirCrossE1 );
   if ( u >= 0 && u <= 1 ) {
      vec3 originCrossE0 = cross( p0ToOrigin, e0 );
      float v = f * dot( ray.direction, originCrossE0 );
      if ( v >= 0 && ( u + v ) <= 1 ) {
         float ct = f * dot( e1, originCrossE0 );
         if ( !isnan( ct ) && ct > 0 && ct < t ) {
             t = ct;
             hitIdx = i;
         }
      }
   }
}
        }

        if ( hitIdx >= 0 ) {
            Triangle triangle = allTriangles[ hitIdx ];
            hit.hasResult = true;
            hit.t = t;
            hit.materialID = triangle.materialID;
            vec3 P = rayAt( ray, t );
            hit.point = P;
            vec3 normal = triangle.n;
            return setFaceNormal( ray, normal, mat4( 1 ), hit );
        }

        return hit;
    }

    HitRecord hitScene( Ray ray, float tMin, float tMax ) {
        HitRecord hit;
        hit.t = tMax;
        hit.hasResult = false;

        hit = hitSpheres( ray, tMin, hit );
        hit = hitBoxes( ray, tMin, hit );
        hit = hitCylinders( ray, tMin, hit );
        hit = hitTriangles( ray, tMin, hit );

        return hit;
    }

    Ray getCameraRay( float u, float v ) {
        float x = 2.0 * u - 1.0;
        float y = 2.0 * v - 1.0;
        vec4 rayClip = vec4( x, y, -1, 1 );

        vec4 rayEye = cameraInvProj * rayClip;
        vec3 cameraForward = mat3( cameraWorld ) * rayEye.xyz;

        vec3 rd = cameraLensRadius * getRandomInUnitDisc();
        vec3 offset = cameraRight * rd.x + cameraUp * rd.y;

        Ray ray;
        ray.origin = cameraOrigin + offset;
        ray.direction = cameraFocusDistance * cameraForward - offset;

        return ray;
    }
)";

const auto BOUNCE_SRC = R"(
    ivec2 getTileSize()
    {
        ivec2 M = ivec2( gl_NumWorkGroups.xy * gl_WorkGroupSize.xy );
        ivec2 N = imageSize( resultImage );
        N = ivec2( ceil( vec2( N ) / vec2( M ) ) * vec2( M ) );
        ivec2 T = N / M;
        return T;
    }

    int getNextRayIndexInTile()
    {
        ivec2 S = imageSize( resultImage );
        ivec2 T = getTileSize();
        ivec2 uv = ivec2( gl_GlobalInvocationID.xy ) * T;
        if ( uv.x >= S.x || uv.y >= S.y ) {
            return -1;
        }
        uv += ivec2( int( getRandom() * T.x ), int( getRandom() * T.y ) );
        if ( uv.x >= S.x || uv.y >= S.y ) {
            return -1;
        }
        return uv.y * S.x + uv.x;
    }

    // Reset all samples for all rays in this tile
    void resetSample( uint idx )
    {
        ivec2 S = imageSize( resultImage );
        ivec2 T = getTileSize();

        ivec2 uv = ivec2( gl_GlobalInvocationID.xy * T );
        if ( uv.x >= S.x || uv.y >= S.y ) {
            return;
        }

        uint firstIdx = uv.y * S.x + uv.x;
        for ( float y = 0; y < T.y; y++ ) {
            for ( float x = 0; x < T.x; x++ ) {
                ivec2 st = uv + ivec2( x, y );
                if ( st.x >= S.x || st.y >= S.y ) {
                    continue;
                } else {
                    uint idx = st.y * S.x + st.x;
                    rays[ idx ].sampleColor = vec3( 1 );
                    rays[ idx ].accumColor = vec3( 0 );
                    rays[ idx ].bounces = 0;
                    rays[ idx ].samples = 0;
                    imageStore( resultImage, ivec2( rays[ idx ].uv ), vec4( 0, 0, 0, 1 ) );
                }
            }
        }
    }

    Ray getNextRay( uint idx )
    {
        Ray ret;
        if ( rays[ idx ].bounces == 0 ) {
            vec2 size = imageSize( resultImage );
            vec2 uv = rays[ idx ].uv;
            uv += vec2( getRandom(), getRandom() );
            uv /= ( size.xy - vec2( 1 ) );
            uv.y = 1.0 - uv.y;
            uv = clamp( uv, 0, 1 );
            ret = getCameraRay( uv.x, uv.y );
        } else {
            ret.origin = rays[ idx ].origin;
            ret.direction = rays[ idx ].direction;
        }
        return ret;
    }

    void onSampleCompleted( uint idx )
    {
        rays[ idx ].accumColor += rays[ idx ].sampleColor;
        rays[ idx ].samples++;

        vec3 color = rays[ idx ].accumColor / float( rays[ idx ].samples );

        vec2 size = imageSize( resultImage );

        imageStore( resultImage, ivec2( rays[ idx ].uv ), vec4( color, 1.0 ) );

        // reset
        rays[ idx ].sampleColor = vec3( 1 );
        rays[ idx ].bounces = 0;
    }

    void doSampleBounce( uint idx, Ray ray )
    {
        float tMin = 0.001;
        float tMax = 9999.9;


        if ( rays[ idx ].bounces > 50 ) {
            rays[ idx ].sampleColor = vec3( 0 );
            onSampleCompleted( idx );
            return;
        }

        HitRecord hit = hitScene( ray, tMin, tMax );
        if ( !hit.hasResult ) {
            // no hit. use background color
            rays[ idx ].sampleColor *= backgroundColor;
            onSampleCompleted( idx );
            return;
        }

        Scattered scattered = scatter( allMaterials[ hit.materialID ], ray, hit );
        if ( scattered.hasResult ) {
            rays[ idx ].sampleColor *= scattered.attenuation;
            if ( scattered.isEmissive ) {
                onSampleCompleted( idx );
            } else {
                rays[ idx ].origin = scattered.ray.origin;
                rays[ idx ].direction = scattered.ray.direction;
                rays[ idx ].bounces++;
            }
        } else {
            rays[ idx ].sampleColor = vec3( 0 );
            onSampleCompleted( idx );
        }
    }

    void main() {
        initRandom( int( seedStart ) );

        vec2 size = imageSize( resultImage );
        float aspectRatio = size.x / size.y;

        int rayIdx = getNextRayIndexInTile();
        if ( rayIdx < 0 ) {
           return;
        }

        if ( sampleCount == 1 ) {
            // reset all samples
            resetSample( rayIdx );
        }

        Ray ray = getNextRay( rayIdx );
        doSampleBounce( rayIdx, ray );
    }
)";

SharedPointer< FrameGraphOperation > crimild::framegraph::computeRT( void ) noexcept
{
    const Real resolutionScale = Simulation::getInstance()->getSettings()->get< Real >( "rt.scale", 1 );
    const int width = resolutionScale * Simulation::getInstance()->getSettings()->get< Int32 >( "video.width", 1024 );
    const int height = resolutionScale * Simulation::getInstance()->getSettings()->get< Int32 >( "video.height", 768 );

    // Reset samples
    Simulation::getInstance()->getSettings()->set( "rt.samples.count", 0 );

    struct SphereDesc {
        alignas( 16 ) Matrix4 invWorld;
        alignas( 4 ) UInt32 materialID;
    };

    struct BoxDesc {
        alignas( 16 ) Matrix4 invWorld;
        alignas( 4 ) UInt32 materialID;
    };

    struct CylinderDesc {
        alignas( 16 ) Matrix4 invWorld;
        alignas( 4 ) UInt32 materialID;
    };

    // Triangles are always in world space
    struct TriangleDesc {
        alignas( 16 ) Point3f p0;
        alignas( 16 ) Point3f p1;
        alignas( 16 ) Point3f p2;
        alignas( 16 ) Vector3 e0;
        alignas( 16 ) Vector3 e1;
        alignas( 16 ) Normal3 n;
        alignas( 4 ) UInt32 materialID;
    };

    struct MaterialProps {
        alignas( 4 ) Int32 type = 0; // 0: BSDF, 1: Volume
        alignas( 16 ) ColorRGB albedo = ColorRGB::Constants::WHITE;
        alignas( 4 ) Real32 metallic = 0;
        alignas( 4 ) Real32 roughness = 0;
        alignas( 4 ) Real32 transmission = 0;
        alignas( 4 ) Real32 indexOfRefraction = 0;
        alignas( 16 ) ColorRGB emissive = ColorRGB::Constants::BLACK;
        alignas( 4 ) Real32 density = 1;
    };

    Array< SphereDesc > spheres;
    Array< BoxDesc > boxes;
    Array< CylinderDesc > cylinders;
    Array< TriangleDesc > triangles;
    Array< MaterialProps > materials;
    Map< Material *, UInt32 > materialIds;

    auto scene = Simulation::getInstance()->getScene();
    if ( scene != nullptr ) {
        scene->perform( UpdateWorldState() );
        scene->perform(
            ApplyToGeometries(
                [ & ]( Geometry *geometry ) {
                    if ( geometry->getLayer() == Node::Layer::SKYBOX ) {
                        return;
                    }

                    auto material = geometry->getComponent< MaterialComponent >()->first();
                    if ( material == nullptr ) {
                        return;
                    }

                    Bool isVolume = material->getClassName() == materials::PrincipledVolume::__CLASS_NAME;

                    if ( !materialIds.contains( material ) ) {
                        const UInt32 materialId = materials.size();
                        if ( isVolume ) {
                            const auto &props = static_cast< materials::PrincipledVolume * >( material )->getProps();
                            materials.add(
                                MaterialProps {
                                    .type = 1,
                                    .albedo = props.albedo,
                                    .density = props.density,
                                }
                            );
                        } else {
                            const auto &props = static_cast< materials::PrincipledBSDF * >( material )->getProps();
                            materials.add(
                                MaterialProps {
                                    .type = 0,
                                    .albedo = props.albedo,
                                    .metallic = props.metallic,
                                    .roughness = props.roughness,
                                    .transmission = props.transmission,
                                    .indexOfRefraction = props.indexOfRefraction,
                                    .emissive = props.emissive,
                                }
                            );
                        }
                        materialIds.insert( material, materialId );
                    }

                    geometry->forEachPrimitive(
                        [ & ]( auto primitive ) {
                            if ( primitive->getType() == Primitive::Type::SPHERE ) {
                                spheres.add(
                                    {
                                        .invWorld = geometry->getWorld().invMat,
                                        .materialID = materialIds[ material ],
                                    }
                                );
                            } else if ( primitive->getType() == Primitive::Type::BOX ) {
                                boxes.add(
                                    {
                                        .invWorld = geometry->getWorld().invMat,
                                        .materialID = materialIds[ material ],
                                    }
                                );
                            } else if ( primitive->getType() == Primitive::Type::CYLINDER ) {
                                cylinders.add(
                                    {
                                        .invWorld = geometry->getWorld().invMat,
                                        .materialID = materialIds[ material ],
                                    }
                                );
                            } else if ( primitive->getType() == Primitive::Type::TRIANGLES ) {
                                auto positions = [ & ] {
                                    BufferAccessor *positions = nullptr;
                                    primitive->getVertexData().each(
                                        [ & ]( auto vertices ) {
                                            if ( positions == nullptr ) {
                                                positions = vertices->get( VertexAttribute::Name::POSITION );
                                            }
                                        }
                                    );
                                    return positions;
                                }();

                                if ( positions == nullptr ) {
                                    return;
                                }

                                if ( auto indices = primitive->getIndices() ) {
                                    const auto N = indices->getIndexCount();
                                    for ( auto i = 0; i < N; i += 3 ) {
                                        const auto T = Triangle {
                                            .p0 = geometry->getWorld()( positions->template get< Point3f >( indices->getIndex( i + 0 ) ) ),
                                            .p1 = geometry->getWorld()( positions->template get< Point3f >( indices->getIndex( i + 1 ) ) ),
                                            .p2 = geometry->getWorld()( positions->template get< Point3f >( indices->getIndex( i + 2 ) ) ),
                                        };

                                        triangles.add( {
                                            .p0 = T.p0,
                                            .p1 = T.p1,
                                            .p2 = T.p2,
                                            .e0 = edge0( T ),
                                            .e1 = edge1( T ),
                                            .n = normal( T, T.p0 ),
                                            .materialID = materialIds[ material ],
                                        } );
                                    }
                                }
                            }
                        }
                    );
                }
            )
        );
    }

    auto ds = crimild::alloc< DescriptorSet >();
    ds->descriptors = Array< Descriptor > {
        Descriptor {
            .descriptorType = DescriptorType::STORAGE_BUFFER,
            .obj = [ & ] {
                struct BufferData {
                    alignas( 16 ) Point3f origin = Point3f { 0, 0, 0 };
                    alignas( 16 ) Vector3 direction = Vector3 { 0, 0, 0 };
                    alignas( 16 ) ColorRGB sampleColor = ColorRGB { 1, 1, 1 };
                    alignas( 16 ) ColorRGB accumColor = ColorRGB { 0, 0, 0 };
                    alignas( 16 ) Vector2f uv = Vector2f { 0, 0 };
                    alignas( 4 ) Int32 bounces = 0;
                    alignas( 4 ) Int32 samples = 0;
                };

                auto data = Array< BufferData >( width * height );
                for ( auto y = 0; y < height; y++ ) {
                    for ( auto x = 0; x < width; x++ ) {
                        data[ y * width + x ].uv = Vector2 { Real( x ), Real( y ) };
                    }
                }
                return crimild::alloc< StorageBuffer >( data );
            }(),
        },
        Descriptor {
            .descriptorType = DescriptorType::UNIFORM_BUFFER,
            .obj = [] {
                struct Uniforms {
                    alignas( 4 ) UInt32 sampleCount;
                    alignas( 4 ) UInt32 maxSamples;
                    alignas( 4 ) UInt32 bounces;
                    alignas( 4 ) UInt32 seed;
                    alignas( 16 ) Matrix4 cameraInvProj;
                    alignas( 16 ) Matrix4 cameraWorld;
                    alignas( 16 ) Point3f cameraOrigin;
                    alignas( 16 ) Vector3 cameraRight;
                    alignas( 16 ) Vector3 cameraUp;
                    alignas( 4 ) Real32 cameraLensRadius;
                    alignas( 4 ) Real32 cameraFocusDistance;
                    alignas( 16 ) ColorRGB backgroundColor;
                };

                return crimild::alloc< CallbackUniformBuffer< Uniforms > >(
                    [] {
                        auto settings = Simulation::getInstance()->getSettings();

                        auto maxSamples = settings->get< UInt32 >( "rt.samples.max", 5000 );
                        auto sampleCount = settings->get< UInt32 >( "rt.samples.count", 1 );
                        auto bounces = UInt32( 1 );                                           // settings->get< UInt32 >( "rt.bounces", 10 );
                        auto focusDist = settings->get< Real >( "rt.focusDist", Real( 10 ) ); // move to camera
                        auto aperture = settings->get< Real >( "rt.aperture", Real( 0.1 ) );  // move to camera
                        auto backgroundColor = ColorRGB {
                            settings->get< Real >( "rt.background_color.r", 0.5f ),
                            settings->get< Real >( "rt.background_color.g", 0.7f ),
                            settings->get< Real >( "rt.background_color.b", 1.0f ),
                        };

                        // Update sample count
                        if ( sampleCount < maxSamples ) {
                            sampleCount += 1;
                        }

                        static auto cameraInvProj = Matrix4::Constants::IDENTITY;
                        static auto cameraWorld = Matrix4::Constants::IDENTITY;

                        auto cameraOrigin = Point3f::ZERO;
                        auto cameraRight = Vector3::Constants::RIGHT;
                        auto cameraUp = Vector3::Constants::UP;

                        // auto camera = Camera::getMainCamera();
                        Camera *camera = nullptr;
                        if ( camera != nullptr ) {
                            const auto invProj = inverse( camera->getProjectionMatrix() );
                            if ( invProj != cameraInvProj ) {
                                cameraInvProj = invProj;
                                sampleCount = 1; // reset sampling
                            }

                            const auto &world = camera->getWorld().mat;
                            if ( cameraWorld != world ) {
                                cameraWorld = world;
                                sampleCount = 1; // reset sampling
                            }

                            cameraOrigin = location( camera->getWorld() );
                            cameraRight = right( camera->getWorld() );
                            cameraUp = up( camera->getWorld() );

                            aperture = camera->getAperture();
                            focusDist = camera->getFocusDistance();
                        }

                        if ( Input::getInstance()->isMouseButtonDown( CRIMILD_INPUT_MOUSE_BUTTON_RIGHT ) ) {
                            sampleCount = 1; // reset sampling
                            bounces = 1;     // only one bounce during interaction
                        }

                        settings->set( "rt.samples.count", sampleCount );

                        static UInt32 seed = 0;

                        return Uniforms {
                            .sampleCount = sampleCount,
                            .maxSamples = maxSamples,
                            .bounces = bounces,
                            .seed = seed++, // Random::generate< UInt32 >( 0, 1000 ),
                            .cameraInvProj = cameraInvProj,
                            .cameraWorld = cameraWorld,
                            .cameraOrigin = cameraOrigin,
                            .cameraRight = cameraRight,
                            .cameraUp = cameraUp,
                            .cameraLensRadius = Real32( 0.5 ) * aperture,
                            .cameraFocusDistance = focusDist,
                            .backgroundColor = backgroundColor,
                        };
                    }
                );
            }(),
        },
        Descriptor {
            .descriptorType = DescriptorType::UNIFORM_BUFFER,
            .obj = [ & ] {
                struct SceneUniforms {
                    alignas( 4 ) UInt32 sphereCount = 0;
                    alignas( 4 ) UInt32 boxCount = 0;
                    alignas( 4 ) UInt32 cylinderCount = 0;
                    alignas( 4 ) UInt32 triangleCount = 0;
                    alignas( 4 ) UInt32 materialCount = 0;
                };

                return crimild::alloc< UniformBuffer >( SceneUniforms {
                    .sphereCount = UInt32( spheres.size() ),
                    .boxCount = UInt32( boxes.size() ),
                    .cylinderCount = UInt32( cylinders.size() ),
                    .triangleCount = UInt32( triangles.size() ),
                    .materialCount = UInt32( materials.size() ),
                } );
            }(),
        },
    };

    auto sceneDescriptors = crimild::alloc< DescriptorSet >();
    sceneDescriptors->descriptors = Array< Descriptor > {
        Descriptor {
            .descriptorType = DescriptorType::STORAGE_BUFFER,
            .obj = crimild::alloc< StorageBuffer >( spheres.size() > 0 ? spheres : Array< SphereDesc > { SphereDesc {} } ),
        },
        Descriptor {
            .descriptorType = DescriptorType::STORAGE_BUFFER,
            .obj = crimild::alloc< StorageBuffer >( boxes.size() > 0 ? boxes : Array< BoxDesc > { BoxDesc {} } ),
        },
        Descriptor {
            .descriptorType = DescriptorType::STORAGE_BUFFER,
            .obj = crimild::alloc< StorageBuffer >( cylinders.size() > 0 ? cylinders : Array< CylinderDesc > { CylinderDesc {} } ),
        },
        Descriptor {
            .descriptorType = DescriptorType::STORAGE_BUFFER,
            .obj = crimild::alloc< StorageBuffer >( triangles.size() > 0 ? triangles : Array< TriangleDesc > { TriangleDesc {} } ),
        },
        Descriptor {
            .descriptorType = DescriptorType::STORAGE_BUFFER,
            .obj = crimild::alloc< StorageBuffer >( materials.size() > 0 ? materials : Array< MaterialProps > { MaterialProps {} } ),
        },
    };

    auto workgroup = [ width, height ] {
        auto settings = Simulation::getInstance()->getSettings();
        auto workers = settings->get< UInt32 >( "rt.workers", 4 );

        const UInt32 WORKGROUP_SIZE = 32;
        const UInt32 MAX_X = ceil( width / WORKGROUP_SIZE );
        const UInt32 MAX_Y = ceil( height / WORKGROUP_SIZE );

        return DispatchWorkgroup {
            .x = std::min( workers, MAX_X ),
            .y = std::min( workers, MAX_Y ),
            .z = 1,
        };
    }();

    Simulation::getInstance()->getSettings()->set( "rt.workgroup.x", workgroup.x );
    Simulation::getInstance()->getSettings()->set( "rt.workgroup.y", workgroup.y );
    Simulation::getInstance()->getSettings()->set( "rt.workgroup.z", workgroup.z );

    return computeImage(
        Extent2D {
            .width = Real32( width ),
            .height = Real32( height ),
        },
        crimild::alloc< Shader >(
            Shader::Stage::COMPUTE,
            PRELUDE_SRC + SCATTER_SRC + HIT_SCENE_SRC + BOUNCE_SRC
        ),
        Format::R32G32B32A32_SFLOAT,
        workgroup,
        { ds, sceneDescriptors }
    );
}
