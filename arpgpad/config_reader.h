#pragma once

#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>
#include <string>

#include "config.h"

class ConfigReader {
 public:
  static std::optional<Config> ParseConfig(std::istream* config);

  explicit ConfigReader(const std::filesystem::path& directory);
  ~ConfigReader();

  bool Reload();

  const std::map<std::string, Config>& name_to_config() const {
    return name_to_config_;
  }

 private:

  const std::filesystem::path directory_;
  std::map<std::string, Config> name_to_config_;
};
