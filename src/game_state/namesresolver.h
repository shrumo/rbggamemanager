//
// Created by shrum on 09.06.19.
//

#ifndef RBGGAMEMANAGER_NAMESRESOLVER_H
#define RBGGAMEMANAGER_NAMESRESOLVER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>

namespace rbg {
  using name_id_t = uint;

  class NamesResolver {
  public:
    NamesResolver() = default;

    name_id_t Id(const std::string &name) {
      auto name_iterator = names_ids_.find(name);
      if (name_iterator != names_ids_.end()) {
        return name_iterator->second;
      }
      names_.push_back(name);
      names_ids_[name] = names_.size() - 1;
      return names_.size() - 1;
    }

    name_id_t Id(const std::string &name) const {
      return names_ids_.at(name);
    }

    const std::string &Name(name_id_t name) const {
      return names_[name];
    }

    size_t size() const {
      return names_.size();
    }

    bool contains(const std::string &name) const {
      return names_ids_.find(name) != names_ids_.end();
    }

  private:
    std::vector<std::string> names_;
    std::unordered_map<std::string, name_id_t> names_ids_;
  };
}

std::ostream &operator<<(std::ostream &o, const rbg::NamesResolver &res);

#endif //RBGGAMEMANAGER_NAMESRESOLVER_H
