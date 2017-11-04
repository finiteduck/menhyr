#include <assert.h>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <chrono>
#include "tinycompo.hpp"

using namespace std;

using int_matrix = vector<vector<int>>;

// Game-only object that stores a matrix of tile types
class Map : public Component {
    int_matrix mat;

  public:
    int size_x, size_y;

    Map(vector<int> v, size_t size_x, size_t size_y) : size_x(size_x), size_y(size_y) {
        assert(size_x * size_y == v.size());
        for (size_t x = 0; x < size_x; x++) {
            mat.emplace_back();
            for (size_t y = 0; y < size_y; y++) {
                mat.at(x).emplace_back(v.at(y + x * size_y));
            }
        }
    }

    const int_matrix &get() const { return mat; }
    int_matrix &get() { return mat; }

    void print() {
        for (auto line : mat) {
            for (auto element : line) {
                cout << element << " ";
            }
            cout << endl;
        }
    }
};

// Graphics-related object
class TileMap : public sf::Drawable, public sf::Transformable, public Component {
    sf::Texture tileset;
    sf::VertexArray array;
    Map *map{nullptr};

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const final {
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
        size_t size_x = map->size_x, size_y = map->size_y;

        tileset.loadFromFile("png/alltiles.png");
        array.setPrimitiveType(sf::Quads);
        array.resize(4 * size_x * size_y);

        for (size_t y = 0; y < size_y; y++) {
            for (size_t x = 0; x < size_x; x++) {
                auto index = x * map->size_y + y;
                auto type = map->get().at(x).at(y);
                draw_tile(index, type, x, y);
            }
        }
    }
};

class Person : public sf::Drawable, public sf::Transformable, public Component {
    sf::Texture person_texture;
    sf::Sprite person_sprite;

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const final {
        states.transform *= getTransform();
        target.draw(person_sprite, states);
    }

  public:
    Person() {
        person_texture.loadFromFile("png/people1.png");
        person_sprite.setTexture(person_texture);
    }
};

class MainLoop : public Component {
    TileMap *tilemap{nullptr};
    bool mouse_pressed{false};
    int mouse_x{0}, mouse_y{0};

  public:
    MainLoop() {
        port("go", &MainLoop::go);
        port("tilemap", &MainLoop::tilemap);
    }

    string _debug() const override { return "MainLoop"; }

    void go() {
        sf::RenderWindow window(sf::VideoMode(1500, 1000), "Test");
        // sf::RenderWindow window(sf::VideoMode(2560, 1080), "Test", sf::Style::Fullscreen);
        window.setFramerateLimit(60);

        sf::View main_view(sf::FloatRect(200, 200, 1700, 1200));
        window.setView(main_view);

        tilemap->load();

        Person john;
        john.setPosition(1300, 600);

        // sf::Texture clipTexture;
        // clipTexture.loadFromFile("test.png");
        // sf::Sprite clipSprite(clipTexture);
        // sf::BlendMode blendMode(sf::BlendMode::Zero, sf::BlendMode::One, sf::BlendMode::Add,
        //                         sf::BlendMode::DstAlpha, sf::BlendMode::OneMinusSrcAlpha,
        //                         sf::BlendMode::Subtract);
        // sf::RenderTexture renderTexture;
        // renderTexture.create(clipTexture.getSize().x, clipTexture.getSize().y);

        // int loops{0};
        // auto start = chrono::system_clock::now();

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

            // cout << "." << flush;
            // loops++;
            // if (loops == 30) {
            //     cout << chrono::duration<double>(chrono::system_clock::now() - start).count()
            //          << "s\n";
            //     loops = 0;
            //     start = chrono::system_clock::now();
            // }

            // sf::CircleShape hexagon(75, 6);
            // // hexagon.setRotation(90);
            // hexagon.setPosition(400, 100);
            // hexagon.setFillColor(sf::Color(0, 0, 0, 0));
            // hexagon.setOutlineThickness(4);
            // hexagon.setOutlineColor(sf::Color(180, 180, 255, 90));

            // renderTexture.clear(sf::Color::Transparent);
            // renderTexture.draw(*tilemap);
            // renderTexture.draw(clipSprite, blendMode);
            // renderTexture.display();
            // sf::Sprite renderSprite(renderTexture.getTexture());

            window.clear();
            window.draw(*tilemap);
            // window.draw(renderSprite);
            window.draw(john);
            // window.draw(hexagon);
            window.display();
        }
    }
};

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

    // Instantiating + calling main loop
    Assembly assembly(model);
    assembly.call("mainloop", "go");
}
