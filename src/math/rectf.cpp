//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#include "math/rectf.hpp"

#include <ostream>

#include "math/rect.hpp"

Rectf::Rectf(const Rect& rect) :
  m_p1(static_cast<float>(rect.left),
     static_cast<float>(rect.top)),
  m_size(static_cast<float>(rect.get_width()),
         static_cast<float>(rect.get_height()))
{
}

bool
Rectf::intersects(const Shape* other) const
{
  auto bbox = other->bounding_box();
  if(other->m_type != RECTF && !intersects(&bbox))
    return false;
  return partial_intersection_check(other) && other->partial_intersection_check(this);
}

bool
Rectf::partial_intersection_check(const Shape* other) const
{
  switch (other->m_type)
  {
  case RECTF:
  {
    const Rectf* other_rectf = static_cast<const Rectf*>(other);
    return other_rectf->get_left() > get_left()
        && other_rectf->get_left() < get_right()
        && other_rectf->get_top() > get_top()
        && other_rectf->get_top() < get_bottom();
  }
  case POLYGON:
    return as_polygon().partial_intersection_check(other);
  default:
    return false;
  }
}

Polygon
Rectf::as_polygon() const
{
  return Polygon(std::vector<Vector> {
    Vector(0,0),
    Vector(get_width(),0),
    Vector(get_width(),get_height()),
    Vector(get_height(),0)
  }, p1(), 0.0f);
}

std::ostream& operator<<(std::ostream& out, const Rectf& rect)
{
  out << "Rectf("
      << rect.get_left() << ", " << rect.get_top() << ", "
      << rect.get_right() << ", " << rect.get_bottom()
      << ")";
  return out;
}

/* EOF */
