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

#ifndef CRIMILD_CORE_VISITORS_BIN_TREE_SCENE_
#define CRIMILD_CORE_VISITORS_BIN_TREE_SCENE_

#include "Visitors/NodeVisitor.hpp"

namespace crimild {

    /**
     * \brief Clones and converts a scene into a binary tree
     * 
     * Most of the nodes are cloned as is, but groups are split based on different strategies.
     */
    class BinTreeScene : public NodeVisitor {
    public:
        using Result = SharedPointer< Node >;

        /**
         * \brief Strategy to split groups of nodes
         * 
         * For large groups of nodes, this enum defines how to decide which nodes
         * are sent to the left or right branches of the resulting binary tree
         */
        enum SplitStrategy {
            /**
             * \brief No strategy. 
             */
            NONE,

            /**
             * \brief Random axis separation
             * 
             * Pick an axis at random, then sort and split nodes based on their bounding volumes
             * according to the picked axis.
             */
            RANDOM_AXIS,
        };

    public:
        explicit BinTreeScene( SplitStrategy strategy = SplitStrategy::RANDOM_AXIS ) noexcept
            : m_splitStrategy( strategy ) { }
        ~BinTreeScene( void ) = default;

        [[nodiscard]] inline Result getResult( void ) noexcept { return m_result; }

        inline void setSplitStrategy( SplitStrategy strategy ) noexcept { m_splitStrategy = strategy; }
        inline SplitStrategy getSplitStrategy( void ) const noexcept { return m_splitStrategy; }

        void traverse( Node *node ) noexcept override;

        void visitGroup( Group *other ) noexcept override;
        void visitGeometry( Geometry *other ) noexcept override;
        void visitCSGNode( CSGNode *other ) noexcept override;

    private:
        void cloneAndAdd( SharedPointer< Node > const &newNode, Node *oldNode ) noexcept;

    private:
        SplitStrategy m_splitStrategy = SplitStrategy::NONE;
        SharedPointer< Node > m_result;
        Group *m_parentGroup = nullptr;
        CSGNode *m_parentCSG = nullptr;
    };

}

#endif