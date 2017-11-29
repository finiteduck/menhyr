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

#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <math.h>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "tinycompo.hpp"

/*
====================================================================================================
  ~*~ using declarations ~*~
==================================================================================================*/
using namespace tc;

using std::vector;
using std::unique_ptr;

using vec = sf::Vector2f;
using ivec = sf::Vector2i;
using cube = sf::Vector3i;
using scalar = float;

/*
====================================================================================================
  ~*~ virtual interfaces ~*~
==================================================================================================*/
struct GameObject : public sf::Drawable, public sf::Transformable, public tc::Component {
    virtual void animate(scalar) {}
};

/*
====================================================================================================
  ~*~ vector operators ~*~
==================================================================================================*/
vec operator/(vec v, scalar s) { return vec(v.x / s, v.y / s); }

#endif  // GLOBALS_HPP
