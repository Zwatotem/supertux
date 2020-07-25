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

#include "polygon.hpp"
#include "math/util.hpp"

Polygon::Polygon() :
  m_count(0),
  m_untransformed(std::vector<Vector>()),
  m_translation(Vector(0,0)),
  m_rotation(0),
  m_transformed(std::vector<Vector>)
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

void
Polygon::update_vertices()
{
  for (int i = 0; i < m_count; i++)
  {
    m_transformed[i] = m_untransformed[i].rotated(m_rotation) + m_translation;
  }
}

/* EOF */
