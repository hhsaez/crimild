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
#include "Mathematics/Matrix4_equality.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Random.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Vector3_constants.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations_computeBuffer.hpp"
#include "Rendering/Operations/Operations_computeImage.hpp"
#include "Rendering/StorageBuffer.hpp"
#include "Rendering/Uniforms/CallbackUniformBuffer.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

// - Ray intersector: each frame, computes one ray per job
// - New rays are created either from the camera or from a bounce
// - When bounces end, color is accumulated and the image is updated

// TILE BASED COMPUTE RT

#if 1

const auto FRAG_SRC = R"(

    layout( local_size_x = 32, local_size_y = 32 ) in;
    layout( set = 0, binding = 0, rgba32f ) uniform image2D resultImage;

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

    uint seed = 0;
    int flat_idx = 0;

    layout( set = 1, binding = 2 ) uniform SceneUniforms {
        int sphereCount;
        int boxCount;
        int cylinderCount;
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

    struct Material {
        vec3 albedo;
        float metallic;
        float roughness;
        float ambientOcclusion;
        float transmission;
        float indexOfRefraction;
        vec3 emissive;
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

    layout( set = 2, binding = 3 ) buffer Materials {
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

    void encrypt_tea( inout uvec2 arg ) {
        uvec4 key = uvec4( 0xa341316c, 0xc8013ea4, 0xad90777d, 0x7e95761e );
        uint v0 = arg[ 0 ], v1 = arg[ 1 ];
        uint sum = 0u;
        uint delta = 0x9e3779b9u;

        for ( int i = 0; i < 32; i++ ) {
            sum += delta;
            v0 += ( ( v1 << 4 ) + key[ 0 ] ) ^ ( v1 + sum ) ^ ( ( v1 >> 5 ) + key[ 1 ] );
            v1 += ( ( v0 << 4 ) + key[ 2 ] ) ^ ( v0 + sum ) ^ ( ( v0 >> 5 ) + key[ 3 ] );
        }
        arg[ 0 ] = v0;
        arg[ 1 ] = v1;
    }

    float getRandom() {
        uvec2 arg = uvec2( flat_idx, seed++ );
        encrypt_tea( arg );
        vec2 r = fract( vec2( arg ) / vec2( 0xffffffffu ) );
        return r.x;
    }

    float getRandomRange( float min, float max ) {
        return min + getRandom() * ( max - min );
    }

    vec3 getRandomVec3() {
        return vec3(
            getRandom(),
            getRandom(),
            getRandom() );
    }

    vec3 getRandomVec3Range( float min, float max ) {
        return vec3(
            getRandomRange( min, max ),
            getRandomRange( min, max ),
            getRandomRange( min, max ) );
    }

    vec3 getRandomInUnitSphere() {
        while ( true ) {
            vec3 p = getRandomVec3Range( -1.0, 1.0 );
            if ( dot( p, p ) < 1.0 ) {
                return p;
            }
        }
        return vec3( 0 );
    }

    vec3 getRandomInUnitDisc() {
        while ( true ) {
            vec3 p = vec3(
                getRandomRange( -1.0, 1.0 ),
                getRandomRange( -1.0, 1.0 ),
                0.0 );
            if ( dot( p, p ) >= 1.0 ) {
                break;
            }
            return p;
        }
    }

    vec3 getRandomUnitVector() {
        return normalize( getRandomInUnitSphere() );
    }

    vec3 getRandomInHemisphere( vec3 N ) {
        vec3 inUnitSphere = getRandomInUnitSphere();
        if ( dot( inUnitSphere, N ) > 0.0 ) {
            return inUnitSphere;
        } else {
            return -inUnitSphere;
        }
    }

    HitRecord setFaceNormal( Ray ray, vec3 N, HitRecord rec ) {
        rec.frontFace = dot( ray.direction, N ) < 0;
        rec.normal = rec.frontFace ? N : -N;
        return rec;
    }

    vec3 rayAt( Ray ray, float t ) {
        return ray.origin + t * ray.direction;
    }

    struct Scattered {
        bool hasResult;
        bool isEmissive;
        Ray ray;
        vec3 attenuation;
    };

    bool isZero( vec3 v ) {
        float s = 0.00001;
        return abs( v.x ) < s && abs( v.y ) < s && abs( v.z ) < s;
    }

    float reflectance( float cosine, float refIdx ) {
        float r0 = ( 1.0 - refIdx ) / ( 1.0 + refIdx );
        r0 = r0 * r0;
        return r0 + ( 1.0 - r0 ) * pow( ( 1.0 - cosine ), 5.0 );
    }

    Scattered scatter( Material material, Ray ray, HitRecord rec ) {
        Scattered scattered;
        scattered.hasResult = false;
        scattered.isEmissive = false;

        if ( material.transmission > 0 ) {
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

        return hasResult ? t : tInMax;
    }

    bool isZero( float x ) 
    {
        float EPSILON = 0.000001;
        return abs( x ) < EPSILON;
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
            vec3 normal = normalize( transpose( mat3( sphere.invWorld ) ) * P );
            return setFaceNormal( ray, normal, hit );
        }

        return hit;
    }

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
            vec3 normal = normalize( transpose( mat3( box.invWorld ) ) * P );
            return setFaceNormal( ray, normal, hit );
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
            return setFaceNormal( ray, normal, hit );
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

    vec3 rayColor( Ray ray ) {
        float multiplier = 1.0;
        float tMin = 0.001;
        float tMax = 9999.9;
        vec3 attenuation = vec3( 1.0 );

        vec3 color = vec3( 1.0 );

        int depth = 0;
        while ( depth < 10 ) {
            HitRecord hit = hitScene( ray, tMin, tMax );
            if ( !hit.hasResult ) {
                // no hit. use background color
                color *= backgroundColor;
                return color;
            }

            Scattered scattered = scatter( allMaterials[ hit.materialID ], ray, hit );
            if ( scattered.hasResult ) {
                color *= scattered.attenuation;
                if ( scattered.isEmissive ) {
                    // no need to continue, just return the color
                    return color;
                }
                color *= scattered.attenuation;
                ray = scattered.ray;
                ++depth;
            } else {
                return vec3( 0 );
            }
        }

        // never happens
        return vec3( 0 );
    }

    uint getNextRayIndexInTile()
    {
        vec2 size = imageSize( resultImage );
        vec2 tileSize = size / ( gl_NumWorkGroups.xy * gl_WorkGroupSize.xy );
        vec2 uv = gl_GlobalInvocationID.xy * tileSize;
        uv += vec2( getRandom(), getRandom() ) * tileSize;
        uv = min( uv, size );
        return uint( uv.y * size.x + uv.x );
    }

    // Reset all samples for all rays in this tile        
    void resetSample( uint idx )
    {
        vec2 size = imageSize( resultImage );
        vec2 tileSize = size / ( gl_NumWorkGroups.xy * gl_WorkGroupSize.xy );
        vec2 uv = gl_GlobalInvocationID.xy * tileSize;
        uv = min( uv, size );
        uint firstIdx = uint( uv.y * size.x + uv.x );

        for ( float y = 0; y < tileSize.y; y++ ) {
            for ( float x = 0; x < tileSize.x; x++ ) {
                vec2 st = uv + vec2( x, y );
                st = min( st, size );
                uint idx = uint( st.y * size.x + st.x );;
                rays[ idx ].sampleColor = vec3( 1 );
                rays[ idx ].accumColor = vec3( 0 );
                rays[ idx ].bounces = 0;
                rays[ idx ].samples = 0;
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

        imageStore( resultImage, ivec2( rays[ idx ].uv ), vec4( color, 1.0 ) );

        // reset
        rays[ idx ].sampleColor = vec3( 1 );
        rays[ idx ].bounces = 0;
    }

    void doSampleBounce( uint idx, Ray ray )
    {
        float tMin = 0.001;
        float tMax = 9999.9;

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
            if ( scattered.isEmissive || rays[ idx ].bounces >= 4 ) {
                onSampleCompleted( idx );
            } else {
                rays[ idx ].origin = scattered.ray.origin;
                rays[ idx ].direction = scattered.ray.direction;
                rays[ idx ].bounces = rays[ idx ].bounces + 1;
            }
        } else {
            rays[ idx ].sampleColor = vec3( 0 );
            onSampleCompleted( idx );
        }
    }

    void main() {
        seed = seedStart;

        // if ( sampleCount >= maxSamples ) {
        //     return;
        // }

        flat_idx = int( dot( gl_GlobalInvocationID.xy, vec2( 1, 4096 ) ) );

        vec2 size = imageSize( resultImage );
        float aspectRatio = size.x / size.y;

        if ( gl_GlobalInvocationID.x >= size.x || gl_GlobalInvocationID.y >= size.y ) {
            return;
        }

#if 1
        uint rayIdx = getNextRayIndexInTile();
        
        if ( sampleCount == 0 ) {
            // reset all samples
            resetSample( rayIdx );
            return;
        }

        Ray ray = getNextRay( rayIdx );
        doSampleBounce( rayIdx, ray );
#else
        if ( sampleCount == 0 ) {
            // reset all samples
            imageStore( resultImage, ivec2( gl_GlobalInvocationID.xy ), vec4( vec3( 0 ), 1.0 ) );
            return;
        }

        vec2 uv = gl_GlobalInvocationID.xy;
        uv += vec2( getRandom(), getRandom() );
        uv /= ( size.xy - vec2( 1 ) );
        uv.y = 1.0 - uv.y;
        Ray ray = getCameraRay( uv.x, uv.y );
        vec3 color = rayColor( ray );

        vec3 destinationColor = imageLoad( resultImage, ivec2( gl_GlobalInvocationID.xy ) ).rgb;
        color = ( destinationColor * float( sampleCount - 1 ) + color ) / float( sampleCount );

        if ( sampleCount == 0 ) {
            color = vec3( 0 );
        }

        imageStore( resultImage, ivec2( gl_GlobalInvocationID.xy ), vec4( color, 1.0 ) );
#endif
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

    Array< SphereDesc > spheres;
    Array< BoxDesc > boxes;
    Array< CylinderDesc > cylinders;
    Array< materials::PrincipledBSDF::Props > materials;
    Map< Material *, UInt32 > materialIds;

    auto scene = Simulation::getInstance()->getScene();
    if ( scene != nullptr ) {
        scene->perform( UpdateWorldState() );
        scene->perform(
            ApplyToGeometries(
                [ & ]( Geometry *geometry ) {
                    const auto material = static_cast< materials::PrincipledBSDF * >( geometry->getComponent< MaterialComponent >()->first() );
                    if ( !materialIds.contains( material ) ) {
                        const UInt32 materialId = materials.size();
                        materials.add( material->getProps() );
                        materialIds.insert( material, materialId );
                    }

                    geometry->forEachPrimitive(
                        [ & ]( auto primitive ) {
                            if ( primitive->getType() == Primitive::Type::SPHERE ) {
                                spheres.add(
                                    {
                                        .invWorld = geometry->getWorld().invMat,
                                        .materialID = materialIds[ material ],
                                    } );
                            } else if ( primitive->getType() == Primitive::Type::BOX ) {
                                boxes.add(
                                    {
                                        .invWorld = geometry->getWorld().invMat,
                                        .materialID = materialIds[ material ],
                                    } );
                            } else if ( primitive->getType() == Primitive::Type::CYLINDER ) {
                                cylinders.add(
                                    {
                                        .invWorld = geometry->getWorld().invMat,
                                        .materialID = materialIds[ material ],
                                    } );
                            }
                        } );
                } ) );
    }

    auto ds = crimild::alloc< DescriptorSet >();
    ds->descriptors = Array< Descriptor > {
        Descriptor {
            .descriptorType = DescriptorType::STORAGE_BUFFER,
            .obj = [ & ] {
                struct BufferData {
                    alignas( 16 ) Point3 origin = Point3 { 0, 0, 0 };
                    alignas( 16 ) Vector3 direction = Vector3 { 0, 0, 0 };
                    alignas( 16 ) ColorRGB sampleColor = ColorRGB { 1, 1, 1 };
                    alignas( 16 ) ColorRGB accumColor = ColorRGB { 0, 0, 0 };
                    alignas( 16 ) Vector2 uv = Vector2 { 0, 0 };
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
                    alignas( 16 ) Point3 cameraOrigin;
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
                        auto bounces = UInt32( 1 );                                           //settings->get< UInt32 >( "rt.bounces", 10 );
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

                        auto cameraOrigin = Point3::Constants::ZERO;
                        auto cameraRight = Vector3::Constants::RIGHT;
                        auto cameraUp = Vector3::Constants::UP;

                        auto camera = Camera::getMainCamera();
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

                        if ( Input::getInstance()->isMouseButtonDown( CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) ) {
                            sampleCount = 1; // reset sampling
                            bounces = 1;     // only one bounce during interaction
                        }

                        settings->set( "rt.samples.count", sampleCount );

                        return Uniforms {
                            .sampleCount = sampleCount,
                            .maxSamples = maxSamples,
                            .bounces = bounces,
                            .seed = Random::generate< UInt32 >( 0, 1000 ),
                            .cameraInvProj = cameraInvProj,
                            .cameraWorld = cameraWorld,
                            .cameraOrigin = cameraOrigin,
                            .cameraRight = cameraRight,
                            .cameraUp = cameraUp,
                            .cameraLensRadius = Real32( 0.5 ) * aperture,
                            .cameraFocusDistance = focusDist,
                            .backgroundColor = backgroundColor,
                        };
                    } );
            }(),
        },
        Descriptor {
            .descriptorType = DescriptorType::UNIFORM_BUFFER,
            .obj = [ & ] {
                struct SceneUniforms {
                    alignas( 4 ) UInt32 sphereCount = 0;
                    alignas( 4 ) UInt32 boxCount = 0;
                    alignas( 4 ) UInt32 cylinderCount = 0;
                    alignas( 4 ) UInt32 materialCount = 0;
                };

                return crimild::alloc< UniformBuffer >( SceneUniforms {
                    .sphereCount = UInt32( spheres.size() ),
                    .boxCount = UInt32( boxes.size() ),
                    .cylinderCount = UInt32( cylinders.size() ),
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
            .obj = crimild::alloc< StorageBuffer >( materials.size() > 0 ? materials : Array< materials::PrincipledBSDF::Props > { materials::PrincipledBSDF::Props {} } ),
        },
    };

    UInt32 workers = Simulation::getInstance()->getSettings()->get< UInt32 >( "rt.workers", 4 );

    return computeImage(
        Extent2D {
            .width = Real32( width ),
            .height = Real32( height ),
        },
        crimild::alloc< Shader >(
            Shader::Stage::COMPUTE,
            FRAG_SRC ),
        Format::R32G32B32A32_SFLOAT,
        DispatchWorkgroup {
            .x = workers,
            .y = workers,
            .z = 1 },
        { ds, sceneDescriptors } );
}

#else

const auto RAY_INTERSECTOR_SRC = R"(
    layout( local_size_x = 32, local_size_y = 32 ) in;

    struct RayData {
        vec3 origin;
        vec3 direction;
        vec3 sampleColor;
        vec3 accumColor;
        int bounces;
        int samples;
    };

    layout( set = 0, binding = 0 ) buffer RayBounceState {
        RayData rays[];
    };

    layout( set = 1, binding = 0 ) uniform Uniforms {
        int width;
        int height;
    } uniforms;

    void main() 
    {
        if ( gl_GlobalInvocationID.x >= uniforms.width || gl_GlobalInvocationID.y >= uniforms.height ) {
            return;
        }

        vec3 color = vec3( gl_GlobalInvocationID.xy / vec2( uniforms.width, uniforms.height ), 1 );

        rays[ gl_GlobalInvocationID.y * uniforms.width + gl_GlobalInvocationID.x ].accumColor = color;
    }
)";

SharedPointer< FrameGraphOperation > rayIntersector( void ) noexcept
{
    const Real resolutionScale = Simulation::getInstance()->getSettings()->get< Real >( "rt.scale", 1 );
    const Int32 width = resolutionScale * Simulation::getInstance()->getSettings()->get< Int32 >( "video.width", 1024 );
    const Int32 height = resolutionScale * Simulation::getInstance()->getSettings()->get< Int32 >( "video.height", 768 );

    struct BufferData {
        alignas( 16 ) Point3 origin = Point3 { 0, 0, 0 };
        alignas( 16 ) Vector3 direction = Vector3 { 0, 0, 0 };
        alignas( 16 ) ColorRGB sampleColor = ColorRGB { 1, 1, 1 };
        alignas( 16 ) ColorRGB accumColor = ColorRGB { 0, 0, 0 };
        alignas( 4 ) Int32 bounces = 0;
        alignas( 4 ) Int32 samples = 0;
    };

    auto data = Array< BufferData >( width * height );
    auto buffer = crimild::alloc< StorageBuffer >( data );

    auto ds = crimild::alloc< DescriptorSet >();
    ds->descriptors = Array< Descriptor > {
        Descriptor {
            .descriptorType = DescriptorType::UNIFORM_BUFFER,
            .obj = [ & ] {
                struct Uniforms {
                    alignas( 4 ) Int32 width;
                    alignas( 4 ) Int32 height;
                };

                return crimild::alloc< UniformBuffer >( Uniforms {
                    .width = width,
                    .height = height,
                } );
            }(),
        },
    };

    return framegraph::computeBuffer(
        buffer,
        crimild::alloc< Shader >(
            Shader::Stage::COMPUTE,
            RAY_INTERSECTOR_SRC ),
        DispatchWorkgroup {
            .x = UInt32( width / DispatchWorkgroup::DEFAULT_WORGROUP_SIZE ),
            .y = UInt32( height / DispatchWorkgroup::DEFAULT_WORGROUP_SIZE ),
            .z = 1 },
        { ds } );
}

const auto ACCUMULATOR_SRC = R"(
    layout( local_size_x = 32, local_size_y = 32 ) in;
    layout( set = 0, binding = 0, rgba32f ) uniform image2D resultImage;

    struct RayData {
        vec3 origin;
        vec3 direction;
        vec3 sampleColor;
        vec3 accumColor;
        int bounces;
        int samples;
    };

    layout( set = 1, binding = 0 ) buffer RayBounceState {
        RayData rays[];
    };

    void main() 
    {
        vec2 size = imageSize( resultImage );
        if ( gl_GlobalInvocationID.x >= size.x || gl_GlobalInvocationID.y >= size.y ) {
            return;
        }

        vec2 uv = gl_GlobalInvocationID.xy;
        
        uint rayId = uint( uv.y * size.x + uv.x );
        RayData ray = rays[ rayId ];
        vec3 color = ray.accumColor;

        // uv /= size;
        // uv.y = 1 - uv.y;
        // vec3 color = vec3( uv, 0 );

        imageStore( resultImage, ivec2( gl_GlobalInvocationID.xy ), vec4( color, 1.0 ) );
    }
)";

SharedPointer< FrameGraphOperation > accumulator( SharedPointer< FrameGraphResource > const &input ) noexcept
{
    const Real resolutionScale = Simulation::getInstance()->getSettings()->get< Real >( "rt.scale", 1 );
    const int width = resolutionScale * Simulation::getInstance()->getSettings()->get< Int32 >( "video.width", 1024 );
    const int height = resolutionScale * Simulation::getInstance()->getSettings()->get< Int32 >( "video.height", 768 );

    auto ds = crimild::alloc< DescriptorSet >();
    ds->descriptors = Array< Descriptor > {
        Descriptor {
            .descriptorType = DescriptorType::STORAGE_BUFFER,
            .obj = crimild::cast_ptr< StorageBuffer >( input ),
        },
    };

    auto ret = framegraph::computeImage(
        Extent2D {
            .width = Real32( width ),
            .height = Real32( height ),
        },
        crimild::alloc< Shader >(
            Shader::Stage::COMPUTE,
            ACCUMULATOR_SRC ),
        Format::R32G32B32A32_SFLOAT,
        { ds } );

    ret->reads( { input } );
    return ret;
}

SharedPointer< FrameGraphOperation > crimild::framegraph::computeRT( void ) noexcept
{
    return accumulator( useResource( rayIntersector() ) );
}

#endif