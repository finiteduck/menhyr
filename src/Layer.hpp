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

#include "Trees.hpp"  // TODO replace by objectprovider file
#include "ViewController.hpp"
#include "globals.hpp"

/*
====================================================================================================
  ~*~ Layer ~*~
==================================================================================================*/
class Layer : public sf::Drawable, public Component {
    vector<GameObject*> objects;
    vector<GameObject*> sorted_objects;
    vector<ObjectProvider*> object_providers;
    View* view;

  public:
    Layer() {
        port("view", &Layer::view);
        port("objects", &Layer::add_object);
        port("providers", &Layer::add_provider);
    }

    void before_draw() {
        sorted_objects.clear();
        sorted_objects.insert(sorted_objects.end(), objects.begin(), objects.end());
        for (auto& provider : object_providers) {
            auto provided_objects = provider->get_refs();
            sorted_objects.insert(sorted_objects.end(), provided_objects.begin(),
                                  provided_objects.end());
        }
        sort(sorted_objects.begin(), sorted_objects.end(), [](GameObject* ptr1, GameObject* ptr2) {
            return ptr1->getPosition().y < ptr2->getPosition().y;
        });
    }

    void set_view() { view->use(); }

    void add_object(GameObject* ptr) { objects.push_back(ptr); }

    void add_provider(ObjectProvider* provider) { object_providers.push_back(provider); }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        for (auto& o : sorted_objects) {
            target.draw(*o, states);
        }
    }
};
