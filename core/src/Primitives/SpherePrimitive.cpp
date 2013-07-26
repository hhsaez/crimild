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

#include "SpherePrimitive.hpp"

#include <vector>

using namespace crimild;

SpherePrimitive::SpherePrimitive( float radius, const VertexFormat &format, Vector2i divisions, Vector3f center )
    : Primitive( Primitive::Type::TRIANGLES )
{
    std::vector< float > vertices;
    for ( float latitude = 0; latitude <= divisions[ 1 ]; latitude++ ) {
        float theta = latitude * Numericf::PI / divisions[ 1 ];
        float sinTheta = std::sin( theta );
        float cosTheta = std::cos( theta );

        for ( float longitude = 0; longitude <= divisions[ 0 ]; longitude++ ) {
            float phi = longitude * Numericf::TWO_PI / divisions[ 0 ];
            float sinPhi = std::sin( phi );
            float cosPhi = std::cos( phi );

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            vertices.push_back( center[ 0 ] + radius * x );
            vertices.push_back( center[ 1 ] + radius * y );
            vertices.push_back( center[ 2 ] + radius * z );

            if ( format.hasNormals() ) {
                vertices.push_back( x );
                vertices.push_back( y );
                vertices.push_back( z );
            }

            if ( format.hasTextureCoords() ) {
                float u = 1.0f - ( longitude / divisions[ 0 ] );
                float v = ( latitude / divisions[ 1 ] );

                vertices.push_back( u );
                vertices.push_back( v );
            }
        }
    }

    VertexBufferObjectPtr vbo( new VertexBufferObject( format, vertices.size() / format.getVertexSize(), &vertices[ 0 ] ) );
    setVertexBuffer( vbo );

    std::vector< unsigned short > indices;
    for ( unsigned short latitude = 0; latitude < divisions[ 1 ]; latitude++ ) {
        for ( unsigned short longitude = 0; longitude < divisions[ 0 ]; longitude++ ) {
            unsigned short first = ( latitude * ( divisions[ 1 ] + 1 ) ) + longitude;
            unsigned short second = first + divisions[ 0 ] + 1;

            indices.push_back( first );
            indices.push_back( first + 1 );
            indices.push_back( second );

            indices.push_back( second );
            indices.push_back( first + 1 );
            indices.push_back( second + 1 );
        }
    }

    IndexBufferObjectPtr ibo( new IndexBufferObject( indices.size(), &indices[ 0 ] ) );
    setIndexBuffer( ibo );
}

SpherePrimitive::~SpherePrimitive( void )
{
    
}

