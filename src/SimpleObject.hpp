#pragma once

#include "HexCoords.hpp"
#include "globals.hpp"

class SimpleObject : public GameObject {
    sf::Texture texture;
    sf::Sprite sprite;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(sprite, states);
    }

  public:
    SimpleObject(scalar w, sf::Texture& texture_ref, HexCoords hex = HexCoords(),
                 scalar shift = 0) {
        sprite.setTexture(texture_ref);
        vec origin(sprite.getLocalBounds().width / 2,
                   (sprite.getLocalBounds().height / 2) * (1 + shift));
        sprite.setOrigin(origin);
        setPosition(hex.get_pixel(w));
    }

    SimpleObject(scalar w, std::string texture_path, HexCoords hex = HexCoords(),
                 scalar shift = 0) {
        texture.loadFromFile(texture_path);
        sprite.setTexture(texture);
        vec origin(sprite.getLocalBounds().width / 2,
                   (sprite.getLocalBounds().height / 2) * (1 + shift));
        sprite.setOrigin(origin);
        setPosition(hex.get_pixel(w));
    }

    sf::Sprite& get_sprite() { return sprite; }
};
