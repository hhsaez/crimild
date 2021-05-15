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

#ifndef CRIMILD_ANIMATION_SKELETON_
#define CRIMILD_ANIMATION_SKELETON_

#include "Components/NodeComponent.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/NamedObject.hpp"
#include "Mathematics/Matrix.hpp"
#include "Mathematics/Transformation.hpp"

namespace crimild {

    namespace animation {

        class Clip;
        class Animation;

        class Joint : public NamedObject,
                      public NodeComponent {
            CRIMILD_IMPLEMENT_RTTI( crimild::animation::Joint )

        public:
            Joint( void );
            explicit Joint( std::string, crimild::UInt32 id );
            virtual ~Joint( void );

            crimild::UInt32 getId( void ) const { return _id; }

            void setOffset( const Transformation &transform ) { _offset = transform; }
            const Transformation &getOffset( void ) const { return _offset; }

            void setPoseMatrix( const Matrix4f &matrix ) { _poseMatrix = matrix; }
            const Matrix4f &getPoseMatrix( void ) const { return _poseMatrix; }

        private:
            crimild::UInt32 _id;
            Transformation _offset;
            Matrix4f _poseMatrix;

            /**
			   \name Cloning
			*/
            //@{

        public:
            virtual SharedPointer< NodeComponent > clone( void ) override;

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

        class Skeleton : public NodeComponent {
            CRIMILD_IMPLEMENT_RTTI( crimild::animation::Skeleton )

        private:
            using JointCatalog = Map< std::string, SharedPointer< Joint > >;
            using ClipCatalog = Map< std::string, SharedPointer< Clip > >;

        public:
            Skeleton( void );
            virtual ~Skeleton( void );

            void setJoints( const JointCatalog &joints ) { _joints = joints; }
            JointCatalog &getJoints( void ) { return _joints; }

            void setGlobalInverseTransform( const Transformation &transform ) { _globalInverseTransform = transform; }
            const Transformation &getGlobalInverseTransform( void ) const { return _globalInverseTransform; }

            void setClips( const ClipCatalog &clips ) { _clips = clips; }
            ClipCatalog &getClips( void ) { return _clips; }

            virtual void start( void ) override;

        private:
            JointCatalog _joints;
            ClipCatalog _clips;
            Transformation _globalInverseTransform;

        public:
            void animate( SharedPointer< Animation > const &animation ) { animate( crimild::get_ptr( animation ) ); }
            void animate( Animation *animation );

            /**
			   \name Cloning
			*/
            //@{

        public:
            virtual SharedPointer< NodeComponent > clone( void ) override;

            //@}

            /**
			   \name Coding
			*/
            //@{

        public:
            virtual void encode( coding::Encoder &encoder ) override;
            virtual void decode( coding::Decoder &decoder ) override;

            //@}

        public:
            void renderDebugInfo( Renderer *renderer, Camera *camera ) override;
        };

    }

}

#endif
