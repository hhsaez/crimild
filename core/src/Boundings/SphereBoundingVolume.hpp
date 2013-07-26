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

#ifndef CRIMILD_CORE_BOUNDINGS_BOUNDING_VOLUME_SPHERE_
#define CRIMILD_CORE_BOUNDINGS_BOUNDING_VOLUME_SPHERE_

#include "BoundingVolume.hpp"
#include "Mathematics/Sphere.hpp"

namespace crimild {

	class SphereBoundingVolume : public BoundingVolume {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( SphereBoundingVolume );

	public:
		SphereBoundingVolume( void );
		SphereBoundingVolume( const Vector3f &center, float radius );
		explicit SphereBoundingVolume( const Sphere3f &sphere );
		virtual ~SphereBoundingVolume( void );

		virtual const Vector3f &getCenter( void ) const override { return _sphere.getCenter(); }
		virtual float getRadius( void ) const override { return _sphere.getRadius(); }

		virtual const Vector3f &getRComponent( void ) const override { return _r; }
		virtual const Vector3f &getSComponent( void ) const override { return _s; }
		virtual const Vector3f &getTComponent( void ) const override { return _t; }

	private:
		Sphere3f _sphere;
		Vector3f _r;
		Vector3f _s;
		Vector3f _t;

	public:
		virtual void computeFrom( const BoundingVolume *volume, const TransformationImpl &transform ) override;
		virtual void computeFrom( const Vector3f *positions, unsigned int positionCount ) override;
		virtual void computeFrom( const VertexBufferObject *vbo ) override;
		virtual void computeFrom( const Vector3f &r, const Vector3f &s, const Vector3f &t ) override;

	public:
		virtual void expandToContain( const Vector3f &point ) override;
		virtual void expandToContain( const Vector3f *positions, unsigned int positionCount ) override;
		virtual void expandToContain( const VertexBufferObject *vbo ) override;
		virtual void expandToContain( const BoundingVolume *input ) override;

	public:
		virtual int whichSide( const Plane3f &plane ) const override;
		virtual bool contains( const Vector3f &point ) const override;

	public:
		virtual bool intersects( const Ray3f &ray ) const override;
		virtual bool intersects( const BoundingVolume *input ) const override;
	};

	typedef std::shared_ptr< SphereBoundingVolume > SphereBoundingVolumePtr;

}

#endif

