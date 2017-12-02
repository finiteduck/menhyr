/*Copyright Vincent Lanore 2017

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
  ~*~ TerrainMap ~*~
==================================================================================================*/
class TerrainMap : public Component {
    using TileType = int;
    std::unordered_map<HexCoords, TileType> map;

  public:
    TileType get(const HexCoords& coords) {
        auto it = map.find(coords);
        if (it == map.end()) {
            map.insert(std::make_pair(coords, rand() % 7));
            return map.at(coords);
        } else {
            return it->second;
        }
    }

    void set(const HexCoords& coords, TileType type) { map[coords] = type; }
};
