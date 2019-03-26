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

#include <Crimild.hpp>
#include <Crimild_Import.hpp>
#include <Crimild_Scripting.hpp>

using namespace crimild;
using namespace crimild::messaging;
using namespace crimild::import;
using namespace crimild::coding;

void convert( std::string inputFile, std::string outputFile )
{
	auto fileExtension = StringUtils::getFileExtension( inputFile );

	SharedPointer< Node > scene;

	CRIMILD_LOG_INFO( "Loading scene from ", inputFile );

	if ( fileExtension == "lua" ) {
		LuaDecoder decoder;
		if ( decoder.decodeFile( inputFile ) && decoder.getObjectCount() > 0 ) {
			scene = decoder.getObjectAt< Node >( 0 );
		}
	}
	else {
		SceneImporter importer;
		scene = importer.import( inputFile );
	}

	if ( scene == nullptr ) {
		CRIMILD_LOG_ERROR( "Cannot load scene" );
		return;
	}

	CRIMILD_LOG_INFO( "Scene loaded" );
	CRIMILD_LOG_INFO( "Encoding scene to ", outputFile );

	FileEncoder encoder;
	encoder.encode( scene );

	if ( !encoder.write( outputFile ) ) {
		CRIMILD_LOG_ERROR( "Cannot encode scene" );
		return;
	}

	CRIMILD_LOG_INFO( "Encoding completed" );
}

void printUsage( void )
{
	std::cout << "\nUsage: "
			  << "\n\t./sceneImporter input=<INPUT_FILE> [output=<OUTPUT_FILE>]"
			  << "\n\n";
}

int main( int argc, char **argv )
{
    crimild::init();

	Log::setLevel( Log::Level::LOG_LEVEL_INFO );

	auto settings = crimild::alloc< Settings >( argc, argv );

	AssetManager assetManager;

    std::string inputPath = settings->get< std::string >( "input", "" );
    if ( inputPath == "" ) {
    	printUsage();
    	return -1;
    }

    std::string outputPath = settings->get< std::string >( "output", "scene.crimild" );

    convert( inputPath, outputPath );

    return 0;
}

