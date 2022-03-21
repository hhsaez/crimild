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

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_inverse.hpp"
#include "Mathematics/cross.hpp"
#include "Mathematics/io.hpp"
#include "Mathematics/normalize.hpp"
#include "Mathematics/perspective.hpp"
#include "Mathematics/trigonometry.hpp"
#include "Simulation/Settings.hpp"

using namespace crimild;

Camera *Camera::_mainCamera = nullptr;

Camera::Camera( void )
    : Camera(
        45.0,
        [] {
            if ( auto settings = Settings::getInstance() ) {
                auto w = settings->get< Real >( "video.width", 1024 );
                auto h = settings->get< Real >( "video.height", 768 );
                return w / h;
            } else {
                return Real( 4.0 / 3.0 );
            }
        }(),
        0.1f,
        1000.0f )
{
}

Camera::Camera( float fov, float aspect, float near, float far )
    : m_fov( fov ),
      m_aspect( aspect ),
      m_near( near ),
      m_far( far ),
      m_frustum( fov, aspect, near, far ),
      _viewport { { 0.0f, 0.0f }, { 1.0f, 1.0f } },
      _viewMatrixIsCurrent( false )
{
    updateProjectionMatrix();

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

void Camera::updateProjectionMatrix( void )
{
    _projectionMatrix = perspective( m_fov, m_aspect, m_near, m_far );
}

bool Camera::getPickRay( float portX, float portY, Ray3 &result ) const
{
    const float x = 2.0f * portX - 1.0f;
    const float y = 1.0f - 2.0f * portY;

    const auto rayClip = Vector4 { x, y, -1.0f, 1.0f };

    auto rayEye = inverse( getProjectionMatrix() ) * rayClip;
    const auto rayOrigin = location( getWorld() );
    const auto rayDirection = normalize( getWorld()( xyz( rayEye ) ) );

    result = Ray3 {
        rayOrigin,
        rayDirection,
    };

    return true;
}

float Camera::computeAspect( void ) const
{
    return m_aspect;
}

void Camera::setAspectRatio( float aspect )
{
    m_aspect = aspect;
    updateProjectionMatrix();
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

void Camera::encode( coding::Encoder &encoder )
{
    Node::encode( encoder );

    encoder.encode( "focus_distance", m_focusDistance );
    encoder.encode( "aperture", m_aperture );
    encoder.encode( "fov", m_fov );
    encoder.encode( "aspect", m_aspect );
    encoder.encode( "near", m_near );
    encoder.encode( "far", m_far );
}

void Camera::decode( coding::Decoder &decoder )
{
    Group::decode( decoder );

    decoder.decode( "focus_distance", m_focusDistance );
    decoder.decode( "aperture", m_aperture );
    decoder.decode( "fov", m_fov );
    decoder.decode( "aspect", m_aspect );
    decoder.decode( "near", m_near );
    decoder.decode( "far", m_far );

    updateProjectionMatrix();
}
