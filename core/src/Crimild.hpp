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

#ifndef CRIMILD_
#define CRIMILD_

#include "Mathematics/Clock.hpp"
#include "Mathematics/Distance.hpp"
#include "Mathematics/Frustum.hpp"
#include "Mathematics/Interpolation.hpp"
#include "Mathematics/Intersection.hpp"
#include "Mathematics/Matrix.hpp"
#include "Mathematics/Numeric.hpp"
#include "Mathematics/Plane.hpp"
#include "Mathematics/Quaternion.hpp"
#include "Mathematics/Random.hpp"
#include "Mathematics/Ray.hpp"
#include "Mathematics/Rect.hpp"
#include "Mathematics/Root.hpp"
#include "Mathematics/Sphere.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Vector.hpp"

#include "Foundation/Memory.hpp"
#include "Foundation/Macros.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/StringUtils.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/Singleton.hpp"
#include "Foundation/Profiler.hpp"
#include "Foundation/Version.hpp"

#include "Boundings/BoundingVolume.hpp"
#include "Boundings/PlaneBoundingVolume.hpp"
#include "Boundings/SphereBoundingVolume.hpp"
#include "Boundings/AABBBoundingVolume.hpp"

#include "Exceptions/Exception.hpp"
#include "Exceptions/FileNotFoundException.hpp"
#include "Exceptions/RuntimeException.hpp"
#include "Exceptions/HasParentException.hpp"

#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Light.hpp"
#include "SceneGraph/Node.hpp"
#include "SceneGraph/ParticleSystem.hpp"
#include "SceneGraph/Switch.hpp"
#include "SceneGraph/Text.hpp"

#include "Components/AnimatorComponent.hpp"
#include "Components/LambdaComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/NodeComponent.hpp"
#include "Components/NodeComponentCatalog.hpp"
#include "Components/OrbitComponent.hpp"
#include "Components/RotationComponent.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Components/UIResponder.hpp"
#include "Components/SkinnedMeshComponent.hpp"

#include "Concurrency/Async.hpp"
#include "Concurrency/Task.hpp"
#include "Concurrency/TaskManager.hpp"
#include "Concurrency/Concurrency.hpp"
#include "Concurrency/Job.hpp"
#include "Concurrency/JobScheduler.hpp"
#include "Concurrency/WorkStealingDeque.hpp"

#include "Visitors/Apply.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/FetchCameras.hpp"
#include "Visitors/FetchLights.hpp"
#include "Visitors/NodeVisitor.hpp"
#include "Visitors/ComputeRenderQueue.hpp"
#include "Visitors/SelectNodes.hpp"
#include "Visitors/StartComponents.hpp"
#include "Visitors/UpdateRenderState.hpp"
#include "Visitors/UpdateWorldState.hpp"
#include "Visitors/Picking.hpp"
#include "Visitors/ShallowCopy.hpp"

#include "Primitives/Primitive.hpp"
#include "Primitives/ArcPrimitive.hpp"
#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/ParametricPrimitive.hpp"
#include "Primitives/ParametricSpherePrimitive.hpp"
#include "Primitives/ConePrimitive.hpp"
#include "Primitives/KleinBottlePrimitive.hpp"
#include "Primitives/MobiusStripPrimitive.hpp"
#include "Primitives/NewellTeapotPrimitive.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Primitives/TorusPrimitive.hpp"
#include "Primitives/TrefoilKnotPrimitive.hpp"

#include "Rendering/AlphaState.hpp"
#include "Rendering/BufferObject.hpp"
#include "Rendering/Catalog.hpp"
#include "Rendering/ColorMaskState.hpp"
#include "Rendering/CullFaceState.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageTGA.hpp"
#include "Rendering/IndexBufferObject.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderState.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderLocation.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShaderUniform.hpp"
#include "Rendering/ShaderUniformImpl.hpp"
#include "Rendering/SkinnedMesh.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/VertexBufferObject.hpp"
#include "Rendering/VertexFormat.hpp"

#include "Rendering/RenderPasses/RenderPass.hpp"
#include "Rendering/RenderPasses/StandardRenderPass.hpp"
#include "Rendering/RenderPasses/PostRenderPass.hpp"
#include "Rendering/RenderPasses/BasicRenderPass.hpp"
#include "Rendering/RenderPasses/DeferredRenderPass.hpp"
#include "Rendering/RenderPasses/ForwardRenderPass.hpp"

#include "Rendering/ImageEffects/ImageEffect.hpp"
#include "Rendering/ImageEffects/ColorTintImageEffect.hpp"

#include "Messaging/MessageQueue.hpp"

#include "Loaders/OBJLoader.hpp"

#include "Simulation/Input.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/FileSystem.hpp"

#include "Simulation/Systems/DebugSystem.hpp"
#include "Simulation/Systems/UpdateSystem.hpp"
#include "Simulation/Systems/RenderSystem.hpp"
#include "Simulation/Systems/StreamingSystem.hpp"
#include "Simulation/Systems/UISystem.hpp"

#include "Streaming/Stream.hpp"
#include "Streaming/FileStream.hpp"
#include "Streaming/MemoryStream.hpp"
#include "Streaming/Scenebuilder.hpp"

#include "Debug/DebugRenderHelper.hpp"
#include "Debug/SceneDebugDump.hpp"

#endif

