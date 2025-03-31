#include <cmath>
#include <iostream>
#include <optional>
#include <vector>

namespace utilities {
  const double kEps = 0.0001;

  bool double_comparison(double first, double second) {
    return std::fabs(first - second) < kEps;
  }
}

struct Point {
  double x;
  double y;

  Point();
  Point(double x, double y);

  ~Point() = default;

  bool operator==(const Point& new_point) const;
  bool operator!=(const Point& new_point) const;

  Point& operator+=(const Point& new_point);
  Point operator+(const Point& new_point) const;
  Point& operator-=(const Point& new_point);
  Point operator-(const Point& new_point) const;
  Point& operator*=(double number);
  Point operator*(double number) const;

  double distance(const Point& new_point) const;
  double distance2(const Point& new_point) const;
  Point rotate(const Point& center, double angle) const;
  Point reflect(const Point& center) const;
  Point scale(const Point& center, double coefficient) const;

  double scalar(const Point& new_point) const;
  double pseudo_scalar(const Point& new_point) const;

  Point get_normalize() const;
};

Point::Point() : x(0), y(0) {}

Point::Point(double x, double y) : x(x), y(y) {}

bool Point::operator==(const Point& new_point) const {
  return utilities::double_comparison(x, new_point.x) &&
         utilities::double_comparison(y, new_point.y);
}

bool Point::operator!=(const Point& new_point) const { return !(*this == new_point); }

Point& Point::operator+=(const Point& new_point) {
  x += new_point.x;
  y += new_point.y;
  return *this;
}

Point Point::operator+(const Point& new_point) const {
  Point ans = *this;
  return ans += new_point;
}

Point& Point::operator-=(const Point& new_point) {
  x -= new_point.x;
  y -= new_point.y;
  return *this;
}

Point Point::operator-(const Point& new_point) const {
  Point ans = *this;
  return ans -= new_point;
}

Point& Point::operator*=(double number) {
  x *= number;
  y *= number;
  return *this;
}

Point Point::operator*(double number) const {
  Point new_point = *this;
  return new_point *= number;
}

double Point::distance(const Point& new_point) const {
  return sqrt((y - new_point.y) * (y - new_point.y) + (x - new_point.x) * (x - new_point.x));
}

double Point::distance2(const Point& new_point) const {
  return (y - new_point.y) * (y - new_point.y) + (x - new_point.x) * (x - new_point.x);
}

Point Point::rotate(const Point& center, double angle) const {
  Point new_point = *this - center;
  angle *= M_PI / 180.0;
  double cosa = cos(angle);
  double sina = sin(angle);

  new_point = Point(new_point.x * cosa - new_point.y * sina,
                    new_point.x * sina + new_point.y * cosa);
  return new_point + center;
}

Point Point::reflect(const Point& center) const {
  Point new_point = center * 2 - *this;
  return new_point;
}

Point Point::scale(const Point& center, double coefficient) const {
  Point new_point = center + (*this - center) * coefficient;
  return new_point;
}

double Point::scalar(const Point& new_point) const { return x * new_point.x + y * new_point.y; }

double Point::pseudo_scalar(const Point& new_point) const {
  return x * new_point.y - y * new_point.x;
}

Point Point::get_normalize() const {
  if (std::fabs(distance2(Point())) < utilities::kEps) {
    return Point();
  }

  return *this * (1 / distance(Point()));
}

class Line {
private:
  Point origin_;
  Point direction_;

public:
  Line() = default;
  Line(const Point& first_point, const Point& second_point);
  Line(double k, double b);
  Line(const Point& origin, double b);
  Line(const Line& new_line);

  ~Line() = default;

  bool operator==(const Line& another) const;
  bool operator!=(const Line& another) const;

  Point reflect(const Point& our_point) const;

  std::optional<Point> crossing(const Line& another) const;
  Line perpend() const;
  double distance(const Point& point) const;
};

Line::Line(const Point& first_point, const Point& second_point)
     : origin_(first_point),
       direction_(second_point - first_point) {}

Line::Line(double k, double b) : origin_(0, b), direction_(1, k) {}

Line::Line(const Point& origin, double k) : origin_(origin), direction_(1, k) {}

Line::Line(const Line& new_line) : origin_(new_line.origin_), direction_(new_line.direction_) {}

bool Line::operator==(const Line& another) const {
  if (std::fabs(direction_.x) < utilities::kEps) {
    if (std::fabs(another.direction_.x) < utilities::kEps) {
      if (utilities::double_comparison(another.origin_.x, origin_.x)) {
        return true;
      }
    }

    return false;
  }

  double tan1 = std::fabs(direction_.y / direction_.x);
  double tan2 = std::fabs(another.direction_.y / another.direction_.x);

  if (utilities::double_comparison(tan1, tan2)) {
    if (std::fabs((another.origin_ - origin_).pseudo_scalar(direction_)) < utilities::kEps) {
      return true;
    }
  }

  return false;
}

bool Line::operator!=(const Line& another) const { return !(*this == another); }

Point Line::reflect(const Point& our_point) const {
  Point new_center = direction_ * ((our_point - origin_).scalar(direction_) /
                     direction_.distance2(Point(0, 0))) +
                     origin_;
  return our_point.reflect(new_center);
}

std::optional<Point> Line::crossing(const Line& another) const {
  double thr = 0.0001;

  if (std::fabs(direction_.pseudo_scalar(another.direction_)) < thr) {
    return {};
  }

  double param = another.direction_.pseudo_scalar(another.origin_ - origin_) /
                 another.direction_.pseudo_scalar(direction_);
  return origin_ + direction_ * param;
}

Line Line::perpend() const {
  return Line(origin_, origin_ + Point(-direction_.y, direction_.x));
}

double Line::distance(const Point& point) const {
  double param1 = std::fabs(direction_.pseudo_scalar(point) -
                            direction_.pseudo_scalar(origin_));
  return param1 / direction_.distance(Point());
}

class Shape {
public:
  Shape() = default;

  virtual ~Shape() {}

  virtual double perimeter() const = 0;
  virtual double area() const = 0;

  virtual bool equalTo(const Shape&) const = 0;
  virtual bool isCongruentTo(const Shape&) const = 0;
  virtual bool isSimilarTo(const Shape&) const = 0;
  virtual bool containsPoint(const Point&) const = 0;

  virtual void rotate(const Point&, double) = 0;
  virtual void reflect(const Point&) = 0;
  virtual void reflect(const Line&) = 0;
  virtual void scale(const Point&, double) = 0;
};

bool operator==(const Shape& shape1, const Shape& shape2) {
  return shape1.equalTo(shape2);
}

class Polygon : public Shape {
protected:
  std::vector<Point> vertices_;

  int pseudo_scalar(const Point& first, const Point& second, const Point& third) const;

  void segments_calculate(Polygon* new_polygon, std::vector<double>& segments,
                          std::vector<double>& new_segments) const;
  void angles_calculate(Polygon* new_polygon, const std::vector<double>& new_segments,
                        std::vector<double>& angles) const;
  bool check_helper(const std::vector<double>& segments, const std::vector<double>& angles,
                    bool is_type) const;

public:
  Polygon() = default;
  Polygon(const std::vector<Point>& vertices);

  template<typename... Args>
  Polygon(const Args&... args);
  Polygon(const Polygon& new_polygon);

  ~Polygon() override = default;

  double perimeter() const final;
  double area() const override;

  bool equalTo(const Shape& another) const final;
  bool isCongruentTo(const Shape& another) const final;
  bool isSimilarTo(const Shape& another) const final;
  bool containsPoint(const Point& point) const final;

  void rotate(const Point& center, double angle) final;
  void reflect(const Point& center) final;
  void reflect(const Line& axis) final;
  void scale(const Point& center, double coefficient) final;

  size_t verticesCount() const;
  const std::vector<Point>& getVertices() const;

  bool isConvex() const;
};

int Polygon::pseudo_scalar(const Point& first, const Point& second, const Point& third) const {
  Point A = second - first;
  Point B = third - first;

  if (A.pseudo_scalar(B) > 0) {
    return 1;
  }
  return -1;
}

void Polygon::segments_calculate(Polygon* new_polygon, std::vector<double>& segments,
                                 std::vector<double>& new_segments) const {
  segments.resize(vertices_.size());
  new_segments.resize(vertices_.size());

  for (size_t i = 0; i < vertices_.size(); ++i) {
    segments[i] = vertices_[i].distance(vertices_[(i + 1) % vertices_.size()]);
    new_segments[i] = new_polygon->vertices_[i].distance(
            new_polygon->vertices_[(i + 1) % vertices_.size()]);
  }
}

void Polygon::angles_calculate(Polygon* new_polygon, const std::vector<double>& new_segments,
                               std::vector<double>& angles) const {
  angles.resize(vertices_.size());

  for (size_t i = 1; i <= vertices_.size(); ++i) {
    double new_side = new_polygon->vertices_[i - 1].distance(
            new_polygon->vertices_[(i + 1) % vertices_.size()]);
    angles[i] = ((pow(new_segments[i - 1], 2) +
                  pow(new_segments[i % vertices_.size()], 2) -
                  pow(new_side, 2)) / (2 * new_segments[i - 1] *
                  new_segments[i % vertices_.size()]));
  }
}

bool Polygon::check_helper(const std::vector<double>& segments, const std::vector<double>& angles,
                           bool is_type) const {
  bool flag;
  for (size_t i = 0; i < vertices_.size(); ++i) {
    for (size_t j = 0; j < vertices_.size(); ++j) {
      if (angles[i] == angles[j]) {
        double coefficient = (is_type ? 1 : segments[i] / segments[j]);
        flag = true;

        for (size_t p = 0; p < vertices_.size(); ++p) {
          size_t index1 = (i + p) % vertices_.size();
          size_t index2 = (j + p) % vertices_.size();
          if (utilities::double_comparison(segments[index1] / segments[index2],
                                           coefficient) &&
              utilities::double_comparison(angles[index1], angles[index2])) {
            continue;
          }

          flag = false;
        }
      }
      if (flag) {
        return true;
      }
    }
  }
  return false;
}

Polygon::Polygon(const std::vector<Point>& vertices) : vertices_(vertices) {}

template<typename... Args>
Polygon::Polygon(const Args&... args) {
  vertices_ = {args...};
}

Polygon::Polygon(const Polygon& new_polygon) : vertices_(new_polygon.vertices_) {}

double Polygon::perimeter() const {
  double ans = 0;

  for (size_t i = 0; i < vertices_.size(); ++i) {
    ans += vertices_[i].distance(vertices_[(i + 1) % vertices_.size()]);
  }

  return ans;
}

double Polygon::area() const {
  double area = 0;
  for (size_t i = 1; i < vertices_.size(); ++i) {
    Point vector1 = vertices_[0] - vertices_[i];
    Point vector2 = vertices_[0] - vertices_[(i + 1) % vertices_.size()];
    area += vector1.pseudo_scalar(vector2);
  }

  return std::fabs(area) / 2;
}

bool Polygon::equalTo(const Shape& another) const {
  Polygon* new_polygon = dynamic_cast<Polygon*>(const_cast<Shape*>(&another));

  if (new_polygon == nullptr) {
    return false;
  }
  if (vertices_.size() != new_polygon->vertices_.size()) {
    return false;
  }

  bool is_points_same = false;
  size_t cur_index = 0;
  for (; cur_index < vertices_.size(); ++cur_index) {
    if (vertices_[cur_index] == new_polygon->vertices_[1]) {
      is_points_same = true;
      break;
    }
  }

  if (!is_points_same) {
    return false;
  }

  if (new_polygon->vertices_[2] == vertices_[(cur_index + 1) % vertices_.size()]) {
    for (size_t i = 0; i < vertices_.size() - 2; ++i, ++cur_index) {
      if (new_polygon->vertices_[(i + 3) % vertices_.size()] !=
          vertices_[(cur_index + 2) % vertices_.size()]) {
        return false;
      }
    }

    return true;
  }
  if (new_polygon->vertices_[0] == vertices_[(cur_index + 1) % vertices_.size()]) {
    std::reverse(new_polygon->vertices_.begin(), new_polygon->vertices_.end());

    for (size_t i = 0; i < vertices_.size() - 2; ++i, ++cur_index) {
      if (new_polygon->vertices_[i] != vertices_[(cur_index + 2) % vertices_.size()]) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool Polygon::isCongruentTo(const Shape& another) const {
  Polygon* new_polygon = dynamic_cast<Polygon*>(const_cast<Shape*>(&another));

  if (new_polygon == nullptr) {
    return false;
  }
  if (vertices_.size() != new_polygon->vertices_.size()) {
    return false;
  }

  std::vector<double> segments;
  std::vector<double> new_segments;
  segments_calculate(new_polygon, segments, new_segments);

  std::vector<double> angles;
  angles_calculate(new_polygon, new_segments, angles);

  return check_helper(segments, angles, true);
}

bool Polygon::isSimilarTo(const Shape& another) const {
  Polygon* new_polygon = dynamic_cast<Polygon*>(const_cast<Shape*>(&another));

  if (new_polygon == nullptr) {
    return false;
  }
  if (vertices_.size() != new_polygon->vertices_.size()) {
    return false;
  }

  std::vector<double> segments;
  std::vector<double> new_segments;
  segments_calculate(new_polygon, segments, new_segments);

  std::vector<double> angles;
  angles_calculate(new_polygon, new_segments, angles);

  return check_helper(segments, angles, false);
}

bool Polygon::containsPoint(const Point& point) const {
  double angle = 0;
  for (size_t i = 0; i < vertices_.size(); ++i) {
    double side1 = point.distance(vertices_[i]);
    double side2 = point.distance(vertices_[(i + 1) % vertices_.size()]);
    double side3 = vertices_[i].distance(vertices_[(i + 1) % vertices_.size()]);
    double angle_i = acos((side1 * side1 + side2 * side2 - side3 * side3) / (2 * side1 * side2));

    angle += angle_i;
  }

  if (utilities::double_comparison(angle, 2 * M_PI)) {
    return true;
  }

  return false;
}

void Polygon::rotate(const Point& center, double angle) {
  for (Point& vertex : vertices_) {
    vertex = vertex.rotate(center, angle);
  }
}

void Polygon::reflect(const Point& center) {
  for (Point& vertex : vertices_) {
    vertex = vertex.reflect(center);
  }
}

void Polygon::reflect(const Line& axis) {
  for (Point& vertex : vertices_) {
    vertex = axis.reflect(vertex);
  }
}

void Polygon::scale(const Point& center, double coefficient) {
  for (Point& vertex : vertices_) {
    vertex = vertex.scale(center, coefficient);
  }
}

size_t Polygon::verticesCount() const { return vertices_.size(); }

const std::vector<Point>& Polygon::getVertices() const { return vertices_; }

bool Polygon::isConvex() const {
  int our_sign = pseudo_scalar(vertices_[0], vertices_[1], vertices_[2]);

  for (size_t i = 1; i < vertices_.size() - 2; ++i) {
    if (pseudo_scalar(vertices_[i], vertices_[i + 1], vertices_[i + 2]) != our_sign) {
      return false;
    }
  }

  return true;
}

class Ellipse : public Shape {
protected:
  Point first_focus_;
  Point second_focus_;
  double distance_;

public:
  Ellipse();
  Ellipse(const Point& first_focus, const Point& second_focus, double distance);
  Ellipse(const Ellipse& ellipse) = default;

  ~Ellipse() override = default;

  double perimeter() const override;
  double area() const override;

  bool equalTo(const Shape& another) const override;
  bool isCongruentTo(const Shape& another) const override;
  bool isSimilarTo(const Shape& another) const override;
  bool containsPoint(const Point& point) const override;

  void rotate(const Point& center, double angle) override;
  void reflect(const Point& center) override;
  void reflect(const Line& axis) override;
  void scale(const Point& center, double coefficient) override;

  std::pair<Point, Point> focuses() const;
  std::pair<Line, Line> directrices() const;
  double eccentricity() const;
  virtual Point center() const;
};

Ellipse::Ellipse() : distance_(0) {}

Ellipse::Ellipse(const Point& first_focus, const Point& second_focus, double distance)
        : first_focus_(first_focus), second_focus_(second_focus), distance_(distance) {}

double Ellipse::perimeter() const {
  double big_axis = distance_ / 2;
  return 4.0 * big_axis * std::comp_ellint_2(eccentricity());
}

double Ellipse::area() const {
  double big_axis = distance_ / 2;
  double small_axis = sqrt(distance_ * distance_ - first_focus_.distance2(second_focus_)) / 2;
  return M_PI * big_axis * small_axis;
}

bool Ellipse::equalTo(const Shape& another) const {
  Ellipse* new_ellipse = dynamic_cast<Ellipse*>(const_cast<Shape*>(&another));

  if (new_ellipse == nullptr) {
    return false;
  }

  if ((((new_ellipse->first_focus_ == first_focus_) && (new_ellipse->second_focus_ == second_focus_)) ||
      (((new_ellipse->first_focus_ == second_focus_) && (new_ellipse->second_focus_ == first_focus_)))) &&
      utilities::double_comparison(new_ellipse->distance_, distance_)) {
    return true;
  }

  return false;
}

bool Ellipse::isCongruentTo(const Shape& another) const {
  Ellipse* new_ellipse = dynamic_cast<Ellipse*>(const_cast<Shape*>(&another));

  if (new_ellipse == nullptr) {
    return false;
  }

  double big_axis = distance_ / 2;
  double small_axis = sqrt(distance_ * distance_ - first_focus_.distance2(second_focus_)) / 2;
  double new_big_axis = new_ellipse->distance_ / 2;
  double new_small_axis = sqrt(new_ellipse->distance_ * new_ellipse->distance_ -
                               new_ellipse->first_focus_.distance2(
                                       new_ellipse->second_focus_)) / 2;

  if (utilities::double_comparison(big_axis, new_big_axis) &&
      utilities::double_comparison(small_axis, new_small_axis)) {
    return true;
  }
  if (utilities::double_comparison(small_axis, new_big_axis) &&
      utilities::double_comparison(big_axis, new_small_axis)) {
    return true;
  }

  return false;
}

bool Ellipse::isSimilarTo(const Shape& another) const {
  Ellipse* new_ellipse = dynamic_cast<Ellipse*>(const_cast<Shape*>(&another));

  if (new_ellipse == nullptr) {
    return false;
  }

  double big_axis = distance_ / 2;
  double small_axis = sqrt(distance_ * distance_ - first_focus_.distance2(second_focus_)) / 2;
  double new_big_axis = new_ellipse->distance_ / 2;
  double new_small_axis = sqrt(new_ellipse->distance_ * new_ellipse->distance_ -
                          new_ellipse->first_focus_.distance2(new_ellipse->second_focus_)) / 2;

  double our_coefficient = big_axis / small_axis;
  double new_coefficient = new_big_axis / new_small_axis;
  if (utilities::double_comparison(our_coefficient, new_coefficient)) {
    return true;
  }
  new_coefficient = new_small_axis / new_big_axis;
  if (utilities::double_comparison(our_coefficient, new_coefficient)) {
    return true;
  }

  return false;
}

bool Ellipse::containsPoint(const Point& point) const {
  return point.distance(first_focus_) + point.distance(second_focus_) < distance_;
}

void Ellipse::rotate(const Point& center, double angle) {
  first_focus_ = first_focus_.rotate(center, angle);
  second_focus_ = second_focus_.rotate(center, angle);
}

void Ellipse::reflect(const Point& center) {
  first_focus_ = first_focus_.reflect(center);
  second_focus_ = second_focus_.reflect(center);
}

void Ellipse::reflect(const Line& axis) {
  first_focus_ = axis.reflect(first_focus_);
  second_focus_ = axis.reflect(second_focus_);
}

void Ellipse::scale(const Point& center, double coefficient) {
  first_focus_ = first_focus_.scale(center, coefficient);
  second_focus_ = second_focus_.scale(center, coefficient);
  distance_ *= coefficient;
}

std::pair<Point, Point> Ellipse::focuses() const { return {first_focus_, second_focus_}; }

std::pair<Line, Line> Ellipse::directrices() const {
  Point first_dir_point1 = Point(distance_ / (2 * eccentricity()), 0) + center();
  Point second_dir_point1 = Point(-distance_ / (2 * eccentricity()), 0) + center();
  Point first_dir_point2 = Point(distance_ / (2 * eccentricity()), 1) + center();
  Point second_dir_point2 = Point(-distance_ / (2 * eccentricity()), 1) + center();

  double angle = -acos((first_focus_ - second_focus_).x / first_focus_.distance(second_focus_));

  first_dir_point1.rotate(center(), angle);
  first_dir_point2.rotate(center(), angle);
  second_dir_point1.rotate(center(), angle);
  second_dir_point2.rotate(center(), angle);

  return {Line(first_dir_point1, first_dir_point2),
          Line(second_dir_point1, second_dir_point2)};
}

double Ellipse::eccentricity() const {
  double dist = first_focus_.distance(second_focus_) / 2;
  return dist / distance_ * 2;
}

Point Ellipse::center() const { return (first_focus_ + second_focus_) * 0.5; }

class Circle : public Ellipse {
private:
  Point center_;
  double radius_;

public:
  Circle();
  Circle(const Point& center, double radius);
  Circle(const Circle& circle) = default;

  ~Circle() = default;

  double perimeter() const override;
  double area() const override;

  bool equalTo(const Shape& another) const override;
  bool isCongruentTo(const Shape& another) const override;
  bool isSimilarTo(const Shape& another) const override;
  bool containsPoint(const Point& point) const override;

  void rotate(const Point& center, double angle) override;
  void reflect(const Point& center) override;
  void reflect(const Line& axis) override;
  void scale(const Point& center, double coefficient) override;

  double radius() const;
  Point center() const override;
};

Circle::Circle() : radius_(0) {}

Circle::Circle(const Point& center, double radius) : center_(center), radius_(radius) {}

double Circle::perimeter() const { return 2 * M_PI * radius_; }

double Circle::area() const { return M_PI * radius_ * radius_; }

bool Circle::equalTo(const Shape& another) const {
  Circle* new_circle = dynamic_cast<Circle*>(const_cast<Shape*>(&another));

  if (new_circle == nullptr) {
    return false;
  }

  if (center_ == new_circle->center_ &&
      utilities::double_comparison(radius_, new_circle->radius_)) {
    return true;
  }

  return false;
}

bool Circle::isCongruentTo(const Shape& another) const {
  Circle* new_circle = dynamic_cast<Circle*>(const_cast<Shape*>(&another));

  if (new_circle == nullptr) {
    return false;
  }

  if (utilities::double_comparison(radius_, new_circle->radius_)) {
    return true;
  }

  return false;
}

bool Circle::isSimilarTo(const Shape& another) const {
  Circle* new_circle = dynamic_cast<Circle*>(const_cast<Shape*>(&another));

  if (new_circle == nullptr) {
    return false;
  }

  return true;
}

bool Circle::containsPoint(const Point& point) const { return point.distance(center_) < radius_; }

void Circle::rotate(const Point& center, double angle) { center_ = center_.rotate(center, angle); }

void Circle::reflect(const Point& center) { center_ = center_.reflect(center); }

void Circle::reflect(const Line& axis) { center_ = axis.reflect(center_); }

void Circle::scale(const Point& center, double coefficient) {
  center_ = center_.scale(center, coefficient);
  radius_ *= coefficient;
}

double Circle::radius() const { return radius_; }

Point Circle::center() const { return center_; }

class Rectangle : public Polygon {
public:
  Rectangle() = default;
  Rectangle(const Point& point1, const Point& point2, double ratio);
  Rectangle(const Rectangle& rect) = default;

  ~Rectangle() override = default;

  Point center() const;
  std::pair<Line, Line> diagonals() const;

  double area() const final;
};

Rectangle::Rectangle(const Point& point1, const Point& point2, double ratio) {
  if (ratio < 1) {
    Point center = (point1 + point2) * 0.5;
    double angle = 180 * (M_PI - 2 * atan(ratio)) / M_PI;
    vertices_ = {point1, point1.rotate(center, angle), point2, point2.rotate(center, angle)};
  } else if (utilities::double_comparison(ratio, 1)) {
    Point center = (point1 + point2) * 0.5;
    vertices_ = {point1, point1.rotate(center, 90), point2, point2.rotate(center, 90)};
  } else {
    ratio =  1 / ratio;
    Point center = (point1 + point2) * 0.5;
    double angle = 180 * (M_PI - 2 * atan(ratio)) / M_PI;
    vertices_ = {point1, point1.rotate(center, angle), point2, point2.rotate(center, angle)};
  }
}

Point Rectangle::center() const { return (vertices_[0] + vertices_[2]) * 0.5; }

std::pair<Line, Line> Rectangle::diagonals() const {
  return {Line(vertices_[0], vertices_[2]),
          Line(vertices_[1], vertices_[3])};
}

double Rectangle::area() const {
  double first_size = vertices_[0].distance(vertices_[1]);
  double second_size = vertices_[1].distance(vertices_[2]);
  return first_size * second_size;
}

class Square : public Rectangle {
public:
  Square() = default;
  Square(const Point& point1, const Point& point2);
  Square(const Square& square) = default;

  ~Square() override = default;

  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
};

Square::Square(const Point& point1, const Point& point2) {
  Point center = (point1 + point2) * 0.5;
  vertices_ = {point1, point1.rotate(center, 90), point2, point2.rotate(center, 90)};
}

Circle Square::circumscribedCircle() const {
  Point circle_center = center();
  return Circle(circle_center, vertices_[0].distance(vertices_[2]) / 2);
}

Circle Square::inscribedCircle() const {
  Point circle_center = center();
  return Circle(circle_center, vertices_[0].distance(vertices_[1]) / 2);
}

class Triangle : public Polygon {
public:
  Triangle() = default;
  Triangle(const Point& point1, const Point& point2, const Point& point3);
  Triangle(const Triangle& triangle) = default;

  ~Triangle() = default;

  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
  Point centroid() const;
  Point orthocenter() const;
  Line EulerLine() const;
  Circle ninePointsCircle() const;

  double area() const final;
};

Triangle::Triangle(const Point& point1, const Point& point2, const Point& point3)
         : Polygon(point1, point2, point3) {}

Circle Triangle::circumscribedCircle() const {
  Line first_middle_perpend = Line((vertices_[0] + vertices_[1]) * 0.5,
                                   vertices_[0]).perpend();
  Line second_middle_perpend = Line((vertices_[1] + vertices_[2]) * 0.5,
                                    vertices_[1]).perpend();
  Point circle_center = first_middle_perpend.crossing(second_middle_perpend).value();
  double radius = circle_center.distance(vertices_[0]);

  return Circle(circle_center, radius);
}

Circle Triangle::inscribedCircle() const {
  Point bis_direction1 = (vertices_[1] - vertices_[0]).get_normalize() +
                         (vertices_[2] - vertices_[0]).get_normalize();
  Line bis1(vertices_[0], vertices_[0] + bis_direction1);
  Point bis_direction2 = (vertices_[0] - vertices_[1]).get_normalize() +
                         (vertices_[2] - vertices_[1]).get_normalize();
  Line bis2(vertices_[1], vertices_[1] + bis_direction2);
  Point circle_center = bis1.crossing(bis2).value();
  double radius = Line(vertices_[0], vertices_[2]).distance(circle_center);

  return Circle(circle_center, radius);
}

Point Triangle::centroid() const {
  Point first_center = (vertices_[1] + vertices_[2]) * 0.5;
  Point second_center = (vertices_[0] + vertices_[2]) * 0.5;

  return Line(vertices_[0], first_center).crossing(
         Line(vertices_[1], second_center)).value();
}

Point Triangle::orthocenter() const {
  Point first_side = vertices_[1] - vertices_[0];
  Point second_side = vertices_[2] - vertices_[0];
  Point first_projective = vertices_[0] + second_side * (first_side.scalar(second_side) /
                           second_side.distance2(Point()));
  Point second_projective = vertices_[0] + first_side * (first_side.scalar(second_side) /
                            first_side.distance2(Point()));

  return Line(vertices_[1], first_projective).crossing(
         Line(vertices_[2], second_projective)).value();
}

Line Triangle::EulerLine() const { return Line(orthocenter(), centroid()); }

Circle Triangle::ninePointsCircle() const {
  Triangle additional_triangle((vertices_[0] + vertices_[1]) * 0.5,
                               (vertices_[1] + vertices_[2]) * 0.5,
                               (vertices_[0] + vertices_[2]) * 0.5);
  return additional_triangle.circumscribedCircle();
}

double Triangle::area() const {
  double half_perimeter = perimeter() / 2;
  double inscribed_radius = inscribedCircle().radius();

  return half_perimeter * inscribed_radius;
}