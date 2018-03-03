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

#include "Cell.hpp"

/*
====================================================================================================
  ~*~ CellGrid ~*~
==================================================================================================*/
class CellGrid : public GameObject, public Component {
    int cell_size{20};

    struct ivec_compare_y {
        bool operator()(const ivec& v1, const ivec& v2) {
            return v1.y == v2.y ? v1.x < v2.x : v1.y < v2.y;
        }
    };

    std::map<ivec, Cell, ivec_compare_y> cells;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        for (auto& cell : cells) {  // sorted by increasing y
            target.draw(cell.second, states);
        }
    }

  public:
    void add_cell(ivec coords) {
        if (cells.find(coords) == cells.end()) {
            auto tl = coords * cell_size;
            auto unit = ivec(1, 1);
            cells[coords] =
                Cell(HexCoords::from_offset(tl), HexCoords::from_offset(tl + cell_size * unit));
        }
    }

    void remove_cell(ivec coords) { cells.erase(coords); }
};
