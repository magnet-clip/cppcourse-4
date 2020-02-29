#include "Common.h"

using namespace std;

class ShapeBase : public IShape {
 public:
  virtual void SetPosition(Point point) override { _position = point; }
  virtual Point GetPosition() const override { return _position; }

  virtual void SetSize(Size size) override { _size = size; }
  virtual Size GetSize() const override { return _size; }

  virtual void SetTexture(shared_ptr<ITexture> texture) override {
    _texture = texture;
  }
  virtual ITexture* GetTexture() const override { return _texture.get(); }

  virtual unique_ptr<IShape> Clone() const override { return nullptr; }
  virtual void Draw(Image&) const override = 0;

 protected:
  Point _position;
  Size _size;
  shared_ptr<ITexture> _texture;
};

class Ellipse : public ShapeBase {
 public:
  virtual void Draw(Image&) const override {}
};
class Rectangle : public ShapeBase {
 public:
  virtual void Draw(Image&) const override {}
};

unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  if (shape_type == ShapeType::Ellipse) {
    return make_unique<Ellipse>();
  } else if (shape_type == ShapeType::Rectangle) {
    return make_unique<Rectangle>();
  } else {
    throw invalid_argument("Unexpected shape type");
  }
}