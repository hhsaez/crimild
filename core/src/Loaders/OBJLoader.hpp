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

#ifndef CRIMILD_LOADERS_OBJ_
#define CRIMILD_LOADERS_OBJ_

#include "SceneGraph/Group.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/AlphaState.hpp"
#include "Rendering/DepthState.hpp"

#include <map>
#include <string>
#include <vector>
#include <fstream>

namespace crimild {

	class OBJLoader {
	public:
		OBJLoader( std::string filePath );
		virtual ~OBJLoader( void );

		Pointer< Group > load( void );

	private:
		void reset( void );
		void processLine( std::ifstream &input );
		void processMaterialFile( std::string materialFileName );

		template< typename T >
		void loadData( std::ifstream &input, std::vector< T > &storage, unsigned int &usedCount, unsigned int dataCount ) 
		{
			for ( int i = 0; i < dataCount; i++ ) {
				T value;
				input >> value; 
				pushData( storage, usedCount, value );
			}
		}

		template< typename T >
		void pushData( std::vector< T > &storage, unsigned int &usedCount, T data ) 
		{
			static const unsigned int OBJ_DATA_BUFFER_GROW_BY = 1024;

			if ( usedCount >= storage.size() ) {
				// resize storage if needed, making sure there is place for at least
				// the number of items we're going to load
				storage.resize( storage.size() + OBJ_DATA_BUFFER_GROW_BY );
			}

			storage[ usedCount++ ] = data;
		}

		void pushGroup( std::string name, std::string materialName );

		Pointer< Group > generateScene( void );

	private:
		std::string _filePath;
		std::vector< float > _positions;
		unsigned int _positionCount;
		std::vector< float > _normals;
		unsigned int _normalCount;
		std::vector< float > _textureCoords;
		unsigned int _textureCoordCount;

		class GroupDef : public SharedObject {
		public:
			GroupDef( std::string name, std::string materialName )
			{
				this->name = name;
				this->materialName = materialName;
			}

			std::string name;
			std::string materialName;

			class Face {
			public:
				Face( std::string v0, std::string v1, std::string v2 )
				{
					this->v0 = v0;
					this->v1 = v1;
					this->v2 = v2;
				}

				Face( const Face &f ) 
				{ 
					v0 = f.v0;
					v1 = f.v1;
					v2 = f.v2;
				}

				Face &operator=( const Face &f )
				{ 
					v0 = f.v0;
					v1 = f.v1;
					v2 = f.v2;
					return *this;
				}

				std::string v0;
				std::string v1;
				std::string v2;
			};

			std::vector< Face > faces;
		};

		std::vector< Pointer< GroupDef > > _groups;
		GroupDef *_currentGroup;

		class MaterialDef : public SharedObject {
		public:
			std::string name;

			Pointer< Texture > diffuseMap;
			Pointer< Texture > normalMap;
			Pointer< Texture > specularMap;
			Pointer< Texture > emissiveMap;
			RGBAColorf diffuseColor;
			RGBAColorf ambientColor;
			RGBAColorf specularColor;
            Pointer< AlphaState > alphaState;
            Pointer< DepthState > depthState;

			MaterialDef( std::string name )
			{
				this->name = name;
                this->alphaState = new AlphaState( false );
                this->depthState = new DepthState( true );
			}
		};	

		std::map< std::string, Pointer< MaterialDef > > _materials;
	};

}

#endif

