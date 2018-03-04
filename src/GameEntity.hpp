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
  ~*~ GameEntity ~*~
  Generic game object with well-separated sub-objects implementing the following concerns:
  drawable representation, game state, game state update, drawable update.
==================================================================================================*/
template <class State, class Appearance>
class GameEntity {
    unique_ptr<Appearance> appearance;  // should provide Appearance(State&), update(), layer(str)
    State state;                        // should provide stream operators, update(...)

    friend std::ostream& operator<<(std::ostream& os, const GameEntity<State, Appearance>& ge) {
        os << ge.state;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, GameEntity<State, Appearance>& ge) {
        is >> ge.state;
        ge.appearance->update();
        return is;
    }

  public:
    GameEntity(State state) : state(state) {}

    // drawable part can be disabled at anytime to free memory
    // (TODO: should appearance presence be checked when used?)
    void enable_appearance() { appearance = make_unique<Appearance>(state); }
    void disable_appearance() { appearance.reset(nullptr); }

    template <class... Args>
    void update(Args&&... args) {
        state.update(std::forward<Args>(args)...);
        appearance->update();
    }

    template <class RenderTarget>
    void draw(RenderTarget& t, const string& layer = "") {  // TODO: separate overloads?
        if (layer == "") {
            t.draw(*appearance);
        } else {
            t.draw(appearance->layer(layer));
        }
    }

    // State& get_state() { return state; }
    // Appearance& get_appearance() { return appearance; }
};

/*
====================================================================================================
  ~*~ Stream operators ~*~
==================================================================================================*/
