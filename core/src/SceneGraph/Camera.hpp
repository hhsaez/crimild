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

#ifndef CRIMILD_SCENEGRAPH_CAMERA_
#define CRIMILD_SCENEGRAPH_CAMERA_

#include "Group.hpp"
#include "Mathematics/Frustum.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Rect.hpp"

#include <memory>

namespace crimild {

    class Camera : public Group {
        CRIMILD_IMPLEMENT_RTTI( crimild::Camera )

    public:
        static Camera *getMainCamera( void ) { return _mainCamera; }

        /**
		   \remarks Internal use only
		 */
        static void setMainCamera( Camera *camera ) { _mainCamera = camera; }

        /**
		   remarks Internal use only
		 */
        static void setMainCamera( SharedPointer< Camera > const &camera ) { _mainCamera = crimild::get_ptr( camera ); }

    private:
        static Camera *_mainCamera;

    public:
        explicit Camera( void );
        Camera( float fov, float aspect, float near, float far );
        virtual ~Camera( void );

        bool isMainCamera( void ) const { return _isMainCamera; }
        void setIsMainCamera( bool value ) { _isMainCamera = value; }

    private:
        bool _isMainCamera = false;

    public:
        const Frustum &getFrustum( void ) const { return m_frustum; }

        void setProjectionMatrix( const Matrix4f &projection ) { _projectionMatrix = projection; }
        const Matrix4f &getProjectionMatrix( void ) const { return _projectionMatrix; }

        void setOrthographicMatrix( const Matrix4f &orthographic ) { _orthographicMatrix = orthographic; }
        const Matrix4f &getOrthographicMatrix( void ) const { return _orthographicMatrix; }

        void setViewMatrix( const Matrix4f &view );
        const Matrix4f &getViewMatrix( void );

        void setViewMatrixIsCurrent( bool value ) { _viewMatrixIsCurrent = value; }
        bool viewMatrixIsCurrent( void ) const { return _viewMatrixIsCurrent; }

        void setViewport( const Rectf &rect ) { _viewport = rect; }
        const Rectf &getViewport( void ) const { return _viewport; }

        virtual bool getPickRay( float normalizedX, float normalizedY, Ray3 &result ) const;

        inline Real getNear( void ) const { return m_near; }
        inline Real getFar( void ) const { return m_far; }

        void setAspectRatio( float aspect );
        float computeAspect( void ) const;

    private:
        void updateProjectionMatrix( void );

    private:
        Real m_fov = 45.0f;
        Real m_aspect = 4.0f / 3.0f;
        Real m_near = 0.1f;
        Real m_far = 1000.0f;

        Frustum m_frustum;
        Rectf _viewport;
        Matrix4f _projectionMatrix;
        Matrix4f _orthographicMatrix;
        Matrix4f _viewMatrix;
        bool _viewMatrixIsCurrent;

    public:
        virtual void accept( NodeVisitor &visitor ) override;

    public:
        void computeCullingPlanes( void );

        void setCullingEnabled( bool value ) { _cullingEnabled = value; }
        bool isCullingEnabled( void ) const { return _cullingEnabled; }

        bool culled( const BoundingVolume *volume ) const;

    private:
        bool _cullingEnabled = true;
        Plane3 _cullingPlanes[ 6 ];

        /**
         * \name Physical properties
         */
        //@{

    public:
        inline Real getFocusDistance( void ) const noexcept { return m_focusDistance; }
        inline void setFocusDistance( Real focusDistance ) noexcept { m_focusDistance = focusDistance; }

        inline Real getAperture( void ) const noexcept { return m_aperture; }
        inline void setAperture( Real aperture ) noexcept { m_aperture = aperture; }

    private:
        /**
         *  \brief Distance from the camera lens to the focus plane
         */
        Real m_focusDistance = Real( 1 );

        /**
         * \brief Aperture of the camera lens (in mm).
         */
        Real m_aperture = Real( 0 );

        //@}

        /**
            \name Coding
         */
        //@{
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

}

#endif
