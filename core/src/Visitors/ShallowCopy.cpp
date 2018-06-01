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

#include "ShallowCopy.hpp"

#include "Components/MaterialComponent.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Components/SkinnedMeshComponent.hpp"

#include "Rendering/SkinnedMesh.hpp"

using namespace crimild;

ShallowCopy::ShallowCopy( void )
{

}

ShallowCopy::~ShallowCopy( void )
{

}

void ShallowCopy::traverse( Node *node )
{
	NodeVisitor::traverse( node );
    
    if ( _result != nullptr ) {
        // make sure the result is not transformed
        _result->local().makeIdentity();
    }
}

void ShallowCopy::visitNode( Node *node )
{
    auto copy = crimild::alloc< Node >();
    copyNode( node, crimild::get_ptr( copy ) );
}

void ShallowCopy::visitGroup( Group *group )
{
    auto copy = crimild::alloc< Group >();
    copyNode( group, crimild::get_ptr( copy ) );

    _parent = crimild::get_ptr( copy );

	NodeVisitor::visitGroup( group );

	_parent = copy->getParent< Group >();
}

void ShallowCopy::visitGeometry( Geometry *geometry )
{
    auto copy = crimild::alloc< Geometry >();
    copyNode( geometry, crimild::get_ptr( copy ) );

	geometry->forEachPrimitive( [&]( Primitive *primitive ) {
		copy->attachPrimitive( primitive );
	});

	auto rs = geometry->getComponent< RenderStateComponent >();
	if ( rs != nullptr ) {
		auto skin = rs->getSkinnedMesh();
		if ( skin != nullptr ) {
			copy->getComponent< RenderStateComponent >()->setSkinnedMesh( crimild::retain( skin ) );
		}
	}
}

void ShallowCopy::visitText( Text *input )
{
    auto copy = crimild::alloc< Text >();
    copyNode( input, crimild::get_ptr( copy ) );

    copy->setFont( input->getFont() );
    copy->setSize( input->getSize() );
}

void ShallowCopy::copyNode( Node *src, Node *dst )
{
	if ( _result == nullptr ) {
        _result = crimild::retain( dst ) ;
	}

	if ( _parent != nullptr ) {
		_parent->attachNode( dst );
	}

	dst->setName( src->getName() );
    dst->setLocal( src->getLocal() );

    src->forEachComponent( [ dst ]( NodeComponent *srcCmp ) {
        if ( srcCmp != nullptr ) {
            if ( auto cmp = srcCmp->clone() ) {
                dst->attachComponent( cmp );
            }
        }
    });
    
}

