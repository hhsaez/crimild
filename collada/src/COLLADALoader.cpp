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

#include "COLLADALoader.hpp"

using namespace crimild;
using namespace crimild::collada;

COLLADALoader::COLLADALoader( std::string filePath )
	: _filePath( filePath )
{
	_assetsDirectory = FileSystem::getInstance().extractDirectory( _filePath ) + "/";
}

COLLADALoader::~COLLADALoader( void )
{

}

Pointer< crimild::Node > COLLADALoader::load( void )
{
	Log::Info << "Loading COLLADA scene from file " << _filePath << Log::End;

	_document = xmlReadFile( _filePath.c_str(), NULL, 0 );
	if ( _document == NULL ) {
		throw FileNotFoundException( _filePath );
	}

	_result = new Group( _filePath );
	_rootElement = xmlDocGetRootElement( _document );

	XMLUtils::loadLibrary( _rootElement, COLLADA_LIBRARY_IMAGES, _images );
	XMLUtils::loadLibrary( _rootElement, COLLADA_LIBRARY_EFFECTS, _effects );
	XMLUtils::loadLibrary( _rootElement, COLLADA_LIBRARY_MATERIALS, _materials );
	XMLUtils::loadLibrary( _rootElement, COLLADA_LIBRARY_GEOMETRIES, _geometries );
	XMLUtils::loadLibrary( _rootElement, COLLADA_LIBRARY_CONTROLLERS, _controllers );
	XMLUtils::loadLibrary( _rootElement, COLLADA_LIBRARY_ANIMATIONS, _animations );
	XMLUtils::loadLibrary( _rootElement, COLLADA_LIBRARY_VISUAL_SCENES, _visualScenes );

	parseVisualScenes();
	parseAnimations();

	xmlFreeDoc( _document );
	xmlCleanupParser();
	xmlMemoryDump();

	return _result;
}

void COLLADALoader::parseVisualScenes( void )
{
	_visualScenes.foreach( [&]( VisualScene *visualScene ) {
		Pointer< Group > scene( new Group( visualScene->getID() ) );

		visualScene->getNodes()->foreach( [&]( collada::Node *node ) {
			parseNode( scene.get(), node );

			InstanceController *instanceController = node->getInstanceController();
			if ( instanceController != NULL ) {
				Controller *controller = _controllers.get( instanceController->getControllerID() );
				if ( controller ) {
					parseController( scene.get(), controller );
				}
				else {
					Log::Warning << "No controller found for id " << instanceController->getControllerID() << Log::End;
				}
			}
		});

		_result->attachNode( scene.get() );
	});
}

void COLLADALoader::parseNode( Group *parent, collada::Node *node )
{
	Pointer< Group > group( new Group() );
	if ( node->getSID() != NULL ) {
		group->setName( node->getSID() );
	}
	else if ( node->getID() != NULL ) {
		group->setName( node->getID() );
	}
	else {
		group->setName( node->getName() );
	}

	parent->attachNode( group.get() );

	collada::Matrix *matrix = node->getMatrix();
	if ( matrix != NULL ) {
		float rotate[ 9 ];
		rotate[ 0 ] = matrix->getData()[ 0 ];
		rotate[ 1 ] = matrix->getData()[ 1 ];
		rotate[ 2 ] = matrix->getData()[ 2 ];
		rotate[ 3 ] = matrix->getData()[ 4 ];
		rotate[ 4 ] = matrix->getData()[ 5 ];
		rotate[ 5 ] = matrix->getData()[ 6 ];
		rotate[ 6 ] = matrix->getData()[ 8 ];
		rotate[ 7 ] = matrix->getData()[ 9 ];
		rotate[ 8 ] = matrix->getData()[ 10 ];

		group->local().setTranslate( matrix->getData()[ 3 ], matrix->getData()[ 7 ], matrix->getData()[ 11 ] );
		group->local().rotate().fromRotationMatrix( Matrix3f( rotate ) );
	}

	node->getNodes()->foreach( [&]( collada::Node *child ) {
		parseNode( group.get(), child );
	});
}

void COLLADALoader::parseController( Group *parent, Controller *controller )
{
	collada::Skin *skin = controller->getSkin();
	if ( skin != NULL ) {
		parseSkin( parent, skin );
	}
}

void COLLADALoader::parseSkin( Group *parent, Skin *skin )
{
	Log::Debug << "Creating geometry nodes from skin" << Log::End;

	collada::Geometry *geometry = _geometries.get( skin->getSourceID() );
	if ( geometry == NULL ) {
		Log::Error << "Cannot find geometry matching with " << skin->getSourceID() << Log::End;
		return;
	}
	
	Log::Debug << "Found geometry with id " << skin->getSourceID() << Log::End;

	collada::Mesh *mesh = geometry->getMesh();
	if ( mesh == NULL ) {
		Log::Error << "Cannot find valid mesh object in geometry" << Log::End;
		return;
	}

	Log::Debug << "Found mesh object in geometry" << Log::End;

	collada::Vertices *vertices = mesh->getVertices();
	if ( vertices == NULL ) {
		Log::Error << "Cannot find vertices object in mesh" << Log::End;
		return;
	}

	Log::Debug << "Found vertices information for mesh" << Log::End;

	mesh->getTriangles()->foreach( [&]( collada::Triangles *triangles ) {
		parseTriangles( parent, mesh, vertices, triangles );
	});
}

void COLLADALoader::parseTriangles( crimild::Group *parent, Mesh *mesh, Vertices *vertices, Triangles *triangles ) 
{
	unsigned int vertexComponentCount = 0;
	unsigned int vertexCount = 0;
	unsigned int positionComponents = 0;
	unsigned int textureCoordinateComponents = 0;
	unsigned int normalComponents = 0;
	const float *positions = NULL;
	unsigned int positionIndexOffset = 0;
	const float *normals = NULL;
	unsigned int normalIndexOffset = 0;
	const float *textureCoords = NULL;
	unsigned int textureCoordsIndexOffset = 0;

	Log::Debug << "Reading input information from triangle" << Log::End;
	triangles->getInputs()->foreach( [&]( collada::Input *input ) {
		std::string semantic = input->getSemantic();

		if ( semantic == COLLADA_SEMANTIC_VERTEX && vertices->getInputs()->getCount() > 0 ) {
			Log::Debug << "Reading input information for " << semantic.c_str() << Log::End;

			bool vertexInputFound = false;
			vertices->getInputs()->foreach( [&]( collada::Input *vertexInput ) {
				std::string semantic = vertexInput->getSemantic();
				Log::Debug << "Processing vertex input semantic: " << semantic << Log::End;

				if ( semantic == COLLADA_SEMANTIC_POSITION ) {
					collada::Source *source = mesh->getSources()->get( vertexInput->getSourceID() );
					if ( source == NULL ) {
						Log::Error << "Cannot find source for VERTEX input" << Log::End;
						return;
					}

					positionComponents = source->getTechniqueCommon()->getAccessor()->getParams()->getCount();

					vertexCount = source->getTechniqueCommon()->getAccessor()->getCount();
					if ( vertexCount > 0 ) {
						positions = source->getFloatArray();
					}
					else {
						Log::Error << "Not enough vertices" << Log::End;
						return;
					}

					++vertexComponentCount;
					vertexInputFound = true;
				}
				else if ( semantic == COLLADA_SEMANTIC_NORMAL ) {
					collada::Source *source = mesh->getSources()->get( vertexInput->getSourceID() );
					if ( source == NULL ) {
						Log::Error << "Cannot find source for NORMAL input" << Log::End;
						return;
					}

					normals = source->getFloatArray();
					normalComponents = source->getTechniqueCommon()->getAccessor()->getParams()->getCount();
					++vertexComponentCount;
				}
				else if ( semantic == COLLADA_SEMANTIC_TEXCOORD ) {
					collada::Source *source = mesh->getSources()->get( vertexInput->getSourceID() );
					if ( source == NULL ) {
						Log::Error << "Cannot find source for TEXCOORD input" << Log::End;
						return;
					}

					textureCoords = source->getFloatArray();
					textureCoordinateComponents = source->getTechniqueCommon()->getAccessor()->getParams()->getCount();
					++vertexComponentCount;
				}
			});
		}
		else if ( semantic == COLLADA_SEMANTIC_NORMAL ) {
			Log::Debug << "Reading input information for " << semantic.c_str() << Log::End;
			collada::Source *source = mesh->getSources()->get( input->getSourceID() );
			if ( source == NULL ) {
				Log::Error << "Cannot find source for NORMAL input" << Log::End;
				return;
			}

			normals = source->getFloatArray();
			normalComponents = source->getTechniqueCommon()->getAccessor()->getParams()->getCount();
			normalIndexOffset = positionIndexOffset + 1;
			++vertexComponentCount;
		}
		else if ( semantic == COLLADA_SEMANTIC_TEXCOORD ) {
			Log::Debug << "Reading input information for " << semantic.c_str() << Log::End;
			collada::Source *source = mesh->getSources()->get( input->getSourceID() );
			if ( source == NULL ) {
				Log::Error << "Cannot find source for TEXCOORD input" << Log::End;
				return;
			}

			textureCoords = source->getFloatArray();
			textureCoordinateComponents = source->getTechniqueCommon()->getAccessor()->getParams()->getCount();
			textureCoordsIndexOffset = normalIndexOffset + 1;
			++vertexComponentCount;
		}
		else {
			Log::Warning << "Unknown semantic " << semantic.c_str() << Log::End;
		}
	});

	if ( positionComponents == 0 || positions == NULL ) {
		Log::Error << "Cannot obtain position information for geometry" << Log::End;
		return;
	}

	VertexFormat format( positionComponents,
						 0,
						 normalComponents,
						 0,
						 textureCoordinateComponents );
	Log::Debug << "Vertex format = " << format << Log::End;

	Log::Debug << "Allocating vertex buffer with size " << vertexCount << Log::End;
	Pointer< VertexBufferObject > vbo( new VertexBufferObject( format, vertexCount, NULL ) );
	float *vertexData = vbo->getData();
	memset( vertexData, 0, format.getVertexSizeInBytes() * vbo->getVertexCount() );

	unsigned int triangleCount = triangles->getCount();
	if ( triangleCount == 0 ) {
		Log::Error << "Not enough data for triangles" << Log::End;
		return;
	}

	const unsigned int *indices = triangles->getIndices();
	if ( indices == NULL ) {
		Log::Error << "No index data found in triangles object" << Log::End;
		return;
	}

	Log::Debug << "Allocating index buffer with size " << triangleCount * 3 << Log::End;
	Pointer< IndexBufferObject > ibo( new IndexBufferObject( triangleCount * 3, NULL ) );
	unsigned short *indexData = ibo->getData();

	unsigned int vertexIndexOffset = Numerici::max( 1, positionIndexOffset + normalIndexOffset + textureCoordsIndexOffset );

	Log::Debug << "Triangle count: " << triangleCount
			   << "\nIndex count: " << triangleCount * 3
			   << "\nPosition Offset: " << positionIndexOffset
			   << "\nNormal offset " << normalIndexOffset
			   << "\nTC offset: " << textureCoordsIndexOffset
			   << "\nVertex components: " << vertexComponentCount
			   << "\nVertex index offset: " << vertexIndexOffset
			   << Log::End;

	for ( unsigned int triangleIdx = 0; triangleIdx < triangleCount; triangleIdx++ ) {
		unsigned int triangleIdxOffset = triangleIdx * 3 * vertexIndexOffset;

		for ( unsigned int vertexIdx = 0; vertexIdx < 3; vertexIdx++ ) {
			unsigned int currentVertexIdx = indices[ triangleIdxOffset + vertexIdx * vertexIndexOffset + positionIndexOffset ];

			indexData[ triangleIdx * 3 + vertexIdx ] = currentVertexIdx;

			if ( format.hasPositions() ) {
				for ( unsigned int k = 0; k < format.getPositionComponents(); k++ ) {
					vertexData[ currentVertexIdx * format.getVertexSize() + format.getPositionsOffset() + k ] = positions[ currentVertexIdx * format.getPositionComponents() + k ];
				}
			}

			if ( format.hasNormals() ) {
				// for normal data, we add whatever value is in the input array
				// to what is already stored (that's why we need to initialize the
				// vertex buffer to zero). Normalization is performed in the rendering
				// step, so it shouldn't be required.
				unsigned int normalIdx = indices[ triangleIdxOffset + vertexIdx * vertexIndexOffset + normalIndexOffset ];
				for ( unsigned int k = 0; k < format.getNormalComponents(); k++ ) {
					vertexData[ currentVertexIdx * format.getVertexSize() + format.getNormalsOffset() + k ] += normals[ normalIdx * format.getNormalComponents() + k ];
				}
			}

			if ( format.hasTextureCoords() ) {
				unsigned int textureCoordsIdx = indices[ triangleIdxOffset + vertexIdx * vertexIndexOffset + textureCoordsIndexOffset ];
				for ( unsigned int k = 0; k < format.getTextureCoordComponents(); k++ ) {
					vertexData[ currentVertexIdx * format.getVertexSize() + format.getTextureCoordsOffset() + k ] += textureCoords[ textureCoordsIdx * format.getTextureCoordComponents() + k ];
				}
			}
		}
	}

	Pointer< Primitive > primitive( new Primitive( Primitive::Type::TRIANGLES ) );
	primitive->setVertexBuffer( vbo.get() );
	primitive->setIndexBuffer( ibo.get() );

	Pointer< crimild::Geometry > geometry( new crimild::Geometry() );
	geometry->attachPrimitive( primitive.get() );
	parent->attachNode( geometry.get() );

	if ( triangles->getMaterial().length() > 0 ) {
		collada::Material *m = _materials.get( triangles->getMaterial() );		
		if ( m != nullptr ) {
			Pointer< crimild::Material > material( new crimild::Material() );
			geometry->getComponent< MaterialComponent >()->attachMaterial( material.get() );

			collada::Effect *e = _effects.get( m->getInstanceEffect()->getUrl() );
			if ( e != nullptr ) {
				ProfileCommon *profileCommon = e->getProfileCommon();
				if ( profileCommon != nullptr ) {
					Technique *technique = profileCommon->getTechnique();
					if ( technique != nullptr ) {
						Phong *phong = technique->getPhong();
						if ( phong != nullptr ) {
							Diffuse *diffuse = phong->getDiffuse();
							if ( diffuse != nullptr ) {
								collada::Texture *texture = diffuse->getTexture();
								if ( texture != nullptr ) {
									std::string samplerId = texture->getTexture();
									NewParam *samplerParam = profileCommon->getNewParams()->get( samplerId );
									if ( samplerParam != nullptr ) {
										std::string surfaceName = samplerParam->getSampler2D()->getSourceName();
										NewParam *surfaceParam = profileCommon->getNewParams()->get( surfaceName );
										if ( surfaceParam != nullptr ) {
											collada::Image *i = _images.get( surfaceParam->getSurface()->getImageID() );
											if ( i != nullptr ) {
												try {
													std::string imagePath = _assetsDirectory + i->getFileName();
													imagePath = FileSystem::getInstance().pathForResource( imagePath );
													Pointer< crimild::Image > image( new ImageTGA( imagePath ) );
													Pointer< crimild::Texture > diffuseTexture( new crimild::Texture( image.get() ) );
													material->setColorMap( diffuseTexture.get() );
												}
												catch ( ... ) {
													Log::Error << "Cannot load image for diffuse texture" << Log::End;
												}
											}
											else {
												Log::Warning << "No image found " << surfaceParam->getSurface()->getImageID() << Log::End;
											}
										}
										else {
											Log::Warning << "No surface param for " << surfaceName << Log::End;
										}
									}
									else {
										Log::Warning << "No sampler param for " << samplerId << Log::End;
									}
								}
							}
						}
					}
				}
			}
			else {
				Log::Warning << "No effect found for url " << m->getInstanceEffect()->getUrl() << Log::End;
			}
		}
		else {
			Log::Warning << "No material found with id " << triangles->getMaterial() << Log::End;
		}
	}
}

void COLLADALoader::parseAnimations( void )
{

}

