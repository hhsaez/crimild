#ifndef CRIMILD_CORE_CONCURRENCY_WORK_STEALING_QUEUE_
#define CRIMILD_CORE_CONCURRENCY_WORK_STEALING_QUEUE_

#include <vector>
#include <mutex>

namespace crimild {

	/**
	   \brief A double-ended queue implemeting the work stealing pattern
	 */
	template< class T >
	class WorkStealingQueue {
	public:
		WorkStealingQueue( size_t initialCapacity )
			: _elems( initialCapacity )
		{

		}

		~WorkStealingQueue( void )
		{

		}

		size_t capacity( void )
		{
			std::unique_lock< std::mutex >( _mutex );
			
			return _elems.size();
		}

		size_t size( void )
		{
			std::unique_lock< std::mutex >( _mutex );
			
			if ( _bottom > _top ) {
				return _bottom - _top;
			}

			return 0;
		}

		bool empty( void )
		{
			return size() == 0;
		}

		/**
		   \brief Adds an element to the private end of the queue (LIFO)
		 */
		void push( T const &elem )
		{
			std::unique_lock< std::mutex >( _mutex );

			assert( size() < capacity() && "WorkStelalingQueue: Not enough storage" );
			
			_elems[ _bottom % capacity() ] = elem;
			_bottom++;
		}

		/**
		   \brief Retrieves an element from the private end of the queue (LIFO)

		   \warning You should check if the collection is empty before calling this method.
		 */
		T pop( void )
		{
			std::unique_lock< std::mutex >( _mutex );

			if ( empty() ) {
				return T();
			}

			return _elems[ --_bottom % capacity() ];
		}

		/**
		   \brief Retrieves an element from the public end of the queue (FIFO)

		   \warning You should check if the collection is empty before calling this method
		 */
		T steal( void )
		{
			std::unique_lock< std::mutex >( _mutex );

			if ( empty() ) {
				return T();
			}
			
			return _elems[ _top++ % capacity() ];
		}

	private:
		size_t _bottom = 0; //< Next available slot in the array for pushing elements
		size_t _top = 0; //< Next element that can be stolen
		std::vector< T > _elems;

		std::mutex _mutex;
	};

}

#endif

