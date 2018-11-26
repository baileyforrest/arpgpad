#include "stdafx.h"

#include "config_reader.h"

#include <fstream>

#include "log.h"

namespace {

// TODO: Don't hard code these
Config PathOfExileConfig() {
  Config config;
  config.name = "poe_rf_rugg";
  config.move_radius_fraction = 0.15f;
  config.middle_offset_fraction = 0.44f;
  config.mouse_position_delay_ms = 10;

  using BA = Config::ButtonAction;
  config.move_command = BA::MouseAction(Mouse::kButtonLeft);

  config.button_to_action.emplace(Controller::kButtonBack,
                                  BA::KeyAction(VK_ESCAPE));

  // Flasks
  config.button_to_action.emplace(Controller::kButtonDpadUp,
                                  BA::KeyAction('1'));
  config.button_to_action.emplace(Controller::kButtonDpadRight,
                                  BA::KeyAction('2'));
  config.button_to_action.emplace(Controller::kButtonDpadDown,
                                  BA::KeyAction('3'));
  config.button_to_action.emplace(Controller::kButtonDpadLeft,
                                  BA::KeyAction('4'));
  config.button_to_action.emplace(Controller::kButtonLStick,
                                  BA::KeyAction('5'));

  // Mouse
  config.button_to_action.emplace(Controller::kButtonA,
                                  BA::MouseAction(Mouse::kButtonRight));
  config.button_to_action.emplace(Controller::kButtonX,
                                  BA::MouseAction(Mouse::kButtonMiddle, true));

  // QWERT
  config.button_to_action.emplace(Controller::kButtonStart, BA::KeyAction('Q'));
  config.button_to_action.emplace(Controller::kButtonLb, BA::KeyAction('W'));
  config.button_to_action.emplace(Controller::kButtonRb, BA::KeyAction('E'));

  // I have RT mapped to mouse forward/back buttons.
  config.button_to_action.emplace(
      Controller::kButtonY, BA::MouseAction(Mouse::kButtonX2));  // Forward
  config.button_to_action.emplace(
      Controller::kButtonB,
      BA::MouseAction(Mouse::kButtonX1, true, 0.48f));  // Back

  return config;
}

Config Diablo3Config() {
  Config config;
  config.move_radius_fraction = 0.15f;
  config.middle_offset_fraction = 0.44f;
  config.mouse_position_delay_ms = 10;

  using BA = Config::ButtonAction;

  config.move_command = BA::KeyAction('5');

  config.button_to_action.emplace(Controller::kButtonBack,
                                  BA::KeyAction(VK_ESCAPE));
  config.button_to_action.emplace(Controller::kButtonStart,
                                  BA::KeyAction(VK_TAB));

  // Mouse
  config.button_to_action.emplace(Controller::kButtonA,
                                  BA::MouseAction(Mouse::kButtonLeft, true));
  config.button_to_action.emplace(Controller::kButtonX,
                                  BA::MouseAction(Mouse::kButtonRight));

  // Attacks
  config.button_to_action.emplace(Controller::kButtonB, BA::KeyAction('3'));
  config.button_to_action.emplace(Controller::kButtonY, BA::KeyAction('2'));
  config.button_to_action.emplace(Controller::kButtonRb, BA::KeyAction('4'));

  config.button_to_action.emplace(Controller::kButtonDpadUp,
                                  BA::KeyAction('Q'));
  config.button_to_action.emplace(Controller::kButtonDpadRight,
                                  BA::KeyAction('1'));

  return config;
}

}  // namespace

// static
std::optional<Config> ConfigReader::ParseConfig(std::istream* config) {
  return PathOfExileConfig();
}

ConfigReader::ConfigReader(const std::filesystem::path& directory)
    : directory_(directory) {
  Reload();
}

ConfigReader::~ConfigReader() {}

bool ConfigReader::Reload() {
  LOG(INFO) << "Loading configs from " << directory_;

  if (!std::filesystem::is_directory(directory_)) {
    LOG(ERR) << "Not a directory: " << directory_;
    return false;
  }
  name_to_config_.clear();

  for (auto& entry : std::filesystem::directory_iterator(directory_)) {
    auto& path = entry.path();
    std::ifstream ifs(path);
    if (!ifs) {
      LOG(INFO) << "Could not open " << path << ": " << GetLastError();
      continue;
    }

    auto config = ParseConfig(&ifs);
    if (!config) {
      LOG(INFO) << "Could not parse config " << path;
    }

    LOG(INFO) << "Loaded config: " << config->name;

    auto name_copy = config->name;
    name_to_config_.emplace(std::move(name_copy), std::move(*config));
  }

  return true;
}