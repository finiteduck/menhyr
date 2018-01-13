#pragma once

#include "SimpleObject.hpp"

/*
====================================================================================================
  ~*~ Person Class ~*~
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

/*
====================================================================================================
  ~*~ Person Class ~*~
==================================================================================================*/
class Person : public GameObject {
    sf::Texture person_texture, clothes_texture;
    sf::Sprite person_sprite, clothes_sprite;

    HexCoords hex;
    scalar w;
    sf::Vector2f target{1350, 625};
    float speed{25};  // in px/s

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(person_sprite, states);
        target.draw(clothes_sprite, states);
        // highlight(person_sprite, target, states);
    }

  public:
    Person(scalar w) : w(w) {
        int number = rand() % 3 + 1;
        person_texture.loadFromFile("png/people" + std::to_string(number) + ".png");
        person_sprite.setTexture(person_texture);
        person_sprite.setColor(sf::Color(240, 230, 230));
        vec origin(person_sprite.getLocalBounds().width / 2,
                   person_sprite.getLocalBounds().height / 2);
        person_sprite.setOrigin(origin);
        clothes_texture.loadFromFile("png/clothes" + std::to_string(number) + ".png");
        clothes_sprite.setTexture(clothes_texture);
        clothes_sprite.setColor(sf::Color(50 + rand() % 100, 50 + rand() % 100, 150 + rand() % 50));
        clothes_sprite.setOrigin(origin);
    }

    void teleport_to(const HexCoords& position) {
        vec pixel_pos = position.random_pixel(w, 0.8);
        setPosition(pixel_pos);
        target = pixel_pos;
        hex = position;
    }

    void go_to(const HexCoords& position) {
        target = position.random_pixel(w, 0.8);
        hex = position;
        speed = 50;
    }

    void animate(float elapsed_time) override {
        auto before_pos = getPosition();
        auto path = target - before_pos;
        float length_path = sqrt(pow(path.x, 2) + pow(path.y, 2));
        if (length_path > 3) {
            auto move = path * (speed * elapsed_time / length_path);
            setPosition(before_pos + move);
        } else {  // if destination reached, choose another target
            speed = 10;
            target = hex.random_pixel(w, 0.8);
        }
    }
};
