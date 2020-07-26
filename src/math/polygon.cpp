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

#include <ostream>

#include "polygon.hpp"
#include "math/util.hpp"

Polygon::Polygon() :
  m_count(0),
  m_untransformed(std::vector<Vector>()),
  m_translation(Vector(0,0)),
  m_rotation(0),
  m_transformed(std::vector<Vector>())
{}

Polygon::Polygon(std::vector<Vector> shape) :
  m_count(shape.size()),
  m_untransformed(shape),
  m_translation(Vector(0,0)),
  m_rotation(0),
  m_transformed(shape)
{}

Polygon::Polygon(std::vector<Vector> shape, Vector translation, float rotation) :
  m_count(shape.size()),
  m_untransformed(shape),
  m_translation(translation),
  m_rotation(rotation),
  m_transformed(shape)
{
    set_translation(translation);
    set_rotation(rotation);
}

Polygon::Polygon(Polygon shape, Vector translation, float rotation) :
  m_count(shape.get_count()),
  m_untransformed(shape.m_untransformed),
  m_translation(translation),
  m_rotation(rotation),
  m_transformed(shape.m_transformed)
{
  set_translation(translation);
  set_rotation(rotation);
}

void
Polygon::set_translation(Vector translation)
{
  m_translation = translation;
  update_vertices();
}

void
Polygon::add_translation(Vector translation)
{
  m_translation += translation;
  for (auto &&vert : m_transformed)
  {
    vert += translation;
  }
}

void
Polygon::set_rotation(float rotation)
{
  m_rotation = rotation;
  update_vertices();
}

void
Polygon::add_rotation(float rotation)
{
  m_rotation += rotation;
  m_rotation = fmod(m_rotation, math::PI);
  update_vertices();
}

Vector
Polygon::get_vertex(int ordinal) const
{
  return m_transformed[ordinal % m_count];
}

std::pair<Vector, Vector>
Polygon::get_edge(int ordinal) const
{
  return std::pair<Vector, Vector>(get_vertex(ordinal % m_count), get_vertex((ordinal + 1) % m_count));
}

Rectf
Polygon::bounding_box() const
{
  float top = m_transformed[0].y;
  float bottom = m_transformed[0].y;
  float left = m_transformed[0].x;
  float right = m_transformed[0].x;
  for (auto &&vert : m_transformed)
  {
    top = std::min(top, vert.y);
    bottom = std::max(bottom, vert.y);
    left = std::min(left, vert.x);
    right = std::max(right, vert.x);
  }
  return Rectf(left, top, right, bottom);
}

bool
Polygon::valid() const
{
  if (m_count < 3)
  {
    return false;
  }
  Vector edge = get_vertex(1) - get_vertex(0);
  for (int i = 0; i < m_count; i++)
  {
    Vector next_edge = get_vertex(i + 2) - get_vertex(i + 1);
    if (edge.x * next_edge.y - edge.y * next_edge.x < 0)
    {
      return false;
    }
    edge = next_edge;
  }
  return true;
}

bool
Polygon::contains(Vector point) const
{
  Vector vertex_relative = get_vertex(0) - point;
  for (int i = 0; i < m_count; i++)
  {
    Vector next_vertex = get_vertex(i + 1) - point;
    if (vertex_relative.x * next_vertex.y - vertex_relative.y * next_vertex.x < 0)
    {
      return false;
    }
    vertex_relative = next_vertex;
  }
}

bool
Polygon::empty() const
{
  for (auto &&vert : m_untransformed)
  {
    if (vert != Vector(0,0))
    {
      return false;
    }
  }
  return true;
}

void
Polygon::update_vertices()
{
  for (int i = 0; i < m_count; i++)
  {
    m_transformed[i] = m_untransformed[i].rotated(m_rotation) + m_translation;
  }
}

std::ostream&
operator<<(std::ostream& out, const Polygon& poly)
{
  out << "Polygon("
      << "[" << poly.get_count() << " vertices], "
      << "translation: " << poly.get_translation() << ", "
      << "rotation: " << poly.get_rotation()
      << ")";
  return out;
}

/* EOF */
