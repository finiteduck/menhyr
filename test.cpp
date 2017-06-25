#include <SFML/Graphics.hpp>
#include "tinycompo.hpp"

class TileMap : public sf::Drawable, public sf::Transformable {
    sf::Texture tileset;
    sf::VertexArray array;

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &tileset;
        target.draw(array, states);
    }

  public:
    void load(const std::vector<int> &tiles) {
        tileset.loadFromFile("tileset.png");
        array.setPrimitiveType(sf::Quads);
        array.resize(100);
        for (auto i = 0; i < 5; i++) {
            for (auto j = 0; j < 5; j++) {
                auto index = i * 5 + j;
                auto type = tiles[index];
                sf::Vertex *quad = &array[index * 4];
                quad[0].position = sf::Vector2f(i * 100, j * 100);
                quad[1].position = sf::Vector2f((i + 1) * 100, j * 100);
                quad[2].position = sf::Vector2f((i + 1) * 100, (j + 1) * 100);
                quad[3].position = sf::Vector2f(i * 100, (j + 1) * 100);
                quad[0].texCoords = sf::Vector2f(type * 100, 0);
                quad[1].texCoords = sf::Vector2f((type + 1) * 100, 0);
                quad[2].texCoords = sf::Vector2f((type + 1) * 100, 100);
                quad[3].texCoords = sf::Vector2f(type * 100, 100);
            }
        }
    }
};

class MainLoop : public Component {
  public:
    MainLoop() {
        port("go", &MainLoop::go);
    }

    std::string _debug() const override { return "MainLoop"; }

    void go() {
        sf::RenderWindow window(sf::VideoMode(500, 500), "Test");
        window.setFramerateLimit(30);

        TileMap map;
        std::vector<int> tiles = {0, 0, 0, 1, 0, 0, 1, 1, 0, 2, 1, 0, 2,
                                  0, 0, 1, 1, 0, 0, 0, 2, 1, 1, 0, 0};
        map.load(tiles);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();
            }

            window.clear();
            window.draw(map);
            window.display();
        }
    }
};

int main() {
    Model<> model;
    model.component<MainLoop>("mainloop");

    Assembly<> assembly(model);
    assembly.call("mainloop", "go");
}
