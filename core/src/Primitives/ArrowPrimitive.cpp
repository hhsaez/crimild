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

#include "ArrowPrimitive.hpp"

#include "ConePrimitive.hpp"
#include "CylinderPrimitive.hpp"
#include "Mathematics/Vector2Ops.hpp"
#include "Mathematics/Vector3Ops.hpp"
#include "Mathematics/cross.hpp"
#include "Mathematics/normalize.hpp"

namespace crimild {

    class Generator {
    public:
        struct ColorMode {
            enum class Type {
                CONSTANT,
                POSITIONS,
            };

            Type type = Type::CONSTANT;
            ColorRGB color = ColorRGB::Constants::WHITE;
        };

        struct Interval {
            Vector2i divisions;
            Vector2f upperBound;
            Vector2f textureCount;
        };

        using Evaluator = std::function< Vector3f( const Vector2f &domain ) >;

        struct Params {
            Primitive::Type type = Primitive::Type::TRIANGLES;
            VertexLayout layout = VertexP3N3::getLayout();
            Interval interval;
            ColorMode colorMode;
            Bool invertNormals = false;
            Evaluator evaluator;
        };

        struct Result {
            Array< Vector3f > positions;
            Array< Vector3f > normals;
            Array< ColorRGB > colors;
            Array< Vector2f > texCoords;
            Array< UInt32 > indices;
        };

    public:
        static Result generate( Params params ) noexcept
        {
            auto layout = params.layout;
            auto interval = params.interval;
            auto divisions = interval.divisions;
            auto upperBound = interval.upperBound;
            auto slices = divisions - Vector2i::Constants::ONE;
            auto textureCount = interval.textureCount;
            auto evaluator = params.evaluator;

            const auto X = divisions.x;
            const auto Y = divisions.y;
            const auto N = X * Y;

            auto hasNormals = layout.hasAttribute( VertexAttribute::Name::NORMAL );
            auto hasColors = layout.hasAttribute( VertexAttribute::Name::COLOR );
            auto hasTexCoords = layout.hasAttribute( VertexAttribute::Name::TEX_COORD );

            Result ret;
            ret.positions.resize( N );
            if ( hasNormals )
                ret.normals.resize( N );
            if ( hasColors )
                ret.colors.resize( N );
            if ( hasTexCoords )
                ret.texCoords.resize( N );

            auto evaluate = [ = ]( auto x, auto y ) -> Vector3f {
                auto domain = Vector2f {
                    x * upperBound.x / slices.x,
                    y * upperBound.y / slices.y,
                };
                return evaluator( domain );
            };

            for ( auto y = 0l; y < Y; ++y ) {
                for ( auto x = 0l; x < X; ++x ) {
                    auto range = evaluate( x, y );

                    auto vertex = y * X + x;

                    ret.positions[ vertex ] = range;

                    if ( hasNormals ) {
                        float s = x;
                        float t = y;

                        // nudge the point if the normal is indeterminate
                        if ( x == 0 )
                            s += 0.01f;
                        if ( x == divisions.x - 1 )
                            s -= 0.01f;
                        if ( y == 0 )
                            t += 0.01f;
                        if ( y == divisions.y - 1 )
                            t -= 0.01f;

                        // compute the tangents and their cross product
                        Vector3f p = evaluate( s, t );
                        Vector3f u = evaluate( s + 0.01f, t ) - p;
                        Vector3f v = evaluate( s, t + 0.01f ) - p;
                        Vector3f normal = normalize( cross( u, v ) );
                        //if ( invertNormal( domain ) ) {
                        //  normal -= normal;
                        //}
                        ret.normals[ vertex ] = normal;
                    }

                    if ( hasColors ) {
                        auto color = ColorRGB::Constants::WHITE;
                        /*
                        if ( _colorMode.type == ColorMode::Type::CONSTANT ) {
                            color = _colorMode.color;
                        }
                        else if ( _colorMode.type == ColorMode::Type::POSITIONS ) {
                            color = 0.5f * ( Vector3f::ONE + range.getNormalized() );
                        }
                        */
                        ret.colors[ vertex ] = color;
                    }

                    if ( hasTexCoords ) {
                        auto s = ( float ) textureCount.x * ( float ) x / ( float ) slices[ 0 ];
                        auto t = ( float ) textureCount.y * ( float ) y / ( float ) slices[ 1 ];
                        ret.texCoords[ vertex ] = Vector2f { s, t };
                    }
                }
            }

            ret.indices.resize( 6 * slices.x * slices.y );
            auto index = 0l;
            auto vertex = 0l;
            for ( auto y = 0l; y < slices.y; ++y ) {
                for ( auto x = 0l; x < slices.x; ++x ) {
                    auto next = ( x + 1 ) % divisions.x;
                    ret.indices[ index++ ] = vertex + x;
                    ret.indices[ index++ ] = vertex + next;
                    ret.indices[ index++ ] = vertex + x + divisions.x;

                    ret.indices[ index++ ] = vertex + next;
                    ret.indices[ index++ ] = vertex + next + divisions.x;
                    ret.indices[ index++ ] = vertex + x + divisions.x;
                }
                vertex += divisions.x;
            }

            return ret;
        }
    };

}

using namespace crimild;

ArrowPrimitive::ArrowPrimitive( const Params &params ) noexcept
    : Primitive( params.type )
{
    auto type = params.type;
    auto layout = params.layout;

    auto height = 1.0f;
    auto radius = 1.0f;
    auto divisions = Vector2i { 9, 4 };

    auto coneHeight = 0.3f * height;
    auto coneRadius = 0.25f * radius;
    auto coneOffset = -( height - coneHeight ) * Vector3f::Constants::UNIT_Z;

    auto cylinderHeight = height - coneHeight;
    auto cylinderRadius = 0.1 * radius;

    auto cone = Generator::generate(
        Generator::Params {
            .type = type,
            .layout = layout,
            .interval = {
                .divisions = divisions,
                .upperBound = Vector2f { Numericf::TWO_PI, 1.0f },
                .textureCount = Vector2 { 30, 20 },
            },
            .evaluator = [ R = coneRadius, H = coneHeight, O = coneOffset ]( const Vector2f &domain ) -> Vector3f {
                float u = domain[ 0 ];
                float v = domain[ 1 ];
                float x = v * R * std::cos( u );
                float y = v * R * std::sin( u );
                float z = ( v - 1.0f ) * H;
                return O + Vector3 { x, y, z };
            },
        } );

    auto coneVertexCount = cone.positions.size();

    // Join the cone and the cylinder.
    // The last points in the cone will be joined with the first ones in the cylinder
    auto slices = divisions - Vector2i::Constants::ONE;
    for ( auto x = 0l; x < slices.x; ++x ) {
        auto next = ( x + 1 ) % divisions.x;
        cone.indices.add( coneVertexCount - divisions.x + x );
        cone.indices.add( coneVertexCount - divisions.x + next );
        cone.indices.add( coneVertexCount + x );

        cone.indices.add( coneVertexCount - divisions.x + next );
        cone.indices.add( coneVertexCount + next );
        cone.indices.add( coneVertexCount + x );
    }

    auto cylinder = Generator::generate(
        Generator::Params {
            .type = type,
            .layout = layout,
            .interval = {
                .divisions = divisions,
                .upperBound = Vector2f { Numericf::TWO_PI, 1.0f },
                .textureCount = Vector2f { 30, 20 },
            },
            .evaluator = [ R = cylinderRadius, H = cylinderHeight ]( const Vector2f &domain ) -> Vector3f {
                float u = domain[ 0 ];
                float v = domain[ 1 ];
                float x = R * std::cos( u );
                float y = R * std::sin( u );
                float z = ( v - 1.0f ) * H;
                return Vector3f { x, y, z };
            },
        } );

    auto cylinderVertexCount = cylinder.positions.size();

    for ( auto x = 0l; x < slices.x; ++x ) {
        auto next = ( x + 1 ) % divisions.x;
        cylinder.indices.add( cylinderVertexCount - divisions.x + x );
        cylinder.indices.add( cylinderVertexCount - divisions.x + next );
        cylinder.indices.add( cylinderVertexCount + x );

        cylinder.indices.add( cylinderVertexCount - divisions.x + next );
        cylinder.indices.add( cylinderVertexCount + next );
        cylinder.indices.add( cylinderVertexCount + x );
    }

    cylinder.positions.add( Vector3f::Constants::ZERO );
    cylinder.normals.add( Vector3f::Constants::UNIT_Z );
    cylinder.texCoords.add( Vector2f::Constants::ZERO );

    auto vertexCount = coneVertexCount + cylinderVertexCount;
    auto vertices = crimild::alloc< VertexBuffer >( layout, vertexCount );
    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    auto normals = vertices->get( VertexAttribute::Name::NORMAL );
    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );

    for ( auto i = 0l; i < cone.positions.size(); ++i ) {
        positions->set( i, cone.positions[ i ] );
        normals->set( i, cone.normals[ i ] );
        texCoords->set( i, cone.texCoords[ i ] );
    }

    for ( auto i = 0l; i < cylinder.positions.size(); ++i ) {
        positions->set( coneVertexCount + i, cylinder.positions[ i ] );
        normals->set( coneVertexCount + i, cylinder.normals[ i ] );
        texCoords->set( coneVertexCount + i, cylinder.texCoords[ i ] );
    }

    auto coneIndexCount = cone.indices.size();
    auto cylinderIndexCount = cylinder.indices.size();
    auto indexCount = coneIndexCount + cylinderIndexCount;
    auto indices = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, indexCount );

    for ( auto i = 0l; i < coneIndexCount; ++i ) {
        indices->setIndex( i, UInt32( cone.indices[ i ] ) );
    }

    for ( auto i = 0l; i < cylinderIndexCount; ++i ) {
        indices->setIndex( coneIndexCount + i, UInt32( coneVertexCount + cylinder.indices[ i ] ) );
    }

    setVertexData( { vertices } );
    setIndices( { indices } );
}
