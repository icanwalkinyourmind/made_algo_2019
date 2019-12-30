/*
 Дано множество точек на плоскости (x, y). Постройте выпуклую оболочку этого множества и вычислите ее периметр.
 Вариант 1. С помощью алгоритма Грэхема.
*/
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

const double EPS = 0.000001;

struct point {
  double x;
  double y;
};

double cross_product(const point &A, const point &B, const point &C) {
  return (B.x - A.x) * (C.y - B.y) - (B.y - A.y) * (C.x - B.x);
}

void place_smallest_point_first(std::vector<point> &points) {
  int smallest_ind = 0;
  for (int i = 1; i < points.size(); ++i) if (points.at(i).x < points.at(smallest_ind).x) smallest_ind = i;
  std::swap(points.at(0), points.at(smallest_ind));
}

double distance(const point &A, const point &B) {
  return sqrt(pow((A.x - B.x), 2) + pow((A.y - B.y), 2));
}

void sort_by_polar_angle(std::vector<point> &points) {
  point K = points.at(0);
  std::sort(points.begin() + 1, points.end(), [K](point M, point N) {
    auto product = cross_product(N, M, K);
    if (product == 0) return distance(M, K) < distance(N, K);
    return product < EPS;
  });
}

double get_perimeter(const std::vector<point> &points) {
  assert(!points.empty());
  double P = 0.;
  for (int i = 0; i < points.size() - 1; ++i) {
    P += distance(points.at(i), points.at(i + 1));
  }
  P += distance(points.at(0), points.at(points.size() - 1));
  return P;
}

double get_minimal_perimeter(std::vector<point> &points) {
  place_smallest_point_first(points);
  sort_by_polar_angle(points);
  std::vector<point> hull;
  // первые две точки точно входят в оболчку
  hull.push_back(points.at(0));
  hull.push_back(points.at(1));
  for (int i = 2; i < points.size(); ++i) {
    while (cross_product(hull[hull.size() - 2], hull[hull.size() - 1], points.at(i)) < EPS and hull.size() > 2)
      hull.pop_back();
    hull.push_back(points.at(i));
  }
  // если не набралось 3-ёх точек, невозможно построить оболчку
  return hull.size() > 2 ? get_perimeter(hull) : 0;
}

int main() {
  int n_points;
  std::vector<point> points;

  std::cin >> n_points;
  if (n_points < 3) {
    std::cout << 0;
    return 0;
  }

  double x, y;
  for (int i = 0; i < n_points; ++i) {
    std::cin >> x >> y;
    points.push_back(point{x, y});
  }
  std::cout.precision(9);
  std::cout << get_minimal_perimeter(points);
  return 0;
}
