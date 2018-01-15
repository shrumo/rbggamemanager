//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_NAME_RESOLVER_H
#define RBGGAMEMANAGER_NAME_RESOLVER_H

#include <vector>
#include <unordered_map>

using token_id_t = std::size_t;

class name_resolver {
    std::vector<std::string> names;
    std::unordered_map<std::string, token_id_t> names_ids;
public:
    name_resolver() = default;

    void add_name(const std::string& name)
    {
        names.push_back(name);
        names_ids[name] = names.size() - 1;
    }

    token_id_t id(const std::string& name) const
    {
        return names_ids.at(name);
    }

    const std::string& name(token_id_t id) const
    {
        return names[id];
    }

    size_t names_count() const
    {
        return names.size();
    }
};


#endif //RBGGAMEMANAGER_NAME_RESOLVER_H
