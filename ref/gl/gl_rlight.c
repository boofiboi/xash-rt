/*
gl_rlight.c - dynamic and static lights
Copyright (C) 2010 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "gl_local.h"
#include "pm_local.h"
#include "xash3d_mathlib.h"
#include "ref_params.h"

#if XASH_RAYTRACING
// because of units are not in meters
#define RT_QUAKE_LIGHT_AREA_INTENSITY_FIX \
	( 1.0f / ( QUAKEUNIT_IN_METERS * QUAKEUNIT_IN_METERS ) )
#define RT_FIXUP_LIGHT_INTENSITY( color, witharea )                                     \
	do                                                                                  \
	{                                                                                   \
		const float rt_globallightmult = 1.0f;                                          \
		float       area = ( witharea ) ? RT_QUAKE_LIGHT_AREA_INTENSITY_FIX : 1.0f;     \
		VectorScale( ( color ), ( rt_globallightmult )*area, ( color ) );               \
	} while( 0 )

void RT_UploadAllLights( void )
{
	for( int i = 0; i < MAX_DLIGHTS; i++ )
	{
		const dlight_t *l = &gp_dlights[i];

		if( l->die < gp_cl->time || !l->radius )
			continue;

		if( l->dark )
			continue;

		float falloff_mult = QUAKEUNIT_TO_METRIC( l->radius );

		vec3_t intensity = { 300.0f * falloff_mult,
							 300.0f * falloff_mult,
							 300.0f * falloff_mult };
		RT_FIXUP_LIGHT_INTENSITY( intensity, true );

		RgSphericalLightUploadInfo info = {
			.uniqueID     = 128 + i,
			.isExportable = false,
			.extra        = { 0 },
			.color        = rgUtilPackColorByte4D( l->color.r, l->color.g, l->color.b, 255 ),
			.intensity    = intensity[ 0 ],
			.position     = { l->origin[ 0 ], l->origin[ 1 ], l->origin[ 2 ] },
			.radius       = METRIC_TO_QUAKEUNIT( 0.1f ),
		};

		RgResult r = rgUploadSphericalLight( rg_instance, &info );
		RG_CHECK( r );
	}
}
#endif // XASH_RAYTRACING
