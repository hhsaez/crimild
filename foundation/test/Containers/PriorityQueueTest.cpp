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

#include "Containers/PriorityQueue.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( PriorityQueueTest, basicConstruction )
{
    PriorityQueue< int > pq;

    EXPECT_EQ( 0, pq.size() );
    EXPECT_TRUE( pq.empty() );
}

TEST( PriorityQueueTest, initializationList )
{
    PriorityQueue< int > pq = { 1, 2, 3, 4, 5 };

    int vals[] = { 1, 2, 3, 4, 5 };
    crimild::Int32 count = 0;
    pq.each( [ vals, &count ]( const int &v, crimild::Size i ) {
        EXPECT_EQ( vals[ i ], v );
        count++;
    } );

    EXPECT_EQ( count, pq.size() );
}

TEST( PriorityQueueTest, enqueue )
{
    PriorityQueue< int > pq;

    EXPECT_EQ( 0, pq.size() );
    EXPECT_TRUE( pq.empty() );

    pq.enqueue( 5 );
    EXPECT_EQ( 1, pq.size() );
    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 5, pq.front() );

    pq.enqueue( 3 );
    EXPECT_EQ( 2, pq.size() );
    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 3, pq.front() );

    pq.enqueue( 8 );
    EXPECT_EQ( 3, pq.size() );
    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 3, pq.front() );

    pq.enqueue( 2 );
    EXPECT_EQ( 4, pq.size() );
    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 2, pq.front() );

    pq.enqueue( 6 );
    EXPECT_EQ( 5, pq.size() );
    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 2, pq.front() );

    pq.enqueue( 1 );
    EXPECT_EQ( 6, pq.size() );
    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 1, pq.front() );
}

TEST( PriorityQueueTest, dequeue )
{
    PriorityQueue< int > pq;
    pq.enqueue( 5 );
    pq.enqueue( 6 );
    pq.enqueue( 1 );
    pq.enqueue( 8 );
    pq.enqueue( 4 );

    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 1, pq.dequeue() );
    EXPECT_EQ( 4, pq.front() );

    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 4, pq.dequeue() );
    EXPECT_EQ( 5, pq.front() );

    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 5, pq.dequeue() );
    EXPECT_EQ( 6, pq.front() );

    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 6, pq.dequeue() );
    EXPECT_EQ( 8, pq.front() );

    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 8, pq.dequeue() );
    EXPECT_TRUE( pq.empty() );
}

TEST( PriorityQueueTest, dequeueLockable )
{
    PriorityQueue< int, crimild::policies::ObjectLevelLockable > pq;

    EXPECT_EQ( 0, pq.size() );
    EXPECT_TRUE( pq.empty() );

    pq.enqueue( 5 );

    EXPECT_EQ( 1, pq.size() );
    EXPECT_FALSE( pq.empty() );
    EXPECT_EQ( 5, pq.front() );

    auto k = pq.dequeue();

    EXPECT_EQ( 0, pq.size() );
    EXPECT_TRUE( pq.empty() );

    EXPECT_EQ( 5, k );
}

TEST( PriorityQueueTest, copyLockable )
{
    PriorityQueue< int, crimild::policies::ObjectLevelLockable > pq1;
    PriorityQueue< int, crimild::policies::ObjectLevelLockable > pq2;

    EXPECT_EQ( 0, pq1.size() );
    EXPECT_TRUE( pq1.empty() );

    EXPECT_EQ( 0, pq2.size() );
    EXPECT_TRUE( pq2.empty() );

    pq1.enqueue( 5 );

    EXPECT_EQ( 1, pq1.size() );
    EXPECT_FALSE( pq1.empty() );
    EXPECT_EQ( 5, pq1.front() );

    pq2 = pq1;

    EXPECT_EQ( 1, pq2.size() );
    EXPECT_FALSE( pq2.empty() );
    EXPECT_EQ( 5, pq2.front() );

    auto k = pq1.dequeue();

    EXPECT_EQ( 0, pq1.size() );
    EXPECT_TRUE( pq1.empty() );

    EXPECT_EQ( 5, k );

    EXPECT_EQ( 1, pq2.size() );
    EXPECT_FALSE( pq2.empty() );
    EXPECT_EQ( 5, pq2.front() );
}
