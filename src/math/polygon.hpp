//  SuperTux
//  Copyright (C) 2020 Grzegorz Przybylski <zwatotem@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_MATH_POLYGON_HPP
#define HEADER_SUPERTUX_MATH_POLYGON_HPP

#include <vector>
#include "math/vector.hpp"
#include "math/rectf.hpp"

/** Only for convex polygons. Concave polygons will be treated as invalid. */
class Polygon
{
public:
  Polygon();
  Polygon(const Polygon& rhs) = default;
  Polygon& operator=(const Polygon& rhs) = default;

  Polygon(std::vector<Vector> shape);
  Polygon(std::vector<Vector> shape, Vector translation, float angle);
  Polygon(Polygon shape, Vector translation, float angle);

public:
  void set_translation(Vector translation);
  void add_translation(Vector translation);
  Vector get_translation() const { return m_translation; }

  void set_rotation(float rotation);
  void add_rotation(float rotation);
  float get_rotation() const { return m_rotation; }

  Vector get_vertex(int ordinal) const;
  std::pair<Vector, Vector> get_edge(int ordinal) const;

  Rectf bounding_box() const;

  int get_count() const { return m_count; }
  bool contains(Vector point) const;
  bool empty() const;
  bool valid() const;
private:
  void update_vertices();

private:
  int m_count;
  std::vector<Vector> m_untransformed;
  Vector m_translation;
  float m_rotation;
  std::vector<Vector> m_transformed;
};

std::ostream& operator<<(std::ostream& out, const Polygon& poly);

#endif

/* EOF */
