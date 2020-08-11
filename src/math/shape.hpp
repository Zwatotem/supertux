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


#ifndef HEADER_SUPERTUX_MATH_SHAPE_HPP
#define HEADER_SUPERTUX_MATH_SHAPE_HPP

#include "math/vector.hpp"

class Rectf;

class Shape
{
public:
  enum ShapeType
  {
    POLYGON,
    RECTF
  };
  friend class Rectf;
  friend class Polygon;

public:
  static bool intersect(Shape& shape1, Shape& shape2);

  Shape() {}
  virtual ~Shape() {}

  virtual bool intersects(const Shape* other) const = 0;
  virtual Vector get_penetration_vector(const Shape* other) const = 0;
  virtual Rectf bounding_box() const = 0;

// protected:
  virtual bool partial_intersection_check(const Shape* other) const = 0;

public:
  ShapeType m_type;
};

#endif

/* EOF */
