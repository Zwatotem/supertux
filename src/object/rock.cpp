//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "object/rock.hpp"

#include "audio/sound_manager.hpp"
#include "object/explosion.hpp"
#include "object/coin.hpp"
#include "supertux/sector.hpp"
#include "supertux/tile.hpp"
#include "util/reader_mapping.hpp"

namespace {
const std::string ROCK_SOUND = "sounds/brick.wav"; //TODO use own sound.
}

Rock::Rock(const Vector& pos, const std::string& spritename) :
  MovingSprite(pos, spritename),
  ExposedObject<Rock, scripting::Rock>(this),
  physic(),
  on_ground(false),
  last_movement(),
  on_grab_script(),
  on_ungrab_script(),
  m_rock_above(nullptr),
  m_rock_below(nullptr),
  rockState(ROCKSTATE_PHYSICAL)
{
  SoundManager::current()->preload(ROCK_SOUND);
  set_group(COLGROUP_MOVING_STATIC);
}

Rock::Rock(const ReaderMapping& reader) :
  MovingSprite(reader, "images/objects/rock/rock.sprite"),
  ExposedObject<Rock, scripting::Rock>(this),
  physic(),
  on_ground(false),
  last_movement(),
  on_grab_script(),
  on_ungrab_script(),
  m_rock_above(nullptr),
  m_rock_below(nullptr),
  rockState(ROCKSTATE_PHYSICAL)
{
  reader.get("on-grab-script", on_grab_script, "");
  reader.get("on-ungrab-script", on_ungrab_script, "");
  SoundManager::current()->preload(ROCK_SOUND);
  set_group(COLGROUP_MOVING_STATIC);
}

Rock::Rock(const ReaderMapping& reader, const std::string& spritename) :
  MovingSprite(reader, spritename),
  ExposedObject<Rock, scripting::Rock>(this),
  physic(),
  on_ground(false),
  last_movement(),
  on_grab_script(),
  on_ungrab_script(),
  m_rock_above(nullptr),
  m_rock_below(nullptr),
  rockState(ROCKSTATE_PHYSICAL)
{
  if (!reader.get("on-grab-script", on_grab_script)) on_grab_script = "";
  if (!reader.get("on-ungrab-script", on_ungrab_script)) on_ungrab_script = "";
  SoundManager::current()->preload(ROCK_SOUND);
  set_group(COLGROUP_MOVING_STATIC);
}

void
Rock::update(float dt_sec)
{
  switch (rockState)
  {
  case ROCKSTATE_GRABBED:
    if(!is_grabbed())
    {
      rockState = ROCKSTATE_PHYSICAL;
    }
    break;
  case ROCKSTATE_FIXED:
  {
    Rectf bound = m_col.get_bbox().moved(Vector(0,1));
    if (!m_rock_below && Sector::get().is_free_of_statics(bound, this, false))
    {
      // Reset speed etc.
      physic = Physic();
      rockState = ROCKSTATE_PHYSICAL;
      break;
    }
    m_col.m_movement = Vector(0,0);
    if (m_rock_below && m_rock_below->rockState == ROCKSTATE_FIXED)
    {
      Vector their_pos = m_rock_below->get_pos();
      Vector our_pos = their_pos - Vector(0, m_col.get_bbox().get_height());
      set_pos(our_pos);
    }
    if (is_grabbed())
    {
      leave_stack();
    }
    break;
  }
  case ROCKSTATE_PHYSICAL:
    leave_stack();
    on_ground = false;
    m_col.m_movement = physic.get_movement(dt_sec);
    break;
  default:
    break;
  }

  if (is_grabbed())
  {
    rockState = ROCKSTATE_GRABBED;
  }
}

void
Rock::collision_solid(const CollisionHit& hit)
{
  if (is_grabbed()) {
    return;
  }
  if (hit.top || hit.bottom)
    physic.set_velocity_y(0);
  if (hit.left || hit.right) {
    // Bounce back slightly when hitting a wall
    float velx = physic.get_velocity_x();
    physic.set_velocity_x(-0.1f * velx);
  }
  if (hit.crush)
    physic.set_velocity(0, 0);

  if (hit.bottom && !on_ground && !is_grabbed()) {
    bool play_sound = true;
    physic.set_velocity_x(0);
    on_ground = true;
    
    for (auto& obj : Sector::get().get_objects_by_type<Rock>()) {
      Rock* rock = dynamic_cast<Rock*>(&obj);

      if (rock && rock->rockState == ROCKSTATE_FIXED)
      {
        play_sound = false;
        if (get_bbox().grown(1).contains(rock->get_bbox()))
        {
          join_stack(rock);
          break;
        }
      }
    }
    rockState = ROCKSTATE_FIXED;
    if (play_sound)
    {
      SoundManager::current()->play(ROCK_SOUND, get_pos());
    }
  }
  
}

void
Rock::leave_stack()
{
  if (m_rock_above)
  {
    m_rock_above->rockState = ROCKSTATE_PHYSICAL;
    m_rock_above->leave_stack();
  }
  if (m_rock_below)
  {
    m_rock_below->m_rock_above = nullptr;
    m_rock_below = nullptr;
  }
}
bool
Rock::join_stack(Rock* rock)
{
  if (rock == m_rock_above || rock == m_rock_below)
  {
    return false;
  }

  Vector pos = m_col.m_bbox.get_middle();
  if (abs(rock->get_bbox().get_middle().x-pos.x) < m_col.m_bbox.get_width() / 2)
  {
    while (rock->m_rock_above != nullptr)
    {
      rock = rock->m_rock_above;
    }

    Vector their_pos = rock->get_pos();
    Vector our_pos = their_pos - Vector(0, m_col.get_bbox().get_height());
    Rectf new_bbox = Rectf(our_pos,m_col.m_bbox.get_size());
    new_bbox.move(Vector(0,1));
    if (Sector::get().is_free_of_statics(new_bbox,rock))
      set_pos(our_pos);
    else
      return false;
    rock->m_rock_above = this;
    m_rock_below = rock;
    rockState = ROCKSTATE_FIXED;
    m_rock_below->rockState = ROCKSTATE_FIXED;
    return true;
  }
  return false;
}

HitResponse
Rock::collision(GameObject& other, const CollisionHit& hit)
{
  auto heavy_coin = dynamic_cast<HeavyCoin*> (&other);
  if (heavy_coin) {
    return ABORT_MOVE;
  }

  auto explosion = dynamic_cast<Explosion*> (&other);
  if (explosion) {
    return ABORT_MOVE;
  }
  Rock* rock = dynamic_cast<Rock*> (&other);
  if(rock)
  {
    if (rockState == ROCKSTATE_PHYSICAL && rock->rockState == ROCKSTATE_FIXED)
    {
      if (hit.left || hit.right)
      {
        CollisionHit wallHit;
        wallHit.left = hit.left;
        wallHit.right = hit.right;
        collision_solid(wallHit);
        return ABORT_MOVE;
      }
      if(join_stack(rock))
        return ABORT_MOVE;
      else
      {
        rockState = ROCKSTATE_FIXED;
        return FORCE_MOVE;
      }
    }
    if (rockState == ROCKSTATE_FIXED && rock->rockState == ROCKSTATE_PHYSICAL)
    {
      if(rock->join_stack(this))
        return ABORT_MOVE;
      return FORCE_MOVE;
    }
    if (rockState == ROCKSTATE_PHYSICAL && rock->rockState == ROCKSTATE_PHYSICAL)
    {
      if(!on_ground && hit.bottom)
      {
        physic.set_velocity_y(0);
        return CONTINUE;
      }
      return ABORT_MOVE;
    }
    if (rockState == ROCKSTATE_GRABBED || rock->rockState == ROCKSTATE_GRABBED)
    {
      return ABORT_MOVE;
    }
  }
  
  if (is_grabbed()) {
    return ABORT_MOVE;
  }

  // Don't fall further if we are on a rock which is on the ground.
  // This is to avoid jittering.
  auto rock = dynamic_cast<Rock*> (&other);
  if (rock && rock->on_ground && hit.bottom) {
    physic.set_velocity_y(0);
    return CONTINUE;
  }

  if (!on_ground) {
    if (hit.bottom && physic.get_velocity_y() > 200) {
      auto moving_object = dynamic_cast<MovingObject*> (&other);
      if (moving_object) {
        //Getting a rock on the head hurts. A lot.
        moving_object->collision_tile(Tile::HURTS);
        physic.set_velocity_y(0);
      }
    }
    return FORCE_MOVE;
  }

  return FORCE_MOVE;
}

void
Rock::grab(MovingObject& object, const Vector& pos, Direction dir_)
{
  Portable::grab(object, pos, dir_);
  leave_stack();
  m_col.m_movement = pos - get_pos();
  last_movement = m_col.m_movement;
  set_group(COLGROUP_TOUCHABLE); //needed for lanterns catching willowisps
  on_ground = false;
  if (!on_grab_script.empty()) {
    Sector::get().run_script(on_grab_script, "Rock::on_grab");
  }
}

void
Rock::ungrab(MovingObject& object, Direction dir)
{
  set_group(COLGROUP_MOVING_STATIC);
  on_ground = false;
  if (dir == Direction::UP) {
    physic.set_velocity(0, -500);
  } else if (dir == Direction::DOWN) {
    physic.set_velocity(0, 500);
  } else if (last_movement.norm() > 1) {
    physic.set_velocity((dir == Direction::RIGHT) ? 200.0f : -200.0f, -200.0f);
  } else {
    physic.set_velocity(0, 0);
  }
  if (!on_ungrab_script.empty()) {
    Sector::get().run_script(on_ungrab_script, "Rock::on_ungrab");
  }
  Portable::ungrab(object, dir);
}

ObjectSettings
Rock::get_settings()
{
  auto result = MovingSprite::get_settings();
  result.add_script(_("On-grab script"), &on_grab_script, "on-grab-script");
  result.add_script(_("On-ungrab script"), &on_ungrab_script, "on-ungrab-script");
  return result;
}


/* EOF */
