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

#include <memory>
#include "HexGrid.hpp"
#include "Layer.hpp"
#include "TerrainMap.hpp"
#include "TileMap.hpp"
#include "ViewController.hpp"
#include "connectors.hpp"
#include "game_objects.hpp"

using namespace std;

/*
====================================================================================================
  ~*~ Interface ~*~
==================================================================================================*/
class Interface : public GameObject {
    sf::Text text;
    sf::Font font;
    int toolbar_size = 3;
    scalar button_size = 100;
    scalar space_between_buttons = 10;
    scalar total_width = toolbar_size * button_size + (toolbar_size - 1) * space_between_buttons;

    vector<sf::RectangleShape> buttons;
    vector<unique_ptr<SimpleObject>> icons;
    sf::RectangleShape selector;

    View* view;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // view->use(); // commented because job of the layer
        states.transform *= getTransform();
        target.draw(text);
        for (auto& button : buttons) {
            target.draw(button);
        }
        for (auto& icon : icons) {
            target.draw(*icon);
        }
        target.draw(selector);
    }

  public:
    int select{1};

    Interface() : selector(vec(button_size, button_size)) {
        port("view", &Interface::view);

        font.loadFromFile("DejaVuSans.ttf");
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color(255, 255, 255, 100));
        text.setStyle(sf::Text::Bold);

        for (int i = 0; i < toolbar_size; i++) {
            buttons.emplace_back(vec(button_size, button_size));
            buttons.back().setFillColor(sf::Color(255, 255, 255, 50));
        }

        icons.push_back(make_unique<SimpleObject>(144, "png/menhir.png"));
        icons.back()->get_sprite().setScale(0.75, 0.75);
        icons.push_back(make_unique<SimpleObject>(144, "png/faith.png"));
        icons.push_back(make_unique<SimpleObject>(144, "png/altar.png"));
        icons.back()->get_sprite().setScale(1.4, 1.4);

        selector.setFillColor(sf::Color(255, 255, 255, 0));
        selector.setOutlineColor(sf::Color::Red);
        selector.setOutlineThickness(2);
    }

    void update(scalar fps = 120) {
        text.setString(std::to_string((int)floor(fps + 0.5f)));
        vec wdim = view->get_size();

        // toolbar
        for (int i = 0; i < toolbar_size; i++) {
            auto& icon = icons.at(i)->get_sprite();
            buttons.at(i).setPosition(
                wdim.x / 2 - total_width / 2 + i * (button_size + space_between_buttons),
                wdim.y - button_size - space_between_buttons);
            icon.setPosition(wdim.x / 2 - total_width / 2 +
                                 i * (button_size + space_between_buttons) + button_size / 2,
                             wdim.y - button_size / 2 - space_between_buttons);
        }
        selector.setPosition(
            wdim.x / 2 - total_width / 2 + (select - 1) * (button_size + space_between_buttons),
            wdim.y - button_size - space_between_buttons);
    }
};

/*
====================================================================================================
  ~*~ MainMode ~*~
  A mode is an object that forwards events to relevant process_event methods.
==================================================================================================*/
class MainMode : public Component {
    HexCoords cursor_coords, last_click_coords;
    bool toggle_grid{true};
    scalar w = 144;
    vector<HexCoords> hexes_to_draw;
    vector<unique_ptr<Person>> persons;
    vector<unique_ptr<SimpleObject>> menhirs;
    vector<unique_ptr<Faith>> faith;

    vector<unique_ptr<Person>>* provide_persons() { return &persons; }

    Window* window;
    ViewController* view_controller;
    TileMap* terrain;
    HexGrid* grid;
    Interface* interface;
    Layer* person_layer;  // TODO find better name

    int selected_tool{1};

  public:
    MainMode() {
        provide("persons", &MainMode::provide_persons);
        port("window", &MainMode::window);
        port("view", &MainMode::view_controller);
        port("terrain", &MainMode::terrain);
        port("grid", &MainMode::grid);
        port("interface", &MainMode::interface);
        port("layer", &MainMode::person_layer);

        for (int i = 0; i < 7; i++) {
            persons.emplace_back(new Person(w));
            persons.back()->teleport_to(HexCoords(0, 0, 0));
        }
    }

    void load() {
        view_controller->update(w);
        terrain->map->set(HexCoords::from_offset(5, 5), 7);

        auto hexes_to_draw = view_controller->get_visible_coords(w);
        terrain->load(w, hexes_to_draw);
        grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);
    }

    bool process_event(sf::Event event) {
        vec pos = view_controller->get_mouse_position();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
            toggle_grid = !toggle_grid;
            grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);

        } else if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Num1:
                    selected_tool = 1;
                    interface->select = 1;
                    break;
                case sf::Keyboard::Num2:
                    selected_tool = 2;
                    interface->select = 2;
                    break;
                case sf::Keyboard::Num3:
                    selected_tool = 3;
                    interface->select = 3;
                    break;
                default:
                    break;
            }

        } else if (event.type == sf::Event::MouseButtonPressed and
                   event.mouseButton.button == sf::Mouse::Right) {
            cursor_coords = HexCoords::from_pixel(w, pos);
            grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);
            for (auto& p : persons) {
                p->go_to(cursor_coords);
            }

        } else if (event.type == sf::Event::MouseButtonPressed and
                   event.mouseButton.button == sf::Mouse::Left) {
            last_click_coords = HexCoords::from_pixel(w, pos);
            if (selected_tool == 1) {
                if (rand() % 2 == 0)
                    menhirs.emplace_back(new SimpleObject(w, "png/menhir.png", last_click_coords));
                else
                    menhirs.emplace_back(new SimpleObject(w, "png/menhir2.png", last_click_coords));
                person_layer->add_object(menhirs.back().get());
            } else if (selected_tool == 2) {
                faith.emplace_back(new Faith(w, last_click_coords));
                person_layer->add_object(faith.back().get());
            } else if (selected_tool == 3) {
                menhirs.emplace_back(new SimpleObject(w, "png/altar.png", last_click_coords));
                person_layer->add_object(menhirs.back().get());
            }

        } else if (!window->process_event(event)) {
            view_controller->process_event(event);
        }

        return window->process_event(event) and view_controller->process_event(event);
    }

    void before_draw(sf::Time elapsed_time) {
        interface->update(120);

        vec pos = view_controller->get_mouse_position();
        if (view_controller->update(w)) {
            hexes_to_draw = view_controller->get_visible_coords(w);
            terrain->load(w, hexes_to_draw);
            grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);
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
    ViewController* view_controller;
    vector<Layer*> layers;

    void add_layer(Layer* ptr) { layers.push_back(ptr); }

  public:
    MainLoop() {
        port("viewcontroller", &MainLoop::view_controller);
        port("main_mode", &MainLoop::main_mode);
        port("go", &MainLoop::go);
        port("layers", &MainLoop::add_layer);
    }

    void go() {
        auto& wref = view_controller->get_draw_ref();
        view_controller->init();

        std::vector<scalar> frametimes;

        sf::Clock clock;
        scalar fps = 0;
        while (wref.isOpen()) {
            sf::Event event;
            while (wref.pollEvent(event)) {
                main_mode->process_event(event);
            }

            sf::Time elapsed_time = clock.restart();
            frametimes.push_back(elapsed_time.asSeconds());
            if (frametimes.size() == 100) {
                scalar total_time = 0;
                for (auto t : frametimes) {
                    total_time += t;
                }
                fps = 100 / total_time;
                frametimes.clear();
            }

            main_mode->before_draw(elapsed_time);
            // TODO FPS

            wref.clear();

            for (auto& l : layers) {
                l->before_draw();
                l->set_view();
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

/*
====================================================================================================
  ~*~ main ~*~
==================================================================================================*/
int main() {
    srand(time(NULL));

    Model model;

    model.component<MainLoop>("mainloop")
        .connect<Use<ViewController>>("viewcontroller", "viewcontroller")
        .connect<Use<MainMode>>("main_mode", "mainmode")
        .connect<Use<Layer>>("layers", "terrainlayer")
        .connect<Use<Layer>>("layers", "gridlayer")
        .connect<Use<Layer>>("layers", "interfacelayer")
        .connect<Use<Layer>>("layers", "personlayer");

    model.component<Layer>("terrainlayer")
        .connect<Use<GameObject>>("objects", "terrain")
        .connect<Use<View>>("view", "mainview");
    model.component<Layer>("gridlayer")
        .connect<Use<GameObject>>("objects", "grid")
        .connect<Use<View>>("view", "mainview");
    model.component<Layer>("personlayer")
        .connect<UseObjectVector<Person>>("objects", PortAddress("persons", "mainmode"))
        .connect<Use<View>>("view", "mainview");
    model.component<Layer>("interfacelayer")
        .connect<Use<GameObject>>("objects", "interface")
        .connect<Use<View>>("view", "interfaceview");

    model.component<MainMode>("mainmode")
        .connect<Use<Window>>("window", "window")
        .connect<Use<HexGrid>>("grid", "grid")
        .connect<Use<TileMap>>("terrain", "terrain")
        .connect<Use<ViewController>>("view", "viewcontroller")
        .connect<Use<Layer>>("layer", "personlayer")
        .connect<Use<Interface>>("interface", "interface");

    model.component<Window>("window");
    model.component<TileMap>("terrain").connect<Use<TerrainMap>>("map", "terrainMap");
    model.component<TerrainMap>("terrainMap");
    model.component<HexGrid>("grid");
    model.component<Interface>("interface").connect<Use<View>>("view", "interfaceview");

    model.component<View>("mainview").connect<Use<Window>>("window", "window");
    model.component<View>("interfaceview", true).connect<Use<Window>>("window", "window");
    model.component<ViewController>("viewcontroller")
        .connect<Use<Window>>("window", "window")
        .connect<Use<View>>("mainview", "mainview")
        .connect<Use<View>>("interfaceview", "interfaceview");

    model.dot_to_file();

    Assembly assembly(model);
    assembly.call("mainloop", "go");
}
