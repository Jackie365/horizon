#pragma once
#include "common/common.hpp"
#include "board/board_junction.hpp"
#include "nlohmann/json_fwd.hpp"
#include "pool/padstack.hpp"
#include "util/uuid.hpp"
#include "util/uuid_ptr.hpp"

namespace horizon {
using json = nlohmann::json;

class Via {
public:
    Via(const UUID &uu, const json &j, class IPool &pool, class Board *brd = nullptr);
    Via(const UUID &uu, const Padstack *ps);
    Via(shallow_copy_t sh, const Via &other);

    UUID uuid;

    uuid_ptr<Net> net_set = nullptr;
    uuid_ptr<BoardJunction> junction = nullptr;
    uuid_ptr<const Padstack> pool_padstack;
    Padstack padstack;
    void expand(const class Board &brd);

    ParameterSet parameter_set;

    bool from_rules = true;
    bool locked = false;

    json serialize() const;
};
} // namespace horizon
