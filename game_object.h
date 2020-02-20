#pragma once

#include "geo2d.h"

class Unit;
class Building;
class Tower;
class Fence;

struct GameObject {
  virtual ~GameObject() = default;

  virtual bool Collide(const GameObject &that) const = 0;
  virtual bool CollideWith(const Unit &that) const = 0;
  virtual bool CollideWith(const Building &that) const = 0;
  virtual bool CollideWith(const Tower &that) const = 0;
  virtual bool CollideWith(const Fence &that) const = 0;
};

class Unit : public GameObject {
public:
  explicit Unit(geo2d::Point position) : _position(position) {}
  virtual bool Collide(const GameObject &that) const override;
  virtual bool CollideWith(const Unit &that) const override;
  virtual bool CollideWith(const Building &that) const override;
  virtual bool CollideWith(const Tower &that) const override;
  virtual bool CollideWith(const Fence &that) const override;

  const geo2d::Point &GetPoint() const { return _position; }

private:
  geo2d::Point _position;
};

class Building : public GameObject {
public:
  explicit Building(geo2d::Rectangle geometry) : _geometry(geometry) {}
  virtual bool Collide(const GameObject &that) const override;
  virtual bool CollideWith(const Unit &that) const override;
  virtual bool CollideWith(const Building &that) const override;
  virtual bool CollideWith(const Tower &that) const override;
  virtual bool CollideWith(const Fence &that) const override;

  const geo2d::Rectangle &GetRectangle() const { return _geometry; }

private:
  geo2d::Rectangle _geometry;
};

class Tower : public GameObject {
public:
  explicit Tower(geo2d::Circle geometry) : _geometry(geometry) {}
  virtual bool Collide(const GameObject &that) const override;
  virtual bool CollideWith(const Unit &that) const override;
  virtual bool CollideWith(const Building &that) const override;
  virtual bool CollideWith(const Tower &that) const override;
  virtual bool CollideWith(const Fence &that) const override;
  const geo2d::Circle &GetCircle() const { return _geometry; }

private:
  geo2d::Circle _geometry;
};

class Fence : public GameObject {
public:
  explicit Fence(geo2d::Segment geometry) : _geometry(geometry) {}
  virtual bool Collide(const GameObject &that) const override;
  virtual bool CollideWith(const Unit &that) const override;
  virtual bool CollideWith(const Building &that) const override;
  virtual bool CollideWith(const Tower &that) const override;
  virtual bool CollideWith(const Fence &that) const override;
  const geo2d::Segment &GetSegment() const { return _geometry; }

private:
  geo2d::Segment _geometry;
};

bool Collide(const GameObject &first, const GameObject &second);
