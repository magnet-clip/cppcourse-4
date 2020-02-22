#include "game_object.h"
#include "geo2d.h"

#include "test_runner.h"

#include <memory>
#include <vector>

using namespace std;

#include "game_object.h"
#include "geo2d.h"

template <class T>
struct Collider : public GameObject {
  bool Collide(const GameObject &that) const override final {
    return that.CollideWith(static_cast<const T &>(*this));
  }
};

#define COLLIDE_WITH_DEF(T) \
  virtual bool CollideWith(const T &that) const override

#define COLLIDE_WITH_IMPL(Me, T)                                \
  bool Me::CollideWith(const T &that) const {                   \
    return geo2d::Collide(this->_geometry, that.GetGeometry()); \
  }

class Unit final : public Collider<Unit> {
 public:
  explicit Unit(geo2d::Point geometry) : _geometry(geometry) {}

  COLLIDE_WITH_DEF(Unit);
  COLLIDE_WITH_DEF(Building);
  COLLIDE_WITH_DEF(Tower);
  COLLIDE_WITH_DEF(Fence);

  const geo2d::Point &GetGeometry() const { return _geometry; }

 private:
  geo2d::Point _geometry;
};

class Building final : public Collider<Building> {
 public:
  explicit Building(geo2d::Rectangle geometry) : _geometry(geometry) {}

  COLLIDE_WITH_DEF(Unit);
  COLLIDE_WITH_DEF(Building);
  COLLIDE_WITH_DEF(Tower);
  COLLIDE_WITH_DEF(Fence);

  const geo2d::Rectangle &GetGeometry() const { return _geometry; }

 private:
  geo2d::Rectangle _geometry;
};

class Tower final : public Collider<Tower> {
 public:
  explicit Tower(geo2d::Circle geometry) : _geometry(geometry) {}

  COLLIDE_WITH_DEF(Unit);
  COLLIDE_WITH_DEF(Building);
  COLLIDE_WITH_DEF(Tower);
  COLLIDE_WITH_DEF(Fence);

  const geo2d::Circle &GetGeometry() const { return _geometry; }

 private:
  geo2d::Circle _geometry;
};

class Fence final : public Collider<Fence> {
 public:
  explicit Fence(geo2d::Segment geometry) : _geometry(geometry) {}

  COLLIDE_WITH_DEF(Unit);
  COLLIDE_WITH_DEF(Building);
  COLLIDE_WITH_DEF(Tower);
  COLLIDE_WITH_DEF(Fence);

  const geo2d::Segment &GetGeometry() const { return _geometry; }

 private:
  geo2d::Segment _geometry;
};

bool Collide(const GameObject &first, const GameObject &second) {
  return first.Collide(second);
}

//=================== Unit
COLLIDE_WITH_IMPL(Unit, Unit);
COLLIDE_WITH_IMPL(Unit, Building);
COLLIDE_WITH_IMPL(Unit, Tower);
COLLIDE_WITH_IMPL(Unit, Fence);

//=================== Building
COLLIDE_WITH_IMPL(Building, Unit);
COLLIDE_WITH_IMPL(Building, Building);
COLLIDE_WITH_IMPL(Building, Tower);
COLLIDE_WITH_IMPL(Building, Fence);

//=================== Tower
COLLIDE_WITH_IMPL(Tower, Unit);
COLLIDE_WITH_IMPL(Tower, Building);
COLLIDE_WITH_IMPL(Tower, Tower);
COLLIDE_WITH_IMPL(Tower, Fence);

//=================== Fence
COLLIDE_WITH_IMPL(Fence, Unit);
COLLIDE_WITH_IMPL(Fence, Building);
COLLIDE_WITH_IMPL(Fence, Tower);
COLLIDE_WITH_IMPL(Fence, Fence);

void TestAddingNewObjectOnMap() {
  // Юнит-тест моделирует ситуацию, когда на игровой карте уже есть какие-то
  // объекты, и мы хотим добавить на неё новый, например, построить новое здание
  // или башню. Мы можем его добавить, только если он не пересекается ни с одним
  // из существующих.
  using namespace geo2d;

  const vector<shared_ptr<GameObject>> game_map = {
      make_shared<Unit>(Point{3, 3}),
      make_shared<Unit>(Point{5, 5}),
      make_shared<Unit>(Point{3, 7}),
      make_shared<Fence>(Segment{{7, 3}, {9, 8}}),
      make_shared<Tower>(Circle{Point{9, 4}, 1}),
      make_shared<Tower>(Circle{Point{10, 7}, 1}),
      make_shared<Building>(Rectangle{{11, 4}, {14, 6}})};

  for (size_t i = 0; i < game_map.size(); ++i) {
    Assert(Collide(*game_map[i], *game_map[i]),
           "An object doesn't collide with itself: " + to_string(i));

    for (size_t j = 0; j < i; ++j) {
      Assert(!Collide(*game_map[i], *game_map[j]),
             "Unexpected collision found " + to_string(i) + ' ' + to_string(j));
    }
  }

  auto new_warehouse = make_shared<Building>(Rectangle{{4, 3}, {9, 6}});
  ASSERT(!Collide(*new_warehouse, *game_map[0]));
  ASSERT(Collide(*new_warehouse, *game_map[1]));
  ASSERT(!Collide(*new_warehouse, *game_map[2]));
  ASSERT(Collide(*new_warehouse, *game_map[3]));
  ASSERT(Collide(*new_warehouse, *game_map[4]));
  ASSERT(!Collide(*new_warehouse, *game_map[5]));
  ASSERT(!Collide(*new_warehouse, *game_map[6]));

  auto new_defense_tower = make_shared<Tower>(Circle{{8, 2}, 2});
  ASSERT(!Collide(*new_defense_tower, *game_map[0]));
  ASSERT(!Collide(*new_defense_tower, *game_map[1]));
  ASSERT(!Collide(*new_defense_tower, *game_map[2]));
  ASSERT(Collide(*new_defense_tower, *game_map[3]));
  ASSERT(Collide(*new_defense_tower, *game_map[4]));
  ASSERT(!Collide(*new_defense_tower, *game_map[5]));
  ASSERT(!Collide(*new_defense_tower, *game_map[6]));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestAddingNewObjectOnMap);
  return 0;
}
