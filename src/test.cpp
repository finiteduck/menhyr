/*Copyright Vincent Lanore 2017

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
#include "Person.hpp"
#include "TerrainMap.hpp"

/*
====================================================================================================
  ~*~ TileMap ~*~
==================================================================================================*/
class TileMap : public GameObject {
    sf::Texture tileset;
    sf::VertexArray array;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &tileset;
        target.draw(array, states);
    }

  public:
    TerrainMap* map;

    TileMap() { port("map", &TileMap::map); }

    void load(double w, const vector<HexCoords>& coords) {
        tileset.loadFromFile("png/alltiles.png");
        array.setPrimitiveType(sf::Quads);
        array.resize(coords.size() * 4);

        for (size_t i = 0; i < coords.size(); i++) {
            auto hex_coords = coords.at(i);
            sf::Vertex* quad = &array[i * 4];
            vec tile_dim{258, 193};
            vec tile_center{109, 88};
            vec hex_center = hex_coords.get_pixel(w);
            vec tl = hex_center - tile_center;
            vec br = tl + tile_dim;
            int tile_type = map->get(hex_coords);
            vec tex_tl = vec{0, tile_type * tile_dim.y};
            quad[0].position = tl;
            quad[1].position = vec{br.x, tl.y};
            quad[2].position = br;
            quad[3].position = vec{tl.x, br.y};
            quad[0].texCoords = tex_tl;
            quad[1].texCoords = tex_tl + vec{tile_dim.x, 0};
            quad[2].texCoords = tex_tl + tile_dim;
            quad[3].texCoords = tex_tl + vec{0, tile_dim.y};
        }
    }
};

/*
====================================================================================================
  ~*~ HexGrid ~*~
==================================================================================================*/
class HexGrid : public GameObject {
    vector<sf::CircleShape> hexes;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        for (auto hex : hexes) {
            target.draw(hex, states);
        }
    }

  public:
    void highlight(float w, HexCoords coords) {
        hexes.emplace_back(w / sqrt(3) - 3, 6);
        auto& hex = hexes.back();
        hex.setOrigin(hex.getRadius(), hex.getRadius());
        hex.setPosition(coords.get_pixel(w));
        hex.setFillColor(sf::Color(255, 0, 0, 15));
    }

    void load(float w, vector<HexCoords> coords, HexCoords cursor = HexCoords(0, 0, 0),
              bool toggle_grid = true) {
        hexes.clear();

        highlight(w, cursor);

        // rest of the grid
        if (toggle_grid) {
            for (auto c : coords) {
                hexes.emplace_back(w / sqrt(3) - 3, 6);
                auto& hex = hexes.back();
                hex.setOrigin(hex.getRadius(), hex.getRadius());
                hex.setFillColor(sf::Color(0, 0, 0, 0));
                hex.setPosition(c.get_pixel(w));
                hex.setOutlineColor(sf::Color(255, 255, 255, 15));
                hex.setOutlineThickness(3);
            }
        }
    }
};

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

    bool process_events(sf::Event& event) {
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
  ~*~ MainLoop ~*~
==================================================================================================*/
class MainLoop : public Component {
    Window* window;
    GameView* main_view;
    TileMap* terrain;
    HexGrid* grid;

  public:
    MainLoop() {
        port("window", &MainLoop::window);
        port("view", &MainLoop::main_view);
        port("terrain", &MainLoop::terrain);
        port("grid", &MainLoop::grid);
        port("go", &MainLoop::go);
    }

    void go() {
        auto& wref = window->get();
        HexCoords cursor_coords;
        bool toggle_grid = true;
        scalar w = 144;

        main_view->update(w);
        terrain->map->set(HexCoords::from_offset(5, 5), 7);

        vector<unique_ptr<Person>> persons;
        for (int i = 0; i < 7; i++) {
            persons.emplace_back(new Person(w));
            persons.back()->teleport_to(HexCoords(0, 0, 0));
        }

        auto hexes_to_draw = main_view->get_visible_coords(w);
        terrain->load(w, hexes_to_draw);
        grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);

        sf::Clock clock;
        while (wref.isOpen()) {
            sf::Time elapsed_time = clock.restart();
            sf::Event event;
            while (wref.pollEvent(event)) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
                    toggle_grid = !toggle_grid;
                    grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);

                } else if (event.type == sf::Event::MouseButtonPressed and
                           event.mouseButton.button == sf::Mouse::Right) {
                    vec pos = main_view->get_mouse_position();
                    cursor_coords = HexCoords::from_pixel(w, pos);
                    grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);
                    for (auto& p : persons) {
                        p->go_to(cursor_coords);
                    }

                } else if (!window->process_events(event)) {
                    main_view->process_events(event);
                }
            }
            if (main_view->update(w)) {
                hexes_to_draw = main_view->get_visible_coords(w);
                terrain->load(w, hexes_to_draw);
                grid->load(w, hexes_to_draw, cursor_coords, toggle_grid);
                // cout << "number of displayed hexes: " << hexes_to_draw.size() << "\n";
            }
            wref.clear();

            // tile map
            wref.draw(*terrain);
            wref.draw(*grid);
            sort(persons.begin(), persons.end(),
                 [](unique_ptr<Person>& p1, unique_ptr<Person>& p2) {
                     return p1->getPosition().y < p2->getPosition().y;
                 });
            for (auto& person : persons) {
                person->animate(elapsed_time.asSeconds());
                wref.draw(*person);
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
        .connect<Use<Window>>("window", "window")
        .connect<Use<GameView>>("view", "mainview")
        .connect<Use<TileMap>>("terrain", "terrain")
        .connect<Use<HexGrid>>("grid", "grid");
    model.component<Window>("window");
    model.component<GameView>("mainview").connect<Use<Window>>("window", "window");
    model.component<TileMap>("terrain").connect<Use<TerrainMap>>("map", "terrainMap");
    model.component<TerrainMap>("terrainMap");
    model.component<HexGrid>("grid");

    Assembly assembly(model);
    assembly.call("mainloop", "go");
}
