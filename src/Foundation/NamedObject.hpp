#ifndef CRIMILD_FOUNDATION_NAMED_OBJECT_
#define CRIMILD_FOUNDATION_NAMED_OBJECT_

#include <string>

namespace Crimild {
	
	class NamedObject {
	public:
		explicit NamedObject( std::string name );

		virtual ~NamedObject( void );

		void setName( std::string name ) { _name = name; }
		std::string getName( void ) const { return _name; }

	private:
		std::string _name;
	};

	typedef std::shared_ptr< NamedObject > NamedObjectPtr;
	
}

#endif

