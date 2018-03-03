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

#include "HexCoords.hpp"

/*
====================================================================================================
  ~*~ HexGrid ~*~
==================================================================================================*/
class HexGrid : public GameObject, public Component {
    vector<sf::CircleShape> hexes;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        for (auto& hex : hexes) {
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
