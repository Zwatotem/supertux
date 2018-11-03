//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/sdlbase_video_system.hpp"

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"

SDLBaseVideoSystem::SDLBaseVideoSystem() :
  m_sdl_window(),
  m_desktop_size()
{
  SDL_DisplayMode mode;
  if (SDL_GetDesktopDisplayMode(0, &mode) != 0)
  {
    log_warning << "Couldn't get desktop display mode: " << SDL_GetError() << std::endl;
  }
  else
  {
    m_desktop_size = Size(mode.w, mode.h);
  }
}

SDLBaseVideoSystem::~SDLBaseVideoSystem()
{
  SDL_DestroyWindow(m_sdl_window);
}

void
SDLBaseVideoSystem::set_title(const std::string& title)
{
  SDL_SetWindowTitle(m_sdl_window, title.c_str());
}

void
SDLBaseVideoSystem::set_gamma(float gamma)
{
  Uint16 ramp[256];
  SDL_CalculateGammaRamp(gamma, ramp);
  SDL_SetWindowGammaRamp(m_sdl_window, ramp, ramp, ramp);
}

void
SDLBaseVideoSystem::set_icon(const SDL_Surface& icon)
{
  SDL_SetWindowIcon(m_sdl_window, const_cast<SDL_Surface*>(&icon));
}

Size
SDLBaseVideoSystem::get_window_size() const
{
  Size size;
  SDL_GetWindowSize(m_sdl_window, &size.width, &size.height);
  return size;
}

void
SDLBaseVideoSystem::on_resize(int w, int h)
{
  g_config->window_size = Size(w, h);
  apply_config();
}

void
SDLBaseVideoSystem::apply_video_mode()
{
  if (!g_config->use_fullscreen)
  {
    SDL_SetWindowFullscreen(m_sdl_window, 0);

    Size window_size;
    SDL_GetWindowSize(m_sdl_window, &window_size.width, &window_size.height);
    if (g_config->window_size.width != window_size.width ||
        g_config->window_size.height != window_size.height)
    {
      SDL_SetWindowSize(m_sdl_window, g_config->window_size.width, g_config->window_size.height);
    }
  }
  else
  {
    if (g_config->fullscreen_size.width == 0 &&
        g_config->fullscreen_size.height == 0)
    {
      if (SDL_SetWindowFullscreen(m_sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
      {
        log_warning << "failed to switch to desktop fullscreen mode: "
                    << SDL_GetError() << std::endl;
      }
      else
      {
        log_info << "switched to desktop fullscreen mode" << std::endl;
      }
    }
    else
    {
      SDL_DisplayMode mode;
      mode.format = SDL_PIXELFORMAT_RGB888;
      mode.w = g_config->fullscreen_size.width;
      mode.h = g_config->fullscreen_size.height;
      mode.refresh_rate = g_config->fullscreen_refresh_rate;
      mode.driverdata = nullptr;

      if (SDL_SetWindowDisplayMode(m_sdl_window, &mode) != 0)
      {
        log_warning << "failed to set display mode: "
                    << mode.w << "x" << mode.h << "@" << mode.refresh_rate << ": "
                    << SDL_GetError() << std::endl;
      }
      else
      {
        if (SDL_SetWindowFullscreen(m_sdl_window, SDL_WINDOW_FULLSCREEN) != 0)
        {
          log_warning << "failed to switch to fullscreen mode: "
                      << mode.w << "x" << mode.h << "@" << mode.refresh_rate << ": "
                      << SDL_GetError() << std::endl;
        }
        else
        {
          log_info << "switched to fullscreen mode: "
                   << mode.w << "x" << mode.h << "@" << mode.refresh_rate << std::endl;
        }
      }
    }
  }
}

/* EOF */
