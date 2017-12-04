#ifndef CRIMILD_CORE_COMPONENTS_BEHAVIOR_CONTROLLER_
#define CRIMILD_CORE_COMPONENTS_BEHAVIOR_CONTROLLER_

#include "NodeComponent.hpp"

#include "Behaviors/Behavior.hpp"

namespace crimild {

	class BehaviorController :
		public crimild::NodeComponent,
		public crimild::Messenger {
		CRIMILD_IMPLEMENT_RTTI( crimild::BehaviorController )

	public:
		static constexpr const char *DEFAULT_BEHAVIOR_NAME = "__default__";
		static constexpr const char *SCENE_STARTED_BEHAVIOR_NAME = "__scene_started__";

		static constexpr const char *SETTINGS_EXECUTE_BEHAVIORS = "execute_behaviors";

	public:
		explicit BehaviorController( void );
		virtual ~BehaviorController( void );

		virtual void onAttach( void ) override;
		virtual void onDetach( void ) override;

		virtual void start( void ) override;
		virtual void update( const crimild::Clock & ) override;

	public:
		void attachBehavior( std::string eventName, behaviors::BehaviorPtr const &behavior );
		
		crimild::behaviors::BehaviorContext *getContext( void ) { return &_context; }

	public:
		/**
		   \brief Executes the behavior matching the given name

		   \remarks If no behavior is found, the current behavior is not modified
		 */
		bool executeBehavior( std::string name );

	private:
		crimild::behaviors::Behavior *getCurrentBehavior( void ) { return _currentBehavior; }
		void setCurrentBehavior( crimild::behaviors::Behavior *behavior ) { _currentBehavior = behavior; }

	private:
		std::map< std::string, crimild::behaviors::BehaviorPtr > _behaviors;
		crimild::behaviors::Behavior *_currentBehavior = nullptr;
		crimild::behaviors::BehaviorContext _context;
	};

}

#endif

