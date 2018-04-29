#include "EnableNode.hpp"

#include "SceneGraph/Group.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

EnableNode::EnableNode( crimild::Bool enabled, std::string node )
	: _enabled( enabled ),
	  _node( node )
{

}

EnableNode::~EnableNode( void )
{
	
}

Behavior::State EnableNode::step( BehaviorContext *context )
{
	auto agent = static_cast< Group * >( context->getAgent() );
	if ( agent->getName() == _node ) {
		agent->setEnabled( _enabled );
	}
	else {	
		auto node = agent->getNode( _node );
		if ( node != nullptr ) {
			node->setEnabled( _enabled );
		}
	}
	
	return Behavior::State::SUCCESS;
}

void EnableNode::encode( coding::Encoder &encoder )
{
	Behavior::encode( encoder );

	encoder.encode( "enabled", _enabled );
	encoder.encode( "node", _node );
}

void EnableNode::decode( coding::Decoder &decoder )
{
	Behavior::decode( decoder );

	decoder.decode( "enabled", _enabled );
	decoder.decode( "node", _node );
}
