#ifndef CRIMILD_CORE_COMPONENTS_ANIMATOR_
#define CRIMILD_CORE_COMPONENTS_ANIMATOR_

#include "Crimild_Mathematics.hpp"
#include "NodeComponent.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/Clock.hpp"

#include <functional>

namespace crimild {

    template< typename T >
    class AnimatorComponent : public NodeComponent {
        CRIMILD_IMPLEMENT_RTTI( crimild::AnimatorComponent )
    private:
        using Interpolator = std::function< void( const T &, const T &, float, T & ) >;
        using OnCompleteCallback = std::function< void( Node * ) >;

    public:
        AnimatorComponent( const T &start, const T &end, T &result, float duration )
            : _start( start ),
              _end( end ),
              _result( result ),
              _timeMult( 1.0f / duration ),
              _interpolator( Interpolation::linear< T, float > )
        {
        }

        virtual ~AnimatorComponent( void )
        {
        }

        Interpolator getInterpolator( void ) { return _interpolator; }
        void setInterpolator( Interpolator interpolator ) { _interpolator = interpolator; }

        void setOnComplete( OnCompleteCallback callback ) { _onComplete = callback; }

        virtual void onAttach( void ) override
        {
            _time = 0.0f;
            _result = _start;
        }

        virtual void onDetach( void ) override
        {
        }

        virtual void update( const Clock &clock ) override
        {
            if ( _time >= 1.0f ) {
                _interpolator( _start, _end, 1.0f, _result );

                auto callback = _onComplete;
                auto node = getNode();

                getNode()->detachComponent( this );

                if ( callback != nullptr ) {
                    callback( node );
                }

                return;
            }

            _interpolator( _start, _end, _time, _result );
            _time += _timeMult * clock.getDeltaTime();
        }

    private:
        T _start;
        T _end;
        T &_result;
        float _timeMult;
        float _time;
        Interpolator _interpolator;
        OnCompleteCallback _onComplete;
    };

}

#endif
