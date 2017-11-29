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
