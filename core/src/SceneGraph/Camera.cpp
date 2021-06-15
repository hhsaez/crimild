/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Camera.hpp"

#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Matrix4_transpose.hpp"
#include "Mathematics/ortho.hpp"
#include "Mathematics/perspective.hpp"
#include "Mathematics/trigonometry.hpp"

using namespace crimild;

Camera *Camera::_mainCamera = nullptr;

Camera::Camera( void )
    : Camera( 45.0, 4.0f / 3.0f, 0.1f, 1000.0f )
{
}

Camera::Camera( float fov, float aspect, float near, float far )
    : //_frustum( fov, aspect, near, far ),
      _viewport { { 0.0f, 0.0f }, { 1.0f, 1.0f } },
      _viewMatrixIsCurrent( false )
{
    _projectionMatrix = perspective( fov, aspect, near, far );
    //    _projectionMatrix = transpose( perspective( -1, 1, -1, 1, 0.1f, 100.0f ) );

    _projectionMatrix = transpose( [] {
        const auto n = 0.01f;
        const auto f = 100.0f;
        const auto r = Real( 1 );
        const auto l = Real( -1 );
        const auto t = Real( 1 );
        const auto b = Real( -1 );

        return Matrix4 {
            2 * n / ( r - l ),
            0,
            0,
            0,
            0,
            2 * n / ( t - b ),
            0,
            0,
            ( r + l ) / ( r - l ),
            ( t + b ) / ( t - b ),
            -( f + n ) / ( f - n ),
            -1.0f,
            0,
            0,
            -( 2.0f * f * n ) / ( f - n ),
            0.0f,
        };
    }() );

    //    _projectionMatrix = perspective( radians( fov ), aspect, near, far );

    _projectionMatrix = []( Radians fovY, Real aspect, Real near, Real far ) {
        // Computes projection matrix for a right handed coordinate system,
        // which a depth range of [0, 1]
        const auto halfAngle = 0.5f * fovY;
        const auto tanHalfAngle = tan( halfAngle );

        const auto a = tanHalfAngle / aspect;
        const auto b = tanHalfAngle;
        const auto c = far / ( near - far );
        const auto d = -( far * near ) / ( far - near );
        const auto e = Real( -1 );

        return Matrix4 {
            { a, 0, 0, 0 },
            { 0, b, 0, 0 },
            { 0, 0, c, e },
            { 0, 0, d, 0 },
        };
    }( radians( fov ), 4.0 / 3.0, near, far );

    //    _projectionMatrix = perspective( -1, 1, -1, 1, 0.1f, 100.0f );

    //	_projectionMatrix = Matrix4 {
    //		0, 1, 2, 3,
    //  		4, 5, 6, 7,
    //    	8, 9, 10, 11,
    //     	12, 13, 14, 15,
    //    };

    //    _projectionMatrix = transpose( perspective( fov, aspect, near, far ) );
    //_projectionMatrix = Matrix4::Constants::IDENTITY;

    //	_projectionMatrix = []( Real left, Real right, Real bottom, Real top, Real near, Real far ) {
    //		const auto a = Real( 2 ) / ( right - left );
    //  		const auto b = Real( 2 ) / ( top - bottom );
    //    	const auto c = -Real( 1 ) / ( far - near );
    //     	const auto d = -( right + left ) / ( right - left );
    //      	const auto e = -( top + bottom ) / ( top - bottom );
    //       	const auto f = -near / ( far - near );
    //		return Matrix4 {
    //  			a, 0, 0, d,
    //     		0, b, 0, e,
    //            0, 0, c, f,
    //            0, 0, 0, 1,
    //        };
    //    }( -1, 1, -1, 1, 0.01, 100 );

    //    _projectionMatrix = transpose( _projectionMatrix );

    const auto s = Real( 3 );
    _projectionMatrix = ortho( -s, s, -s, s, -s, s );

    //_projectionMatrix = _frustum.computeProjectionMatrix();
    //_orthographicMatrix = _frustum.computeOrthographicMatrix();

    if ( getMainCamera() == nullptr ) {
        // Set itself as the main camera if there isn't one already set
        setMainCamera( this );
    }
}

Camera::~Camera( void )
{
    if ( getMainCamera() == this ) {
        setMainCamera( nullptr );
    }
}

void Camera::accept( NodeVisitor &visitor )
{
    visitor.visitCamera( this );
}

void Camera::setViewMatrix( const Matrix4f &view )
{
    /*
    _viewMatrix = view;
    world().fromMatrix( view.getInverse() );
    setWorldIsCurrent( true );
    */
}

const Matrix4f &Camera::getViewMatrix( void )
{
    return getWorld().invMat;
}

bool Camera::getPickRay( float portX, float portY, Ray3 &result ) const
{
    assert( false );

#if 0
    float x = 2.0f * portX - 1.0f;
    float y = 1.0f - 2.0f * portY;

    Vector4f rayClip( x, y, -1.0f, 1.0f );

    Vector4f rayEye = getProjectionMatrix().getInverse().getTranspose() * rayClip;
    rayEye = Vector4f( rayEye[ 0 ], rayEye[ 1 ], -1.0f, 0.0f );

    Vector4f rayWorld = getWorld().computeModelMatrix().getTranspose() * rayEye;

    Vector3f rayDirection( rayWorld[ 0 ], rayWorld[ 1 ], rayWorld[ 2 ] );
    rayDirection.normalize();

    result.setOrigin( getWorld().getTranslate() );
    result.setDirection( rayDirection );
#endif

    return true;
}

float Camera::computeAspect( void ) const
{
    //return getFrustum().computeAspect();
    return 1;
}

void Camera::setAspectRatio( float aspect )
{
    /*
    auto &f = getFrustum();
    setFrustum( Frustumf( -aspect * f.getUMax(), aspect * f.getUMax(), f.getUMin(), f.getUMax(), f.getDMin(), f.getDMax() ) );
    */
}

void Camera::computeCullingPlanes( void )
{
    /*
    Vector3f normal;

    Vector3f position = getWorld().getTranslate();
    Vector3f direction = getWorld().computeDirection().getNormalized();
    Vector3f up = getWorld().computeUp().getNormalized();
    Vector3f right = getWorld().computeRight().getNormalized();

    // near plane
    _cullingPlanes[ 0 ] = Plane3f( direction, position + getFrustum().getDMin() * direction );

    // far plane
    _cullingPlanes[ 1 ] = Plane3f( -direction, position + getFrustum().getDMax() * direction );

    // top plane
    float invLengthTop = 1.0f / sqrtf( getFrustum().getDMin() * getFrustum().getDMin() + getFrustum().getUMax() * getFrustum().getUMax() );
    normal = ( -getFrustum().getDMin() * up + getFrustum().getUMax() * direction ) * invLengthTop;
    _cullingPlanes[ 2 ] = Plane3f( normal, position );

    // bottom plane
    float invLengthBottom = 1.0f / sqrtf( getFrustum().getDMin() * getFrustum().getDMin() + getFrustum().getUMin() * getFrustum().getUMin() );
    normal = ( getFrustum().getDMin() * up - getFrustum().getUMin() * direction ) * invLengthBottom;
    _cullingPlanes[ 3 ] = Plane3f( normal, position );

    // left plane
    float invLengthLeft = 1.0f / sqrtf( getFrustum().getDMin() * getFrustum().getDMin() + getFrustum().getRMin() * getFrustum().getRMin() );
    normal = ( getFrustum().getDMin() * right - getFrustum().getRMin() * direction ) * invLengthLeft;
    _cullingPlanes[ 4 ] = Plane3f( normal, position );

    // right plane
    float invLengthRight = 1.0f / sqrtf( getFrustum().getDMin() * getFrustum().getDMin() + getFrustum().getRMax() * getFrustum().getRMax() );
    normal = ( -getFrustum().getDMin() * right + getFrustum().getRMax() * direction ) * invLengthRight;
    _cullingPlanes[ 5 ] = Plane3f( normal, position );
    */
}

bool Camera::culled( const BoundingVolume *volume ) const
{
    if ( !isCullingEnabled() ) {
        return false;
    }

    for ( auto &p : _cullingPlanes ) {
        if ( volume->whichSide( p ) < 0 ) {
            return true;
        }
    }

    return false;
}
