#include <assert.h>
#include <math.h>
#include <SFML/Graphics.hpp>
#include "tinycompo.hpp"

/*
====================================================================================================
  ~*~ Global definitions ~*~
==================================================================================================*/
using namespace std;

struct GameObject : public sf::Drawable, public sf::Transformable, public Component {
    virtual void animate(float) {}
};

/*
====================================================================================================
  ~*~ Map Class ~*~
  Game-only object that stores a matrix of tile types
==================================================================================================*/
class Map : public Component {
    using TileType = int;

    vector<vector<int>> mat;

  public:
    Map(vector<int> v, size_t size_x, size_t size_y) {
        assert(size_x * size_y == v.size());
        for (size_t x = 0; x < size_x; x++) {
            mat.emplace_back();
            for (size_t y = 0; y < size_y; y++) {
                mat.at(x).emplace_back(v.at(y + x * size_y));
            }
        }
    }

    TileType at(int x, int y) const { return mat.at(x).at(y); }

    sf::Vector2i size() { return sf::Vector2i(mat.size(), mat.at(0).size()); }
};

/*
====================================================================================================
  ~*~ TileMap Class ~*~
  Graphics-related object.
==================================================================================================*/
class TileMap : public GameObject {
    sf::Texture tileset;
    sf::VertexArray array;
    Map *map{nullptr};

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &tileset;
        target.draw(array, states);
    }

    // x, y = coords of top left angle
    void draw_tile(int index, int type, int x, int y) {
        int size_y = 193, size_x = 258;  // tile size
        int w = 150;                     // hexagon width (distance between parallel edges)
        int overlap = 6;

        int hd = w - overlap;
        int vd = hd * sqrt(3) / 2;
        int shift_hexa = (y % 2 == 0) ? (hd / 2) : 0;

        sf::Vertex *quad = &array[index * 4];
        quad[0].position = sf::Vector2f(shift_hexa + x * hd, y * vd);
        quad[1].position = sf::Vector2f(shift_hexa + x * hd + size_x, y * vd);
        quad[2].position = sf::Vector2f(shift_hexa + x * hd + size_x, y * vd + size_y);
        quad[3].position = sf::Vector2f(shift_hexa + x * hd, y * vd + size_y);
        quad[0].texCoords = sf::Vector2f(0, type * size_y);
        quad[1].texCoords = sf::Vector2f(size_x, type * size_y);
        quad[2].texCoords = sf::Vector2f(size_x, (type + 1) * size_y);
        quad[3].texCoords = sf::Vector2f(0, (type + 1) * size_y);
    }

  public:
    int tile_size{150};

    TileMap() { port("map", &TileMap::map); }

    void load() {
        sf::Vector2i chunk_size = map->size();

        tileset.loadFromFile("png/alltiles.png");
        array.setPrimitiveType(sf::Quads);
        array.resize(4 * chunk_size.x * chunk_size.y);

        for (int y = 0; y < chunk_size.y; y++) {
            for (int x = 0; x < chunk_size.x; x++) {
                auto index = x * chunk_size.y + y;
                auto type = map->at(x, y);
                draw_tile(index, type, x, y);
            }
        }
    }
};

/*
====================================================================================================
  ~*~ Person Class ~*~
==================================================================================================*/
class Person : public GameObject {
    sf::Texture person_texture, clothes_texture;
    sf::Sprite person_sprite, clothes_sprite;

    sf::Vector2f target{1350, 625};
    float speed{25};  // in px/s

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(person_sprite, states);
        target.draw(clothes_sprite, states);
    }

  public:
    Person() {
        int number = rand() % 3 + 1;
        person_texture.loadFromFile("png/people" + to_string(number) + ".png");
        person_sprite.setTexture(person_texture);
        person_sprite.setColor(sf::Color(240, 230, 230));
        clothes_texture.loadFromFile("png/clothes" + to_string(number) + ".png");
        clothes_sprite.setTexture(clothes_texture);
        clothes_sprite.setColor(sf::Color(120, 120, 200));
    }

    void animate(float elapsed_time) override {
        auto before_pos = getPosition();
        auto path = target - before_pos;
        float length_path = sqrt(pow(path.x, 2) + pow(path.y, 2));
        if (length_path > 3) {
            auto move = path * (speed * elapsed_time / length_path);
            setPosition(before_pos + move);
        } else {  // if destination reached, choose another target
            target = before_pos + sf::Vector2f(rand() % 250 - 125, rand() % 250 - 125);
        }
    }
};

/*
====================================================================================================
  ~*~ Person Class ~*~
==================================================================================================*/
class HexGrid : public GameObject {
    vector<sf::CircleShape> hexagons;
    bool toggle_value{true};

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        if (toggle_value) {
            states.transform *= getTransform();
            for (auto h : hexagons) {
                target.draw(h, states);
            }
        }
    }

    void add_hexagon(int x, int y) {
        hexagons.emplace_back(80, 6);
        auto &hexagon = hexagons.back();

        int w = 150;  // hexagon width (distance between parallel edges)
        int overlap = 6;
        sf::Vector2f offset(102, 134);

        int hd = w - overlap;
        int vd = hd * sqrt(3) / 2;
        int shift_hexa = (y % 2 == 0) ? (hd / 2) : 0;

        hexagon.setPosition(sf::Vector2f(shift_hexa + x * hd, y * vd) + offset);

        // hexagon.setPosition(400, 100);
        hexagon.setFillColor(sf::Color(0, 0, 0, 0));
        hexagon.setOutlineThickness(10);
        hexagon.setOutlineColor(sf::Color(180, 180, 255, 15));
    }

  public:
    HexGrid() {
        for (int i = -2; i < 20; i++) {
            for (int j = -2; j < 15; j++) {
                add_hexagon(i, j);
            }
        }
    }

    void toggle() { toggle_value = !toggle_value; }
};

/*
====================================================================================================
  ~*~ MainLoop Class ~*~
==================================================================================================*/
class MainLoop : public Component {
    TileMap *tilemap{nullptr};
    bool mouse_pressed{false};
    int mouse_x{0}, mouse_y{0};

    HexGrid *grid;
    vector<GameObject *> objects;

  public:
    MainLoop() {
        port("go", &MainLoop::go);
        port("tilemap", &MainLoop::tilemap);
        port("objects", &MainLoop::add_object);
        port("grid", &MainLoop::grid);
    }

    sf::Vector2i dimensions{150 * 20, 150 * 15};

    void add_object(GameObject *object) { objects.push_back(object); }

    void go() {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;

        int window_width = 1500, window_height = 1000;
        sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Test",
                                sf::Style::Default, settings);
        // sf::RenderWindow window(sf::VideoMode(2560, 1080), "Test", sf::Style::Fullscreen);
        window.setFramerateLimit(60);

        sf::View main_view(sf::FloatRect(200, 200, 1700, 1200));
        window.setView(main_view);

        tilemap->load();

        sf::Clock clock;
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();

                // mouse scrolling
                else if (event.type == sf::Event::MouseButtonPressed and
                         event.mouseButton.button == sf::Mouse::Left) {
                    mouse_pressed = true;
                    mouse_x = event.mouseButton.x;
                    mouse_y = event.mouseButton.y;
                } else if (event.type == sf::Event::MouseButtonReleased and
                           event.mouseButton.button == sf::Mouse::Left) {
                    mouse_pressed = false;
                } else if (event.type == sf::Event::MouseWheelScrolled) {
                    main_view.zoom(1 - (event.mouseWheelScroll.delta * 0.15));
                    window.setView(main_view);
                } else if (event.type == sf::Event::Resized) {
                    float zoom = main_view.getSize().x / window_width;
                    main_view.setSize(zoom * event.size.width, zoom * event.size.height);
                    window.setView(main_view);
                    window_width = event.size.width;
                    window_height =
                        event.size.height;  // unused for now but might as well update it
                } else if (event.type == sf::Event::KeyPressed &&
                           event.key.code == sf::Keyboard::G) {
                    grid->toggle();
                }
            }
            if (mouse_pressed) {
                main_view.move(mouse_x - sf::Mouse::getPosition(window).x,
                               mouse_y - sf::Mouse::getPosition(window).y);
                window.setView(main_view);
                mouse_x = sf::Mouse::getPosition(window).x;
                mouse_y = sf::Mouse::getPosition(window).y;
                tilemap->load();
            }

            sf::Time elapsed_time = clock.restart();

            window.clear();
            window.draw(*tilemap);
            sort(objects.begin(), objects.end(), [](GameObject *p1, GameObject *p2) {
                return p1->getPosition().y < p2->getPosition().y;
            });
            for (auto object : objects) {
                object->animate(elapsed_time.asSeconds());
                window.draw(*object);
            }
            window.display();
        }
    }
};

/*
====================================================================================================
  ~*~ Place* connectors ~*~
==================================================================================================*/
struct PlaceObject {
    static void _connect(Assembly &assembly, Address world, Address object) {
        auto &world_ref = assembly.at<MainLoop>(world);
        auto &object_ref = assembly.at<GameObject>(object);
        auto dimensions = world_ref.dimensions;
        world_ref.add_object(&object_ref);
        object_ref.setPosition(rand() % dimensions.x, rand() % dimensions.y);
    }
};

struct PlaceArray {
    static void _connect(Assembly &assembly, Address world, Address array) {
        int array_size = assembly.at<Assembly>(array).size();
        for (int i = 0; i < array_size; i++) {
            PlaceObject::_connect(assembly, world, Address(array, i));
        }
    }
};

/*
====================================================================================================
  ~*~ main ~*~
==================================================================================================*/
int main() {
    srand(time(NULL));

    // Initializing tile map with random tiles + fixed menhir
    vector<int> tile_map;
    for (int i = 0; i < 300; i++) {
        tile_map.push_back(rand() % 7);
    }
    tile_map[156] = 7;

    // Declaring component assembly
    Model model;
    model.component<MainLoop>("mainloop");
    model.component<TileMap>("tilemap");
    model.component<Map>("map", tile_map, 20, 15);
    model.connect<Use<TileMap>>(PortAddress("tilemap", "mainloop"), Address("tilemap"));
    model.connect<Use<Map>>(PortAddress("map", "tilemap"), Address("map"));

    model.composite<Array<Person>>("objects", 30);
    model.connect<PlaceArray>(Address("mainloop"), Address("objects"));

    model.component<HexGrid>("grid");
    model.connect<Use<GameObject>>(PortAddress("objects", "mainloop"), Address("grid"));
    model.connect<Use<HexGrid>>(PortAddress("grid", "mainloop"), Address("grid"));

    // Instantiating + calling main loop
    Assembly assembly(model);
    assembly.call("mainloop", "go");
}
