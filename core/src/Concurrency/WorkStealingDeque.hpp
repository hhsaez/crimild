#ifndef CRIMILD_CORE_CONCURRENCY_WORK_STEALING_QUEUE_
#define CRIMILD_CORE_CONCURRENCY_WORK_STEALING_QUEUE_

#include "Foundation/SharedObject.hpp"

#include <vector>
#include <list>
#include <mutex>

namespace crimild {

	/**
	   \brief A double-ended queue implemeting the work stealing pattern
	 */
	template< class T >
    class WorkStealingQueue : public SharedObject {
		using Mutex = std::mutex;
		using Lock = std::lock_guard< Mutex >;
		
	public:
		WorkStealingQueue( void )
		{

		}

		~WorkStealingQueue( void )
		{

		}
        
		size_t size( void )
		{
			Lock lock( _mutex );
			return _elems.size();
		}

		bool empty( void )
		{
			return size() == 0;
		}

		void clear( void )
		{
			Lock lock( _mutex );

			_elems.clear();
		}

		/**
		   \brief Adds an element to the private end of the queue (LIFO)

		 */
		void push( T const &elem )
		{
			Lock lock( _mutex );
			_elems.push_back( elem );
		}

		/**
		   \brief Retrieves an element from the private end of the queue (LIFO)

		   \warning You should check if the collection is empty before calling this method.
		 */
		T pop( void )
		{
			Lock lock( _mutex );

            if ( _elems.size() == 0 ) {
                return nullptr;
            }
            
			auto e = _elems.back();
			_elems.pop_back();
			return e;
		}

		/**
		   \brief Retrieves an element from the public end of the queue (FIFO)

		   \warning You should check if the collection is empty before calling this method
		 */
		T steal( void )
		{
			Lock lock( _mutex );
            
            if ( _elems.size() == 0 ) {
                return nullptr;
            }

			auto e = _elems.front();
			_elems.pop_front();
			return e;
		}

	private:
        std::list< T > _elems;
		std::mutex _mutex;
	};

}

#endif

