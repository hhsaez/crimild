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
 *     * Neither the name of the copyright holders nor the
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

#include "Geometry.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Uniforms/ModelUniformBuffer.hpp"

#include <algorithm>

using namespace crimild;

Geometry::Geometry( std::string name )
    : Node( name )
{
    attachComponent( crimild::alloc< MaterialComponent >() );
    attachComponent( crimild::alloc< RenderStateComponent >() );
}

Geometry::~Geometry( void )
{
    detachAllPrimitives();
}

void Geometry::attachPrimitive( Primitive *primitive )
{
    attachPrimitive( crimild::retain( primitive ) );
}

void Geometry::attachPrimitive( SharedPointer< Primitive > const &primitive )
{
    _primitives.add( primitive );
    updateModelBounds();
}

void Geometry::detachPrimitive( Primitive *primitive )
{
    _primitives.remove( crimild::retain( primitive ) );
}

void Geometry::detachPrimitive( SharedPointer< Primitive > const &primitive )
{
    _primitives.remove( primitive );
}

void Geometry::forEachPrimitive( std::function< void( Primitive * ) > callback )
{
    _primitives.each( [ callback ]( SharedPointer< Primitive > &p ) {
        callback( crimild::get_ptr( p ) );
    } );
}

void Geometry::detachAllPrimitives( void )
{
    _primitives.clear();
}

void Geometry::accept( NodeVisitor &visitor )
{
    visitor.visitGeometry( this );
}

void Geometry::updateModelBounds( void )
{
    forEachPrimitive(
        [ first = true,
          bound = localBound() ]( Primitive *primitive ) mutable {
            switch ( primitive->getType() ) {
                case Primitive::Type::SPHERE:
                case Primitive::Type::BOX:
                case Primitive::Type::OPEN_CYLINDER:
                case Primitive::Type::CYLINDER: {
                    if ( first ) {
                        bound->computeFrom( Point3 { -1, -1, -1 }, Point3 { 1, 1, 1 } );
                        first = false;
                    } else {
                        bound->expandToContain( Point3 { -1, -1, -1 } );
                        bound->expandToContain( Point3 { 1, 1, 1 } );
                    }
                    break;
                }

                default: {
                    primitive->getVertexData().each(
                        [ & ]( auto vbo ) {
                            auto positions = vbo->get( VertexAttribute::Name::POSITION );
                            if ( positions != nullptr ) {
                                if ( first ) {
                                    bound->computeFrom( crimild::get_ptr( vbo ) );
                                    first = false;
                                } else {
                                    bound->expandToContain( crimild::get_ptr( vbo ) );
                                }
                            }
                        } );
                    break;
                }
            }
        } );
}

DescriptorSet *Geometry::getDescriptors( void ) noexcept
{
    if ( auto ds = crimild::get_ptr( m_descriptors ) ) {
        return ds;
    }

    m_descriptors = [ & ] {
        auto descriptorSet = crimild::alloc< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = crimild::alloc< ModelUniform >( this ),
            },
        };
        return descriptorSet;
    }();

    return crimild::get_ptr( m_descriptors );
}

void Geometry::encode( coding::Encoder &encoder )
{
    Node::encode( encoder );

    encoder.encode( "primitives", _primitives );
}

void Geometry::decode( coding::Decoder &decoder )
{
    Node::decode( decoder );

    Array< SharedPointer< Primitive > > ps;
    decoder.decode( "primitives", ps );
    ps.each( [ this ]( SharedPointer< Primitive > &p ) {
        attachPrimitive( p );
    } );
}
