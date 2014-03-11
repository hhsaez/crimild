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

#include "Rendering/ForwardRenderPass.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/VisibilitySet.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Components/SkinComponent.hpp"
#include "Components/JointComponent.hpp"
#include "Primitives/QuadPrimitive.hpp"

using namespace crimild;

ForwardRenderPass::ForwardRenderPass( void )
{
    
}

ForwardRenderPass::~ForwardRenderPass( void )
{
    
}

void ForwardRenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    computeShadowMaps( renderer, renderQueue, camera );
    
    if ( _forwardPassBuffer == nullptr ) {
        _forwardPassBuffer.set( new FrameBufferObject( renderer->getScreenBuffer() ) );
    }
    
    renderer->bindFrameBuffer( _forwardPassBuffer.get() );
    
    renderShadedObject( renderer, renderQueue, camera );
    
    renderer->unbindFrameBuffer( _forwardPassBuffer.get() );
    
    RenderPass::render( renderer, _forwardPassBuffer.get(), nullptr );
}

void ForwardRenderPass::computeShadowMaps( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    ShaderProgram *program = renderer->getDepthProgram();
    if ( program == nullptr ) {
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    renderQueue->getLights().each( [&]( Light *light, int ) {
        
        ShadowMap *map = _shadowMaps[ light ].get();
        if ( map == nullptr ) {
            Pointer< ShadowMap > shadowMapPtr( new ShadowMap( light ) );
            shadowMapPtr->getBuffer()->setClearColor( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
            shadowMapPtr->setLightProjectionMatrix( light->computeProjectionMatrix() );
            _shadowMaps[ light ] = shadowMapPtr;
            map = shadowMapPtr.get();
        }
        
        map->setLightViewMatrix( light->computeViewMatrix() );
        
        renderer->bindFrameBuffer( map->getBuffer() );
        
        renderQueue->getOpaqueObjects().each( [&]( Geometry *geometry, int ) {
            RenderStateComponent *renderState = geometry->getComponent< RenderStateComponent >();
            if ( renderState->hasMaterials() ) {
                geometry->foreachPrimitive( [&]( Primitive *primitive ) mutable {
                    // bind joints and other skinning information
                    SkinComponent *skinning = geometry->getComponent< SkinComponent >();
                    if ( skinning != nullptr && skinning->hasJoints() ) {
                        skinning->foreachJoint( [&]( Node *node, unsigned int index ) {
                            JointComponent *joint = node->getComponent< JointComponent >();
                            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::JOINT_WORLD_MATRIX_UNIFORM + index ), joint->getWorldMatrix() );
                            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::JOINT_INVERSE_BIND_MATRIX_UNIFORM + index ), joint->getInverseBindMatrix() );
                        });
                    }
                    
                    // bind vertex and index buffers
                    renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
                    renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
                    
                    renderer->applyTransformations( program, map->getLightProjectionMatrix(), map->getLightViewMatrix(), geometry->getWorld().computeModelMatrix(), geometry->getWorld().computeNormalMatrix() );
                    
                    // draw primitive
                    renderer->drawPrimitive( program, primitive );
                    
                    // unbind primitive buffers
                    renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
                    renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
                    
                });
            }
        });
        
        renderer->unbindFrameBuffer( map->getBuffer() );
    });
    
    // unbind the shader program
    renderer->unbindProgram( program );
}

void ForwardRenderPass::renderShadedObject( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    renderQueue->getOpaqueObjects().each( [&]( Geometry *geometry, int ) {
        
        RenderStateComponent *renderState = geometry->getComponent< RenderStateComponent >();
        renderState->foreachMaterial( [&]( Material *material ) {
            geometry->foreachPrimitive( [&]( Primitive *primitive ) mutable {
                ShaderProgram *program = renderer->getForwardPassProgram();
                if ( program == nullptr ) {
                    program = material->getProgram() != nullptr ? material->getProgram() : renderer->getFallbackProgram( material, geometry, primitive );
                }
                
                if ( program == nullptr ) {
                    return;
                }
                
                renderer->bindProgram( program );
                
                // bind material properties
                renderer->bindMaterial( program, material );
                
                for ( auto it : _shadowMaps ) {
                    if ( it.second != nullptr ) {
                        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_SOURCE_PROJECTION_MATRIX_UNIFORM ), it.second->getLightProjectionMatrix() );
                        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_SOURCE_VIEW_MATRIX_UNIFORM ), it.second->getLightViewMatrix() );
                        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_SHADOW_MAP_UNIFORM ), true );
                        renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SHADOW_MAP_UNIFORM ), it.second->getBuffer()->getTexture() );
                    }
                }
                
                // bind lights
                renderQueue->getLights().each( [&]( Light *light, int ) {
                    renderer->bindLight( program, light );
                });
                
                // bind joints and other skinning information
                SkinComponent *skinning = geometry->getComponent< SkinComponent >();
                if ( skinning != nullptr && skinning->hasJoints() ) {
                    skinning->foreachJoint( [&]( Node *node, unsigned int index ) {
                        JointComponent *joint = node->getComponent< JointComponent >();
                        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::JOINT_WORLD_MATRIX_UNIFORM + index ), joint->getWorldMatrix() );
                        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::JOINT_INVERSE_BIND_MATRIX_UNIFORM + index ), joint->getInverseBindMatrix() );
                    });
                }
                
                // bind vertex and index buffers
                renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
                renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
                
                renderer->applyTransformations( program, geometry, camera );
                
                // draw primitive
                renderer->drawPrimitive( program, primitive );
                
                renderer->restoreTransformations( program, geometry, camera );
                
                // unbind primitive buffers
                renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
                renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
                
                // unbind lights
                renderQueue->getLights().each( [&]( Light *light, int ) {
                    renderer->unbindLight( program, light );
                });
                
                for ( auto it : _shadowMaps ) {
                    if ( it.second != nullptr ) {
                        renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SHADOW_MAP_UNIFORM ), it.second->getBuffer()->getTexture() );
                    }
                }
                
                // unbind material properties
                renderer->unbindMaterial( program, material );
                
                renderer->unbindProgram( program );
            });
        });
    });

}

