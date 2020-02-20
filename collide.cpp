#include "game_object.h"

bool Collide(const GameObject &first, const GameObject &second) {
  return first.Collide(second);
}

bool Unit::Collide(const GameObject &that) const {
  return this->CollideWith(that);
}

bool Unit::CollideWith(const Unit &that) const {
  return geo2d::Collide(this->_position, that.GetPoint());
}
bool Unit::CollideWith(const Building &that) const {
  return geo2d::Collide(this->_position, that.GetRectangle());
}
bool Unit::CollideWith(const Tower &that) const {
  return geo2d::Collide(this->_position, that.GetCircle());
}
bool Unit::CollideWith(const Fence &that) const {
  return geo2d::Collide(this->_position, that.GetSegment());
}