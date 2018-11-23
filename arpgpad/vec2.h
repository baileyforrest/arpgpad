#pragma once

template <typename T>
class Vec2 {
 public:
  Vec2() = default;
  Vec2(T x, T y) : x_(x), y_(y) {}

  T x() const { return x_; }
  T y() const { return y_; }

  void set_x(T x) { x_ = x; }
  void set_y(T y) { y_ = y; }

 private:
  T x_ = 0;
  T y_ = 0;
};

using FloatVec2 = Vec2<float>;
