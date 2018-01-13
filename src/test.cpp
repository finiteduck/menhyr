/*Copyright Vincent Lanore 2017-2018

  This file is part of Menhyr.

  Menhyr is free software: you can redistribute it and/or modify it under the terms of the GNU
  Lesser General Public License as published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  Menhyr is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with Menhyr. If
  not, see <http://www.gnu.org/licenses/>.*/

#include "HexCoords.hpp"
#include "HexGrid.hpp"
#include "Layer.hpp"
#include "Person.hpp"
#include "SimpleObject.hpp"
#include "TerrainMap.hpp"
#include "TileMap.hpp"

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

    bool process_event(sf::Event& event) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else {
            return false;
        }
        return true;
    }

    sf::RenderWindow& get() { return window; }
};

/*
====================================================================================================
  ~*~ GameView ~*~
==================================================================================================*/
class GameView : public Component {
    bool mouse_pressed{false};
    int mouse_x{0}, mouse_y{0};
    sf::View main_view;
    HexCoords ctl, cbr;

    Window* window;

  public:
    GameView() : main_view(sf::FloatRect(-200, -200, 1300, 800)) {
        port("window", &GameView::window);
    }

    bool process_event(sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed and
            event.mouseButton.button == sf::Mouse::Middle) {
            mouse_pressed = true;
            mouse_x = event.mouseButton.x;
            mouse_y = event.mouseButton.y;
        } else if (event.type == sf::Event::MouseButtonReleased and
                   event.mouseButton.button == sf::Mouse::Middle) {
            mouse_pressed = false;
        } else if (event.type == sf::Event::MouseWheelScrolled) {
            main_view.zoom(1 - (event.mouseWheelScroll.delta * 0.15));
            window->set_view(main_view);
        } else if (event.type == sf::Event::Resized) {
            scalar zoom = main_view.getSize().x / window->width;
            main_view.setSize(zoom * event.size.width, zoom * event.size.height);
            window->set_view(main_view);
            window->width = event.size.width;
            window->height = event.size.height;  // unused for now but might as well update it
        } else {
            return false;
        }
        return true;
    }

    bool update(scalar w) {
        if (mouse_pressed) {
            auto mouse_pos = window->get_mouse_position();
            scalar zoom = main_view.getSize().x / window->width;
            main_view.move((mouse_x - mouse_pos.x) * zoom, (mouse_y - mouse_pos.y) * zoom);
            window->set_view(main_view);
            mouse_x = mouse_pos.x;
            mouse_y = mouse_pos.y;
        }
        vec dim = main_view.getSize();
        vec center = main_view.getCenter();
        vec tl = center - dim / 2;
        vec br = tl + dim;
        return !(HexCoords::from_pixel(w, tl) == ctl) or !(HexCoords::from_pixel(w, br) == cbr);
    }

    vec get_mouse_position() {
        return window->get().mapPixelToCoords(window->get_mouse_position());
    }

    vector<HexCoords> get_visible_coords(int w) {
        // gather relevant view coordinates
        vec dim = main_view.getSize();
        vec center = main_view.getCenter();
        vec tl = center - dim / 2;
        vec br = tl + dim;

        vector<HexCoords> result;

        ctl = HexCoords::from_pixel(w, tl);
        cbr = HexCoords::from_pixel(w, br);
        for (int i = ctl.get_offset().x - 1; i <= cbr.get_offset().x + 1; i++) {
            for (int j = ctl.get_offset().y - 1; j <= cbr.get_offset().y + 1; j++) {
                result.push_back(HexCoords::from_offset(i, j));
            }
        }
        return result;
    }
};

/*
====================================================================================================
  ~*~ MainMode ~*~
  A mode is an object that forwards events to relevant process_event methods.
==================================================================================================*/
class Faith : public SimpleObject {
    vec target{0, 0};
    scalar speed{100}, rotation_speed{15};

  public:
    Faith(scalar w, HexCoords hex = HexCoords()) : SimpleObject(w, "png/faith.png", hex) {}

    void set_target(vec new_target) { target = new_target; }

    void animate(float elapsed_time) override {
        auto before_pos = getPosition();
        auto path = target - before_pos;
        float length_path = sqrt(pow(path.x, 2) + pow(path.y, 2));
        if (length_path > 3) {
            auto move = path * (speed * elapsed_time / length_path);
            setPosition(before_pos + move);
        }

        setRotation(fmod(getRotation() + rotation_speed * elapsed_time, 360));
    }
};

class MainMode : public Component {
    HexCoords cursor_coords;
    bool toggle_grid{true};
    scalar w = 144;
    vector<HexCoords> hexes_to_draw;
    vector<unique_ptr<Person>> persons;
    vector<unique_ptr<SimpleObject>> menhirs;
    vector<unique_ptr<Faith>> faith;

    vector<unique_ptr<Person>>* provide_persons() { return &persons; }

    Window* window;
    GameView* main_view;
    TileMap* terrain;
    HexGrid* grid;
    Layer* person_layer;  // TODO find better name

  public:
    MainMode() {
        provide("persons", &MainMode::provide_persons);
        port("window", &MainMode::window);
        port("view", &MainMode::main_view);
        port("terrain", &MainMode::terrain);
        port("grid", &MainMode::grid);
        port("layer", &MainMode::person_layer);

        for (int i = 0; i < 7; i++) {
            persons.emplace_back(new Person(w));
            persons.back()->teleport_to(HexCoords(0, 0, 0));
        }
    }

    void load() {
        main_view->update(w);
        terrain->map->set(HexCoords::from_offset(5, 5), 7);

        auto hexes_to_draw = main_view->get_visible_coords(w);
        terrain->load(w, hexes_to_draw);
        grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);
    }

    bool process_event(sf::Event event) {
        vec pos = main_view->get_mouse_position();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
            toggle_grid = !toggle_grid;
            grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);

        } else if (event.type == sf::Event::MouseButtonPressed and
                   event.mouseButton.button == sf::Mouse::Right) {
            cursor_coords = HexCoords::from_pixel(w, pos);
            grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);
            for (auto& p : persons) {
                p->go_to(cursor_coords);
            }
        } else if (event.type == sf::Event::MouseButtonPressed and
                   event.mouseButton.button == sf::Mouse::Left) {
            cursor_coords = HexCoords::from_pixel(w, pos);
            menhirs.emplace_back(new SimpleObject(w, "png/menhir.png", cursor_coords));
            person_layer->add_object(menhirs.back().get());

            faith.emplace_back(new Faith(w, cursor_coords));
            person_layer->add_object(faith.back().get());

        } else if (!window->process_event(event)) {
            main_view->process_event(event);
        }

        return window->process_event(event) and main_view->process_event(event);
    }

    void before_draw(sf::Time elapsed_time) {
        vec pos = main_view->get_mouse_position();
        if (main_view->update(w)) {
            hexes_to_draw = main_view->get_visible_coords(w);
            terrain->load(w, hexes_to_draw);
            grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);
            // cout << "number of displayed hexes: " << hexes_to_draw.size() << "\n";
        }

        for (auto& person : persons) {
            person->animate(elapsed_time.asSeconds());
        }
        for (auto& f : faith) {
            f->set_target(pos);
            f->animate(elapsed_time.asSeconds());
        }
    }
};

/*
====================================================================================================
  ~*~ MainLoop ~*~
==================================================================================================*/
class MainLoop : public Component {
    MainMode* main_mode;
    Window* window;
    vector<Layer*> layers;

    void add_layer(Layer* ptr) { layers.push_back(ptr); }

  public:
    MainLoop() {
        port("window", &MainLoop::window);
        port("main_mode", &MainLoop::main_mode);
        port("go", &MainLoop::go);
        port("layers", &MainLoop::add_layer);
    }

    void go() {
        auto& wref = window->get();

        sf::Clock clock;
        while (wref.isOpen()) {
            sf::Event event;
            while (wref.pollEvent(event)) {
                main_mode->process_event(event);
            }

            sf::Time elapsed_time = clock.restart();
            main_mode->before_draw(elapsed_time);

            wref.clear();

            for (auto& l : layers) {
                l->before_draw();
                wref.draw(*l);
            }

            // origin
            sf::CircleShape origin(5);
            origin.setFillColor(sf::Color::Red);
            origin.setOrigin(origin.getRadius(), origin.getRadius());
            origin.setPosition(0, 0);
            wref.draw(origin);

            wref.display();
        }
    }
};

template <class ElemType>
struct UseObjectVector {
    static void _connect(Assembly& assembly, tc::PortAddress user, tc::PortAddress provider) {
        auto& ref_user = assembly.at(user.address);
        auto& ref_provider = assembly.at(provider.address);
        vector<unique_ptr<ElemType>>& v_ref =
            *ref_provider.template get<vector<unique_ptr<ElemType>>>(provider.prop);
        for (auto& e : v_ref) {
            ref_user.set(user.prop, dynamic_cast<GameObject*>(e.get()));
        }
    }
};

/*
====================================================================================================
  ~*~ main ~*~
==================================================================================================*/
int main() {
    srand(time(NULL));

    Model model;

    model.component<MainLoop>("mainloop")
        .connect<Use<Window>>("window", "window")
        .connect<Use<MainMode>>("main_mode", "mainmode")
        .connect<Use<Layer>>("layers", "terrainlayer")
        .connect<Use<Layer>>("layers", "gridlayer")
        .connect<Use<Layer>>("layers", "personlayer");

    model.component<Layer>("terrainlayer").connect<Use<GameObject>>("objects", "terrain");
    model.component<Layer>("gridlayer").connect<Use<GameObject>>("objects", "grid");
    model.component<Layer>("personlayer")
        .connect<UseObjectVector<Person>>("objects", PortAddress("persons", "mainmode"));

    model.component<MainMode>("mainmode")
        .connect<Use<Window>>("window", "window")
        .connect<Use<HexGrid>>("grid", "grid")
        .connect<Use<TileMap>>("terrain", "terrain")
        .connect<Use<GameView>>("view", "mainview")
        .connect<Use<Layer>>("layer", "personlayer");

    model.component<Window>("window");
    model.component<GameView>("mainview").connect<Use<Window>>("window", "window");
    model.component<TileMap>("terrain").connect<Use<TerrainMap>>("map", "terrainMap");
    model.component<TerrainMap>("terrainMap");
    model.component<HexGrid>("grid");

    Assembly assembly(model);
    assembly.call("mainloop", "go");
}
