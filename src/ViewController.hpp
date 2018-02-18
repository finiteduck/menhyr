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

#include "HexCoords.hpp"
#include "View.hpp"
#include "Window.hpp"
#include "globals.hpp"

/*
====================================================================================================
  ~*~ ViewController ~*~
==================================================================================================*/
class ViewController : public Component {
    bool mouse_pressed{false};
    int mouse_x{0}, mouse_y{0};
    View *main_view, *interface_view;
    HexCoords ctl, cbr;

    Window* window;

  public:
    ViewController() {
        port("interfaceview", &ViewController::interface_view);
        port("mainview", &ViewController::main_view);
        port("window", &ViewController::window);
    }

    void init() {
        main_view->set(window->get().getDefaultView());
        main_view->get().move(-300, -300);
        interface_view->set(window->get().getDefaultView());
    }

    bool process_event(sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed and
            event.mouseButton.button == sf::Mouse::Middle) {
            mouse_pressed = true;
            mouse_x = event.mouseButton.x;
            mouse_y = event.mouseButton.y;

        } else if (event.type == sf::Event::MouseButtonReleased and
                   event.mouseButton.button == sf::Mouse::Middle) {
            mouse_pressed = false;

        } else if (event.type == sf::Event::MouseWheelScrolled) {
            scalar zoom = 1 - (event.mouseWheelScroll.delta * 0.15);
            main_view->get().zoom(zoom);

        } else if (event.type == sf::Event::Resized) {
            scalar zoom = main_view->get().getSize().x / window->width;
            vec event_size(event.size.width, event.size.height);
            vec new_size = zoom * event_size;
            main_view->get().setSize(new_size);
            window->width = event.size.width;
            window->height = event.size.height;

        } else {
            return false;
        }
        return true;
    }

    sf::RenderWindow& get_draw_ref() { return window->get(); }

    bool update(scalar w) {
        if (mouse_pressed) {
            auto mouse_pos = window->get_mouse_position();
            scalar zoom = main_view->get().getSize().x / window->width;
            main_view->get().move((mouse_x - mouse_pos.x) * zoom, (mouse_y - mouse_pos.y) * zoom);
            mouse_x = mouse_pos.x;
            mouse_y = mouse_pos.y;
        }
        vec dim = main_view->get().getSize();
        vec center = main_view->get().getCenter();
        vec tl = center - dim / 2;
        vec br = tl + dim;
        return !(HexCoords::from_pixel(w, tl) == ctl) or !(HexCoords::from_pixel(w, br) == cbr);
    }

    vec get_mouse_position() {
        return window->get().mapPixelToCoords(window->get_mouse_position());
    }

    vec get_window_size() { return vec(window->width, window->height); }

    vector<HexCoords> get_visible_coords(int w) {
        // gather relevant view coordinates
        vec dim = main_view->get().getSize();
        vec center = main_view->get().getCenter();
        vec tl = center - dim / 2;
        vec br = tl + dim;

        vector<HexCoords> result;

        ctl = HexCoords::from_pixel(w, tl);
        cbr = HexCoords::from_pixel(w, br);
        for (int i = ctl.get_offset().x - 1; i <= cbr.get_offset().x + 1; i++) {
            for (int j = ctl.get_offset().y - 1; j <= cbr.get_offset().y + 1; j++) {
                result.push_back(HexCoords::from_offset(i, j));
            }
        }
        return result;
    }
};
