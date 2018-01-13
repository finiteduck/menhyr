#pragma once

#include "tinycompo.hpp"

struct GameObject;  // forward decl

template <class ElemType>
struct UseObjectVector {
    static void _connect(tc::Assembly& assembly, tc::PortAddress user, tc::PortAddress provider) {
        auto& ref_user = assembly.at(user.address);
        auto& ref_provider = assembly.at(provider.address);
        std::vector<std::unique_ptr<ElemType>>& v_ref =
            *ref_provider.template get<std::vector<std::unique_ptr<ElemType>>>(provider.prop);
        for (auto& e : v_ref) {
            ref_user.set(user.prop, dynamic_cast<GameObject*>(e.get()));
        }
    }
};
