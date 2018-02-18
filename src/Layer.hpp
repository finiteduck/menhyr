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

#include "ViewController.hpp"
#include "globals.hpp"
/*
  ====================================================================================================
  ~*~ Layer ~*~
  ==================================================================================================*/
class Layer : public sf::Drawable, public Component {
    vector<GameObject*> objects;
    ViewController* view;

  public:
    Layer() {
        port("view", &Layer::view);
        port("objects", &Layer::add_object);
    }

    void before_draw() {
        sort(objects.begin(), objects.end(), [](GameObject* ptr1, GameObject* ptr2) {
            return ptr1->getPosition().y < ptr2->getPosition().y;
        });
    }

    void add_object(GameObject* ptr) { objects.push_back(ptr); }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        view->set_main();  // HACK should be a port directly to a view
        for (auto& o : objects) {
            target.draw(*o, states);
        }
    }
};
