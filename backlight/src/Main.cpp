#include "Main.hpp"

pFrame *Current_Frame;
pCamera *Current_Camera;

pTexture Default_Texture;
pBitmap First_Bitmap;

cSettings_Global Globals;
cTexture SkySphereSurface;
lift::vector<double, 3> SkySphereVelocity;

int main(int argc, char *argv[]) {
  cScene World;
  cTexture Default;

  printf(
      "\n    BACKLIGHT Relativistic Raytracer v2.9\n\n    Copyright (c) Antony "
      "Searle 1997-8\n\n");

  // Look at arguments...

  if (argc != 2) {
    printf("Syntax: BackLight [input file name]\n");
    exit(0);
  }

  // Setup some globals...

  First_Bitmap = NULL;

  Default_Texture = &Default;

  SkySphereSurface = Default;
  SkySphereVelocity = lift::make_vector<double>(0, 0, 0);

  World.Load(argv[1]);

  World.Raytrace();

  return (EXIT_SUCCESS);
}

// Compiler notes

// CURRENT:

// Attempting to fix shadow speckle on coincident surfaces; attempting to clip
// away
// non-origin surfaces on shadow rays over short distances.

// GLOBAL FIXUPS:
// Currently, the interior surfaces of unions show m_up in differences where
// they shouldn't.
// This is hard to fix - is a parenting solution the answer?

// Notes:

// Verbose [1]
// 0 -> NO OUTPUT  (EXCEPT ERRORS)
// 1 -> INTERESTING / STRUCTURAL OUTPUT (WARNINGS)
// 2 -> MOST STUFF
// 3 -> RIDICULOUS STUFF
// 4 -> EVERYTHING!!!!

// Note that non-zero verbose settings can severely impare the parsing
// performance of the
// program for complex scenes (it will spew forth megabytes of text!)

// KNOWN BUG: In Win95, it can max out the 16Mb of DPMI memory available. So,
// if you get "Virtual memory exceeded by `new'" then alter the properties
// of the program (or DOS box it runs from) to type in 65535 DPMI memory,
// If that fails: Check you have a few 10s of Mb free on the hard drive, or
// run in DOS with CWSDPMI

// BIG PROBLEM: The parser chews m_up vast amounts of memory making 20-byte
// tokens out of files. It needs a total overhaul, and probably the
// abandonment of the entire system

// Look into hardcoding or #including a series of #declares to enhance
// compatibility, e.g(). x = < 1, 0, 0 >, y, z, on, off, and to enhance ease of
// use e.g(). map_Planar = 0, map_Spherical = 1

// Hmm... Perhaps the cameras should be treated like light_sources to allow
// transformations and nesting...? Yes, this allows their attachment to objects,
// a
// desireable effect.

// Make no_shadow recursive to higher levels

// AA looks suspect - why? / fix

// Make textures work in frame {}

// Look m_up the total spectral reflection properties of the average surface

// A lesson from POV - the camera (after m_look_at) is wrong-handed

/* Add these bits...

    ? Jitter=bool
    ? Jitter_Amount=n.n
    ?? camera focal blur
    ? height_field  Can do as a group of unions..., but will take forever
    ? torus         Can I write a 4th order iterative solver?
    ?? disc         Why ?
    ? mesh          Just use to mask a union (maybe)
    ?? area lights
    ? spotlight     Make it a texture!
    ?? clipped_by
    ? IMPLEMENT REFRACTION -> ?? caustics
    ?? finish(fade_distance, fade_power)

    Display=bool
    Preview_Start_Size=n
    Preview_End_Size=n

    // By default, on, off, on , on

    Relativity=bool     // Gives all effects
    Observation=bool    // Gives observation, not sight

 */
