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

#include "Foundation/Macros.hpp"
#include "Foundation/Singleton.hpp"
#include "Foundation/Log.hpp"

#include <functional>
#include <map>
#include <vector>

namespace crimild {

	class Messenger;

	template< class MessageType >
	using MessageHandler = std::function< void( MessageType const & ) >;

	class MessageQueueDispatcher : public NonCopyable {
	protected:
		MessageQueueDispatcher( void )
		{

		}

	public:
		virtual ~MessageQueueDispatcher( void )
		{

		}

		virtual void unregisterHandler( Messenger *handler ) = 0;

		virtual void dispatchDeferredMessages( void ) = 0;

		virtual void clear( void ) = 0;
	};

	template< class MessageType >
	class MessageQueueDispatcherImpl : 
		public MessageQueueDispatcher,
		public StaticSingleton< MessageQueueDispatcherImpl< MessageType >> {

	public:
		MessageQueueDispatcherImpl( void );

		virtual ~MessageQueueDispatcherImpl( void ) { }

		void registerHandler( Messenger *target, MessageHandler< MessageType > handler )
		{
			_handlers[ target ] = handler;
		}

		virtual void unregisterHandler( Messenger *handler ) override
		{
			auto it = _handlers.find( handler );
			if ( it != _handlers.end() ) {
				_handlers.erase( it );
			}
		}

	private:
		std::map< Messenger *, MessageHandler< MessageType >> _handlers;

	public:
		void broadcastMessage( MessageType const &message )
		{
            auto hs = _handlers;
			for ( auto it : hs ) {
				if ( it.first != nullptr && it.second != nullptr ) {
					it.second( message );
				}
			}
		}

	public:
		void pushMessage( MessageType const &message )
		{
			_deferredMessages.push_back( message );
		}

		virtual void dispatchDeferredMessages( void ) override
		{
			for ( auto &m : _deferredMessages ) {
				broadcastMessage( m );
			}
			_deferredMessages.clear();
		}

	public:
		virtual void clear( void ) override
		{
			_deferredMessages.clear();
		}

	private:
		std::vector< MessageType > _deferredMessages;
	};

	class MessageQueue : public StaticSingleton< MessageQueue > {
	public:
        MessageQueue( void )
        {
            Log::Debug << "Initializing message queue" << Log::End;
        }
        
        virtual ~MessageQueue( void )
        {
            
        }
        
		template< class MessageType >
		void registerHandler( Messenger *target, MessageHandler< MessageType > handler )
		{
			MessageQueueDispatcherImpl< MessageType >::getInstance()->registerHandler( target, handler );
		}

		template< class MessageType >
		void unregisterHandler( Messenger *target )
		{
			MessageQueueDispatcherImpl< MessageType >::getInstance()->unregisterHandler( target );
		}

		void unregisterHandler( Messenger *target )
		{
			for ( auto d : _dispatchers ) {
				if ( d != nullptr ) {
					d->unregisterHandler( target );
				}
			}
		}

	public:
		template< class MessageType >
		void broadcastMessage( MessageType const &message )
		{
			MessageQueueDispatcherImpl< MessageType >::getInstance()->broadcastMessage( message );
		}

	public:
		void registerMessageDispatcher( MessageQueueDispatcher *dispatcher )
		{
			_dispatchers.push_back( dispatcher );
		}

	public:
		template< class MessageType >
		void pushMessage( MessageType const &message )
		{
			MessageQueueDispatcherImpl< MessageType >::getInstance()->pushMessage( message );
		}

		void dispatchDeferredMessages( void )
		{
			for ( auto d : _dispatchers ) {
				if ( d != nullptr ) {
					d->dispatchDeferredMessages();
				}
			}
		}

	public:
		void clear( void )
		{
			for ( auto d : _dispatchers ) {
				if ( d != nullptr ) {
					d->clear();
				}
			}
		}

	private:
		std::vector< MessageQueueDispatcher * > _dispatchers;
	};

	template< class T >
	MessageQueueDispatcherImpl< T >::MessageQueueDispatcherImpl( void )
	{
		MessageQueue::getInstance()->registerMessageDispatcher( this );
	}
    
	class Messenger {
	public:
		Messenger( void );
		virtual ~Messenger( void );

	public:
		template< class T >
		void broadcastMessage( T const &message )
		{
			getMessageQueue()->broadcastMessage( message );
		}

//		template< class T >
//		void pushMessage( T const &message )
//		{
//			getMessageQueue()->pushMessage( message );
//		}

	public:
		template< class T >
		void registerMessageHandler( MessageHandler< T > handler )
		{
			getMessageQueue()->registerHandler( this, handler );
		}

		template< class MessageType >
		void unregisterMessageHandler( void )
		{
			getMessageQueue()->unregisterHandler< MessageType >( this );
		}

	private:
		MessageQueue *getMessageQueue( void )
		{
			return MessageQueue::getInstance();
		}
	};

}

#define CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( MESSAGE_TYPE, CLASS_NAME, MEMBER_FUNCTION ) \
	registerMessageHandler< MESSAGE_TYPE >( std::bind( &CLASS_NAME::MEMBER_FUNCTION, this, std::placeholders::_1 ) );

#endif

