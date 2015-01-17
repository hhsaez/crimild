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

#include "ShaderProgram.hpp"

using namespace crimild;

ShaderProgram::ShaderProgram( VertexShaderPtr const &vs, FragmentShaderPtr const &fs )
	: _vertexShader( vs ),
	  _fragmentShader( fs )
{
}

ShaderProgram::~ShaderProgram( void )
{
	resetLocations();
	detachAllUniforms();
}

void ShaderProgram::registerLocation( ShaderLocationPtr const &location )
{
	_locations[ location->getName() ] = location;
}

void ShaderProgram::resetLocations( void )
{
	for ( auto it : _locations ) {
		if ( it.second != nullptr ) {
			it.second->reset();
		}
	}
}

void ShaderProgram::foreachLocation( std::function< void( ShaderLocationPtr const & ) > callback )
{
	for ( auto it : _locations ) {
		if ( it.second != nullptr ) {
			callback( it.second );
		}
	}
}

void ShaderProgram::registerStandardLocation( ShaderLocation::Type locationType, unsigned int standardLocationId, std::string name )
{
	_standardLocations[ standardLocationId ] = name;
    registerLocation( crimild::alloc< ShaderLocation >( locationType, name ) );
}

ShaderLocationPtr ShaderProgram::getStandardLocation( unsigned int standardLocationId )
{
	return _locations[ _standardLocations[ standardLocationId ] ];
}

void ShaderProgram::attachUniform( ShaderUniformPtr const &uniform )
{
	_uniforms.push_back( uniform );
    ShaderLocationPtr location( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, uniform->getName() ) );
	uniform->setLocation( location );
	registerLocation( location );
}

void ShaderProgram::foreachUniform( std::function< void( ShaderUniformPtr const & ) > callback )
{
	for ( auto it : _uniforms ) {
		if ( it != nullptr ) {
			callback( it );
		}
	}
}

void ShaderProgram::detachAllUniforms( void )
{
	for ( auto it : _uniforms ) {
		auto location = it->getLocation();
		if ( location != nullptr ) {
			location->reset();
			it->setLocation( ShaderLocationPtr() );
		}
	}

	_uniforms.clear();
}

