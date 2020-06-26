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

#ifndef CRIMILD_RENDERING_SHADER_PROGRAM_
#define CRIMILD_RENDERING_SHADER_PROGRAM_

#include "Rendering/Shader.hpp"
#include "Rendering/VertexBuffer.hpp"

#include "Rendering/ShaderLocation.hpp"
#include "Rendering/ShaderUniform.hpp"
#include "Rendering/Catalog.hpp"

#include "Foundation/Containers/Array.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/RTTI.hpp"
#include "Mathematics/Vector.hpp"
#include "Mathematics/Matrix.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <list>

namespace crimild {

    class DescriptorSetLayout;
    class Light;
	class Texture;

	namespace shadergraph {

		class ShaderGraph;

	}
    
	class ShaderProgram :
		public SharedObject,
		public RTTI,
		public Catalog< ShaderProgram >::Resource {
        CRIMILD_IMPLEMENT_RTTI( crimild::ShaderProgram )

	public:
		using ShaderArray = Array< SharedPointer< Shader >>;

	public:
		explicit ShaderProgram( const ShaderArray &shaders ) noexcept;
		virtual ~ShaderProgram( void ) = default;

		ShaderArray &getShaders( void ) noexcept { return m_shaders; }

        Array< VertexLayout > vertexLayouts;
        Array< SharedPointer< DescriptorSetLayout >> descriptorSetLayouts;

    private:
		ShaderArray m_shaders;

		/**
		   \deprecated
		 */
		//@{

	private:
		using UniformArray = Array< SharedPointer< ShaderUniform >>;
		using UniformMap = Map< std::string, SharedPointer< ShaderUniform >>;
		
	public:
		class StandardLocation {
		public:
			enum {
				POSITION_ATTRIBUTE = 0,
				NORMAL_ATTRIBUTE,
				TANGENT_ATTRIBUTE,
				TEXTURE_COORD_ATTRIBUTE,
				COLOR_ATTRIBUTE,
				BONE_IDS_ATTRIBUTE,
				BONE_WEIGHTS_ATTRIBUTE,

				PROJECTION_MATRIX_UNIFORM = 100,
				VIEW_MATRIX_UNIFORM,
				MODEL_MATRIX_UNIFORM,
				NORMAL_MATRIX_UNIFORM,

                MATERIAL = 1000,
				MATERIAL_AMBIENT_UNIFORM,
				MATERIAL_DIFFUSE_UNIFORM,
				MATERIAL_SPECULAR_UNIFORM,
				MATERIAL_SHININESS_UNIFORM,
				MATERIAL_EMISSIVE_UNIFORM,

				MATERIAL_COLOR_MAP_UNIFORM = 1500,
				MATERIAL_USE_COLOR_MAP_UNIFORM,
				MATERIAL_SPECULAR_MAP_UNIFORM,
				MATERIAL_USE_SPECULAR_MAP_UNIFORM,
				MATERIAL_NORMAL_MAP_UNIFORM,
				MATERIAL_USE_NORMAL_MAP_UNIFORM,
				MATERIAL_EMISSIVE_MAP_UNIFORM,
				MATERIAL_USE_EMISSIVE_MAP_UNIFORM,
                
                USE_COLOR_MAP_UNIFORM = 2000,
                COLOR_MAP_UNIFORM,
                USE_DEPTH_MAP_UNIFORM,
                DEPTH_MAP_UNIFORM,
                USE_SHADOW_MAP_UNIFORM,
                SHADOW_MAP_UNIFORM,
				SHADOW_MAP_BIAS_UNIFORM,
				SHADOW_MAP_OFFSET_UNIFORM,
                USE_SSAO_MAP_UNIFORM,
                SSAO_MAP_UNIFORM,
                USE_GLOW_MAP_UNIFORM,
                GLOW_MAP_UNIFORM,

                USE_NORMAL_ATTRIBUTE = 3000,
                USE_TANGENT_ATTRIBUTE,
                USE_TEXTURE_COORD_ATTRIBUTE,
                USE_COLOR_ATTRIBUTE,

				LIGHT_COUNT_UNIFORM = 5000,
                LIGHT = 5050,
                LIGHT_TYPE_UNIFORM = 5051,
				LIGHT_POSITION_UNIFORM = 5100,
				LIGHT_ATTENUATION_UNIFORM = 5200,
				LIGHT_DIRECTION_UNIFORM = 5300,
				LIGHT_COLOR_UNIFORM = 5400,
				LIGHT_OUTER_CUTOFF_UNIFORM = 5500,
				LIGHT_INNER_CUTOFF_UNIFORM = 5600,
				LIGHT_EXPONENT_UNIFORM = 5700,
				LIGHT_AMBIENT_UNIFORM = 5750,
                LIGHT_SOURCE_PROJECTION_MATRIX_UNIFORM = 5800,
                LIGHT_SOURCE_VIEW_MATRIX_UNIFORM = 5900,

                SKINNED_MESH_JOINT_COUNT_UNIFORM = 6000,
                SKINNED_MESH_JOINT_POSE_UNIFORM,
                
                LINEAR_DEPTH_CONSTANT_UNIFORM = 8000,
                
                G_BUFFER_COLOR_MAP_UNIFORM = 10000,
                G_BUFFER_POSITION_MAP_UNIFORM,
                G_BUFFER_NORMAL_MAP_UNIFORM,
                G_BUFFER_EMISSIVE_MAP_UNIFORM,
                G_BUFFER_DEPTH_MAP_UNIFORM,
                
                BLEND_SRC_MAP_UNIFORM = 20000,
                BLEND_DST_MAP_UNIFORM,
                BLEND_MODE_UNIFORM,
			};
		};

	public:
		ShaderProgram( void ) = default;
		ShaderProgram( SharedPointer< VertexShader > const &vs, SharedPointer< FragmentShader > const &fs );

		void setVertexShader( SharedPointer< VertexShader > const &vs ) { _vertexShader = vs; }
        VertexShader *getVertexShader( void ) { return crimild::get_ptr( _vertexShader ); }

		void setFragmentShader( SharedPointer< FragmentShader > const &fs ) { _fragmentShader = fs; }
        FragmentShader *getFragmentShader( void ) { return crimild::get_ptr( _fragmentShader ); }

	private:
		SharedPointer< VertexShader > _vertexShader;
		SharedPointer< FragmentShader > _fragmentShader;

	public:
		void registerLocation( SharedPointer< ShaderLocation > const &location );
		ShaderLocation *registerUniformLocation( std::string name );
        ShaderLocation *getLocation( std::string name ) { return crimild::get_ptr( _locations[ name ] ); }
		void resetLocations( void );
		void forEachLocation( std::function< void( ShaderLocation * ) > callback );

		ShaderLocation *registerStandardLocation( ShaderLocation::Type locationType, unsigned int standardLocationId, std::string name );
        ShaderLocation *getStandardLocation( unsigned int standardLocationId );

	private:
		std::map< std::string, SharedPointer< ShaderLocation >> _locations;
		std::map< unsigned int, std::string > _standardLocations;

	public:
		void attachUniforms( UniformArray const &uniforms );
		void attachUniform( SharedPointer< ShaderUniform > const &uniform );

		template< class ShaderUniformType >
		ShaderUniformType *getUniform( std::string name )
		{
			return static_cast< ShaderUniformType * >(
				_uniforms.contains( name )
				    ? crimild::get_ptr( _uniforms[ name ] )
				    : nullptr 
			);
		}

		void detachAllUniforms( void );
		void forEachUniform( std::function< void( ShaderUniform * ) > callback );

		void bindUniform( std::string name, crimild::Int32 value );
        void bindUniform( std::string name, const Array< crimild::Int32 > &value );
		void bindUniform( std::string name, crimild::Real32 value );
		void bindUniform( std::string name, const Matrix3f &value );
		void bindUniform( std::string name, const Matrix4f &value );
		void bindUniform( std::string name, const RGBAColorf &value );
		void bindUniform( std::string name, const RGBColorf &value );
		void bindUniform( std::string name, Texture *value );

	private:
		UniformMap _uniforms;

	public:
		/**
		   \remarks Override to perform operations before binding uniforms
		   (like computing uniform values from other uniforms)
		*/
		virtual void willBind( Renderer *renderer );

		virtual void didBind( Renderer *renderer );

		virtual void willUnbind( Renderer *renderer );

		virtual void didUnbind( Renderer *renderer );

	public:
		void buildVertexShader( shadergraph::ShaderGraph *graph );
		void buildVertexShader( SharedPointer< shadergraph::ShaderGraph > const &graph );

		void buildFragmentShader( shadergraph::ShaderGraph *graph );
		void buildFragmentShader( SharedPointer< shadergraph::ShaderGraph > const &graph );

		//@}
	};

}

#endif

