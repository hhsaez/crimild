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

#include "Importers/GLTFImporter.hpp"

#include "Components/MaterialComponent.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/STBUtils.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/normalize.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Vertex.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"

// Tell TinyGLTF that STB is loaded elsewhere (see STBUtils.hpp/STBUtils.cpp)
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#include <tiny_gltf.h>

using namespace crimild;
using namespace crimild::importers;

std::vector< std::shared_ptr< Image > > loadImages( tinygltf::Model &input ) noexcept
{
    std::vector< std::shared_ptr< Image > > images( input.images.size() );
    for ( size_t i = 0; i < input.images.size(); ++i ) {
        auto &inImage = input.images[ i ];
        unsigned char *buffer = nullptr;
        size_t bufferSize = 0;
        bool deleteBuffer = false;
        if ( inImage.component == 3 ) {
            // Convert RGB images to RGBA, since most devices do not support RGB
            bufferSize = inImage.width * inImage.height * 4;
            buffer = new unsigned char[ bufferSize ];
            unsigned char *rgba = buffer;
            unsigned char *rgb = &inImage.image[ 0 ];
            for ( size_t j = 0; j < inImage.width * inImage.height; ++j ) {
                memcpy( rgba, rgb, sizeof( unsigned char ) * 3 );
                rgba += 4;
                rgb += 3;
            }
            deleteBuffer = true;
        } else {
            buffer = &inImage.image[ 0 ];
            bufferSize = inImage.image.size();
        }

        auto image = crimild::alloc< Image >();
        image->extent = Extent3D {
            .width = Real32( inImage.width ),
            .height = Real32( inImage.height ),
        };
        image->format = Format::R8G8B8A8_UNORM;
        image->setBufferView(
            crimild::alloc< BufferView >(
                BufferView::Target::IMAGE,
                crimild::alloc< Buffer >( Array< Byte >( ByteArray( bufferSize, buffer ) ) )
            )
        );
        images[ i ] = image;

        if ( deleteBuffer ) {
            delete[] buffer;
        }
    }
    return images;
}

std::vector< std::shared_ptr< ImageView > > loadImageViews( std::vector< std::shared_ptr< Image > > &images ) noexcept
{
    std::vector< std::shared_ptr< ImageView > > imageViews( images.size() );
    for ( size_t i = 0; i < images.size(); ++i ) {
        if ( images[ i ] != nullptr ) {
            auto imageView = crimild::alloc< ImageView >();
            imageView->image = images[ i ];
            imageViews[ i ] = imageView;
        }
    }
    return imageViews;
}

std::vector< std::shared_ptr< Texture > > loadTextures( tinygltf::Model &input, std::vector< std::shared_ptr< ImageView > > &imageViews ) noexcept
{
    std::vector< std::shared_ptr< Texture > > textures( input.textures.size() );
    for ( size_t i = 0; i < input.textures.size(); ++i ) {
        const auto &inTexture = input.textures[ i ];
        auto imageView = imageViews[ inTexture.source ];
        if ( imageView == nullptr ) {
            continue;
        }
        auto texture = crimild::alloc< Texture >();
        texture->setName( inTexture.name );
        texture->imageView = imageView;
        texture->sampler = [] {
            auto sampler = crimild::alloc< Sampler >();
            sampler->setMinFilter( Sampler::Filter::LINEAR );
            sampler->setMagFilter( Sampler::Filter::LINEAR );
            return sampler;
        }();
        textures[ i ] = texture;
    }
    return textures;
}

std::vector< std::shared_ptr< Material > > loadMaterials( tinygltf::Model &input, std::vector< std::shared_ptr< Texture > > &textures ) noexcept
{
    std::vector< std::shared_ptr< Material > > materials( input.materials.size() );

    for ( size_t i = 0; i < input.materials.size(); ++i ) {
        const auto &inputMaterial = input.materials[ i ];

        auto material = crimild::alloc< materials::PrincipledBSDF >();
        material->setName( inputMaterial.name );

        // Get base color
        if ( inputMaterial.values.find( "baseColorFactor" ) != inputMaterial.values.end() ) {
            const auto &inColor = inputMaterial.values.at( "baseColorFactor" ).ColorFactor();
            const auto albedo = ColorRGB {
                Real( inColor[ 0 ] ),
                Real( inColor[ 1 ] ),
                Real( inColor[ 2 ] ),
            };
            material->setAlbedo( albedo );
        }

        if ( inputMaterial.values.find( "baseColorTexture" ) != inputMaterial.values.end() ) {
            auto texture = textures[ inputMaterial.values.at( "baseColorTexture" ).TextureIndex() ];
            if ( texture != nullptr ) {
                material->setAlbedoMap( texture );
            }
        }

        materials[ i ] = material;
    }

    return materials;
}

Transformation loadTransformation( const tinygltf::Node &inNode )
{
    if ( inNode.matrix.size() == 16 ) {
        // Input matrix is in column-major form
        const auto matrix = Matrix4 {
            { Real( inNode.matrix[ 0 ] ), Real( inNode.matrix[ 1 ] ), Real( inNode.matrix[ 2 ] ), Real( inNode.matrix[ 3 ] ) },
            { Real( inNode.matrix[ 4 ] ), Real( inNode.matrix[ 5 ] ), Real( inNode.matrix[ 6 ] ), Real( inNode.matrix[ 7 ] ) },
            { Real( inNode.matrix[ 8 ] ), Real( inNode.matrix[ 9 ] ), Real( inNode.matrix[ 10 ] ), Real( inNode.matrix[ 11 ] ) },
            { Real( inNode.matrix[ 12 ] ), Real( inNode.matrix[ 13 ] ), Real( inNode.matrix[ 14 ] ), Real( inNode.matrix[ 15 ] ) },
        };
        return Transformation { matrix, inverse( matrix ) };
    }

    const auto T = [ & ] {
        if ( inNode.translation.size() == 3 ) {
            return translation( Real( inNode.translation[ 0 ] ), Real( inNode.translation[ 1 ] ), Real( inNode.translation[ 2 ] ) );
        } else {
            return Transformation::Constants::IDENTITY;
        }
    }();

    // TODO
    auto R = Transformation::Constants::IDENTITY;

    const auto S = [ & ] {
        if ( inNode.scale.size() == 3 ) {
            return scale( Real( inNode.scale[ 0 ] ), Real( inNode.scale[ 1 ] ), Real( inNode.scale[ 2 ] ) );
        } else {
            return Transformation::Constants::IDENTITY;
        }
    }();

    return T * R * S;
}

std::shared_ptr< Node > loadNode( const tinygltf::Node &inNode, const tinygltf::Model &input, std::shared_ptr< Group > const &parent, std::vector< std::shared_ptr< Material > > &materials ) noexcept
{
    auto node = crimild::alloc< Group >( inNode.name );
    if ( parent != nullptr ) {
        parent->attachNode( node );
    }

    node->setLocal( loadTransformation( inNode ) );

    // Load children
    for ( size_t i = 0; i < inNode.children.size(); ++i ) {
        loadNode( input.nodes[ inNode.children[ i ] ], input, node, materials );
    }

    // Node has geoemtry data
    // TODO: Optimize vertex and index buffers in order to share them between all geometries and primitives
    if ( inNode.mesh >= 0 ) {
        const auto &mesh = input.meshes[ inNode.mesh ];

        auto geometry = crimild::alloc< Geometry >( mesh.name );

        std::shared_ptr< Material > material;

        for ( size_t primitiveIdx = 0; primitiveIdx < mesh.primitives.size(); ++primitiveIdx ) {
            const auto &inPrimitive = mesh.primitives[ primitiveIdx ];

            if ( material == nullptr ) {
                material = materials[ inPrimitive.material ];
            }

            const float *inPositions = nullptr;
            const float *inNormals = nullptr;
            const float *inTexCoords = nullptr;
            size_t vertexCount = 0;

            if ( inPrimitive.attributes.find( "POSITION" ) != inPrimitive.attributes.end() ) {
                const auto &accessor = input.accessors[ inPrimitive.attributes.find( "POSITION" )->second ];
                const auto &view = input.bufferViews[ accessor.bufferView ];
                inPositions = reinterpret_cast< const float * >( &( input.buffers[ view.buffer ].data[ accessor.byteOffset + view.byteOffset ] ) );
                vertexCount = accessor.count;
            }

            if ( inPrimitive.attributes.find( "NORMAL" ) != inPrimitive.attributes.end() ) {
                const auto &accessor = input.accessors[ inPrimitive.attributes.find( "NORMAL" )->second ];
                const auto &view = input.bufferViews[ accessor.bufferView ];
                inNormals = reinterpret_cast< const float * >( &( input.buffers[ view.buffer ].data[ accessor.byteOffset + view.byteOffset ] ) );
            }

            if ( inPrimitive.attributes.find( "TEXCOORD_0" ) != inPrimitive.attributes.end() ) {
                const auto &accessor = input.accessors[ inPrimitive.attributes.find( "TEXCOORD_0" )->second ];
                const auto &view = input.bufferViews[ accessor.bufferView ];
                inTexCoords = reinterpret_cast< const float * >( &( input.buffers[ view.buffer ].data[ accessor.byteOffset + view.byteOffset ] ) );
            }

            Array< VertexP3N3TC2 > vertices( vertexCount );
            for ( size_t v = 0; v < vertexCount; ++v ) {
                auto &vertex = vertices[ v ];
                vertex.position = Vector3 {
                    inPositions[ v * 3 + 0 ],
                    inPositions[ v * 3 + 1 ],
                    inPositions[ v * 3 + 2 ],
                };
                vertex.normal =
                    inNormals
                        ? normalize(
                            Vector3 {
                                inNormals[ v * 3 + 0 ],
                                inNormals[ v * 3 + 1 ],
                                inNormals[ v * 3 + 2 ],
                            }
                        )
                        : Vector3 { 0, 0, 0 };
                vertex.texCoord =
                    inTexCoords
                        ? Vector2 {
                              inTexCoords[ v * 2 + 0 ],
                              inTexCoords[ v * 2 + 1 ],
                          }
                        : Vector2 { 0, 0 };
            }

            const auto &accessor = input.accessors[ inPrimitive.indices ];
            const auto &bufferView = input.bufferViews[ accessor.bufferView ];
            const auto &buffer = input.buffers[ bufferView.buffer ];
            const auto indexCount = accessor.count;

            Array< UInt32 > indices( indexCount );

            switch ( accessor.componentType ) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                    const uint32_t *buff = reinterpret_cast< const uint32_t * >( &buffer.data[ accessor.byteOffset + bufferView.byteOffset ] );
                    for ( size_t index = 0; index < accessor.count; ++index ) {
                        indices[ index ] = buff[ index ];
                    }
                    break;
                }
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                    const uint16_t *buff = reinterpret_cast< const uint16_t * >( &buffer.data[ accessor.byteOffset + bufferView.byteOffset ] );
                    for ( size_t index = 0; index < accessor.count; ++index ) {
                        indices[ index ] = buff[ index ];
                    }
                    break;
                }
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                    const uint8_t *buff = reinterpret_cast< const uint8_t * >( &buffer.data[ accessor.byteOffset + bufferView.byteOffset ] );
                    for ( size_t index = 0; index < accessor.count; ++index ) {
                        indices[ index ] = buff[ index ];
                    }
                    break;
                }
                default: {
                    CRIMILD_LOG_ERROR( "Index component type ", accessor.componentType, " not supported!" );
                    return;
                }
            }

            auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
            primitive->setVertexData(
                {
                    crimild::alloc< VertexBuffer >( VertexP3N3TC2::getLayout(), vertices ),
                }
            );
            primitive->setIndices( crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, indices ) );
            geometry->attachPrimitive( primitive );
        }

        if ( material != nullptr ) {
            geometry->attachComponent< MaterialComponent >( material );
        }

        node->attachNode( geometry );
    }

    return node;
}

std::shared_ptr< Node > GLTFImporter::import( const std::filesystem::path &path ) noexcept
{

    tinygltf::Model gltfInput;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool loaded = loader.LoadASCIIFromFile( &gltfInput, &err, &warn, path.string() );
    // bool ret = loader.LoadBinaryFromFile(&gltfInput, &err, &warn, argv[1]); // for binary glTF(.glb)

    if ( !warn.empty() ) {
        CRIMILD_LOG_WARNING( "When loading ", path.string(), ": ", warn );
    }

    if ( !err.empty() ) {
        CRIMILD_LOG_ERROR( "When loading ", path.string(), ": ", err );
    }

    if ( !loaded ) {
        CRIMILD_LOG_ERROR( "Failed to load ", path.string() );
        return nullptr;
    }

    auto images = loadImages( gltfInput );
    auto imageViews = loadImageViews( images );
    auto textures = loadTextures( gltfInput, imageViews );
    auto materials = loadMaterials( gltfInput, textures );

    auto root = crimild::alloc< Group >( path.filename().string() );
    for ( size_t i = 0; i < gltfInput.scenes.size(); ++i ) {
        const auto &inScene = gltfInput.scenes[ i ];
        auto scene = crimild::alloc< Group >( inScene.name );
        for ( size_t j = 0; j < inScene.nodes.size(); ++j ) {
            const auto &inNode = gltfInput.nodes[ inScene.nodes[ j ] ];
            loadNode( inNode, gltfInput, scene, materials );
        }
        root->attachNode( scene );
    }

    return root;
}
