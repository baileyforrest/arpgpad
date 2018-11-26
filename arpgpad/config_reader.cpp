#include "stdafx.h"

#include "config_reader.h"

#include <fstream>

#include "log.h"
#include "third_party/cpptoml/include/cpptoml.h"

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
std::optional<Config> ConfigReader::ParseConfig(std::istream* stream) {
  cpptoml::parser parser(*stream);
  std::shared_ptr<cpptoml::table> parsed;
  try {
    parsed = parser.parse();
  } catch (cpptoml::parse_exception& e) {
    LOG(ERR) << "Failed to parse config as TOML: '" << e.what() << "'";
    return std::nullopt;
  }

  Config ret;

  auto name = parsed->get_as<std::string>("name");
  if (!name) {
    LOG(ERR) << "Missing field: name";
    return std::nullopt;
  }
  ret.name = *name;

  auto move_radius_fraction = parsed->get_as<double>("move_radius_fraction");
  if (!move_radius_fraction) {
    LOG(ERR) << "Missing field: move_radius_fraction";
    return std::nullopt;
  }
  if (*move_radius_fraction <= 0.0f || *move_radius_fraction > 0.4f) {
    LOG(ERR) << "Out of range: move_radius_fraction";
    return std::nullopt;
  }
  ret.move_radius_fraction = static_cast<float>(*move_radius_fraction);

  auto middle_offset_fraction =
      parsed->get_as<double>("middle_offset_fraction");
  if (!middle_offset_fraction) {
    LOG(ERR) << "Missing field: middle_offset_fraction";
    return std::nullopt;
  }
  if (*middle_offset_fraction < 0.2f || *middle_offset_fraction > 0.8f) {
    LOG(ERR) << "Out of range: middle_offset_fraction";
    return std::nullopt;
  }
  ret.middle_offset_fraction = static_cast<float>(*middle_offset_fraction);

  auto mouse_position_delay_ms = parsed->get_as<int>("mouse_position_delay_ms");
  if (!mouse_position_delay_ms) {
    LOG(ERR) << "Missing field: mouse_position_delay_ms";
    return std::nullopt;
  }
  if (*mouse_position_delay_ms < 0) {
    LOG(ERR) << "Out of range: mouse_position_delay_ms";
    return std::nullopt;
  }
  ret.mouse_position_delay_ms = static_cast<int>(*mouse_position_delay_ms);

  using BA = Config::ButtonAction;
  auto parse_button_action =
      [](const cpptoml::table& table) -> std::optional<BA> {
    BA ba;
    auto type = table.get_as<std::string>("type");
    if (!type) {
      LOG(ERR) << "Missing field: type";
      return std::nullopt;
    }
    auto code = table.get_as<std::string>("code");
    if (!code) {
      LOG(ERR) << "Missing field: code";
      return std::nullopt;
    }

    if (*type == "MOUSE") {
      ba.type = BA::Type::kMouse;
      auto button = Mouse::StringToButton(*code);
      if (!button) {
        LOG(ERR) << "Failed to parse button: " << *code;
        return std::nullopt;
      }
      LOG(ERR) << "FOO " << *button << " " << *code;
      ba.code.mouse_button = *button;
    } else if (*type == "KEY") {
      ba.type = BA::Type::kKeyboard;
      auto key = Keyboard::ParseKeyCode(*code);
      if (!key) {
        LOG(ERR) << "Failed to parse key: " << *code;
        return std::nullopt;
      }
      ba.code.key_code = *key;
    } else {
      LOG(ERR) << "Unknown type: " << *type;
      return std::nullopt;
    }

    if (auto no_move = table.get_as<bool>("no_move")) {
      ba.no_move = *no_move;
    }

    if (auto distance_fraction = table.get_as<double>("distance_fraction")) {
      if (*distance_fraction <= 0.0f || *distance_fraction >= 1.0f) {
        LOG(ERR) << "Invalid value for distance_fraction: "
                 << *distance_fraction;
        return std::nullopt;
      }
      ba.distance_fraction = static_cast<float>(*distance_fraction);
    }

    return ba;
  };

  auto move_command = parsed->get_table("move_command");
  if (!move_command) {
    LOG(ERR) << "Missing field: move_command";
    return std::nullopt;
  }

  auto move_ba = parse_button_action(*move_command);
  if (!move_ba) {
    return std::nullopt;  // Error logged.
  }

  auto button_to_action = parsed->get_table_array("button_to_action");
  if (!button_to_action) {
    LOG(ERR) << "Missing field: button_to_action";
    return std::nullopt;
  }

  for (const auto& table : *button_to_action) {
    auto button = table->get_as<std::string>("button");
    if (!button) {
      LOG(ERR) << "Missing field: button";
      return std::nullopt;
    }

    auto parsed_button = Controller::StringToButton(*button);
    if (!parsed_button) {
      LOG(ERR) << "Failed to parse button: " << *button;
    }

    auto action = table->get_table("action");
    if (!action) {
      LOG(ERR) << "Missing field: action";
      return std::nullopt;
    }

    auto ba = parse_button_action(*action);
    if (!ba) {
      return std::nullopt;  // Error logged.
    }

    ret.button_to_action.emplace(*parsed_button, *ba);
  }

  return ret;
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
      continue;
    }

    LOG(INFO) << "Loaded config: " << config->name;

    auto name_copy = config->name;
    name_to_config_.emplace(std::move(name_copy), std::move(*config));
  }

  return true;
}