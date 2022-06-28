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

#include "Visitors/BinTreeScene.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Random.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/NullNode.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

static Bool boundCompare( SharedPointer< Node > &a, SharedPointer< Node > &b, Index axis )
{
    return a->getWorldBound()->getMin()[ axis ] < b->getWorldBound()->getMin()[ axis ];
}

static Bool boundCompareX( SharedPointer< Node > &a, SharedPointer< Node > &b )
{
    return boundCompare( a, b, 0 );
}

static Bool boundCompareY( SharedPointer< Node > &a, SharedPointer< Node > &b )
{
    return boundCompare( a, b, 1 );
}

static Bool boundCompareZ( SharedPointer< Node > &a, SharedPointer< Node > &b )
{
    return boundCompare( a, b, 2 );
}

using AxisSplitter = std::function< int( void ) >;

void splitByAxis( Array< SharedPointer< Node > > &nodes, Size start, Size end, SharedPointer< Group > const &parent, AxisSplitter const &axisSplitter ) noexcept
{
    int axis = axisSplitter();
    auto comparator = axis == 0 ? boundCompareX : ( axis == 1 ? boundCompareY : boundCompareZ );

    const auto span = end - start;
    if ( span == 3 ) {
        // Prevent empty nodes or nodes with only one child
        parent->attachNode(
            [ & ] {
                auto group = crimild::alloc< Group >();
                if ( comparator( nodes[ start ], nodes[ start + 1 ] ) ) {
                    group->attachNode( nodes[ start + 0 ] );
                    group->attachNode( nodes[ start + 1 ] );
                } else {
                    group->attachNode( nodes[ start + 1 ] );
                    group->attachNode( nodes[ start + 0 ] );
                }
                return group;
            }()
        );
        parent->attachNode( nodes[ start + 2 ] );
        return;
    } else if ( span == 2 ) {
        // if ( span == 2 ) {
        if ( comparator( nodes[ start ], nodes[ start + 1 ] ) ) {
            parent->attachNode( nodes[ start + 0 ] );
            parent->attachNode( nodes[ start + 1 ] );
        } else {
            parent->attachNode( nodes[ start + 1 ] );
            parent->attachNode( nodes[ start + 0 ] );
        }
        return;
    } else if ( span <= 1 ) {
        // Should never happen
        assert( false && "Invalid node tree" );
    }

    auto mid = start + span / 2;

    nodes.sort( comparator );

    Array< SharedPointer< Node > > left;
    for ( auto i = start; i < mid; ++i ) {
        left.add( nodes[ i ] );
    }

    Array< SharedPointer< Node > > right;
    for ( auto i = mid; i < end; ++i ) {
        right.add( nodes[ i ] );
    }

    // TODO(hernan): Is this necessary?
    left.sort( comparator );
    right.sort( comparator );

    parent->attachNode(
        [ & ] {
            auto group = crimild::alloc< Group >();
            splitByAxis( left, 0, left.size(), group, axisSplitter );
            return group;
        }()
    );

    parent->attachNode(
        [ & ] {
            auto group = crimild::alloc< Group >();
            splitByAxis( right, 0, right.size(), group, axisSplitter );
            return group;
        }()
    );
}

void split( Array< SharedPointer< Node > > &nodes, Size start, Size end, SharedPointer< Group > const &parent ) noexcept
{
    const auto span = end - start;
    if ( span == 3 ) {
        // Prevent empty nodes or nodes with only one child
        parent->attachNode(
            [ & ] {
                auto group = crimild::alloc< Group >();
                group->attachNode( nodes[ start + 0 ] );
                group->attachNode( nodes[ start + 1 ] );
                return group;
            }()
        );
        parent->attachNode( nodes[ start + 2 ] );
        return;
    } else if ( span == 2 ) {
        parent->attachNode( nodes[ start + 0 ] );
        parent->attachNode( nodes[ start + 1 ] );
        return;
    } else if ( span <= 1 ) {
        // Should never happen
        assert( false && "Invalid node tree" );
    }

    auto mid = start + span / 2;

    parent->attachNode(
        [ & ] {
            auto group = crimild::alloc< Group >();
            split( nodes, start, mid, group );
            return group;
        }()
    );
    parent->attachNode(
        [ & ] {
            auto group = crimild::alloc< Group >();
            split( nodes, mid, end, group );
            return group;
        }()
    );
}

void BinTreeScene::traverse( Node *node ) noexcept
{
    node->perform( UpdateWorldState() );
    NodeVisitor::traverse( node );

    if ( m_result == nullptr ) {
        m_result = crimild::alloc< NullNode >();
    }
}

void BinTreeScene::visitGroup( Group *other ) noexcept
{
    auto tempParentGroup = m_parentGroup;
    auto tempParentCSG = m_parentCSG;

    auto group = crimild::alloc< Group >();
    cloneAndAdd( group, other );

    m_parentGroup = get_ptr( group );
    m_parentCSG = nullptr;

    NodeVisitor::visitGroup( other );

    m_parentGroup = tempParentGroup;
    m_parentCSG = tempParentCSG;

    if ( group->getNodeCount() == 1 ) {
        // This node ended up having only one child, so we can discard it and use its child directly
        auto child = retain( group->getNodeAt( 0 ) );
        group->detachNode( child );

        // Sanity check.
        // Detach from parent (either another group or CSG node) and make sure
        // all references are correct.
        if ( m_parentGroup != nullptr ) {
            m_parentGroup->detachNode( group );
            m_parentGroup->attachNode( child );
        } else if ( m_parentCSG != nullptr ) {
            if ( m_parentCSG->getLeft() == get_ptr( group ) ) {
                m_parentCSG->setLeft( child );
            } else if ( m_parentCSG->getRight() == get_ptr( group ) ) {
                m_parentCSG->setRight( child );
            }
        } else if ( m_result == group ) {
            m_result = child;
        }

        return;
    }

    if ( group->getNodeCount() == 0 ) {
        // This group ended up having no children, so we can ignore it

        // Sanity check.
        // Detach from parent (either another group or CSG node) and make sure
        // all references are correct.
        if ( m_parentGroup != nullptr ) {
            m_parentGroup->detachNode( group );
        } else if ( m_parentCSG != nullptr ) {
            if ( m_parentCSG->getLeft() == get_ptr( group ) ) {
                auto right = retain( m_parentCSG->getRight() );
                m_parentCSG->setRight( nullptr );
                m_parentCSG->setLeft( right );
            } else if ( m_parentCSG->getRight() == get_ptr( group ) ) {
                m_parentCSG->setRight( nullptr );
            }
        } else if ( m_result == group ) {
            m_result = nullptr;
        }

        return;
    }

    Array< SharedPointer< Node > > children;
    group->forEachNode(
        [ & ]( auto node ) {
            children.add( retain( node ) );
        }
    );
    group->detachAllNodes();

    auto splitStrategy = getSplitStrategy();
    if ( splitStrategy == SplitStrategy::NONE ) {
        split( children, 0, children.size(), group );
    } else {
        splitByAxis(
            children,
            0,
            children.size(),
            group,
            [ splitStrategy ]( void ) {
                switch ( splitStrategy ) {
                    case SplitStrategy::X_AXIS:
                        return 0;
                    case SplitStrategy::Y_AXIS:
                        return 1;
                    case SplitStrategy::Z_AXIS:
                        return 2;
                    case SplitStrategy::RANDOM_AXIS:
                    default:
                        return Random::generate< Int >( 0, 3 );
                }
            }
        );
    }
}

void BinTreeScene::visitGeometry( Geometry *other ) noexcept
{
    auto node = crimild::alloc< Geometry >();
    other->forEachPrimitive(
        [ & ]( auto primitive ) {
            node->attachPrimitive( primitive );
        }
    );

    if ( auto ms = other->getComponent< MaterialComponent >() ) {
        auto materials = node->attachComponent< MaterialComponent >();
        ms->forEachMaterial(
            [ & ]( auto material ) {
                materials->attachMaterial( material );
            }
        );
    }

    cloneAndAdd( node, other );
}

void BinTreeScene::visitCSGNode( CSGNode *other ) noexcept
{
    if ( other->getLeft() == nullptr && other->getRight() == nullptr ) {
        return;
    }

    auto tempParentGroup = m_parentGroup;
    auto tempParentCSG = m_parentCSG;

    auto csg = crimild::alloc< CSGNode >( other->getOperator() );
    cloneAndAdd( csg, other );

    m_parentGroup = nullptr;
    m_parentCSG = get_ptr( csg );

    if ( auto left = other->getLeft() ) {
        left->accept( *this );
    }

    if ( auto right = other->getRight() ) {
        right->accept( *this );
    }

    m_parentGroup = tempParentGroup;
    m_parentCSG = tempParentCSG;
}

void BinTreeScene::cloneAndAdd( SharedPointer< Node > const &newNode, Node *oldNode ) noexcept
{
    newNode->setName( oldNode->getName() );
    newNode->setLocal( oldNode->getLocal() );

    if ( m_result == nullptr ) {
        m_result = newNode;
    } else if ( m_parentGroup != nullptr ) {
        m_parentGroup->attachNode( newNode );
    } else if ( m_parentCSG != nullptr ) {
        if ( m_parentCSG->getLeft() == nullptr ) {
            m_parentCSG->setLeft( newNode );
        } else if ( m_parentCSG->getRight() == nullptr ) {
            m_parentCSG->setRight( newNode );
        } else {
            // TODO: handle error?
        }
    }
}
