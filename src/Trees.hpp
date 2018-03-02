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
#include "TerrainMap.hpp"

/*
====================================================================================================
  ~*~ ObjectProvider ~*~
==================================================================================================*/
class ObjectProvider : public tc::Component {
  public:
    virtual vector<GameObject*> get_refs() const = 0;
};

/*
====================================================================================================
  ~*~ DisplayUpdate ~*~
==================================================================================================*/
class DisplayUpdate {
  public:
    virtual void update(std::vector<HexCoords>& visible_coords) = 0;
};

/*
====================================================================================================
  ~*~ Trees ~*~
==================================================================================================*/
class Trees : public ObjectProvider, public DisplayUpdate {
    vector<unique_ptr<SimpleObject>> trees;
    TerrainMap* map;

  public:
    Trees() { port("map", &Trees::map); }

    void update(std::vector<HexCoords>& visible_coords) override {
        trees.clear();
        for (auto& coords : visible_coords) {
            if (map->get(coords).second == 0) {
                trees.emplace_back(make_unique<SimpleObject>(144, "png/tree1.png", coords));
            }
        }
    }

    vector<GameObject*> get_refs() const override {
        vector<GameObject*> result;
        for (auto& ptr : trees) {
            result.push_back(ptr.get());
        }
        return result;
    }
};
