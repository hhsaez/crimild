#include "Node.hpp"

using namespace Crimild;

Node::Node( std::string name )
	: NamedObject( name ),
	  _parent( nullptr )
{
}

Node::~Node( void )
{
	detachAllComponents();
}

void Node::perform( NodeVisitor &visitor )
{
	visitor.traverse( this );
}

void Node::perform( const NodeVisitor &visitor )
{
	const_cast< NodeVisitor & >( visitor ).traverse( this );
}

void Node::accept( NodeVisitor &visitor )
{
	visitor.visitNode( this );
}

void Node::attachComponent( NodeComponentPtr component )
{
	if ( component->getNode() == this ) {
		// the component is already attached to this node
		return;
	}

	detachComponentWithName( component->getName() );
	component->setNode( this );
	_components[ component->getName() ] = component;
	component->onAttach();
}

void Node::detachComponent( NodeComponentPtr component )
{
	if ( component->getNode() != this ) {
		// the component is not attached to this node
		return;
	}

	detachComponentWithName( component->getName() );
}

void Node::detachComponentWithName( std::string name )
{
	if ( _components.find( name ) != _components.end() ) {
		_components[ name ]->onDetach();
		_components[ name ]->setNode( nullptr );
		_components.erase( name );
	}
}

NodeComponent *Node::getComponentWithName( std::string name )
{
	return _components[ name ].get();
}

void Node::detachAllComponents( void )
{
	for ( auto cmp : _components ) {
		if ( cmp.second != nullptr ) {
			cmp.second->onDetach();
			cmp.second->setNode( nullptr );
		}
	}

	_components.clear();
}

void Node::updateComponents( void )
{
	for ( auto cmp : _components ) {
		if ( cmp.second != nullptr ) {
			cmp.second->update();
		}
	}
}

