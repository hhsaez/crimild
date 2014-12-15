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

using namespace crimild;

ShallowCopy::ShallowCopy( void )
{

}

ShallowCopy::~ShallowCopy( void )
{

}

void ShallowCopy::traverse( NodePtr const &node )
{
	NodeVisitor::traverse( node );
}

void ShallowCopy::visitNode( NodePtr const &node )
{
    auto copy = std::make_shared< Node >();
	copyNode( node, copy );
}

void ShallowCopy::visitGroup( GroupPtr const &group )
{
    auto copy = std::make_shared< Group >();
	copyNode( group, copy );

	_parent = copy;

	NodeVisitor::visitGroup( group );

	_parent = copy->getParent< Group >();
}

void ShallowCopy::visitGeometry( GeometryPtr const &geometry )
{
    auto copy = std::make_shared< Geometry >();
	copyNode( geometry, copy );

	geometry->foreachPrimitive( [&]( PrimitivePtr const &primitive ) {
		copy->attachPrimitive( primitive );
	});
}

void ShallowCopy::copyNode( NodePtr const &src, NodePtr const &dst )
{
	if ( _result == nullptr ) {
		_result = dst;
	}

	if ( _parent != nullptr ) {
		_parent->attachNode( dst );
	}

	dst->setName( src->getName() );
	dst->setLocal( src->getLocal() );

	auto srcMaterials = src->getComponent< MaterialComponent >();
	if ( srcMaterials != nullptr ) {
        auto dstMaterials = std::make_shared< MaterialComponent >();
		srcMaterials->foreachMaterial( [&]( MaterialPtr const &material ) {
			dstMaterials->attachMaterial( material );
		});
		dst->attachComponent( dstMaterials );
	}
}

