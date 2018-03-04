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
    unique_ptr<Appearance> appearance;  // drawable should be constructible from state
    State state;                        // state should provide stream operators

  public:
    GameEntity(State state) : state(state) {}

    void enable_drawable() { appearance = make_unique(state); }
    void disable_drawable() { appearance.reset(nullptr); }

    template <class... Args>
    void state_update(Args&&... args) {
        state.update(std::forward<Args>(args)...);
    }

    template <class... Args>
    void appearance_update(Args&&... args) {
        appearance.update(std::forward<Args>(args)...);
    }

    template <class... Args>
    void update(Args... args) {
        state.update(args...);
        appearance.update(args...);
    }

    // State& get_state() { return state; }
    // Appearance& get_appearance() { return appearance; }
};

/*
====================================================================================================
  ~*~ Stream operators ~*~
==================================================================================================*/
template <class State, class Appearance>
std::ostream& operator<<(std::ostream& os, const GameEntity<State, Appearance>& ge) {
    os << ge.state;
    return os;
}

template <class State, class Appearance>
std::istream& operator>>(std::istream& is, GameEntity<State, Appearance>& ge) {
    is >> ge.state;
    return is;
}
