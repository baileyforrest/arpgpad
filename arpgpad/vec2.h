#pragma once

#include <cmath>

template <typename T>
class Vec2 {
 public:
  Vec2(const Vec2&) = default;
  Vec2(T x = 0, T y = 0) : x_(x), y_(y) {}
  Vec2<T>& operator=(const Vec2&) = default;

  T Magnitude() const { return std::sqrt(x_ * x_ + y_ * y_); }

  Vec2<T> Normal() const {
    T mag = Magnitude();
    return Vec2(x_ / mag, y_ / mag);
  }

  Vec2<T> Scale(T scale) const { return Vec2(x_ * scale, y_ * scale); }

  Vec2<T>& operator+=(const Vec2<T>& other) {
    x_ += other.x_;
    y_ += other.y_;
    return *this;
  }

  T x() const { return x_; }
  T& x() { return x_; }

  T y() const { return y_; }
  T& y() { return y_; }

 private:
  T x_;
  T y_;
};

template <typename T>
bool operator==(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

template <typename T>
bool operator!=(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
Vec2<T> operator+(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  return Vec2<T>(lhs.x() + rhs.x(), lhs.y() + rhs.y());
}

template <typename T>
Vec2<T> operator-(const Vec2<T>& lhs, const Vec2<T>& rhs) {
  return Vec2<T>(lhs.x() - rhs.x(), lhs.y() - rhs.y());
}

using FloatVec2 = Vec2<float>;
