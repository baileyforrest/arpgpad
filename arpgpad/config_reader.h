#pragma once

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "config.h"

class ConfigReader {
 public:
  static std::optional<Config> ParseConfig(std::istream* stream);

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
