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

#ifndef CRIMILD_CORE_BEHAVIORS_BEHAVIOR_CONTEXT_
#define CRIMILD_CORE_BEHAVIORS_BEHAVIOR_CONTEXT_

#include "Common/Variant.hpp"
#include "Crimild_Coding.hpp"
#include "Crimild_Foundation.hpp"
#include "Crimild_Mathematics.hpp"
#include "Simulation/Clock.hpp"

#include <functional>
#include <string>
#include <vector>

namespace crimild {

    class Node;

    namespace behaviors {

        /**
                         \brief Stores an inmutable value for the context
                */
        class BehaviorContextValue : public coding::Codable {
            CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::BehaviorContextValue )
        public:
            BehaviorContextValue( void );
            explicit BehaviorContextValue( std::string key, std::string value );
            virtual ~BehaviorContextValue( void );

            const std::string &getKey( void ) const { return _key; }
            const std::string &getValue( void ) const { return _value; }

        private:
            std::string _key;
            std::string _value;

            /**
                                \name Coding support
                        */
            //@{

        public:
            virtual void encode( coding::Encoder &encoder ) override;
            virtual void decode( coding::Decoder &decoder ) override;

            //@}
        };

        /**
                   \brief Execution context

                   \todo Add support for value observers
                */
        class BehaviorContext : public coding::Codable {
            CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::BehaviorContext )

        public:
            BehaviorContext( void );
            virtual ~BehaviorContext( void );

            void reset( void );
            void update( const crimild::Clock &c );

        public:
            void setAgent( crimild::Node *agent ) { _agent = agent; }
            crimild::Node *getAgent( void ) { return _agent; }

        private:
            crimild::Node *_agent = nullptr;

        public:
            crimild::Bool hasTargets( void ) const { return getTargetCount() > 0; }
            crimild::Size getTargetCount( void ) const;

            void addTarget( crimild::Node *target );
            crimild::Node *getTargetAt( crimild::Size index );
            void removeAllTargets( void );

            using TargetCallback = std::function< void( crimild::Node * ) >;
            void foreachTarget( TargetCallback const &callback );

        private:
            Array< Node * > _targets;

        public:
            const crimild::Clock &getClock( void ) const { return _clock; }

        private:
            crimild::Clock _clock;

        public:
            inline bool has( std::string_view key ) const noexcept
            {
                return m_values.count( std::string( key ) ) != 0;
            }

            inline void set( std::string_view key, std::shared_ptr< Variant > const &var ) noexcept
            {
                m_values[ std::string( key ) ] = var;
            }

            /**
             * @brief Set a value in the context
             *
             * If a value already exists with the given key, it will be overriden with
             * the new one.
             */
            template< typename T >
            inline SharedPointer< Variant > set( std::string_view key, const T &value ) noexcept
            {
                auto var = crimild::alloc< Variant >( value );
                set( key, var );
                return var;
            }

            std::shared_ptr< Variant > &get( std::string_view key ) noexcept
            {
                assert( has( key ) );
                return m_values.at( std::string( key ) );
            }

            /**
             * @brief Get an existing value or create a new entry with the default one
             *
             * This is different than using set(), since we only create a new entry if
             * one does not already exists (in which case we return the existing one
             * without overriding).
             */
            template< typename T >
            inline SharedPointer< Variant > getOrCreate( std::string_view key, const T &defaultValue ) noexcept
            {
                if ( has( key ) ) {
                    return get( key );
                } else {
                    return set( key, defaultValue );
                }
            }

        private:
            std::unordered_map< std::string, std::shared_ptr< Variant > > m_values;

        public:
            [[deprecated]] bool hasValue( std::string key )
            {
                return _values.contains( key );
            }

            template< typename T >
            [[deprecated]] void setValue( std::string key, T value )
            {
                std::stringstream ss;
                ss << value;
                _values[ key ] = crimild::alloc< BehaviorContextValue >( key, ss.str() );
            }

            template< typename T >
            [[deprecated]] T getValue( std::string key )
            {
                if ( !hasValue( key ) ) {
                    crimild::Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No context value set for key ", key );
                    return T();
                }

                T value;
                std::stringstream ss;
                ss << _values[ key ]->getValue();
                ss >> value;
                return value;
            }

        private:
            [[deprecated]] Map< std::string, SharedPointer< BehaviorContextValue > > _values;

            /**
                           \name Coding support
                        */
            //@{

        public:
            virtual void encode( coding::Encoder &encoder ) override;
            virtual void decode( coding::Decoder &decoder ) override;

            //@}

        public:
            void dump( void ) const;
        };

        template<>
        inline void BehaviorContext::setValue< Vector3f >( std::string key, Vector3f value )
        {
            setValue< crimild::Real32 >( key + ".x", value.x );
            setValue< crimild::Real32 >( key + ".y", value.y );
            setValue< crimild::Real32 >( key + ".z", value.z );
        }

        template<>
        inline crimild::Vector3f BehaviorContext::getValue( std::string key )
        {
            return Vector3f {
                getValue< crimild::Real32 >( key + ".x" ),
                getValue< crimild::Real32 >( key + ".y" ),
                getValue< crimild::Real32 >( key + ".z" ),
            };
        }

        template<>
        inline crimild::Vector4f BehaviorContext::getValue( std::string key )
        {
            std::stringstream ss;
            ss << _values[ key ];

            float x, y, z, w;
            ss >> x >> y >> z >> w;
            return crimild::Vector4f { x, y, z, w };
        }

    }

}

#endif
