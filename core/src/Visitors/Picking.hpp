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

#ifndef CRIMILD_VISITORS_PICKING_
#define CRIMILD_VISITORS_PICKING_

#include "NodeVisitor.hpp"

#include "Mathematics/Ray.hpp"

#include "SceneGraph/Node.hpp"
#include "SceneGraph/Group.hpp"

#include <functional>
#include <list>

namespace crimild {

	class Picking : public NodeVisitor {
	private:
		typedef std::function< bool( Node * ) > FilterType;

	public:
		class Results {
		public:
			Results( void ) { }
			~Results( void ) { }

			void reset( void )
			{
				_candidates.clear();
			}

			void sortCandidates( std::function< bool( Node *, Node * ) > callback )
			{
				_candidates.sort( callback );
			}

			void pushCandidate( Node *candidate )
			{
				_candidates.push_back( candidate );
			}

			void foreachCandidate( std::function< void( Node * ) > callback )
			{
                auto cs = _candidates;
				for ( auto c : cs ) {
					callback( c );
				}
			}

			bool hasResults( void )
			{
				return _candidates.size() > 0;
			}

			Node *getBestCandidate( void )
			{
				if ( !hasResults() ) {
					return nullptr;
				}

				return _candidates.front();
			}

		private:
			std::list< Node * > _candidates;
		};

	public:
		Picking( const Ray3f &tester, Results &results, FilterType filter = nullptr );
		virtual ~Picking( void );

		virtual void traverse( Node *node ) override;

		virtual void visitNode( Node *node ) override;
		virtual void visitGroup( Group *node ) override;

	private:
		Ray3f _tester;
		Results &_results;
		FilterType _filter;
	};

}

#endif

