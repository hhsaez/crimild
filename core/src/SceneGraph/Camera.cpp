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
	: _frustum( 90.0, 4.0f / 3.0f, 1.0f, 1000.0f ),
	  _viewport( 0.0f, 0.0f, 1.0f, 1.0f ),
      _viewMatrixIsCurrent( false ),
      _renderPass( std::make_shared< ForwardRenderPass >() )
{
	_projectionMatrix = _frustum.computeProjectionMatrix();
	_orthographicMatrix = _frustum.computeOrthographicMatrix();
	_viewMatrix.makeIdentity();
}

Camera::Camera( float fov, float aspect, float near, float far )
	: _frustum( fov, aspect, near, far ),
	  _viewport( 0.0f, 0.0f, 1.0f, 1.0f ),
      _viewMatrixIsCurrent( false ),
      _renderPass( std::make_shared< ForwardRenderPass >() )
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
	visitor.visitCamera( getShared< Camera >() );
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

