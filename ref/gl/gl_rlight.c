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

typedef struct rt_sun_light_t
{
    RgColor4DPacked32 pcolor;
    float             intensity;
    float             pitch;
    float             angle;
} rt_sun_light_t;

typedef struct rt_static_light_t
{
    vec3_t            abs_position;
    vec3_t            dir;
    RgColor4DPacked32 pcolor;
    float             intensity;
    uint32_t          index;
    int               light_style;
    qboolean          is_spot;
} rt_static_light_t;

#define RT_MAX_STATIC_LIGHTS        256
#define RT_MAX_POTENTIAL_SUN_LIGHTS 16

struct
{
    // Need to save on the map start to add them in each frame
    rt_static_light_t static_lights[ RT_MAX_STATIC_LIGHTS ];
    int               static_lights_count;

    rt_sun_light_t potential_sun[ RT_MAX_POTENTIAL_SUN_LIGHTS ];
    uint32_t       potential_sun_count;

    rt_sun_light_t sun;
    qboolean       sun_exists;
} g_lights;


static void AddStaticLight( const vec3_t      abs_position,
                            const vec3_t      dir,
                            RgColor4DPacked32 pcolor,
                            float             intensity,
                            int               light_style,
                            qboolean          is_spot )
{
    uint32_t index = g_lights.static_lights_count;
    assert( index < RT_MAX_STATIC_LIGHTS );

    rt_static_light_t* dst = &g_lights.static_lights[ index ];
    {
        VectorCopy( abs_position, dst->abs_position );
        VectorCopy( dir, dst->dir );
        dst->pcolor      = pcolor;
        dst->intensity   = intensity;
        dst->index       = index;
        dst->light_style = light_style;
        dst->is_spot     = is_spot;
    }

    g_lights.static_lights_count++;
}


static void AddPotentialSun( float pitch, float angle, RgColor4DPacked32 pcolor, float intensity )
{
    uint32_t index = g_lights.potential_sun_count;
    assert( index < RT_MAX_POTENTIAL_SUN_LIGHTS );

    rt_sun_light_t* dst = &g_lights.potential_sun[ index ];
    {
        dst->pcolor    = pcolor;
        dst->intensity = intensity;
        dst->pitch     = pitch;
        dst->angle     = angle;
    }

    g_lights.potential_sun_count++;
}


static void FilterTheSunFromPotential( const char* mapname )
{
    g_lights.sun_exists = false;

    // for now, just get the first one
    if( g_lights.potential_sun_count > 0 )
    {
        g_lights.sun_exists = true;
        memcpy( &g_lights.sun, &g_lights.potential_sun[ 0 ], sizeof( rt_sun_light_t ) );
    }
}


void RT_ParseStaticLightEntities()
{
    g_lights.static_lights_count = 0;
    g_lights.potential_sun_count = 0;
    g_lights.sun_exists          = 0;

    const model_t* const world   = WORLDMODEL;
    assert( world );

    enum
    {
        LT_CLASS_UNKNOWN,
        LT_CLASS_POINT,
        LT_CLASS_SPOT,
        LT_CLASS_SUN_POTENTIAL,
    } classname = LT_CLASS_UNKNOWN;

    struct
    {
        vec3_t            origin;
        vec3_t            allangles;
        RgColor4DPacked32 pcolor;
        float             intensity;
        float             pitch;
        float             angle;
        int               style;
    } light = { 0 };

    enum
    {
        LT_HAS_CLASS     = 1,
        LT_HAS_ORIGIN    = 2,
        LT_HAS_COLOR     = 4,
        LT_HAS_PITCH     = 8,
        LT_HAS_ANGLE     = 16,
        LT_HAS_SKY       = 32,
        LT_HAS_STYLE     = 64,
        LT_HAS_ALLANGLES = 128,
    };


    uint32_t has = 0;
    char*    pos = world->entities;

    for( ;; )
    {
        string key, value;

        pos = COM_ParseFile( pos, key, sizeof( key ) );
        if( pos == NULL )
        {
            break;
        }

        if( key[ 0 ] == '{' )
        {
            classname = LT_CLASS_UNKNOWN;
            has       = 0;
            continue;
        }

        if( key[ 0 ] == '}' )
        {
            if( !( has & LT_HAS_CLASS ) )
            {
                continue;
            }

            if( ( classname == LT_CLASS_POINT || classname == LT_CLASS_SPOT ) &&
                !( has & LT_HAS_SKY ) )
            {
                qboolean has_info = !!( has & LT_HAS_ORIGIN ) && !!( has & LT_HAS_COLOR );

                qboolean has_light_style = !!( has & LT_HAS_STYLE );

                vec3_t direction = { 0 };

                if( classname == LT_CLASS_SPOT )
                {
                    if( has & LT_HAS_ALLANGLES )
                    {
                        AngleVectors( light.allangles, direction, NULL, NULL );
                    }
                    else
                    {
                        has_info = false;
                    }
                }

                if( has_info )
                {
                    AddStaticLight( light.origin,
                                    direction,
                                    light.pcolor,
                                    light.intensity,
                                    has_light_style ? light.style : 0,
                                    classname == LT_CLASS_SPOT );
                }
            }
            else if( classname == LT_CLASS_SUN_POTENTIAL || ( has & LT_HAS_SKY ) )
            {
                qboolean has_info = !!( has & LT_HAS_COLOR );

                float pitch = 0;
                float angle = 0;

                if( has & LT_HAS_PITCH )
                {
                    pitch = light.pitch;
                }

                if( has & LT_HAS_ANGLE )
                {
                    angle = light.angle;
                }

                if( has_info )
                {
                    AddPotentialSun( pitch, angle, light.pcolor, light.intensity );
                }
            }

            continue;
        }

        pos = COM_ParseFile( pos, value, sizeof( value ) );
        if( pos == NULL )
        {
            break;
        }

        if( Q_strcmp( key, "origin" ) == 0 )
        {
            vec3_t    tmp_origin;
            const int components =
                sscanf( value, "%f %f %f", &tmp_origin[ 0 ], &tmp_origin[ 1 ], &tmp_origin[ 2 ] );
            if( components == 3 )
            {
                VectorCopy( tmp_origin, light.origin );
                has |= LT_HAS_ORIGIN;
            }
        }
        else if( Q_strcmp( key, "angles" ) == 0 )
        {
            vec3_t    tmp_allangles;
            const int components = sscanf(
                value, "%f %f %f", &tmp_allangles[ 0 ], &tmp_allangles[ 1 ], &tmp_allangles[ 2 ] );
            if( components == 3 )
            {
                VectorCopy( tmp_allangles, light.allangles );
                has |= LT_HAS_ALLANGLES;
            }
        }
        else if( Q_strcmp( key, "_light" ) == 0 )
        {
            vec3_t    tmp_color;
            float     scale;
            const int components = sscanf(
                value, "%f %f %f %f", &tmp_color[ 0 ], &tmp_color[ 1 ], &tmp_color[ 2 ], &scale );
            if( components == 1 )
            {
                float val = tmp_color[ 0 ];

                light.pcolor =
                    rgUtilPackColorFloat4D( val / 255.0f, val / 255.0f, val / 255.0f, 1.0f );
                light.intensity = 1.0f;

                has |= LT_HAS_COLOR;
            }
            else if( components == 3 )
            {
                light.pcolor    = rgUtilPackColorFloat4D( tmp_color[ 0 ] / 255.0f,
                                                       tmp_color[ 1 ] / 255.0f,
                                                       tmp_color[ 2 ] / 255.0f,
                                                       1.0f );
                light.intensity = 1.0f;

                has |= LT_HAS_COLOR;
            }
            else if( components == 4 )
            {
                light.pcolor    = rgUtilPackColorFloat4D( tmp_color[ 0 ] / 255.0f,
                                                       tmp_color[ 1 ] / 255.0f,
                                                       tmp_color[ 2 ] / 255.0f,
                                                       1.0f );
                light.intensity = scale / 255.0f;

                has |= LT_HAS_COLOR;
            }
        }
        else if( Q_strcmp( key, "classname" ) == 0 )
        {
            if( Q_strcmp( value, "light" ) == 0 )
            {
                classname = LT_CLASS_POINT;
            }
            else if( Q_strcmp( value, "light_spot" ) == 0 )
            {
                classname = LT_CLASS_SPOT;
            }
            else if( Q_strcmp( value, "light_environment" ) == 0 )
            {
                classname = LT_CLASS_SUN_POTENTIAL;
            }

            has |= LT_HAS_CLASS;
        }
        else if( Q_strcmp( key, "pitch" ) == 0 )
        {
            float     tmp;
            const int components = sscanf( value, "%f", &tmp );

            if( components == 1 )
            {
                light.pitch = tmp;
                has |= LT_HAS_PITCH;
            }
        }
        else if( Q_strcmp( key, "angle" ) == 0 )
        {
            float     tmp;
            const int components = sscanf( value, "%f", &tmp );

            if( components == 1 )
            {
                light.angle = tmp;
                has |= LT_HAS_ANGLE;
            }
        }
        else if( Q_strcmp( key, "_sky" ) == 0 )
        {
            int       tmp;
            const int components = sscanf( value, "%i", &tmp );

            if( components == 1 && tmp != 0 )
            {
                has |= LT_HAS_SKY;
            }
        }
        else if( Q_strcmp( key, "style" ) == 0 )
        {
            int       tmp;
            const int components = sscanf( value, "%i", &tmp );

            if( components == 1 )
            {
                light.style = tmp;
                has |= LT_HAS_STYLE;
            }
        }
    }

    FilterTheSunFromPotential( WORLDMODEL->name );
}


#define RT_ARRAYSIZE( arr ) ( sizeof( arr ) / sizeof( arr[ 0 ] ) )


static float GetLightStyleIntensity( int light_style )
{
    assert( light_style >= 0 && light_style < ( int )RT_ARRAYSIZE( g_lightstylevalue ) );
    // from CL_LightVecInternal and R_RecursiveLightPoint, assuming lm = (1, 1, 1)

    uint style_scale = g_lightstylevalue[ light_style ];

    return ( float )bound( 0, style_scale, 255 ) / 255.0f;
}


static qboolean IsPlayerFlashlight( const dlight_t* l )
{
    int flashlight_key = RT_CVAR_TO_INT32( _rt_flsh_key );
    return l->key == flashlight_key;
}


static void CalculateFlaslightPosition( vec3_t out_position )
{
    vec3_t u;
    VectorCopy( RI.vup, u );
    VectorScale( u, METRIC_TO_QUAKEUNIT( RT_CVAR_TO_FLOAT( rt_flsh_u ) ), u );

    vec3_t r;
    VectorCopy( RI.vright, r );
    VectorScale( r, METRIC_TO_QUAKEUNIT( RT_CVAR_TO_FLOAT( rt_flsh_r ) ), r );

    VectorCopy( RI.vieworg, out_position );
    VectorAdd( out_position, u, out_position );
    VectorAdd( out_position, r, out_position );
}


static float FixupAreaLightIntensity()
{
    const float rt_globallightmult      = 1.0f;
    const float rt_quake_light_area_fix = 1.0f / ( QUAKEUNIT_IN_METERS * QUAKEUNIT_IN_METERS );

    return rt_globallightmult * rt_quake_light_area_fix;
}

    #define VectorPow( in, pw, out )                  \
        ( ( out )[ 0 ] = powf( ( in )[ 0 ], ( pw ) ), \
          ( out )[ 1 ] = powf( ( in )[ 1 ], ( pw ) ), \
          ( out )[ 2 ] = powf( ( in )[ 2 ], ( pw ) ) )


    #define RT_IDBASE_SUN         0
    #define RT_IDBASE_FLASHLIGHT  256
    #define RT_IDBASE_DLIGHT      512
    #define RT_IDBASE_ELIGHT      768
    #define RT_IDBASE_STATICLIGHT 1024


void RT_UploadAllLights( void )
{
    rt_state.flashlight_uniqueid = 0;

    if( g_lights.sun_exists && RI.isSkyVisible )
    {
        rt_sun_light_t* sun = &g_lights.sun;

        vec3_t angles = { -sun->pitch, sun->angle, 0 };
        vec3_t direction;
        AngleVectors( angles, direction, NULL, NULL );

        RgDirectionalLightUploadInfo info = {
            .uniqueID               = RT_IDBASE_SUN,
            .isExportable           = true,
            .color                  = sun->pcolor,
            .intensity              = sun->intensity * RT_CVAR_TO_FLOAT( rt_sun ),
            .direction              = RT_VEC3( direction ),
            .angularDiameterDegrees = RT_CVAR_TO_FLOAT( rt_sun_diameter ),
        };

        RgResult r = rgUploadDirectionalLight( rg_instance, &info );
        RG_CHECK( r );
    }

    for( int i = 0; i < g_lights.static_lights_count; i++ )
    {
        const rt_static_light_t* src = &g_lights.static_lights[ i ];

        if( src->is_spot )
        {
            RgSpotLightUploadInfo info = {
                .uniqueID     = RT_IDBASE_STATICLIGHT + src->index,
                .isExportable = true,
                .color        = src->pcolor,
                .intensity    = 300 * src->intensity * GetLightStyleIntensity( src->light_style ) *
                             FixupAreaLightIntensity(),
                .position   = RT_VEC3( src->abs_position ),
                .direction  = RT_VEC3( src->dir ),
                .radius     = METRIC_TO_QUAKEUNIT( 0.1f ),
                .angleOuter = ( float )M_PI / 2,
                .angleInner = 0,
            };

            RgResult r = rgUploadSpotLight( rg_instance, &info );
            RG_CHECK( r );
        }
        else
        {
            RgSphericalLightUploadInfo info = {
                .uniqueID     = RT_IDBASE_STATICLIGHT + src->index,
                .isExportable = true,
                .color        = src->pcolor,
                .intensity =
                    300 * GetLightStyleIntensity( src->light_style ) * FixupAreaLightIntensity(),
                .position = RT_VEC3( src->abs_position ),
                .radius   = METRIC_TO_QUAKEUNIT( 0.1f ),
            };

            RgResult r = rgUploadSphericalLight( rg_instance, &info );
            RG_CHECK( r );
        }
    }

    for( qboolean is_e_light = false; is_e_light <= true; is_e_light++ )
    {
        for( int i = 0; i < ( is_e_light ? MAX_ELIGHTS : MAX_DLIGHTS ); i++ )
        {
            dlight_t* l = is_e_light ? &tr.elights[ i ] : &gp_dlights[ i ];

            if( !l || l->dark )
            {
                continue;
            }

            if( l->die < gp_cl->time || !l->radius )
            {
                continue;
            }


            if( IsPlayerFlashlight( l ) )
            {
                vec3_t pos;
                CalculateFlaslightPosition( pos );

                RgSpotLightUploadInfo info = {
                    .uniqueID     = RT_IDBASE_FLASHLIGHT + i,
                    .isExportable = false,
                    .color        = rgUtilPackColorByte4D( 220, 243, 255, 255 ),
                    .intensity    = RT_CVAR_TO_FLOAT( rt_flsh ) * FixupAreaLightIntensity(),
                    .position     = RT_VEC3( pos ),
                    .direction    = RT_VEC3( RI.vforward ),
                    .radius       = METRIC_TO_QUAKEUNIT( RT_CVAR_TO_FLOAT( rt_flsh_radius ) ),
                    .angleOuter   = DEG2RAD( RT_CVAR_TO_FLOAT( rt_flsh_angle ) ),
                    .angleInner   = 0,
                };

                RgResult r = rgUploadSpotLight( rg_instance, &info );
                RG_CHECK( r );

                // uploaded flashlight, reset the index, for future reuse
                rt_cvars._rt_flsh_key->value = -1;
                rt_state.flashlight_uniqueid = info.uniqueID;
            }
            else
            {
                float falloff_mult = QUAKEUNIT_TO_METRIC( l->radius );
                // Lerp( 1.0f,
                //       GOLDSRCUNIT_TO_METRIC( l->radius ),
                //       CVAR_TO_FLOAT( rt_cvars.rt_fLightDlightFalloffToIntensity ) );

                RgSphericalLightUploadInfo info = {
                    .uniqueID     = is_e_light ? RT_IDBASE_ELIGHT + i : RT_IDBASE_DLIGHT + i,
                    .isExportable = false,
                    .color     = rgUtilPackColorByte4D( l->color.r, l->color.g, l->color.b, 255 ),
                    .intensity = 300 * falloff_mult * FixupAreaLightIntensity(),
                    .position  = RT_VEC3( l->origin ),
                    .radius    = METRIC_TO_QUAKEUNIT( 0.1f ),
                };

                RgResult r = rgUploadSphericalLight( rg_instance, &info );
                RG_CHECK( r );
            }
        }
    }

}
#endif // XASH_RAYTRACING
