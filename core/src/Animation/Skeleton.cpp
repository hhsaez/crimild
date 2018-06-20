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
#include "SceneGraph/Node.hpp"
#include "Visitors/Apply.hpp"
#include "Debug/DebugRenderHelper.hpp"

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

Skeleton::Skeleton( void )
{

}

Skeleton::~Skeleton( void )
{

}

void Skeleton::animate( Animation *animation )
{
	getJoints().each( [ animation ]( const std::string &, SharedPointer< Joint > const &joint ) {
		auto name = joint->getName();
		auto node = joint->getNode();

		animation->getValue( name + "[p]", node->local().translate() );
		animation->getValue( name + "[r]", node->local().rotate() );
		animation->getValue( name + "[s]", node->local().scale() );

		Transformation pose;
		pose.computeFrom( node->getParent()->getWorld(), node->getLocal() );
		pose.computeFrom( pose, joint->getOffset() );
		joint->setPoseMatrix( pose.computeModelMatrix() );
	});
}

void Skeleton::renderDebugInfo( Renderer *renderer, Camera *camera )
{
	std::vector< Vector3f > lines;
	getNode()->perform( Apply( [ &lines ]( Node *node ) {
		if ( node->hasParent() ) {
			lines.push_back( node->getParent()->getWorld().getTranslate() );
			lines.push_back( node->getWorld().getTranslate() );
		}
	}));

	DebugRenderHelper::renderLines( renderer, camera, &lines[ 0 ], lines.size(), RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) );	
}

