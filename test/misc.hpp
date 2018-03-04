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
  ~*~ GameEntity ~*~
==================================================================================================*/
using namespace std;

struct DummyRender {
    ostream& os;
    void draw(string s) { os << s; }
};

struct DummyState {
    friend ostream& operator<<(ostream& os, const DummyState& ds) {
        os << ds.data;
        return os;
    }
    friend istream& operator>>(istream& is, DummyState& ds) {
        is >> ds.data;
        return is;
    }
    DummyState(int i) : data(i) {}
    int data;
    void update(int i) { data += i; }
};

struct DummyAppearance : public string {
    DummyState& state;
    void update() { assign(to_string(state.data)); }
    DummyAppearance(DummyState& state) : state(state) { update(); }
    string layer(string layer) { return layer + ":" + *this; }
};

TEST_CASE("Basic GameEntity test.") {
    stringstream ss;
    GameEntity<DummyState, DummyAppearance> entity(17);
    DummyRender render{ss};

    entity.draw(render);
    CHECK(ss.str() == "17");
    ss.str("");

    entity.update(3);
    entity.draw(render, "mylayer");
    CHECK(ss.str() == "mylayer:20");
    ss.str("");

    ss << entity;
    CHECK(ss.str() == "20");
    ss.str("");

    stringstream ss2{"119"};
    ss2 >> entity;
    entity.draw(render, "other");
    CHECK(ss.str() == "other:119");
}
