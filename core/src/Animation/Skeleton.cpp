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

#include "Skeleton.hpp"

#include "Animation.hpp"
#include "Clip.hpp"
#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Debug/DebugRenderHelper.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/Apply.hpp"

using namespace crimild;
using namespace crimild::animation;

Joint::Joint( void )
{
}

Joint::Joint( std::string name, crimild::UInt32 id )
    : NamedObject( name ),
      _id( id )
{
}

Joint::~Joint( void )
{
}

void Joint::encode( coding::Encoder &encoder )
{
    NodeComponent::encode( encoder );

    encoder.encode( "name", getName() );
    encoder.encode( "id", _id );
    encoder.encode( "offset", _offset );
}

void Joint::decode( coding::Decoder &decoder )
{
    NodeComponent::decode( decoder );

    std::string name;
    decoder.decode( "name", name );
    setName( name );

    decoder.decode( "id", _id );
    decoder.decode( "offset", _offset );
}

SharedPointer< NodeComponent > Joint::clone( void )
{
    auto other = crimild::alloc< Joint >( getName(), getId() );
    other->setOffset( getOffset() );
    other->setPoseMatrix( getPoseMatrix() );
    return other;
}

Skeleton::Skeleton( void )
{
}

Skeleton::~Skeleton( void )
{
}

void Skeleton::start( void )
{
    NodeComponent::start();

    getNode()->perform( Apply( [ this ]( Node *node ) {
        if ( auto joint = node->getComponent< Joint >() ) {
            getJoints().insert( joint->getName(), crimild::retain( joint ) );
        }
    } ) );
}

void Skeleton::animate( Animation *animation )
{
    getJoints().each( [ animation ]( const std::string &, SharedPointer< Joint > const &joint ) {
        auto name = joint->getName();
        auto node = joint->getNode();

        // TODO
        //animation->getValue( name + "[p]", node->local().translate() );
        //animation->getValue( name + "[r]", node->local().rotate() );
        //animation->getValue( name + "[s]", node->local().scale() );

        //Transformation pose;
        //pose.computeFrom( node->getParent()->getWorld(), node->getLocal() );
        //pose.computeFrom( pose, joint->getOffset() );
        //joint->setPoseMatrix( pose.computeModelMatrix() );
    } );
}

void Skeleton::encode( coding::Encoder &encoder )
{
    NodeComponent::encode( encoder );

    auto clips = getClips().values();
    encoder.encode( "clips", clips );
    encoder.encode( "globalInverseTransform", _globalInverseTransform );
}

void Skeleton::decode( coding::Decoder &decoder )
{
    NodeComponent::decode( decoder );

    Array< SharedPointer< Clip > > clips;
    decoder.decode( "clips", clips );
    clips.each( [ this ]( SharedPointer< Clip > const &clip ) {
        getClips()[ clip->getName() ] = clip;
    } );

    decoder.decode( "globalInverseTransform", _globalInverseTransform );
}

SharedPointer< NodeComponent > Skeleton::clone( void )
{
    auto other = crimild::alloc< Skeleton >();
    other->setClips( getClips() );
    other->setGlobalInverseTransform( getGlobalInverseTransform() );

    return other;
}

void Skeleton::renderDebugInfo( Renderer *renderer, Camera *camera )
{
    std::vector< Vector3f > lines;
    auto parent = getNode();
    parent->perform( Apply( [ &lines, parent ]( Node *node ) {
        if ( node != parent && node->hasParent() ) {
            //lines.push_back( node->getParent()->getWorld().getTranslate() );
            //lines.push_back( node->getWorld().getTranslate() );
        }
    } ) );

    DebugRenderHelper::renderLines( renderer, camera, &lines[ 0 ], lines.size(), ColorRGBA( 1.0f, 0.0f, 0.0f, 1.0f ) );
}
