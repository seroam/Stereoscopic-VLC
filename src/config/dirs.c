/*****************************************************************************
 * dirs.c: crossplatform directories configuration
 *****************************************************************************
 * Copyright (C) 2009 the VideoLAN team
 *
 * Authors: Antoine Cellerier <dionoea at videolan dot org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>

#include "../libvlc.h"

#include "configuration.h"

/**
 * Determines the shared data directory
 *
 * @return a string (always succeeds). Needs to be freed.
 */
char *__config_GetDataDir( vlc_object_t *p_obj )
{
    char *psz_path = config_GetPsz( p_obj, "data-path" );
    if( psz_path && *psz_path )
        return psz_path;
    free( psz_path );
    return strdup( config_GetDataDirDefault() );
}
