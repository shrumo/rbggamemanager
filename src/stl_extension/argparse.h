//
// Created by shrum on 21/09/19.
//

#ifndef RBGGAMEMANAGER_ARGPARSE_H
#define RBGGAMEMANAGER_ARGPARSE_H

#include <string>
#include <unordered_map>

namespace std_ext {
struct args {
  std::vector<std::string> positional_args;
  std::unordered_map<std::string, std::string> flags;
};

args parse_args(int argc, const char *argv[]) {
  std::vector<std::string> positional_args;
  std::unordered_map<std::string, std::string> flags;
  std::string flag;
  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (arg[0] == '-') {
      while (arg[0] == '-') arg++;
      flag = std::string(arg);
    } else {
      if (flag.length() > 0) {
        flags[flag] = std::string(arg);
        flag = "";
      } else {
        positional_args.emplace_back(arg);
      }
    }
  }
  return {positional_args, flags};
}
}  // namespace std_ext

#endif  // RBGGAMEMANAGER_ARGPARSE_H
