#include "geometry.h"

void ComparisonTest() {
  Line tmp1(Point(0, 0), 2);
  Line tmp2(Point(1, 2), 2);

  std::cout << "1 = " << (tmp1 == tmp2) << "\n";

  tmp1 = Line(Point(0, 0), 2);
  tmp2 = Line(Point(2, 2), 2);

  std::cout << "0 = " << (tmp1 == tmp2) << "\n";

  tmp1 = Line(Point(0, 0), Point(1, 1));
  tmp2 = Line(Point(2, 2), 1);

  std::cout << "1 = " << (tmp1 == tmp2) << "\n";

  tmp1 = Line(Point(0, 0), Point(0, 1));
  tmp2 = Line(Point(0, 2), Point(0, 1));

  std::cout << "1 = " << (tmp1 == tmp2) << "\n";

  tmp1 = Line(Point(0, 0), Point(0, 3));
  tmp2 = Line(Point(1, 0), Point(1, 3));

  std::cout << "0 = " << (tmp1 == tmp2) << "\n";
}

void TransformationPolygonTest() {
  Polygon pol(Point(1, 1), Point(-1, 1), Point(-1, -1), Point(1, -1));

  pol.rotate(Point(0, 0), 45);
  bool is_ok_rotate = true;

  if (pol.getVertices()[0] != Point(0, 1.41421356237)) {
    is_ok_rotate = false;
  }
  if (pol.getVertices()[1] != Point(-1.41421356237, 0)) {
    is_ok_rotate = false;
  }
  if (pol.getVertices()[2] != Point(0, -1.41421356237)) {
    is_ok_rotate = false;
  }
  if (pol.getVertices()[3] != Point(1.41421356237, 0)) {
    is_ok_rotate = false;
  }

  std::cout << "1 = " << is_ok_rotate << "\n";

  pol.rotate(Point(0, 1.41421356237), 180);
  is_ok_rotate = true;

  if (pol.getVertices()[0] != Point(0, 1.41421356237)) {
    is_ok_rotate = false;
  }
  if (pol.getVertices()[1] != Point(1.41421356237, 2 * 1.41421356237)) {
    is_ok_rotate = false;
  }
  if (pol.getVertices()[2] != Point(0, 3 * 1.41421356237)) {
    is_ok_rotate = false;
  }
  if (pol.getVertices()[3] != Point(-1.41421356237, 2 * 1.41421356237)) {
    is_ok_rotate = false;
  }

  std::cout << "1 = " << is_ok_rotate << "\n";

  pol.rotate(Point(0, 1.41421356237), 0);
  is_ok_rotate = true;

  if (pol.getVertices()[0] != Point(0, 1.41421356237)) {
    is_ok_rotate = false;
  }
  if (pol.getVertices()[1] != Point(1.41421356237, 2 * 1.41421356237)) {
    is_ok_rotate = false;
  }
  if (pol.getVertices()[2] != Point(0, 3 * 1.41421356237)) {
    is_ok_rotate = false;
  }
  if (pol.getVertices()[3] != Point(-1.41421356237, 2 * 1.41421356237)) {
    is_ok_rotate = false;
  }

  std::cout << "1 = " << is_ok_rotate << "\n";

  std::vector<Point> points = {Point(1, 1), Point(-1, 1), Point(-1, -1), Point(1, -1)};
  pol = Polygon(points);

  pol.reflect(Point(1, 1));
  bool is_ok_reflect = true;

  if (pol.getVertices()[0] != Point(1, 1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[1] != Point(3, 1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[2] != Point(3, 3)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[3] != Point(1, 3)) {
    is_ok_reflect = false;
  }

  std::cout << "1 = " << is_ok_reflect << "\n";

  pol.reflect(Point(0, 0));
  is_ok_reflect = true;

  if (pol.getVertices()[0] != Point(-1, -1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[1] != Point(-3, -1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[2] != Point(-3, -3)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[3] != Point(-1, -3)) {
    is_ok_reflect = false;
  }

  std::cout << "1 = " << is_ok_reflect << "\n";

  pol = Polygon(points);

  pol.reflect(Line(0, 1));
  is_ok_reflect = true;

  if (pol.getVertices()[0] != Point(1, 1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[1] != Point(-1, 1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[2] != Point(-1, 3)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[3] != Point(1, 3)) {
    is_ok_reflect = false;
  }

  std::cout << "1 = " << is_ok_reflect << "\n";

  pol.reflect(Line(Point(1, 0), Point(1, 2)));
  is_ok_reflect = true;

  if (pol.getVertices()[0] != Point(1, 1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[1] != Point(3, 1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[2] != Point(3, 3)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[3] != Point(1, 3)) {
    is_ok_reflect = false;
  }

  std::cout << "1 = " << is_ok_reflect << "\n";

  pol.reflect(Line(-1, 2));
  is_ok_reflect = true;

  if (pol.getVertices()[0] != Point(1, 1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[1] != Point(1, -1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[2] != Point(-1, -1)) {
    is_ok_reflect = false;
  }
  if (pol.getVertices()[3] != Point(-1, 1)) {
    is_ok_reflect = false;
  }

  std::cout << "1 = " << is_ok_reflect << "\n";

  pol = Polygon(points);

  pol.scale(Point(0, 0), 2);
  bool is_ok_scale = true;

  if (pol.getVertices()[0] != Point(2, 2)) {
    is_ok_scale = false;
  }
  if (pol.getVertices()[1] != Point(-2, 2)) {
    is_ok_scale = false;
  }
  if (pol.getVertices()[2] != Point(-2, -2)) {
    is_ok_scale = false;
  }
  if (pol.getVertices()[3] != Point(2, -2)) {
    is_ok_scale = false;
  }

  std::cout << "1 = " << is_ok_scale << "\n";
}

void TriangleCircle() {
  Triangle tr(Point(0, 0), Point(1, 1), Point(2, 0));
  double thr = 0.0001;

  Circle cl1 = tr.circumscribedCircle();
  Circle cl2 = tr.inscribedCircle();
  double d1 = cl1.center().distance(cl2.center());
  double d2 = sqrt(cl1.radius() * (cl1.radius() - 2 * cl2.radius()));
  if (std::abs(d1 - d2) < thr) {
    std::cout << "OK\n";
  } else {
    std::cout << "BAD\n";
  }

  tr = Triangle(Point(-1, 0), Point(1, 1), Point(2, 0));

  cl1 = tr.circumscribedCircle();
  cl2 = tr.inscribedCircle();
  d1 = cl1.center().distance(cl2.center());
  d2 = sqrt(cl1.radius() * (cl1.radius() - 2 * cl2.radius()));
  if (std::abs(d1 - d2) < thr) {
    std::cout << "OK\n";
  } else {
    std::cout << "BAD\n";
  }

  tr = Triangle(Point(-1, 0), Point(1, 4), Point(2, 0));

  cl1 = tr.circumscribedCircle();
  cl2 = tr.inscribedCircle();
  d1 = cl1.center().distance(cl2.center());
  d2 = sqrt(cl1.radius() * (cl1.radius() - 2 * cl2.radius()));
  if (std::abs(d1 - d2) < thr) {
    std::cout << "OK\n";
  } else {
    std::cout << "BAD\n";
  }

  tr = Triangle(Point(-1, 0.3), Point(1, 1), Point(2, 0));

  cl1 = tr.circumscribedCircle();
  cl2 = tr.inscribedCircle();
  d1 = cl1.center().distance(cl2.center());
  d2 = sqrt(cl1.radius() * (cl1.radius() - 2 * cl2.radius()));
  if (std::abs(d1 - d2) < thr) {
    std::cout << "OK\n";
  } else {
    std::cout << "BAD\n";
  }
}

int main() {
  std::cout << "comparison line test:\n";
  ComparisonTest();
  std::cout << "transformation polygon test:\n";
  TransformationPolygonTest();
  std::cout << "triangle circles test:\n";
  TriangleCircle();

  Triangle tr(Point(-1, -2), Point(1, 2), Point(-2, 2));
  Circle nn = tr.ninePointsCircle();
  Point mamaa = tr.orthocenter();
  nn.scale(mamaa, 2);
  Circle kaka = tr.circumscribedCircle();

  return 0;
}