#ifndef CRIMILD_NEXT_EDITABLE_
#define CRIMILD_NEXT_EDITABLE_

#include "Node.hpp"

namespace crimild::next {

    class Editable : public Object::Extension {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Editable )
    public:
        float x = 0;
        float y = 0;
        float width = 1;
        float height = 1;
    };
}

#endif
