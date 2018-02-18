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

#include "Window.hpp"
#include "globals.hpp"

/*
====================================================================================================
  ~*~ View ~*~
==================================================================================================*/
class View : public Component {
    sf::View view;
    Window* window;
    bool is_default;

  public:
    View(bool is_default = false) : is_default(is_default) { port("window", &View::window); }

    sf::View& get() { return view; }

    void set(sf::View new_view) {
        if (!is_default)
            view = new_view;
        else {
            view = window->get().getDefaultView();
        }
    }

    void use() {
        if (!is_default)
            window->set_view(view);
        else {
            vec wdim = vec(window->width, window->height);
            view.setSize(wdim);
            view.setCenter(wdim / 2);
            window->set_view(view);
        }
    }

    vec get_size() { return view.getSize(); }
};
