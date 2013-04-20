#include "GroupNode.hpp"
#include "Exceptions/HasParentException.hpp"

#include <cassert>

using namespace Crimild;

GroupNode::GroupNode( std::string name )
	: Node( name )
{

}

GroupNode::~GroupNode( void )
{
	detachAllNodes();
}

void GroupNode::attachNode( NodePtr node )
{
	if ( node->getParent() == this ) {
		// the node is already attach to this group
		return;
	}

	if ( node->getParent() != nullptr ) {
		throw HasParentException( node->getName(), this->getName(), node->getParent()->getName() );
	}

	node->setParent( this );
	_nodes.push_back( node );
}

void GroupNode::detachNode( NodePtr node )
{
	if ( node->getParent() == this ) {
		_nodes.remove( node );
		node->setParent( nullptr );
	}
}

void GroupNode::detachAllNodes( void )
{
	for ( NodePtr &node : _nodes ) {
		node->setParent( nullptr );
	}
	_nodes.clear();
}

void GroupNode::foreachNode( std::function< void( NodePtr & ) > callback )
{
	std::for_each( std::begin( _nodes ), std::end( _nodes ), callback );
}

void GroupNode::accept( NodeVisitor &visitor )
{
	visitor.visitGroupNode( this );
}

