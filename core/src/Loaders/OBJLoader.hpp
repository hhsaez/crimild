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
#include "SceneGraph/Geometry.hpp"

#include "Rendering/Material.hpp" 
#include "Rendering/Texture.hpp"
#include "Rendering/AlphaState.hpp"
#include "Rendering/DepthState.hpp"

#include <map>
#include <string>
#include <vector>
#include <fstream>

namespace crimild {

	class OBJLoader {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( OBJLoader )

	private:
		class FileProcessor {
		private:
			using LineProcessor = std::function< void( std::stringstream &line ) >;

		public:
			FileProcessor( void );
			~FileProcessor( void );

			void readFile( std::string fileName );

			void registerLineProcessor( std::string type, LineProcessor lineProcessor );

		private:
			std::string getLine( std::ifstream &input );
			void processLine( std::ifstream &input );

		private:
			std::map< std::string, LineProcessor > _lineProcessors;
		};

	public:
		explicit OBJLoader( std::string fileName );
		~OBJLoader( void );

		SharedPointer< Group > load( void );

	private:
		const std::string &getFileName( void ) const { return _fileName; }

		FileProcessor &getOBJProcessor( void ) { return _objProcessor; }
		FileProcessor &getMTLProcessor( void ) { return _mtlProcessor; }

		void reset( void );
		SharedPointer< Group > generateScene( void );

		void generateGeometry( void );

        void readObject( std::stringstream &line );
		void readObjectPositions( std::stringstream &line );
		void readObjectNormals( std::stringstream &line );
		void readObjectTextureCoords( std::stringstream &line );
		void readObjectFaces( std::stringstream &line );
		void readObjectMaterial( std::stringstream &line );

		void readMaterialFile( std::stringstream &line );
		void readMaterialName( std::stringstream &line );
		void readMaterialAmbient( std::stringstream &line );
		void readMaterialDiffuse( std::stringstream &line );
		void readMaterialSpecular( std::stringstream &line );
		void readMaterialColorMap( std::stringstream &line );
		void readMaterialNormalMap( std::stringstream &line );
		void readMaterialSpecularMap( std::stringstream &line );
		void readMaterialEmissiveMap( std::stringstream &line );
		void readMaterialShaderProgram( std::stringstream &line );
        
        SharedPointer< Texture > loadTexture( std::string fileName );

	private:
		std::string _fileName;

		FileProcessor _objProcessor;
		FileProcessor _mtlProcessor;

		std::list< SharedPointer< Group > > _objects;
		Group *_currentObject = nullptr;

		std::map< std::string, SharedPointer< Material > > _materials;
		Material *_currentMaterial = nullptr;

		std::vector< Vector3f > _positions;
		std::vector< Vector2f > _textureCoords;
		std::vector< Vector3f > _normals;
        std::list< std::string > _faces;
	};

}

#endif

