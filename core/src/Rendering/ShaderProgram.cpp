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
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ShaderProgram.hpp"
#include "Shader.hpp"
#include "ShaderUniformImpl.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

ShaderProgram::ShaderProgram( const ShaderArray &shaders ) noexcept
	: m_shaders( shaders )
{

}

ShaderProgram::ShaderProgram( SharedPointer< VertexShader > const &vs, SharedPointer< FragmentShader > const &fs )
	: _vertexShader( vs ),
	  _fragmentShader( fs )
{
}

void ShaderProgram::registerLocation( SharedPointer< ShaderLocation > const &location )
{
	_locations[ location->getName() ] = location;
	location->setProgram( this );
}

ShaderLocation *ShaderProgram::registerUniformLocation( std::string name )
{
	auto loc = crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, name );
	registerLocation( loc );
	return crimild::get_ptr( loc );
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
	_uniforms[ uniform->getName() ] = uniform;

    auto location = crimild::retain( uniform->getLocation() );
    if ( location == nullptr ) {
        location = crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, uniform->getName() );
        uniform->setLocation( location );
    }

    registerLocation( location );
}

void ShaderProgram::forEachUniform( std::function< void( ShaderUniform * ) > callback )
{
	_uniforms.eachValue( [ callback ]( SharedPointer< ShaderUniform > const &uniform ) {
		if ( auto u = crimild::get_ptr( uniform ) ) {
			callback( u );
		}
	});
}

void ShaderProgram::detachAllUniforms( void )
{
	forEachUniform( []( ShaderUniform *uniform ) {
		if ( auto location = uniform->getLocation() ) {
			location->reset();
			uniform->setLocation( nullptr );
		}
	});

	_uniforms.clear();
}

void ShaderProgram::bindUniform( std::string name, crimild::Int32 value )
{
	if ( auto uniform = getUniform< IntUniform >( name ) ) {
		uniform->setValue( value );
	}
}

void ShaderProgram::bindUniform( std::string name, const Array< crimild::Int32 > &value )
{
    if ( auto uniform = getUniform< Int32ArrayUniform >( name ) ) {
        uniform->setValue( value );
    }
}

void ShaderProgram::bindUniform( std::string name, crimild::Real32 value )
{
	if ( auto uniform = getUniform< FloatUniform >( name ) ) {
		uniform->setValue( value );
	}
}

void ShaderProgram::bindUniform( std::string name, const Matrix3f &value )
{
	if ( auto uniform = getUniform< Matrix3fUniform >( name ) ) {
		uniform->setValue( value );
	}
}

void ShaderProgram::bindUniform( std::string name, const Matrix4f &value )
{
	if ( auto uniform = getUniform< Matrix4fUniform >( name ) ) {
		uniform->setValue( value );
	}
}

void ShaderProgram::bindUniform( std::string name, const RGBAColorf &value )
{
	if ( auto uniform = getUniform< RGBAColorfUniform >( name ) ) {
		uniform->setValue( value );
	}
}

void ShaderProgram::bindUniform( std::string name, const RGBColorf &value )
{
	if ( auto uniform = getUniform< RGBColorfUniform >( name ) ) {
		uniform->setValue( value );
	}
}

void ShaderProgram::bindUniform( std::string name, Texture *value )
{
	if ( auto uniform = getUniform< TextureUniform >( name ) ) {
		uniform->setValue( value );
	}
}

void ShaderProgram::willBind( Renderer *renderer )
{
	// no-op
}

void ShaderProgram::didBind( Renderer *renderer )
{
	forEachUniform( [ renderer ]( ShaderUniform *uniform ) {
		if ( uniform->getLocation() != nullptr ) {
			uniform->onBind( renderer );
		}
	});
}

void ShaderProgram::willUnbind( Renderer *renderer )
{
	// no-op
}

void ShaderProgram::didUnbind( Renderer *renderer )
{
	forEachUniform( [ renderer ]( ShaderUniform *uniform ) {
		if ( uniform->getLocation() != nullptr ) {
			uniform->onUnbind( renderer );
		}
	});
}

void ShaderProgram::buildVertexShader( ShaderGraph *graph )
{
    /*
	auto src = graph->build();
	setVertexShader( crimild::alloc< VertexShader >( src ) );
	graph->eachUniform( [ this ]( ShaderUniform *uniform ) {
		attachUniform( crimild::retain( uniform ) );
	});
    */
}

void ShaderProgram::buildVertexShader( SharedPointer< ShaderGraph > const &graph )
{
	buildVertexShader( crimild::get_ptr( graph ) );
}

void ShaderProgram::buildFragmentShader( ShaderGraph *graph )
{
    /*
	auto src = graph->build();
	setFragmentShader( crimild::alloc< FragmentShader >( src ) );
	graph->eachUniform( [ this ]( ShaderUniform *uniform ) {
		attachUniform( crimild::retain( uniform ) );
	});
    */
}

void ShaderProgram::buildFragmentShader( SharedPointer< ShaderGraph > const &graph )
{
	buildFragmentShader( crimild::get_ptr( graph ) );
}
