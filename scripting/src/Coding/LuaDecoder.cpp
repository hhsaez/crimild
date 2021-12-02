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

#include "LuaDecoder.hpp"

#include <Foundation/ObjectFactory.hpp>
#include <Coding/Codable.hpp>
#include <Coding/FileDecoder.hpp>
#include <Loaders/OBJLoader.hpp>
#include <Simulation/FileSystem.hpp>
#include <Simulation/AssetManager.hpp>
#include <Exceptions/RuntimeException.hpp>

using namespace crimild;
using namespace crimild::coding;
using namespace crimild::scripting;

#ifdef CRIMILD_ENABLE_IMPORT
    #include <Crimild_Import.hpp>

    using namespace crimild::import;
#endif

LuaDecoder::LuaDecoder( std::string rootObjectName )
	: Scripted( true ),
	  _rootObjectName( rootObjectName )
{

}

LuaDecoder::~LuaDecoder( void )
{

}
            
void LuaDecoder::parse( std::string str )
{
	if ( !getScriptContext().parse( str ) ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot parse string:\n", str );
		return;
	}

	ScriptEvaluator eval( &getScriptContext(), _rootObjectName );
	_evals.push( eval );
	
	auto obj = buildObject();
	if ( obj != nullptr ) {
		addRootObject( obj );
	}
}

void LuaDecoder::parseFile( std::string filename )
{
	if ( !getScriptContext().load( filename ) ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot open lua file: ", filename );
		return;
	}

	ScriptEvaluator eval( &getScriptContext(), _rootObjectName );
	_evals.push( eval );
	
	auto obj = buildObject();
	if ( obj != nullptr ) {
		addRootObject( obj );
	}	
}

bool LuaDecoder::decodeFile( std::string fileName )
{
	if ( !getScriptContext().load( fileName ) ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot open lua file: ", fileName );
		return false;
	}

	ScriptEvaluator eval( &getScriptContext(), _rootObjectName );
	_evals.push( eval );
	
	auto obj = buildObject();
	if ( obj != nullptr ) {
		addRootObject( obj );
	}	

	return true;
}

SharedPointer< SharedObject > LuaDecoder::buildObject( void )
{
	auto &eval = _evals.top();

	SharedPointer< Codable > obj;

	std::string fileName;
	if ( eval.getPropValue( "sceneFileName", fileName ) ) {
		auto scene = AssetManager::getInstance()->get< Group >( fileName );
		if ( scene == nullptr ) {
			SharedPointer< Group > tmp;
			auto fullPath = FileSystem::getInstance().pathForResource( fileName );
            auto fileExtension = StringUtils::getFileExtension( fullPath );
            if ( fileExtension == "crimild" ) {
                coding::FileDecoder decoder;
                if ( decoder.read( fullPath ) ) {
                    if ( decoder.getObjectCount() > 0 ) {
                        tmp = decoder.getObjectAt< Group >( 0 );
                    }
                }
            }
			else if ( fileExtension == "obj" ) {
				OBJLoader loader( fullPath );
				tmp = loader.load();
			}
#ifdef CRIMILD_ENABLE_IMPORT
			else {
				SceneImporter importer;
				tmp = importer.import( fullPath );
			}
#endif
			AssetManager::getInstance()->set( fileName, tmp );
			scene = crimild::get_ptr( tmp );
		}
		// always copy assets from the cache
		ShallowCopy shallowCopy;
		scene->perform( shallowCopy );
		obj = shallowCopy.getResult< Group >();
	}
	else {
		std::string type;
		if ( !eval.getPropValue( "type", type ) ) {
			Log::warning( CRIMILD_CURRENT_CLASS_NAME, "Cannot obtain 'type' member for object while evaluating " + eval.getPrefix() );
			return nullptr;
		}

		auto builder = ObjectFactory::getInstance()->getBuilder( type );
		if ( builder == nullptr ) {
			throw crimild::RuntimeException( CRIMILD_CURRENT_CLASS_NAME, "No builder defined for type: " + type );
		}
		
        obj = crimild::cast_ptr< Codable >( builder () );
		if ( obj == nullptr ) {
			throw crimild::RuntimeException( CRIMILD_CURRENT_CLASS_NAME, "Cannot build object of type: " + type );
		}
	}

	obj->decode( *this );

	return obj;
}

crimild::Bool LuaDecoder::decode( std::string key, SharedPointer< Codable > &codable ) 
{
	if ( key.length() > 0 ) {
		std::stringstream ss;
		ss << _evals.top().getPrefix() << "." << key;
		ScriptEvaluator eval( &getScriptContext(), ss.str() );
		_evals.push( eval );
	}

    codable = crimild::dynamic_cast_ptr< Codable >( buildObject() );

	if ( key.length() > 0 ) {
		_evals.pop();
	}

	return codable != nullptr;
}

crimild::Size LuaDecoder::beginDecodingArray( std::string key )
{
	crimild::Size count = 0;
	//_evals.top().getPropValue( "#" + _evals.top().expandExpression( key ), count );
	getScriptContext().getEvaluator().getPropValue( "#" + _evals.top().expandExpression( key ), count );
	return count;
}

std::string LuaDecoder::beginDecodingArrayElement( std::string key, crimild::Size index )
{
	std::stringstream ss;
	ss << _evals.top().expandExpression( key ) << "[" << ( index + 1 ) << "]";
	ScriptEvaluator eval( &getScriptContext(), ss.str() );
	_evals.push( eval );

	return "";
}

void LuaDecoder::endDecodingArrayElement( std::string key, crimild::Size index )
{
	_evals.pop();
}

void LuaDecoder::endDecodingArray( std::string key )
{

}

