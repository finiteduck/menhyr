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

#include "../src/GameEntity.hpp"
#include "doctest.h"

/*
====================================================================================================
  ~*~ TESTS ~*~
==================================================================================================*/
using namespace std;

struct DummyRender {
    ostream& os;
    void draw(string s) { os << s; }
};

struct DummyState {
    int data;
    void update(int i) { data += i; }
};

inline ostream& operator<<(ostream& os, DummyState& ds) {
    os << ds.data;
    return os;
}

struct DummyAppearance : public string {
    DummyAppearance(DummyState state) { assign(to_string(state.data)); }
    void update() { push_back('-'); }
    void update(int) { push_back('x'); }
    string layer(string layer) { return layer + ":" + *this; }
};

TEST_CASE("Basic GameEntity test.") {
    stringstream ss;
    GameEntity<DummyState, DummyAppearance> entity(DummyState{17});
    entity.enable_appearance();
    DummyRender render{ss};

    entity.draw(render);
    CHECK(ss.str() == "17");
    ss.str("");

    entity.update(3);
    entity.draw(render, "mylayer");
    CHECK(ss.str() == "mylayer:17x");
}
