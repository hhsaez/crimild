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

#include "Components/MaterialComponent.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderableSet.hpp"
#include "Rendering/ScenePass.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/ApplyToGeometries.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::fetchRenderables( void ) noexcept
{
    auto fetch = crimild::alloc< ScenePass >();
    fetch->setName( "fetchRenderables" );

    auto litRenderables = crimild::alloc< RenderableSet >();
    auto unlitRenderables = crimild::alloc< RenderableSet >();
    auto envRenderables = crimild::alloc< RenderableSet >();

    fetch->apply = [ litRenderables, unlitRenderables, envRenderables ]( auto, auto ) {
        envRenderables->reset();
        litRenderables->reset();
        unlitRenderables->reset();

        // envRenderables->setCamera( Camera::getMainCamera() );
        // litRenderables->setCamera( Camera::getMainCamera() );
        // unlitRenderables->setCamera( Camera::getMainCamera() );

        auto scene = Simulation::getInstance()->getScene();
        if ( scene == nullptr ) {
            return false;
        }

        scene->perform(
            ApplyToGeometries(
                [ & ]( Geometry *geometry ) {
                    if ( geometry->getLayer() == Node::Layer::SKYBOX ) {
                        envRenderables->addGeometry( geometry );
                    } else if ( auto material = geometry->getComponent< MaterialComponent >()->first() ) {
                        // TODO: What if there are multiple materials?
                        // Can we add the same geometry to multiple lists? That will result
                        // in multiple render passes for a single geometry, but I don't know
                        // if that's ok.
                        if ( auto pipeline = material->getGraphicsPipeline() ) {
                            if ( auto program = crimild::get_ptr( pipeline->getProgram() ) ) {
                                auto isLit = false;
                                program->descriptorSetLayouts.each(
                                    [ & ]( auto layout ) {
                                        if ( layout->bindings.filter( []( auto &binding ) { return binding.descriptorType == DescriptorType::ALBEDO_MAP; } ).size() > 0 ) {
                                            isLit = true;
                                        }
                                    } );
                                if ( isLit ) {
                                    litRenderables->addGeometry( geometry );
                                } else {
                                    unlitRenderables->addGeometry( geometry );
                                }
                            }
                        }
                    }
                } ) );
        return true;
    };

    fetch->writes( { litRenderables, unlitRenderables, envRenderables } );
    fetch->produces( { litRenderables, unlitRenderables, envRenderables } );

    return fetch;
}
