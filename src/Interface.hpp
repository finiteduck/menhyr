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

#pragma once

#include "SimpleObject.hpp"
#include "globals.hpp"

/*
====================================================================================================
  ~*~ Interface ~*~
==================================================================================================*/
class Interface : public GameObject {
    sf::Text text;
    sf::Font font;
    int toolbar_size = 4;
    scalar button_size = 100;
    scalar space_between_buttons = 10;
    scalar total_width = toolbar_size * button_size + (toolbar_size - 1) * space_between_buttons;

    vector<sf::RectangleShape> buttons;
    vector<unique_ptr<SimpleObject>> icons;
    sf::RectangleShape selector;

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
        icons.back()->get_sprite().setScale(0.45, 0.45);
        icons.push_back(make_unique<SimpleObject>(144, "png/faith.png"));
        icons.push_back(make_unique<SimpleObject>(144, "png/altar.png"));
        icons.back()->get_sprite().setScale(0.9, 0.9);
        icons.push_back(make_unique<SimpleObject>(144, "png/tree1.png"));
        icons.back()->get_sprite().setScale(0.45, 0.45);

        selector.setFillColor(sf::Color(255, 255, 255, 0));
        selector.setOutlineColor(sf::Color::Red);
        selector.setOutlineThickness(2);
    }

    void before_draw(vec wdim, scalar fps) {
        text.setString(std::to_string((int)floor(fps + 0.5f)));
        // vec wdim = view->get_size();

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
