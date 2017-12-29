#include "Transform.hpp"

#include "SceneGraph/Node.hpp"
#include "Mathematics/Interpolation.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

Transform::Transform(
	const crimild::Transformation &targetTransformation,
	crimild::Real32 duration,
	crimild::Bool computeInTargetSpace,
	crimild::Bool computeFromTargetPosition,
	crimild::Bool applyTranslation,
	crimild::Bool applyRotation )
	: _targetTransformation( targetTransformation ),
	  _duration( duration ),
	  _computeInTargetSpace( computeInTargetSpace ),
	  _computeFromTargetPosition( computeFromTargetPosition ),
	  _applyTranslation( applyTranslation ),
	  _applyRotation( applyRotation )
{

}

Transform::~Transform( void )
{
	
}

void Transform::init( BehaviorContext *context )
{
	Behavior::init( context );

	auto agent = context->getAgent();

	_start = agent->getLocal();

	_end.setTranslate( _applyTranslation ? _targetTransformation.getTranslate() : _start.getTranslate() );
	_end.setRotate( _applyRotation ? _targetTransformation.getRotate() : _start.getRotate() );
	
	if ( context->getTargetCount() > 0 ) {
		auto firstTarget = context->getTargetAt( 0 );
		if ( firstTarget != nullptr ) {
			if ( _computeInTargetSpace ) {
				_end.computeFrom( firstTarget->getWorld(), _end );
			}
			else if ( _computeFromTargetPosition ) {
				_end.translate() += firstTarget->getWorld().getTranslate();
			}
		}
	}		
}

Behavior::State Transform::step( BehaviorContext *context )
{
	if ( _duration <= 0 ) {
		context->getAgent()->setLocal( _end );
		return Behavior::State::SUCCESS;
	}

	auto agent = context->getAgent();

	Interpolation::linear(
		_start.getTranslate(),
		_end.getTranslate(),
		_clock.getAccumTime(),
		agent->local().translate()
	);

	Interpolation::slerp(
		_start.getRotate(),
		_end.getRotate(),
		_clock.getAccumTime(),
		agent->local().rotate()
	);

	_clock += ( 1.0f / _duration ) * context->getClock().getDeltaTime();
	if ( _clock.getAccumTime() >= 1.0f ) {
		context->getAgent()->setLocal( _end );
		return Behavior::State::SUCCESS;
	}

	return Behavior::State::RUNNING;
}

