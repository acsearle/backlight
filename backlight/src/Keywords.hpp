// keywords.hpp

#ifndef KEYWORDS_HPP
#define KEYWORDS_HPP

#include "Global.hpp"

enum Identifier {
  ID_frame,
  ID_velocity,
  ID_union,
  ID_merge,
  ID_intersection,
  ID_difference,
  ID_inverse,
  ID_triangle,
  ID_smooth_triangle,
  ID_sphere,
  ID_box,
  ID_cylinder,
  ID_cone,
  ID_plane,
  ID_quadric,
  ID_bounded_by,
  ID_translate,
  ID_scale,
  ID_rotate,
  ID_matrix,
  ID_texture,
  ID_pigment,
  ID_normal,
  ID_finish,
  ID_tga,
  ID_image_map,
  ID_bump_map,
  ID_map_type,
  ID_interpolate,
  ID_intensity,
  ID_doppler,
  ID_bump_size,
  ID_ambient,
  ID_diffuse,
  ID_brilliance,
  ID_specular,
  ID_roughness,
  ID_reflection,
  ID_refraction,
  ID_ior,
  ID_light_source,
  ID_colour,
  ID_color,
  ID_rgb,
  ID_rgbt,
  ID_rgbf,
  ID_rgbtf,
  ID_rgbft,
  ID_red,
  ID_blue,
  ID_green,
  ID_transmit,
  ID_filter,
  ID_camera,
  ID_perspective,
  ID_orthographic,
  ID_fisheye,
  ID_location,
  ID_look_at,
  ID_right,
  ID_up,
  ID_direction,
  ID_sky,
  ID_angle,
  ID_frequency,
  ID_phase,
  ID_Width,
  ID_Height,
  ID_Output_File_Name,
  ID_mercator,
  ID_fade_power,
  ID_fade_distance,
  ID_Verbose,
  ID_Antialias,
  ID_Sampling_Method,
  ID_Antialias_Threshold,
  ID_Antialias_Depth,
  ID_global_settings,
  ID_adc_bailout,
  ID_assumed_gamma,
  ID_max_trace_level,
  ID_background,
  ID_sky_sphere,
  ID_no_shadow,
  ID_shadowless,
  ID_phong,
  ID_phong_size,
  ID_once,
  ID_open,
  ID_default,
  ID_Doppler,
  ID_Intensity,
  ID_drift,
  ID_Display,
  ID_metallic,
  ID_Window_l,
  ID_Window_r,
  ID_Window_t,
  ID_Window_b,
  ID_Window_Doppler,
  ID_Window_Intensity,
  ID_HUD,
  ID_lens_flare,
  ID_Continue,

  ID_ARRAY_SIZE,  // This marks the last words we want
                  // parsed normally
  ID_FLOAT,       // Data holders
  ID_DOUBLE,
  ID_VECTOR,
  ID_COLOUR,
  ID_MATRIX,
  ID_STRING,

  ID_LEFT_BRACE,   // Punctuation (single characters which do
  ID_RIGHT_BRACE,  // not need whitespace between them)
  ID_LEFT_ANGLE,
  ID_RIGHT_ANGLE,
  ID_COMMA,
  ID_EQUALS,

  ID_include,  // These are preparse directives - require
  ID_define,   // the tokeniser to jump through hoops

  ID_END
};  // This caps linked lists of tokens

// We setup a simple structure to group Identifiers and their corresponding
// keywords

struct String_Index {
  Identifier m_ID;
  const char* m_String;
};

extern String_Index Keyword[];

#endif
