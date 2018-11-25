#pragma once

class Action {
 public:
  virtual ~Action() = default;

  virtual void OnActivated() = 0;
  virtual void OnDeactivated() = 0;
  virtual void Poll() {}
};