#include "Common.h"

using namespace std;

template <class T>
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

  virtual unique_ptr<IShape> Clone() const override {
    T* copy = new T();
    copy->SetPosition(GetPosition());
    copy->SetSize(GetSize());
    copy->_texture = _texture;
    return unique_ptr<IShape>(copy);
  }

  virtual bool PointInShape(Point point) const = 0;

  virtual void Draw(Image& image) const override {
    // draw nothing if image is zero-sized
    if (image.size() == 0) return;

    // draw nothing should the shape be out of bounds
    if (_position.y > static_cast<int>(image.size())) return;
    if (_position.x > static_cast<int>(image[0].length())) return;

    // possible overflow; could make check
    auto rows_count = static_cast<int>(image.size());
    auto column_count = static_cast<int>(image[0].length());

    auto last_row_idx = min(_position.y + _size.height, rows_count);
    auto last_col_idx = min(_position.x + _size.width, column_count);

    auto first_row = next(image.begin(), _position.y);
    auto last_row = next(image.begin(), last_row_idx);

    int y = _position.y;
    for (auto it = first_row; it != last_row; ++it) {
      auto& row = *it;
      int texture_y = y - _position.y;
      for (int x = _position.x; x < last_col_idx; ++x) {
        int texture_x = x - _position.x;
        if (!PointInShape({texture_x, texture_y})) continue;
        if (_texture != nullptr && texture_x < _texture->GetSize().width &&
            texture_y < _texture->GetSize().height) {
          row[x] = _texture->GetImage()[texture_y][texture_x];
        } else {
          row[x] = '.';
        }
      }
      ++y;
    }
  }

 protected:
  Point _position;
  Size _size;
  shared_ptr<ITexture> _texture;
};

class Ellipse : public ShapeBase<Ellipse> {
 public:
  virtual bool PointInShape(Point point) const override {
    return IsPointInEllipse(point, _size);
  }
};

class Rectangle : public ShapeBase<Rectangle> {
 public:
  virtual bool PointInShape(Point) const override { return true; }
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