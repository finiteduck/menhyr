#include <math.h>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "tinycompo.hpp"

using namespace std;
using namespace tc;
using vec = sf::Vector2f;
using ivec = sf::Vector2i;
using cube = sf::Vector3i;
using scalar = float;

/*
====================================================================================================
  ~*~ Global defs and interfaces ~*~
==================================================================================================*/
struct GameObject : public sf::Drawable, public sf::Transformable, public Component {
    virtual void animate(scalar) {}
};

// vec operator*(scalar f, vec v) { return vec(f * v.x, f * v.y); }

/*
====================================================================================================
  ~*~ HexCoords ~*~
==================================================================================================*/
class HexCoords {
    int x{0}, y{0}, z{0};
    friend std::hash<HexCoords>;

  public:
    HexCoords() = default;
    HexCoords(int x, int y, int z) : x(x), y(y), z(z) {}
    HexCoords(cube c) : x(c.x), y(c.y), z(c.z) {}

    ivec get_axial() const { return ivec(x, y); }
    cube get_cube() const { return cube(x, y, z); }
    vec get_pixel(int w) const { return vec((x + float(y) / 2) * w, float(y) * w * sqrt(3) / 2); }

    bool operator==(const HexCoords& other) {
        return x == other.x and y == other.y and z == other.z;
    }

    static HexCoords from_axial(ivec v) { return HexCoords(v.x, v.y, -v.x - v.y); }
    static HexCoords from_axial(int x, int y) { return HexCoords(x, y, -x - y); }
    static HexCoords from_offset(int x, int y) {
        return HexCoords::from_axial(x - y / 2 - (y < 0 ? y % 2 : 0), y);  // TODO review formula
    }

    static HexCoords from_pixel(int w, int x, int y) {
        scalar fx((x - y / sqrt(3)) / w), fy(y * 2 / (sqrt(3) * w)), fz(-fx - fy);
        scalar rx(round(fx)), ry(round(fy)), rz(round(fz));
        scalar dx(abs(fx - rx)), dy(abs(fy - ry)), dz(abs(fz - rz));
        if (dx > dy and dx > dz) {
            return HexCoords(-ry - rz, ry, rz);
        } else if (dy > dz) {
            return HexCoords(rx, -rx - rz, rz);
        } else {
            return HexCoords(rx, ry, -rx - ry);
        }
    }

    static HexCoords from_cube(cube c) { return HexCoords(c); }
    static HexCoords from_cube(int x, int y, int z) { return HexCoords(x, y, z); }

    bool operator==(const HexCoords& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

ostream& operator<<(ostream& os, HexCoords c) {
    os << '(' << c.get_axial().x << ", " << c.get_axial().y << ')';
    return os;
}

namespace std {
    template <>
    struct hash<HexCoords> {
        size_t operator()(const HexCoords& key) const {
            size_t result = 17;
            result = result * 79 + hash<int>()(key.x);
            result = result * 79 + hash<int>()(key.y);
            result = result * 79 + hash<int>()(key.z);
            return result;
        }
    };
}  // namespace std

/*
====================================================================================================
  ~*~ TerrainMap ~*~
==================================================================================================*/
class TerrainMap : public Component {
    using TileType = int;
    unordered_map<HexCoords, TileType> map;

  public:
    TileType get(const HexCoords& coords) {
        auto it = map.find(coords);
        if (it == map.end()) {
            map.insert(make_pair(coords, rand() % 7));
            return map.at(coords);
        } else {
            return it->second;
        }
    }
};

/*
====================================================================================================
  ~*~ TileMap ~*~
==================================================================================================*/
class TileMap : public GameObject {
    sf::Texture tileset;
    sf::VertexArray array;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &tileset;
        target.draw(array, states);
    }

  public:
    void load(double w) {
        tileset.loadFromFile("png/alltiles.png");
        array.setPrimitiveType(sf::Quads);
        array.resize(35 * 35 * 4);

        TerrainMap tmap;

        for (int i = -10; i < 25; i++) {
            for (int j = -10; j < 25; j++) {
                int index = i + j * 35 + 360;
                sf::Vertex* quad = &array[index * 4];
                vec tile_dim{258, 193};
                vec tile_center{100, 100};
                vec hex_center = HexCoords::from_offset(i, j).get_pixel(w);
                vec tl = hex_center - tile_center;
                vec br = tl + tile_dim;
                int tile_type = tmap.get(HexCoords::from_offset(i,j));
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

    void update() {
        if (mouse_pressed) {
            auto mouse_pos = window->get_mouse_position();
            main_view.move(mouse_x - mouse_pos.x, mouse_y - mouse_pos.y);
            window->set_view(main_view);
            mouse_x = mouse_pos.x;
            mouse_y = mouse_pos.y;
        }
    }

    vec get_mouse_position() {
        return window->get().mapPixelToCoords(window->get_mouse_position());
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

        auto& wref = window->get();
        HexCoords yolo;
        scalar w = 144;
        TileMap map;
        map.load(w);
        while (wref.isOpen()) {
            sf::Event event;
            while (wref.pollEvent(event)) {
                if (event.type == sf::Event::MouseButtonPressed and
                    event.mouseButton.button == sf::Mouse::Right) {
                    vec pos = main_view->get_mouse_position();
                    yolo = HexCoords::from_pixel(w, pos.x, pos.y);
                } else if (!window->process_events(event))
                    main_view->process_events(event);
            }

            main_view->update();
            wref.clear();

            // tile map
            wref.draw(map);

            // hexagons
            for (int i = -10; i < 25; i++) {
                for (int j = -10; j < 25; j++) {
                    auto c = HexCoords::from_offset(i, j);
                    sf::CircleShape hex(25, 6);
                    hex.setOrigin(hex.getRadius(), hex.getRadius());
                    hex.setPosition(c.get_pixel(w));
                    if (c == yolo) {
                        hex.setFillColor(sf::Color::Red);
                    }
                    wref.draw(hex);
                }
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
        .connect<Use<GameView>>("view", "mainview");
    model.component<Window>("window");
    model.component<GameView>("mainview").connect<Use<Window>>("window", "window");

    Assembly assembly(model);
    assembly.call("mainloop", "go");
}
