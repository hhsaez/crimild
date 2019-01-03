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

ShaderProgram::ShaderProgram( void )
{

}

ShaderProgram::ShaderProgram( SharedPointer< VertexShader > const &vs, SharedPointer< FragmentShader > const &fs )
	: _vertexShader( vs ),
	  _fragmentShader( fs )
{
}

ShaderProgram::~ShaderProgram( void )
{
	resetLocations();
	detachAllUniforms();
}

void ShaderProgram::registerLocation( SharedPointer< ShaderLocation > const &location )
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

void ShaderProgram::forEachLocation( std::function< void( ShaderLocation * ) > callback )
{
	for ( auto it : _locations ) {
		if ( it.second != nullptr ) {
            callback( crimild::get_ptr( it.second ) );
		}
	}
}

ShaderLocation *ShaderProgram::registerStandardLocation( ShaderLocation::Type locationType, unsigned int standardLocationId, std::string name )
{
	_standardLocations[ standardLocationId ] = name;
    auto location = crimild::alloc< ShaderLocation >( locationType, name );
    registerLocation( location );
    return crimild::get_ptr( location );
}

ShaderLocation *ShaderProgram::getStandardLocation( unsigned int standardLocationId )
{
    return crimild::get_ptr( _locations[ _standardLocations[ standardLocationId ] ] );
}

void ShaderProgram::attachUniforms( ShaderProgram::UniformArray const &uniforms )
{
	uniforms.each( [ this ]( SharedPointer< ShaderUniform > const &uniform ) {
		attachUniform( uniform );
	});
}

void ShaderProgram::attachUniform( SharedPointer< ShaderUniform > const &uniform )
{
	_uniforms.add( uniform );
    
    auto location = crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, uniform->getName() );
	uniform->setLocation( location );
	registerLocation( location );
}

void ShaderProgram::forEachUniform( std::function< void( ShaderUniform * ) > callback )
{
	_uniforms.each( [ callback ]( SharedPointer< ShaderUniform > &uniform ) {
		callback( crimild::get_ptr( uniform ) );
	});
}

void ShaderProgram::detachAllUniforms( void )
{
    _uniforms.each( []( SharedPointer< ShaderUniform > &uniform ) {
        auto location = uniform->getLocation();
        if ( location != nullptr ) {
            location->reset();
            uniform->setLocation( nullptr );
        }
    });

	_uniforms.clear();
}

