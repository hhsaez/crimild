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

#include "SceneGraph/Node.hpp"
#include "Rendering/Texture.hpp"

#include <map>
#include <string>
#include <vector>
#include <fstream>

namespace Crimild {

	class OBJLoader {
	public:
		OBJLoader( std::string filePath );
		virtual ~OBJLoader( void );

		NodePtr load( void );

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

		NodePtr generateScene( void );

	private:
		std::string _filePath;
		std::vector< float > _positions;
		unsigned int _positionCount;
		std::vector< float > _normals;
		unsigned int _normalCount;
		std::vector< float > _textureCoords;
		unsigned int _textureCoordCount;

		class GroupDef {
		public:
			std::string name;
			std::string materialName;
			std::vector< std::string > faces;

			GroupDef( std::string name, std::string materialName )
			{
				this->name = name;
				this->materialName = materialName;
			}
		};

		std::vector< std::shared_ptr< GroupDef >> _groups;
		GroupDef *_currentGroup;

		class MaterialDef {
		public:
			std::string name;

			TexturePtr diffuseMap;

			MaterialDef( std::string name )
			{
				this->name = name;
			}
		};	

		std::map< std::string, std::shared_ptr< MaterialDef >> _materials;
	};

}

#endif

