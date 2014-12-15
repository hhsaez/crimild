/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_MESSAGING_MESSAGE_QUEUE_
#define CRIMILD_MESSAGING_MESSAGE_QUEUE_

#include "Foundation/SharedObject.hpp"
#include "Foundation/Pointer.hpp"

#include <algorithm>
#include <list>
#include <cassert>

namespace crimild {

	class Message : public SharedObject {
	public:
		virtual ~Message( void ) { }

		virtual bool useDeferredQueue( void ) const { return false; }
	};
    
    using MessagePtr = std::shared_ptr< Message >;

	class DeferredMessage : public Message {
	public:
		virtual ~DeferredMessage( void ) { }
		
		virtual bool useDeferredQueue( void ) const { return true; }
	};
    
    using DeferredMessagePtr = std::shared_ptr< DeferredMessage >;

	template< class MessageImpl >
	class MessageHandler {
	public:
		/**
			\brief Destructor

			The destructor automatically unregister the handler
			from its corresponding message dispatcher
		 */
		virtual ~MessageHandler( void );

        virtual void handleMessage( std::shared_ptr< MessageImpl > const &message ) = 0;

	protected:
		/**
			\brief Default constructor

			The constructor automatically register the handler
			from its corresponding message dispatcher
		 */
		MessageHandler( void );
	};

	class MessageDispatcher {
	public:
		virtual void dispatchMessages( void ) = 0;
		virtual void discardAllMessages( void ) = 0;
		virtual void reset( void ) = 0;
	};

	template< class MessageImpl >
	class MessageDispatcherImpl : public MessageDispatcher {
        typedef std::shared_ptr< MessageImpl > MessageImplPtr;
		typedef std::list< MessageImplPtr > MessageImplList;
		typedef MessageHandler< MessageImpl > MessageHandlerImpl;
		typedef std::list< MessageHandlerImpl * > MessageHandlerList;

	public:
		static MessageDispatcherImpl &getInstance( void )
		{
			static MessageDispatcherImpl instance;
			return instance;
		}

	private:
		MessageDispatcherImpl( void );
		virtual ~MessageDispatcherImpl( void );

	public:
		virtual void dispatchMessages( void ) override
		{
			if ( _handlers.size() > 0 && _messages.size() ) {
				for ( auto message : _messages ) {
					for ( auto handler : _handlers ) {
						handler->handleMessage( message );
					}
				}
			}

			discardAllMessages();
		}

		virtual void discardAllMessages( void )
		{
			_messages.clear();
		}

		virtual void reset( void )
		{
			discardAllMessages();

			_handlers.clear();
		}

		/**
			\brief Dispatchs a message

			If the message requires to use the deferred queue, it will be delayed.
			Otherwise, it will be dispatched immediatelly.
		*/
		void pushMessage( MessageImplPtr const &message )
		{
			if ( message->useDeferredQueue() ) {
				_messages.push_back( message );
			}
			else {
				for ( auto handler : _handlers ) {
					handler->handleMessage( message );
				}
			}
		}

		/**
			\brief Dispatchs a message

			This function can be used only for forward messages (non-deferred). Since it
			does not require to cache the message instance in the queue, we can do a nice
			trick and use references to objects instead of new instances, avoiding the
			overhead related with object allocation in memory

			\warning This method can be use only with non-deferred message. 
		 */
        /*
		void pushMessage( MessageImpl &message )
		{
			assert( message.useDeferredQueue() == false && "Deferred messages cannot be statically allocated (did you forget a 'new' somewhere?)");
			for ( auto handler : _handlers ) {
				handler->handleMessage( &message );
			}
		}
         */

		void registerHandler( MessageHandlerImpl *handler )
		{
			_handlers.push_back( handler );
		}

		void unregisterHandler( MessageHandlerImpl *handler )
		{
			_handlers.remove( handler );
		}

	private:
		MessageImplList _messages;
		MessageHandlerList _handlers;
	};

	/**
		\brief Entry point for the messaging sub-system

		This class is the heart of the message sub-system. It is used
		to manage all message execution.

		\todo This class needs proper documentation
	 */
	class MessageQueue {
    private:
        static MessageQueue _instance;
        
	public:
		/**
			\brief Get the queue shared instance
		 */
        static MessageQueue &getInstance( void ) { return _instance; }

	private:
		/**
			\brief Default constructor
		 */
		MessageQueue( void );

		/**
			\brief Destructor
		 */
		virtual ~MessageQueue( void );

	public:
		/**
			\brief Register a message dispatcher

			\remarks This method is for internal use of the messaging
			system only. Do not use it.
		 */
		void registerDispatcher( MessageDispatcher *dispatcher );

	public:
		/**
			\brief Dispatch received messages
		 */
		void dispatchMessages( void );

		/**
			\brief Discard all received messages
		 */
		void discardAllMessages( void );

		void reset( void );

		/**
			\brief Push a new message into the queue
		 */
		template< class MessageImpl >
        void pushMessage( std::shared_ptr< MessageImpl > const &message )
		{
			MessageDispatcherImpl< MessageImpl >::getInstance().pushMessage( message );
		}

		/**
			\brief Push a new message into the queue

			This implementation works with references to static allocated instances
			and its only valid for regular, non-deferred, messages
		 */
        /*
		template< class MessageImpl >
		void pushMessage( MessageImpl &message )
		{
			MessageDispatcherImpl< MessageImpl >::getInstance().pushMessage( message );
		}
         */

	private:
		std::list< MessageDispatcher * > _dispatchers;
	};

	template< class T >
	MessageHandler< T >::MessageHandler( void )
	{
		MessageDispatcherImpl< T >::getInstance().registerHandler( this );
	}

	template< class T >
	MessageHandler< T >::~MessageHandler( void )
	{
		MessageDispatcherImpl< T >::getInstance().unregisterHandler( this );
	}

	template< class T >
	MessageDispatcherImpl< T >::MessageDispatcherImpl( void )
	{
		MessageQueue::getInstance().registerDispatcher( this );
	}

	template< class T >
	MessageDispatcherImpl< T >::~MessageDispatcherImpl( void )
	{
	}

}

#endif

