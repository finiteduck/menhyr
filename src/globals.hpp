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

#include <math.h>
#include <SFML/Graphics.hpp>
#include <random>
#include <unordered_map>
#include "tinycompo.hpp"

// TODO TODO TODO separate into two headers, one with and one without sfml

/*
====================================================================================================
  ~*~ using declarations ~*~
==================================================================================================*/
using namespace tc;

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::make_pair;
using std::string;
using std::pair;

using vec = sf::Vector2f;
using ivec = sf::Vector2i;
using cube = sf::Vector3i;
using scalar = float;

using sf::Drawable;

/*
====================================================================================================
  ~*~ virtual interfaces ~*~
==================================================================================================*/
struct GameObject : public Drawable, public sf::Transformable {
    virtual void animate(scalar) {}
};

/*
====================================================================================================
  ~*~ vector operators ~*~
==================================================================================================*/
vec operator/(vec v, scalar s) { return vec(v.x / s, v.y / s); }
vec operator*(vec v, scalar s) { return vec(v.x * s, v.y * s); }
vec operator*(scalar s, vec v) { return vec(v.x * s, v.y * s); }
