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

Polygon::Polygon() :
  m_count(0),
  m_untransformed(std::vector<Vector>()),
  m_translation(Vector(0,0)),
  m_angle(0),
  m_transformed(std::vector<Vector>)
{}

Polygon::Polygon(std::vector<Vector> shape) :
  m_count(shape.size()),
  m_untransformed(shape),
  m_translation(Vector(0,0)),
  m_angle(0),
  m_transformed(shape)
{}

Polygon::Polygon(std::vector<Vector> shape, Vector translation, float angle) :
  m_count(shape.size()),
  m_untransformed(shape),
  m_translation(translation),
  m_angle(angle),
  m_transformed(shape)
{
    set_translation(translation);
    set_rotation(angle);
}

Polygon::Polygon(Polygon shape, Vector translation, float angle) :
  m_count(shape.get_count()),
  m_untransformed(shape.m_untransformed),
  m_translation(translation),
  m_angle(angle),
  m_transformed(shape.m_transformed)
{
  set_translation(translation);
  set_rotation(angle);
}

/* EOF */
