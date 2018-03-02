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

#include "globals.hpp"

/*
====================================================================================================
  ~*~ HexCoords class ~*~
==================================================================================================*/
class HexCoords {
    int x{0}, y{0}, z{0};
    friend std::hash<HexCoords>;

  public:
    HexCoords() = default;
    HexCoords(const HexCoords&) = default;
    HexCoords(int x, int y, int z) : x(x), y(y), z(z) {}
    HexCoords(cube c) : x(c.x), y(c.y), z(c.z) {}

    ivec get_axial() const { return ivec(x, y); }
    cube get_cube() const { return cube(x, y, z); }
    vec get_pixel(int w) const { return vec((x + scalar(y) / 2) * w, scalar(y) * w * sqrt(3) / 2); }
    ivec get_offset() const { return ivec(get_axial().x + (get_axial().y >> 1), get_axial().y); }

    static HexCoords from_axial(ivec v) { return HexCoords(v.x, v.y, -v.x - v.y); }
    static HexCoords from_axial(int x, int y) { return HexCoords(x, y, -x - y); }
    static HexCoords from_offset(int x, int y) { return HexCoords::from_axial(x - (y >> 1), y); }
    static HexCoords from_offset(ivec v) { return HexCoords::from_offset(v.x, v.y); }
    static HexCoords from_cube(cube c) { return HexCoords(c); }
    static HexCoords from_cube(int x, int y, int z) { return HexCoords(x, y, z); }
    static HexCoords from_pixel(scalar w, vec v) { return from_pixel(w, v.x, v.y); }
    static HexCoords from_pixel(scalar w, scalar x, scalar y) {
        scalar fx((x - y / sqrt(3)) / w), fy(y * 2 / (sqrt(3) * w)), fz(-fx - fy);
        scalar rx(round(fx)), ry(round(fy)), rz(round(fz));
        scalar dx(abs(fx - rx)), dy(abs(fy - ry)), dz(abs(fz - rz));
        if (dx > dy and dx > dz) {
            return HexCoords(-ry - rz, ry, rz);
        } else if (dy > dz) {
            return HexCoords(rx, -rx - rz, rz);
        } else {
            return HexCoords(rx, ry, -rx - ry);
        }
    }

    vec random_pixel(scalar w, scalar tuning = 1.0) const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        scalar aw = sqrt(3) * w / 4.0;  // adjusted w
        scalar r = dis(gen) * aw * aw;
        scalar theta = dis(gen) * 2 * M_PI;
        vec center = get_pixel(w);
        return center + vec(tuning * sqrt(r) * cos(theta), tuning * sqrt(r) * sin(theta));
    }

    bool operator==(const HexCoords& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

/*
====================================================================================================
  ~*~ stream operator ~*~
==================================================================================================*/
std::ostream& operator<<(std::ostream& os, HexCoords c) {
    os << '(' << c.get_axial().x << ", " << c.get_axial().y << ')';
    return os;
}

/*
====================================================================================================
  ~*~ hash ~*~
==================================================================================================*/
namespace std {
    template <>
    struct hash<HexCoords> {
        size_t operator()(const HexCoords& key) const {
            size_t result = 17;
            result = result * 79 + hash<int>()(key.x);
            result = result * 79 + hash<int>()(key.y);
            result = result * 79 + hash<int>()(key.z);
            return result;
        }
    };
}  // namespace std
