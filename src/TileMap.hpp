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

#include "TerrainMap.hpp"

/*
====================================================================================================
  ~*~ TileMap ~*~
==================================================================================================*/
class TileMap : public GameObject {
    sf::Texture tileset;
    sf::VertexArray array;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &tileset;
        target.draw(array, states);
    }

  public:
    TerrainMap* map;

    TileMap() { port("map", &TileMap::map); }

    void load(double w, const vector<HexCoords>& coords) {
        tileset.loadFromFile("png/alltiles.png");
        array.setPrimitiveType(sf::Quads);
        array.resize(coords.size() * 4);

        for (size_t i = 0; i < coords.size(); i++) {
            auto hex_coords = coords.at(i);
            sf::Vertex* quad = &array[i * 4];
            vec tile_dim{258, 193};
            vec tile_center{109, 88};
            vec hex_center = hex_coords.get_pixel(w);
            vec tl = hex_center - tile_center;
            vec br = tl + tile_dim;
            int tile_type = map->get(hex_coords);
            vec tex_tl = vec{0, tile_type * tile_dim.y};
            quad[0].position = tl;
            quad[1].position = vec{br.x, tl.y};
            quad[2].position = br;
            quad[3].position = vec{tl.x, br.y};
            quad[0].texCoords = tex_tl;
            quad[1].texCoords = tex_tl + vec{tile_dim.x, 0};
            quad[2].texCoords = tex_tl + tile_dim;
            quad[3].texCoords = tex_tl + vec{0, tile_dim.y};
        }
    }
};