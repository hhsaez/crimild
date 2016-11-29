#ifndef CRIMILD_CORE_CONCURRENCY_WORK_STEALING_QUEUE_
#define CRIMILD_CORE_CONCURRENCY_WORK_STEALING_QUEUE_

#include <vector>
#include <mutex>
#include <list>

namespace crimild {

	/**
	   \brief A double-ended queue implemeting the work stealing pattern
	 */
	template< class T >
	class WorkStealingQueue {
	public:
		WorkStealingQueue( void )
		{

		}

		~WorkStealingQueue( void )
		{

		}

		size_t size( void )
		{
			std::unique_lock< std::mutex >( _mutex );

			return _elems.size();
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

			_elems.push_back( elem );
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

            auto elem = std::move( _elems.back() );
			_elems.pop_back();
			return elem;
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

            auto elem = std::move( _elems.front() );
			_elems.pop_front();
			return elem;
		}

	private:
		std::list< T > _elems;
		std::mutex _mutex;
	};

}

#endif

