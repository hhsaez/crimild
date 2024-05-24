#include "Transform.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Crimild_Mathematics.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

Transform::Transform( void )
{
}

Transform::Transform(
    const crimild::Transformation &targetTransformation,
    crimild::Real32 duration,
    crimild::Bool computeInTargetSpace,
    crimild::Bool computeFromTargetPosition,
    crimild::Bool applyTranslation,
    crimild::Bool applyRotation
)
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

#if 0

    auto agent = context->getAgent();

    _start = agent->getLocal();

    _end.setTranslate( _applyTranslation ? _targetTransformation.getTranslate() : _start.getTranslate() );
    _end.setRotate( _applyRotation ? _targetTransformation.getRotate() : _start.getRotate() );

    if ( context->getTargetCount() > 0 ) {
        auto firstTarget = context->getTargetAt( 0 );
        if ( firstTarget != nullptr ) {
            if ( _computeInTargetSpace ) {
                _end.computeFrom( firstTarget->getWorld(), _end );
            } else if ( _computeFromTargetPosition ) {
                _end.translate() += firstTarget->getWorld().getTranslate();
            }
        }
    }

#endif
}

Behavior::State Transform::step( BehaviorContext *context )
{
    assert( false && "TODO" );

#if 0
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
#endif

    return Behavior::State::RUNNING;
}

void Transform::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );

    encoder.encode( "transformation", _targetTransformation );
    encoder.encode( "duration", _duration );
    encoder.encode( "computeInTargetSpace", _computeInTargetSpace );
    encoder.encode( "computeFromTargetPosition", _computeFromTargetPosition );
    encoder.encode( "applyTranslation", _applyTranslation );
    encoder.encode( "applyRotation", _applyRotation );
}

void Transform::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );

    decoder.decode( "transformation", _targetTransformation );
    decoder.decode( "duration", _duration );
    decoder.decode( "computeInTargetSpace", _computeInTargetSpace );
    decoder.decode( "computeFromTargetPosition", _computeFromTargetPosition );
    decoder.decode( "applyTranslation", _applyTranslation );
    decoder.decode( "applyRotation", _applyRotation );
}
