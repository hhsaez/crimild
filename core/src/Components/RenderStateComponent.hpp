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

#ifndef CRIMILD_COMPONENTS_RENDER_STATE_
#define CRIMILD_COMPONENTS_RENDER_STATE_

#include "NodeComponent.hpp"

#include "Rendering/Material.hpp"
#include "SceneGraph/Light.hpp"
#include "Foundation/Containers/Array.hpp"

#include <functional>
#include <list>

namespace crimild {

	namespace animation {
		
		class Skeleton;

	}

    class CommandBuffer;
    class DescriptorPool;
    class DescriptorSet;
    class DescriptorSetLayout;
    class IndexBuffer;
    class Pipeline;
    class Texture;
    class UniformBuffer;
    class VertexBuffer;

	class RenderStateComponent : public NodeComponent {
		CRIMILD_IMPLEMENT_RTTI( crimild::RenderStateComponent )

	public:
		RenderStateComponent( void );
		virtual ~RenderStateComponent( void );

        SharedPointer< Pipeline > pipeline;
        SharedPointer< VertexBuffer > vbo;
        SharedPointer< IndexBuffer > ibo;
        containers::Array< SharedPointer< UniformBuffer >> uniforms;
        containers::Array< SharedPointer< Texture >> textures;
        SharedPointer< DescriptorSetLayout > descriptorSetLayout;
        SharedPointer< DescriptorPool > descriptorPool;
        SharedPointer< DescriptorSet > descriptorSet;

        std::function< void( CommandBuffer * ) > commandRecorder;

        void prepare( void ) noexcept;

        SharedPointer< DescriptorSet > createDescriptorSet( DescriptorSetLayout *descriptorSetLayout ) noexcept;
        
        // Deprecated
    public:

		void reset( void );

		bool hasMaterials( void ) const { return _materials.size() > 0; }
        void attachMaterial( Material *material ) { _materials.add( crimild::retain( material ) ); }
        void attachMaterial( SharedPointer< Material > const &material ) { _materials.add( material ); }
        void detachAllMaterials( void ) { _materials.clear(); }
        void forEachMaterial( std::function< void( Material * ) > callback );

		bool hasLights( void ) const { return _lights.size() > 0; }
        void attachLight( Light *light ) { _lights.add( crimild::retain( light ) ); }
        void attachLight( SharedPointer< Light > const &light ) { _lights.add( light ); }
        void detachAllLights( void ) { _lights.clear(); }
        void forEachLight( std::function< void( Light * ) > callback );

        void setSkeleton( SharedPointer< animation::Skeleton > const &skeleton ) { _skeleton = skeleton; }
		animation::Skeleton *getSkeleton( void ) { return crimild::get_ptr( _skeleton ); }

		bool renderOnScreen( void ) const { return _renderOnScreen; }
		void setRenderOnScreen( bool value ) { _renderOnScreen = value; }

	private:
		containers::Array< SharedPointer< Material >> _materials;
		containers::Array< SharedPointer< Light >> _lights;

		SharedPointer< animation::Skeleton > _skeleton;

		bool _renderOnScreen;
	};

}

#endif

