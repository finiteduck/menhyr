#include <math.h>
#include <SFML/Graphics.hpp>
#include "tinycompo.hpp"

using namespace std;
using namespace tc;

/*
====================================================================================================
  ~*~ Window ~*~
==================================================================================================*/
class Window : public Component {
  public:
    int width{1500}, height{1000};

  private:
    sf::RenderWindow window;

  public:
    Window() : window(sf::VideoMode(width, height), "Menhyr") { window.setFramerateLimit(60); }

    void set_view(const sf::View& view) { window.setView(view); }

    sf::Vector2i get_mouse_position() { return sf::Mouse::getPosition(window); }

    bool is_open() { return window.isOpen(); }

    bool poll_event(sf::Event& event) { return window.pollEvent(event); }

    bool process_events(sf::Event& event) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else {
            return false;
        }
        return true;
    }

    void update() {
        window.clear();
        sf::CircleShape thing(100, 6);
        window.draw(thing);
        window.display();
    }
};

/*
====================================================================================================
  ~*~ GameView ~*~
==================================================================================================*/
class GameView : public Component {
    bool mouse_pressed{false};
    int mouse_x{0}, mouse_y{0};
    sf::View main_view;

    Window* window;

  public:
    GameView() : main_view(sf::FloatRect(200, 200, 1700, 1200)) {
        port("window", &GameView::window);
    }

    bool process_events(sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed and
            event.mouseButton.button == sf::Mouse::Left) {
            mouse_pressed = true;
            mouse_x = event.mouseButton.x;
            mouse_y = event.mouseButton.y;
        } else if (event.type == sf::Event::MouseButtonReleased and
                   event.mouseButton.button == sf::Mouse::Left) {
            mouse_pressed = false;
        } else if (event.type == sf::Event::MouseWheelScrolled) {
            main_view.zoom(1 - (event.mouseWheelScroll.delta * 0.15));
            window->set_view(main_view);
        } else if (event.type == sf::Event::Resized) {
            float zoom = main_view.getSize().x / window->width;
            main_view.setSize(zoom * event.size.width, zoom * event.size.height);
            window->set_view(main_view);
            window->width = event.size.width;
            window->height = event.size.height;  // unused for now but might as well update it
        } else {
            return false;
        }
        return true;
    }

    void update() {
        if (mouse_pressed) {
            auto mouse_pos = window->get_mouse_position();
            main_view.move(mouse_x - mouse_pos.x, mouse_y - mouse_pos.y);
            window->set_view(main_view);
            mouse_x = mouse_pos.x;
            mouse_y = mouse_pos.y;
        }
    }
};

/*
====================================================================================================
  ~*~ MainLoop ~*~
==================================================================================================*/
class MainLoop : public Component {
    Window* window;
    GameView* main_view;

  public:
    MainLoop() {
        port("window", &MainLoop::window);
        port("view", &MainLoop::main_view);
        port("go", &MainLoop::go);
    }

    void go() {
        main_view->update();

        while (window->is_open()) {
            sf::Event event;
            while (window->poll_event(event)) {
                if (window->process_events(event)) break;
                if (main_view->process_events(event)) break;
            }

            main_view->update();
            window->update();
        }
    }
};

/*
====================================================================================================
  ~*~ main ~*~
==================================================================================================*/
int main() {
    Model model;
    model.component<MainLoop>("mainloop")
        .connect<Use<Window>>("window", "window")
        .connect<Use<GameView>>("view", "mainview");
    model.component<Window>("window");
    model.component<GameView>("mainview").connect<Use<Window>>("window", "window");

    Assembly assembly(model);
    assembly.call("mainloop", "go");
}
