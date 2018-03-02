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

#include <map>
#include "HexCoords.hpp"
#include "TileMap.hpp"

/*
====================================================================================================
  ~*~ Cell ~*~
  A cell is a fairly large square of the world. It's atomic in terms of rendering and loading into
  memory. It contains map info and references to objects.
==================================================================================================*/
class Cell : public sf::Drawable,
             public sf::Transformable {  // not a component :) at least for now (too dynamic)
    // storing objects by y coordinate to be able to draw them in order :)
    struct vec_compare_y {
        bool operator()(const vec& v1, const vec& v2) { return v1.y < v2.y; }
    };

    HexCoords tl, br;
    std::unordered_map<HexCoords, TileData> terrain_map;
    std::multimap<vec, GameObject*, vec_compare_y> objects;
    TileMap terrain_tilemap;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(terrain_tilemap, states);
        for (auto& o : objects) {
            target.draw(*o.second, states);
        }
    }

  public:
    Cell(HexCoords tl = HexCoords::from_offset(0, 0), HexCoords br = HexCoords::from_offset(10, 10))
        : tl(tl), br(br) {
        // at cell creation, randomly initialize terrain
        for (int x = tl.get_offset().x; x < br.get_offset().x; x++) {
            for (int y = tl.get_offset().y; y < br.get_offset().y; y++) {
                // random tile among 7 + forest or not
                auto data = std::make_pair(rand() % 7, rand() % 2);
                terrain_map.insert(std::make_pair(HexCoords::from_offset(x, y), data));
            }
        }
        terrain_tilemap.load(terrain_map);
    }

    void add_object(vec coords, GameObject* object) { objects.insert(make_pair(coords, object)); }

    TileData get_terrain_at(const HexCoords& coords) const { return terrain_map.at(coords); }

    // tests hex rectangle (in offset coordinates) intersection
    bool intersects(HexCoords& tl2, HexCoords& br2) const {
        auto tl_o = tl.get_offset(), br_o = br.get_offset();
        auto tl2_o = tl2.get_offset(), br2_o = br2.get_offset();
        return tl2_o.x <= br_o.x and tl2_o.y <= br_o.y and tl_o.x <= br2_o.x and tl_o.y <= br2_o.y;
    }

    // TODO to and from file function

    // TODO transfer object to other Cell
};
