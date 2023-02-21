#ifndef CRIMILD_EDITOR_PANELS_MAIN_MENU_BAR
#define CRIMILD_EDITOR_PANELS_MAIN_MENU_BAR

namespace crimild::editor::panels {

    class MainMenuBar {
    public:
        void render( void ) noexcept;

    private:
        void renderFileMenu( void ) noexcept;
        void renderEditMenu( void ) noexcept;
        void renderSceneMenu( void ) noexcept;
        void renderViewMenu( void ) noexcept;
        void renderHelpMenu( void ) noexcept;
    };

}

#endif
