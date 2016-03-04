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

#include "Rendering/RenderPass.hpp"
#include "Rendering/ForwardRenderPass.hpp"

using namespace crimild;

Camera::Camera( void )
	: Camera( 60.0, 4.0f / 3.0f, 1.0f, 1000.0f )
{

}

Camera::Camera( float fov, float aspect, float near, float far )
	: _frustum( fov, aspect, near, far ),
	  _viewport( 0.0f, 0.0f, 1.0f, 1.0f ),
      _viewMatrixIsCurrent( false ),
      _renderPass( crimild::alloc< ForwardRenderPass >() )
{
	_projectionMatrix = _frustum.computeProjectionMatrix();
	_orthographicMatrix = _frustum.computeOrthographicMatrix();
	_viewMatrix.makeIdentity();
}

Camera::~Camera( void )
{

}

void Camera::setFrustum( const Frustumf &f )
{
    _frustum = f;
	_projectionMatrix = _frustum.computeProjectionMatrix();
	_orthographicMatrix = _frustum.computeOrthographicMatrix();
}

void Camera::accept( NodeVisitor &visitor )
{
	visitor.visitCamera( this );
}

void Camera::setViewMatrix( const Matrix4f &view ) 
{ 
	_viewMatrix = view;
    world().fromMatrix( view.getInverse() );
    setWorldIsCurrent( true );
}

const Matrix4f &Camera::getViewMatrix( void )
{
    if ( !_viewMatrixIsCurrent ) {
        _viewMatrix = getWorld().computeModelMatrix();
        _viewMatrix.makeInverse();
    }
    
	return _viewMatrix;
}

bool Camera::getPickRay( float portX, float portY, Ray3f &result ) const
{
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

	return true;
}

float Camera::computeAspect( void ) const
{
	return getFrustum().computeAspect();
}

void Camera::setAspectRatio( float aspect )
{
	auto &f = getFrustum();
	setFrustum( Frustumf( -aspect * f.getUMax(), aspect * f.getUMax(), f.getUMin(), f.getUMax(), f.getDMin(), f.getDMax() ) );
}

void Camera::computeCullingPlanes( void )
{
	Vector3f normal;
	float constant;

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
	normal = ( -getFrustum().getDMin() * up + getFrustum().getUMax() * direction  ) * invLengthTop;
	constant = normal * position;
	_cullingPlanes[ 2 ] = Plane3f( normal, constant );

	// bottom plane
	float invLengthBottom = 1.0f / sqrtf( getFrustum().getDMin() * getFrustum().getDMin() + getFrustum().getUMin() * getFrustum().getUMin() );
	normal = ( getFrustum().getDMin() * up - getFrustum().getUMin() * direction ) * invLengthBottom;
	constant = normal * position;
	_cullingPlanes[ 3 ] = Plane3f( normal, constant );

	// left plane
	float invLengthLeft = 1.0f / sqrtf( getFrustum().getDMin() * getFrustum().getDMin() + getFrustum().getRMin() * getFrustum().getRMin() );
	normal = ( getFrustum().getDMin() * right - getFrustum().getRMin() * direction ) * invLengthLeft;
	constant = normal * position;
	_cullingPlanes[ 4 ] = Plane3f( normal, constant );

	// right plane
	float invLengthRight = 1.0f / sqrtf( getFrustum().getDMin() * getFrustum().getDMin() + getFrustum().getRMax() * getFrustum().getRMax() );
	normal = ( -getFrustum().getDMin() * right + getFrustum().getRMax() * direction ) * invLengthRight;
	constant = normal * position;
	_cullingPlanes[ 5 ] = Plane3f( normal, constant );
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

