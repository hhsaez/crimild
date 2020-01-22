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

#ifndef CRIMILD_RENDERING_VERTEX_BUFFER_
#define CRIMILD_RENDERING_VERTEX_BUFFER_

#include "Rendering/Buffer.hpp"
#include "Mathematics/Vector.hpp"
#include "Foundation/RTTI.hpp"

namespace crimild {

    class VertexBuffer : public Buffer, public RTTI {
    public:
        virtual ~VertexBuffer( void ) noexcept = default;

        Buffer::Usage getUsage( void ) const noexcept { return Buffer::Usage::VERTEX_BUFFER; }

        virtual crimild::Bool getExtent( Vector3f &min, Vector3f &max ) const noexcept { return false; }
    };

    namespace policies {

        template< typename VertexType >
        class VertexBufferNoExtentPolicy {
        public:
            static crimild::Bool getExtent( const containers::Array< VertexType > &vertices, Vector3f &min, Vector3f &max ) noexcept
            {
                return false;
            }
        };

        template< typename VertexType >
        class VertexBuffer2DExtentPolicy {
        public:
            static crimild::Bool getExtent( const containers::Array< VertexType > &vertices, Vector3f &min, Vector3f &max ) noexcept
            {
                const auto N = vertices.size();
                if ( N == 0 ) {
                    return false;
                }

                min.x() = max.y() = vertices[ 0 ].position.x();
                min.y() = max.y() = vertices[ 0 ].position.y();
                min.z() = max.z() = 0.0f;

                for ( auto i = 1l; i < N; i++ ) {
                    const auto &P = vertices[ i ].position;

                    min.x() = Numericf::min( min.x(), P.x() );
                    min.y() = Numericf::min( min.y(), P.y() );

                    max.x() = Numericf::max( max.x(), P.x() );
                    max.y() = Numericf::max( max.y(), P.y() );
                }

                return true;

            }
        };

        template< typename VertexType >
        class VertexBuffer3DExtentPolicy {
        public:
            static crimild::Bool getExtent( const containers::Array< VertexType > &vertices, Vector3f &min, Vector3f &max ) noexcept
            {
                const auto N = vertices.size();
                if ( N == 0 ) {
                    return false;
                }

                min = vertices[ 0 ].position;
                max = vertices[ 0 ].position;
                for ( auto i = 1l; i < N; i++ ) {
                    const auto &P = vertices[ i ].position;

                    min.x() = Numericf::min( min.x(), P.x() );
                    min.y() = Numericf::min( min.y(), P.y() );
                    min.z() = Numericf::min( min.z(), P.z() );

                    max.x() = Numericf::max( max.x(), P.x() );
                    max.y() = Numericf::max( max.y(), P.y() );
                    max.z() = Numericf::max( max.z(), P.z() );
                }

                return true;
            }
        };

    }

    template<
    	typename VertexType,
    	template < typename > class ExtentPolicy
    >
    class VertexBufferImpl : public VertexBuffer {
    public:
        /**
			\remarks When implementing custom vertex formats, don't
         	forget to implement a scecialization for getClassName()
         	so RTTI works. 
         */
        virtual const char *getClassName( void ) const override;

    private:
        using ExtentPolicyImpl = ExtentPolicy< VertexType >;

    public:
        explicit VertexBufferImpl( crimild::Size count ) noexcept
        	: m_vertices( count )
        {
            // nothing to do
        }

        explicit VertexBufferImpl( const containers::Array< VertexType > &vertices ) noexcept
            : m_vertices( vertices )
        {
            // nothing to do
        }

        virtual ~VertexBufferImpl( void ) noexcept = default;

        crimild::Size getSize( void ) const noexcept override { return m_vertices.size() * sizeof( VertexType ); }
        crimild::Size getStride( void ) const noexcept override { return sizeof( VertexType ); }

        void *getRawData( void ) noexcept override { return ( void * ) m_vertices.getData(); }
        const void *getRawData( void ) const noexcept override { return ( void * ) m_vertices.getData(); }

        crimild::Size getCount( void ) const noexcept { return getSize() / getStride(); }

        VertexType *getData( void ) noexcept { return m_vertices.getData(); }
        const VertexType *getData( void ) const noexcept { return m_vertices.getData(); }

        crimild::Bool getExtent( Vector3f &min, Vector3f &max ) const noexcept override
        {
            return ExtentPolicyImpl::getExtent( m_vertices, min, max );
        }

    private:
        containers::Array< VertexType > m_vertices;
    };

    struct VertexInputAttributeDescription {
        enum class Format {
            R32,
            R32G32,
            R32G32B32,
            R32G32B32A32
        };

        crimild::UInt32 location;
        crimild::UInt32 binding;
        Format format;
        crimild::UInt32 offset;
    };

    struct VertexInputBindingDescription {
        crimild::UInt32 binding;
        crimild::UInt32 stride;
    };

    struct VertexP2 {
        crimild::Vector2f position;

        static std::vector< VertexInputAttributeDescription > getAttributeDescriptions( crimild::UInt32 binding ) noexcept
        {
            return {
                {
                    .location = 0,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32,
                    .offset = offsetof( VertexP2, position ),
                }
            };
        }

        static VertexInputBindingDescription getBindingDescription( crimild::UInt32 binding ) noexcept
        {
            return {
				.binding = binding,
                .stride = sizeof( VertexP2 ),
            };
        }
    };

    using VertexP2Buffer = VertexBufferImpl< VertexP2, policies::VertexBuffer2DExtentPolicy >;

    struct VertexP2C3 {
        crimild::Vector2f position;
        crimild::RGBColorf color;

        static std::vector< VertexInputAttributeDescription > getAttributeDescriptions( crimild::UInt32 binding ) noexcept
        {
            return {
                {
                    .location = 0,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32,
                    .offset = offsetof( VertexP2C3, position ),
                },
                {
                    .location = 1,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32B32,
                    .offset = offsetof( VertexP2C3, color ),
                }
            };
        }

        static VertexInputBindingDescription getBindingDescription( crimild::UInt32 binding ) noexcept
        {
            return {
                .binding = binding,
                .stride = sizeof( VertexP2C3 ),
            };
        }
    };

    using VertexP2C3Buffer = VertexBufferImpl< VertexP2C3, policies::VertexBuffer2DExtentPolicy >;

    struct VertexP2C3TC2 {
        crimild::Vector2f position;
        crimild::RGBColorf color;
        crimild::Vector2f texCoord;

        static std::vector< VertexInputAttributeDescription > getAttributeDescriptions( crimild::UInt32 binding ) noexcept
        {
            return {
                {
                    .location = 0,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32,
                    .offset = offsetof( VertexP2C3TC2, position ),
                },
                {
                    .location = 1,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32B32,
                    .offset = offsetof( VertexP2C3TC2, color ),
                },
                {
                    .location = 2,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32,
                    .offset = offsetof( VertexP2C3TC2, texCoord ),
                },
            };
        }

        static VertexInputBindingDescription getBindingDescription( crimild::UInt32 binding ) noexcept
        {
            return {
                .binding = binding,
                .stride = sizeof( VertexP2C3TC2 ),
            };
        }
    };

    using VertexP2C3TC2Buffer = VertexBufferImpl< VertexP2C3TC2, policies::VertexBuffer2DExtentPolicy >;

    struct VertexP3 {
        crimild::Vector3f position;

        static std::vector< VertexInputAttributeDescription > getAttributeDescriptions( crimild::UInt32 binding ) noexcept
        {
            return {
                {
                    .location = 0,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32B32,
                    .offset = offsetof( VertexP3, position ),
                }
            };
        }

        static VertexInputBindingDescription getBindingDescription( crimild::UInt32 binding ) noexcept
        {
            return {
                .binding = binding,
                .stride = sizeof( VertexP3 ),
            };
        }
    };

    using VertexP3Buffer = VertexBufferImpl< VertexP3, policies::VertexBuffer3DExtentPolicy >;

    struct VertexP3C3 {
        crimild::Vector3f position;
        crimild::RGBColorf color;

        static std::vector< VertexInputAttributeDescription > getAttributeDescriptions( crimild::UInt32 binding ) noexcept
        {
            return {
                {
                    .location = 0,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32B32,
                    .offset = offsetof( VertexP3C3, position ),
                },
                {
                    .location = 1,
                    .binding = binding,
                    .format = VertexInputAttributeDescription::Format::R32G32B32,
                    .offset = offsetof( VertexP3C3, color ),
                }
            };
        }

        static VertexInputBindingDescription getBindingDescription( crimild::UInt32 binding ) noexcept
        {
            return {
                .binding = binding,
                .stride = sizeof( VertexP3C3 ),
            };
        }
    };

    using VertexP3C3Buffer = VertexBufferImpl< VertexP3C3, policies::VertexBuffer3DExtentPolicy >;

}

#endif

