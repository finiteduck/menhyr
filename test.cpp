#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 500), "Test");
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(30);

    sf::Texture texture;
    texture.loadFromFile("test.png");

    sf::Sprite sprite;
    sprite.setTexture(texture);

    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::Blue);

    while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                }

            window.clear();
            window.draw(sprite);
            window.display();
        }

    return 0;
}
