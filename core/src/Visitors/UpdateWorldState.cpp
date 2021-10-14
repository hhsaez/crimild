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

#include "UpdateWorldState.hpp"

#include "Mathematics/Transformation_operators.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;

UpdateWorldState::UpdateWorldState( void )
{
}

UpdateWorldState::~UpdateWorldState( void )
{
}

void UpdateWorldState::visitNode( Node *node )
{
    if ( node->worldIsCurrent() ) {
        return;
    }

    if ( node->hasParent() ) {
        node->setWorld( node->getParent()->getWorld() * node->getLocal() );
    } else {
        node->setWorld( node->getLocal() );
    }

    node->worldBound()->computeFrom( node->getLocalBound(), node->getWorld() );
}

void UpdateWorldState::visitGroup( Group *group )
{
    visitNode( group );
    NodeVisitor::visitGroup( group );

    if ( group->hasNodes() ) {
        bool firstChild = true;
        group->forEachNode( [ & ]( Node *node ) {
            if ( firstChild ) {
                firstChild = false;
                group->worldBound()->computeFrom( node->getWorldBound() );
            } else {
                group->worldBound()->expandToContain( node->getWorldBound() );
            }
        } );
    }
}

void UpdateWorldState::visitCSGNode( CSGNode *csg )
{
    visitNode( csg );
    NodeVisitor::visitCSGNode( csg );

    bool firstChild = true;
    if ( auto left = csg->getLeft() ) {
        if ( firstChild ) {
            firstChild = false;
            csg->worldBound()->computeFrom( left->getWorldBound() );
        } else {
            csg->worldBound()->expandToContain( left->getWorldBound() );
        }
    }

    if ( auto right = csg->getRight() ) {
        if ( firstChild ) {
            firstChild = false;
            csg->worldBound()->computeFrom( right->getWorldBound() );
        } else {
            csg->worldBound()->expandToContain( right->getWorldBound() );
        }
    }
}
