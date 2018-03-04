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
#include "GameEntity.hpp"
#include "HexCoords.hpp"
#include "SimpleObject.hpp"
#include "TileMap.hpp"

/*
====================================================================================================
  ~*~ Cell ~*~
  A cell is a fairly large square of the world. It's atomic in terms of rendering and loading into
  memory. It contains map info and references to objects.
==================================================================================================*/
class CellState {
    std::unordered_map<HexCoords, TileData> terrain_map;

  public:
    CellState(HexCoords tl = HexCoords::from_offset(0, 0),
              HexCoords br = HexCoords::from_offset(10, 10)) {
        // at cell creation, randomly initialize terrain
        for (int x = tl.get_offset().x; x < br.get_offset().x; x++) {
            for (int y = tl.get_offset().y; y < br.get_offset().y; y++) {
                // random tile among 7 + forest or not
                auto coords = HexCoords::from_offset(x, y);
                auto data = std::make_pair(rand() % 7, rand() % 2);
                terrain_map.insert(std::make_pair(coords, data));
            }
        }
    }

    std::unordered_map<HexCoords, TileData>& get_map() { return terrain_map; }

    // TileData get_terrain_at(const HexCoords& coords) const { return terrain_map.at(coords); }
};

class CellAppearance : public GameObject {
    // storing objects by y coordinate to be able to draw them in order :)
    struct vec_compare_y {
        bool operator()(const vec& v1, const vec& v2) { return v1.y < v2.y; }
    };

    CellState& state;
    std::multimap<vec, GameObject*, vec_compare_y> objects;
    TileMap terrain_tilemap;
    std::list<SimpleObject> trees;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(terrain_tilemap, states);
        for (auto& o : objects) {
            target.draw(*o.second, states);
        }
    }

  public:
    CellAppearance(CellState& state) : state(state) { update(); }

    void update() {
        trees.clear();
        objects.clear();
        for (auto& hex : state.get_map()) {
            auto data = std::make_pair(rand() % 7, rand() % 2);
            if (data.second == 0) {  // in case of forest, add tree
                trees.emplace_back(144, "png/tree1.png", hex.first, 0.5);  // TODO : w
                objects.insert(make_pair(trees.back().getOrigin(), &trees.back()));
            }
        }
        terrain_tilemap.load(state.get_map());
    }
};

using Cell = GameEntity<CellState, CellAppearance>;
