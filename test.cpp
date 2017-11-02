#include <assert.h>
#include <SFML/Graphics.hpp>
#include "tinycompo.hpp"

using namespace std;

using int_matrix = vector<vector<int>>;

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
    void draw_tile(int index, int type, int x, int y, int shiftx = 0, int shifty = 0) {
        int size_y = 193, size_x = 258;  // tile size
        int w = 150;                     // hexagon width (distance between parallel edges)
        int overlap = 6;

        int hd = w - overlap;
        int vd = hd * sqrt(3) / 2;
        int shift_hexa = (y % 2 == 0) ? (hd / 2) : 0;

        sf::Vertex *quad = &array[index * 4];
        quad[0].position = sf::Vector2f(shiftx + shift_hexa + x * hd, shifty + y * vd);
        quad[1].position = sf::Vector2f(shiftx + shift_hexa + x * hd + size_x, shifty + y * vd);
        quad[2].position =
            sf::Vector2f(shiftx + shift_hexa + x * hd + size_x, shifty + y * vd + size_y);
        quad[3].position = sf::Vector2f(shiftx + shift_hexa + x * hd, shifty + y * vd + size_y);
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

        tileset.loadFromFile("alltiles.png");
        array.setPrimitiveType(sf::Quads);
        array.resize(4 * size_x * size_y);
        // cout << "size x is " << size_x << " and size y is " << size_y << endl;

        // map->print();

        for (size_t y = 0; y < size_y; y++) {
            for (size_t x = 0; x < size_x; x++) {
                auto index = x * map->size_y + y;
                auto type = map->get().at(x).at(y);
                draw_tile(index, type, x, y, -180, -180);
            }
        }
    }
};

class MainLoop : public Component {
    TileMap *tilemap{nullptr};
    Map *map{nullptr};

  public:
    MainLoop() {
        port("go", &MainLoop::go);
        port("tilemap", &MainLoop::tilemap);
        port("map", &MainLoop::map);
    }

    string _debug() const override { return "MainLoop"; }

    void go() {
        sf::RenderWindow window(sf::VideoMode(1024, 768), "Test");
        window.setFramerateLimit(30);

        tilemap->load();

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();
            }

            window.clear();
            window.draw(*tilemap);
            window.display();
        }
    }
};

int main() {
    srand(time(NULL));

    vector<int> tile_map;
    for (int i = 0; i < 100; i++) {
        tile_map.push_back(rand() % 4);
    }
    tile_map[44] = 4;

    Model model;
    model.component<MainLoop>("mainloop");
    model.component<TileMap>("tilemap");
    model.component<Map>("map", tile_map, 10, 10);
    model.connect<Use<TileMap>>(PortAddress("tilemap", "mainloop"), Address("tilemap"));
    model.connect<Use<Map>>(PortAddress("map", "tilemap"), Address("map"));
    model.connect<Use<Map>>(PortAddress("map", "mainloop"), Address("map"));

    Assembly assembly(model);
    assembly.call("mainloop", "go");
}
