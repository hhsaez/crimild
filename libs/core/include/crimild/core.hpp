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

#ifndef CRIMILD_CORE_
#define CRIMILD_CORE_

#include <crimild/core/Animation/Animation.hpp>
#include <crimild/core/Animation/ChannelImpl.hpp>
#include <crimild/core/Animation/Clip.hpp>
#include <crimild/core/Animation/Skeleton.hpp>
#include <crimild/core/Assemblies/Assembly.hpp>
#include <crimild/core/Audio/AudioListener.hpp>
#include <crimild/core/Audio/AudioSource.hpp>
#include <crimild/core/Behaviors/Actions/AnimateContextValue.hpp>
#include <crimild/core/Behaviors/Actions/AnimateParticleSystem.hpp>
#include <crimild/core/Behaviors/Actions/AnimateSettingValue.hpp>
#include <crimild/core/Behaviors/Actions/ClampPosition.hpp>
#include <crimild/core/Behaviors/Actions/ClampPositionToView.hpp>
#include <crimild/core/Behaviors/Actions/ClearTargets.hpp>
#include <crimild/core/Behaviors/Actions/CopyTransformFromTarget.hpp>
#include <crimild/core/Behaviors/Actions/EnableNode.hpp>
#include <crimild/core/Behaviors/Actions/ExecuteBehavior.hpp>
#include <crimild/core/Behaviors/Actions/ExecuteBehaviorOnTarget.hpp>
#include <crimild/core/Behaviors/Actions/FindTarget.hpp>
#include <crimild/core/Behaviors/Actions/FollowPath.hpp>
#include <crimild/core/Behaviors/Actions/LoadScene.hpp>
#include <crimild/core/Behaviors/Actions/LookAt.hpp>
#include <crimild/core/Behaviors/Actions/MotionApply.hpp>
#include <crimild/core/Behaviors/Actions/MotionAvoidOthers.hpp>
#include <crimild/core/Behaviors/Actions/MotionAvoidWalls.hpp>
#include <crimild/core/Behaviors/Actions/MotionComputePathToTarget.hpp>
#include <crimild/core/Behaviors/Actions/MotionFromInput.hpp>
#include <crimild/core/Behaviors/Actions/MotionReset.hpp>
#include <crimild/core/Behaviors/Actions/MotionSeek.hpp>
#include <crimild/core/Behaviors/Actions/PrintMessage.hpp>
#include <crimild/core/Behaviors/Actions/ResetNavigation.hpp>
#include <crimild/core/Behaviors/Actions/Rotate.hpp>
#include <crimild/core/Behaviors/Actions/SetContextValue.hpp>
#include <crimild/core/Behaviors/Actions/SetSettingValue.hpp>
#include <crimild/core/Behaviors/Actions/Success.hpp>
#include <crimild/core/Behaviors/Actions/Transform.hpp>
#include <crimild/core/Behaviors/Actions/Trigger.hpp>
#include <crimild/core/Behaviors/Actions/Wait.hpp>
#include <crimild/core/Behaviors/Behavior.hpp>
#include <crimild/core/Behaviors/BehaviorContext.hpp>
#include <crimild/core/Behaviors/BehaviorController.hpp>
#include <crimild/core/Behaviors/BehaviorTree.hpp>
#include <crimild/core/Behaviors/Composites/Composite.hpp>
#include <crimild/core/Behaviors/Composites/Parallel.hpp>
#include <crimild/core/Behaviors/Composites/Selector.hpp>
#include <crimild/core/Behaviors/Composites/Sequence.hpp>
#include <crimild/core/Behaviors/Conditions/DistanceToTarget.hpp>
#include <crimild/core/Behaviors/Conditions/HasTargets.hpp>
#include <crimild/core/Behaviors/Conditions/IsAtTarget.hpp>
#include <crimild/core/Behaviors/Conditions/TestContextValue.hpp>
#include <crimild/core/Behaviors/Conditions/TestInputAxis.hpp>
#include <crimild/core/Behaviors/Conditions/TestSettingValue.hpp>
#include <crimild/core/Behaviors/Decorators/Decorator.hpp>
#include <crimild/core/Behaviors/Decorators/Inverter.hpp>
#include <crimild/core/Behaviors/Decorators/Repeat.hpp>
#include <crimild/core/Behaviors/Decorators/RepeatUntilFail.hpp>
#include <crimild/core/Behaviors/Decorators/RepeatUntilSuccess.hpp>
#include <crimild/core/Behaviors/Decorators/Succeeder.hpp>
#include <crimild/core/Behaviors/Decorators/Throttle.hpp>
#include <crimild/core/Behaviors/withBehavior.hpp>
#include <crimild/core/Boundings/AABBBoundingVolume.hpp>
#include <crimild/core/Boundings/BoundingVolume.hpp>
#include <crimild/core/Boundings/PlaneBoundingVolume.hpp>
#include <crimild/core/Boundings/SphereBoundingVolume.hpp>
#include <crimild/core/Common/Signal.hpp>
#include <crimild/core/Components/AnimatorComponent.hpp>
#include <crimild/core/Components/AudioListenerComponent.hpp>
#include <crimild/core/Components/AudioSourceComponent.hpp>
#include <crimild/core/Components/BillboardComponent.hpp>
#include <crimild/core/Components/FreeLookCameraComponent.hpp>
#include <crimild/core/Components/LambdaComponent.hpp>
#include <crimild/core/Components/MaterialComponent.hpp>
#include <crimild/core/Components/MotionStateComponent.hpp>
#include <crimild/core/Components/NodeComponent.hpp>
#include <crimild/core/Components/NodeComponentCatalog.hpp>
#include <crimild/core/Components/OrbitComponent.hpp>
#include <crimild/core/Components/PathComponent.hpp>
#include <crimild/core/Components/RenderStateComponent.hpp>
#include <crimild/core/Components/RotationComponent.hpp>
#include <crimild/core/Components/SingleValueComponent.hpp>
#include <crimild/core/Components/SkinnedMeshComponent.hpp>
#include <crimild/core/Components/TimerComponent.hpp>
#include <crimild/core/Components/UIResponder.hpp>
#include <crimild/core/Concurrency/Async.hpp>
#include <crimild/core/Concurrency/Job.hpp>
#include <crimild/core/Concurrency/JobScheduler.hpp>
#include <crimild/core/Concurrency/WorkStealingDeque.hpp>
#include <crimild/core/Debug/DebugRenderComponent.hpp>
#include <crimild/core/Debug/DebugRenderHelper.hpp>
#include <crimild/core/Debug/SceneDebugDump.hpp>
#include <crimild/core/Entity/Entity.hpp>
#include <crimild/core/Entity/Extension.hpp>
#include <crimild/core/Exceptions/Exception.hpp>
#include <crimild/core/Exceptions/FileNotFoundException.hpp>
#include <crimild/core/Exceptions/HasParentException.hpp>
#include <crimild/core/Exceptions/RuntimeException.hpp>
#include <crimild/core/Loaders/OBJLoader.hpp>
#include <crimild/core/Messaging/MessageQueue.hpp>
#include <crimild/core/Navigation/NavigationCell.hpp>
#include <crimild/core/Navigation/NavigationCellEdge.hpp>
#include <crimild/core/Navigation/NavigationController.hpp>
#include <crimild/core/Navigation/NavigationMesh.hpp>
#include <crimild/core/Navigation/NavigationMeshContainer.hpp>
#include <crimild/core/Navigation/NavigationMeshOBJ.hpp>
#include <crimild/core/ParticleSystem/Generators/AccelerationParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/BoxPositionParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/ColorParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/DefaultValueParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/EulerAnglesParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/GridPositionParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/NodePositionParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/OrbitPositionParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/RandomValueParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/SpherePositionParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/SphereVelocityParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/TimeParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/UniformScaleParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/Generators/VelocityParticleGenerator.hpp>
#include <crimild/core/ParticleSystem/ParticleData.hpp>
#include <crimild/core/ParticleSystem/ParticleSystemComponent.hpp>
#include <crimild/core/ParticleSystem/Renderers/AnimatedSpriteParticleRenderer.hpp>
#include <crimild/core/ParticleSystem/Renderers/InstancedParticleRenderer.hpp>
#include <crimild/core/ParticleSystem/Renderers/NodeParticleRenderer.hpp>
#include <crimild/core/ParticleSystem/Renderers/OrientedQuadParticleRenderer.hpp>
#include <crimild/core/ParticleSystem/Renderers/PointSpriteParticleRenderer.hpp>
#include <crimild/core/ParticleSystem/Updaters/AttractorParticleUpdater.hpp>
#include <crimild/core/ParticleSystem/Updaters/CameraSortParticleUpdater.hpp>
#include <crimild/core/ParticleSystem/Updaters/ColorParticleUpdater.hpp>
#include <crimild/core/ParticleSystem/Updaters/EulerParticleUpdater.hpp>
#include <crimild/core/ParticleSystem/Updaters/FloorParticleUpdater.hpp>
#include <crimild/core/ParticleSystem/Updaters/PositionVelocityParticleUpdater.hpp>
#include <crimild/core/ParticleSystem/Updaters/SetAttribValueParticleUpdater.hpp>
#include <crimild/core/ParticleSystem/Updaters/TimeParticleUpdater.hpp>
#include <crimild/core/ParticleSystem/Updaters/UniformScaleParticleUpdater.hpp>
#include <crimild/core/ParticleSystem/Updaters/ZSortParticleUpdater.hpp>
#include <crimild/core/Primitives/ArcPrimitive.hpp>
#include <crimild/core/Primitives/ArrowPrimitive.hpp>
#include <crimild/core/Primitives/BoxPrimitive.hpp>
#include <crimild/core/Primitives/ConePrimitive.hpp>
#include <crimild/core/Primitives/CylinderPrimitive.hpp>
#include <crimild/core/Primitives/KleinBottlePrimitive.hpp>
#include <crimild/core/Primitives/MobiusStripPrimitive.hpp>
#include <crimild/core/Primitives/NewellTeapotPrimitive.hpp>
#include <crimild/core/Primitives/ParametricPrimitive.hpp>
#include <crimild/core/Primitives/ParametricSpherePrimitive.hpp>
#include <crimild/core/Primitives/Primitive.hpp>
#include <crimild/core/Primitives/QuadPrimitive.hpp>
#include <crimild/core/Primitives/SpherePrimitive.hpp>
#include <crimild/core/Primitives/TorusPrimitive.hpp>
#include <crimild/core/Primitives/TrefoilKnotPrimitive.hpp>
#include <crimild/core/Rendering/Buffer.hpp>
#include <crimild/core/Rendering/Catalog.hpp>
#include <crimild/core/Rendering/ColorMaskState.hpp>
#include <crimild/core/Rendering/CommandBuffer.hpp>
#include <crimild/core/Rendering/ComputePass.hpp>
#include <crimild/core/Rendering/DescriptorSet.hpp>
#include <crimild/core/Rendering/Font.hpp>
#include <crimild/core/Rendering/Image.hpp>
#include <crimild/core/Rendering/ImageManager.hpp>
#include <crimild/core/Rendering/ImageTGA.hpp>
#include <crimild/core/Rendering/IndexBuffer.hpp>
#include <crimild/core/Rendering/Material.hpp>
#include <crimild/core/Rendering/Materials/PrincipledBSDFMaterial.hpp>
#include <crimild/core/Rendering/Materials/PrincipledVolumeMaterial.hpp>
#include <crimild/core/Rendering/Materials/SkyboxMaterial.hpp>
#include <crimild/core/Rendering/Materials/UnlitMaterial.hpp>
#include <crimild/core/Rendering/Materials/WorldGridMaterial.hpp>
#include <crimild/core/Rendering/Operations/OperationUtils.hpp>
#include <crimild/core/Rendering/Operations/Operations.hpp>
#include <crimild/core/Rendering/Operations/Operations_computeImage.hpp>
#include <crimild/core/Rendering/Operations/Operations_computeRT.hpp>
#include <crimild/core/Rendering/Operations/Operations_softRT.hpp>
#include <crimild/core/Rendering/Pipeline.hpp>
#include <crimild/core/Rendering/Programs/UnlitShaderProgram.hpp>
#include <crimild/core/Rendering/RenderPass.hpp>
#include <crimild/core/Rendering/RenderResource.hpp>
#include <crimild/core/Rendering/RenderState.hpp>
#include <crimild/core/Rendering/Renderer.hpp>
#include <crimild/core/Rendering/Sampler.hpp>
#include <crimild/core/Rendering/Shader.hpp>
#include <crimild/core/Rendering/ShaderLibrary.hpp>
#include <crimild/core/Rendering/ShaderLocation.hpp>
#include <crimild/core/Rendering/ShaderProgram.hpp>
#include <crimild/core/Rendering/ShaderProgramLibrary.hpp>
#include <crimild/core/Rendering/ShaderUniform.hpp>
#include <crimild/core/Rendering/ShaderUniformImpl.hpp>
#include <crimild/core/Rendering/ShadowMap.hpp>
#include <crimild/core/Rendering/SkinnedMesh.hpp>
#include <crimild/core/Rendering/Swapchain.hpp>
#include <crimild/core/Rendering/Texture.hpp>
#include <crimild/core/Rendering/UniformBuffer.hpp>
#include <crimild/core/Rendering/Uniforms/CallbackUniformBuffer.hpp>
#include <crimild/core/Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp>
#include <crimild/core/Rendering/Uniforms/LightingUniform.hpp>
#include <crimild/core/Rendering/Uniforms/ModelUniformBuffer.hpp>
#include <crimild/core/Rendering/Uniforms/ModelViewProjectionUniformBuffer.hpp>
#include <crimild/core/Rendering/VertexBuffer.hpp>
#include <crimild/core/SceneGraph/CSGNode.hpp>
#include <crimild/core/SceneGraph/Camera.hpp>
#include <crimild/core/SceneGraph/Geometry.hpp>
#include <crimild/core/SceneGraph/Group.hpp>
#include <crimild/core/SceneGraph/Light.hpp>
#include <crimild/core/SceneGraph/Node.hpp>
#include <crimild/core/SceneGraph/Node_withTransformation.hpp>
#include <crimild/core/SceneGraph/Skybox.hpp>
#include <crimild/core/SceneGraph/Switch.hpp>
#include <crimild/core/SceneGraph/Text.hpp>
#include <crimild/core/Simulation/Console/Console.hpp>
#include <crimild/core/Simulation/Event.hpp>
#include <crimild/core/Simulation/FileSystem.hpp>
#include <crimild/core/Simulation/Input.hpp>
#include <crimild/core/Simulation/Simulation.hpp>
#include <crimild/core/Simulation/Systems/AudioSystem.hpp>
#include <crimild/core/Simulation/Systems/DebugSystem.hpp>
#include <crimild/core/Simulation/Systems/RenderSystem.hpp>
#include <crimild/core/Simulation/Systems/StreamingSystem.hpp>
#include <crimild/core/Simulation/Systems/UISystem.hpp>
#include <crimild/core/Simulation/Systems/UpdateSystem.hpp>
#include <crimild/core/UI/UIBackground.hpp>
#include <crimild/core/UI/UIButton.hpp>
#include <crimild/core/UI/UICanvas.hpp>
#include <crimild/core/UI/UIFrame.hpp>
#include <crimild/core/UI/UIFrameConstraint.hpp>
#include <crimild/core/UI/UIFrameConstraintMaker.hpp>
#include <crimild/core/UI/UILabel.hpp>
#include <crimild/core/Visitors/Apply.hpp>
#include <crimild/core/Visitors/ApplyToGeometries.hpp>
#include <crimild/core/Visitors/ComputeRenderQueue.hpp>
#include <crimild/core/Visitors/FetchCameras.hpp>
#include <crimild/core/Visitors/FetchGeometries.hpp>
#include <crimild/core/Visitors/FetchLights.hpp>
#include <crimild/core/Visitors/NodeVisitor.hpp>
#include <crimild/core/Visitors/Picking.hpp>
#include <crimild/core/Visitors/SelectNodes.hpp>
#include <crimild/core/Visitors/ShallowCopy.hpp>
#include <crimild/core/Visitors/StartComponents.hpp>
#include <crimild/core/Visitors/UpdateComponents.hpp>
#include <crimild/core/Visitors/UpdateRenderState.hpp>
#include <crimild/core/Visitors/UpdateWorldState.hpp>
#include <crimild/core/experimental/nodes/3d/Camera3D.hpp>
#include <crimild/core/experimental/nodes/3d/Geometry3D.hpp>
#include <crimild/core/experimental/nodes/3d/Spatial3D.hpp>
#include <crimild/core/experimental/nodes/Node.hpp>
#include <crimild/core/experimental/nodes/visitors/NodeConstVisitor.hpp>
#include <crimild/core/experimental/nodes/visitors/NodeVisitor.hpp>
#include <crimild/core/init.hpp>

#endif
