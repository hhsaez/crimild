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

#include "Crimild.hpp"

void crimild::init( void )
{
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Variant );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::coding::EncodedData );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Node );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Group );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Geometry );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Camera );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Light );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Text );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Skybox );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Primitive );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Image );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ImageView );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ImageTGA );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Texture );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Sampler );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Buffer );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::BufferView );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::BufferAccessor );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::IndexBuffer );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::VertexBuffer );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ShaderProgram );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Shader );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Material );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::UnlitMaterial );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::materials::PrincipledBSDF );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::materials::WorldGrid );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::FreeLookCameraComponent );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::MaterialComponent );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::RenderStateComponent );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::OrbitComponent );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::RotationComponent );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::MotionState );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::BehaviorController );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::BehaviorContext );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::BehaviorContextValue );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::BehaviorTree );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::AnimateContextValue );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::AnimateParticleSystem );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::AnimateSettingValue );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::ClearTargets );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::CopyTransformFromTarget );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::ResetNavigation );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::EnableNode );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::ExecuteBehavior );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::ExecuteBehaviorOnTarget );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::FindTarget );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::LoadScene );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::LookAt );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::MotionApply );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::MotionFromInput );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::MotionAvoidOthers );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::MotionAvoidWalls );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::MotionComputePathToTarget );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::MotionReset );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::MotionSeek );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::Rotate );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::PrintMessage );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::SetContextValue );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::SetSettingValue );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::Success );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::Transform );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::Trigger );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::actions::Wait );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::composites::Parallel );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::composites::Selector );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::composites::Sequence );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::conditions::DistanceToTarget );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::conditions::HasTargets );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::conditions::IsAtTarget );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::conditions::TestContextValue );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::conditions::TestInputAxis );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::conditions::TestSettingValue );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::decorators::Inverter );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::decorators::Repeat );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::decorators::RepeatUntilFail );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::decorators::RepeatUntilSuccess );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::decorators::Succeeder );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::navigation::NavigationMeshContainer );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::navigation::NavigationController );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ParticleData );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ParticleSystemComponent );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::BoxPositionParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::GridPositionParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::SpherePositionParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::SphereVelocityParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::RandomVector3fParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::RandomReal32ParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::DefaultVector3fParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::DefaultReal32ParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ColorParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::TimeParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::NodePositionParticleGenerator );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::EulerParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::PositionVelocityParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::TimeParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::FloorParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CameraSortParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ZSortParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::AttractorParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::SetVector3fValueParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::UniformScaleParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ColorParticleUpdater );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::PointSpriteParticleRenderer );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::OrientedQuadParticleRenderer );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::NodeParticleRenderer );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::AnimatedSpriteParticleRenderer );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::animation::Animation );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::animation::Clip );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::animation::Joint );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::animation::QuaternionChannel );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::animation::Real32Channel );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::animation::Skeleton );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::animation::Vector3fChannel );

    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ui::UIBackground );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ui::UIButton );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ui::UICanvas );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ui::UIFrame );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ui::UIFrameConstraint );
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ui::UILabel );
}
