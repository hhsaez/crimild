#ifndef CRIMILD_NEXT_NODE3D_
#define CRIMILD_NEXT_NODE3D_

#include "Node.hpp"
#include <Crimild_Mathematics.hpp>

namespace crimild::next {

    /**
     * @brief An empty node in 3D space
     */
    class Node3D : public Node {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Node3D )

    public:
        [[nodiscard]] inline bool hasParent( void ) const { return !m_parent.expired(); }
        [[nodiscard]] inline std::shared_ptr< Node3D > getParent( void ) const { return m_parent.lock(); }
        inline void setParent( std::shared_ptr< Node3D > const &parent ) { m_parent = parent; }

        virtual void setLocal( const Transformation &t )
        {
            m_local = t;
            m_localIsCurrent = true;
            m_worldIsCurrent = false;
        }

        [[nodiscard]] const Transformation &getLocal( void ) const 
        {
            if ( !m_localIsCurrent ) {
                // TODO
            }
            return m_local;
        }

        void setWorldIsCurrent( bool current )
        {
            m_worldIsCurrent = false;
        }

        // virtual void setWorld( const Transformation &t )
        // {
        //     m_world = t;
        //     m_worldIsCurrent = true;

        //     // Updating the world transform from outside should invalidate the
        //     // local one.
        //     m_localIsCurrent = false;
        // }

        [[nodiscard]] const Transformation &getWorld( void ) const
        {
            if ( !m_worldIsCurrent ) {
                assert( m_localIsCurrent ); // Local should always be current in this case.
                if ( auto parent = getParent() ) {
                    m_world = parent->getWorld()( m_local );
                } else {
                    m_world = m_local;
                }
                m_worldIsCurrent = true;
            }
            return m_world;
        }

    private:
        std::weak_ptr< Node3D > m_parent;

        Transformation m_local;
        bool m_localIsCurrent = true;
        
        mutable Transformation m_world;
        mutable bool m_worldIsCurrent = true;
    };

}

#endif

