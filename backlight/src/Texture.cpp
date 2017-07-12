//
// Raytracing Functions
//

#include "Texture.hpp"
#include "Map.hpp"

cPigment::cPigment() {
  m_map_type = MT_COLOUR;

  m_doppler = 1.0;
  m_intensity = 1.0;

  // Globals.MemoryUsed() += sizeof(cPigment);
};

cPigment::~cPigment() {
  delete m_bitmap;

  Globals.MemoryUsed() -= sizeof(cPigment);
}

/*
cFColour cPigment::GetPigment(cEvent o) {
    double r;
    double u = 0, v = 0;
    cFColour s;

    switch(m_map_type) {

    case MT_NONE:
        return cFColour( 1, 1, 1, 0, 0 );
        break;

    case MT_COLOUR:
        return m_colour;
        break;

    case WT_PLANAR:
        o[0] = m_Transformation.TransformLocation(o[0]);
        u = o[0];
        v = o[1];
        break;

    case WT_SPHERICAL:
        o[0] = m_Transformation.TransformLocation(o[0]);
        if((o[0] != 0) || (o[2] != 0)) {
            u = atan2(o[0], o[2]) / (2 *PI);
            v = atan2(o[1], sqrt(o[0] *o[0] + o[2] *o[2])) / PI + 0.5;
        }
        break;
    case WT_CYLINDRICAL:
        o[0] = m_Transformation.TransformLocation(o[0]);
        if((o[0] != 0) || (o[2] != 0)) {
            u = atan2(o[0], o[2]) / (2 *PI);
        }
        v = o[1];
        break;

    case WT_TOROIDAL:   // Not much point without a torus!
        o[0] = m_Transformation.TransformLocation(o[0]);
        if((o[0] != 0) || (o[2] != 0)) {
            r = sqrt(o[0] *o[0] + o[1] *o[1]) - 1;
            if((r != 0) || (o[1] != 0)) {
                u = atan2(o[0], o[2]) / (2 *PI);
                v = atan2(o[1], r) / (2 *PI);
            }
        }
        break;

    case WT_DISK:
        o[0] = m_Transformation.TransformLocation(o[0]);
        if((o[0] != 0) || (o[2] != 0)) {
            u = atan2(o[0], o[2]) / (2 *PI);
        }
        v = sqrt((o[0] * o[0]) + (o[2] * o[2]));
        break;

    default:
        printf("Unknown map type %li in cTexture::GetPigment\n", m_map_type);
        break;
    }

    u *= PEEK_MAP(p_map)->frequency()[0];
    v *= PEEK_MAP(p_map)->frequency()[1];
    u += PEEK_MAP(p_map)->phase()[0];
    v += PEEK_MAP(p_map)->phase()[1];
    u += o.t() * PEEK_MAP(p_map)->drift()[0];
    v += o.t() * PEEK_MAP(p_map)->drift()[1];

    if(m_once) {
        if((u < 0) || (v < 0) || (u > 1) || (v > 1)) {
            return cFColour( 1, 1, 1, 0, 1 ); // Transparent
        }
    }

    s = m_bitmap->GetPixel(u, v, m_interpolate);
    s.f() *= m_colour.f();
    s.t() *= m_colour.t();

    return s;

}
 */

//////////////////////////////////////////////////////////////////////////////////
////
////    cNormal implementation
////
//////////////////////////////////////////////////////////////////////////////////

cNormal::cNormal() {
  m_map_type = MT_NONE;
  m_bump_size = 0;

  // Globals.MemoryUsed() += sizeof(cNormal);
};

cNormal::~cNormal() {
  delete m_bitmap;

  Globals.MemoryUsed() -= sizeof(cNormal);
}

lift::vector<double, 3> cNormal::GetNormal(cEvent o) {
  cCoordinate_Frame f;
  lift::vector<double, 3> du;
  double u = 0, v = 0;

  /*
    lift::vector<double, 3> du, dv;
    double u = 0, v = 0, r;

    du = lift::make_vector<double>( 1, 0, 0 );
    dv = lift::make_vector<double>( 0, 1, 0 );


    // FIXUP: When you want to m_interpolate splines & differentiate them
    // in 2D space etc.

    switch(m_map_type) {

    case WT_PLANAR:
        o[0] = m_Transformation.TransformLocation(o[0]);
        u = o[0];
        v = o[1];
        break;
    case WT_SPHERICAL:
        o[0] = m_Transformation.TransformLocation(o[0]);

        if((o[0] != 0) || (o[2] != 0)) {
            u = atan2(o[0], o[2]) / (2 *PI);
            v = atan2(o[1], sqrt(o[0] *o[0] + o[2] *o[2])) / PI + 0.5;
            du = lift::make_vector<double>( cos(u * 2 * PI), 0, sin(u * 2 * PI)
    );
            dv = lift::make_vector<double>( sin(u * 2 * PI) *cos(v * PI), sin(v
    * PI), cos(u * 2 * PI) *cos(v * PI) );
        }
        break;
    case WT_CYLINDRICAL:
        o[0] = m_Transformation.TransformLocation(o[0]);

        if((o[0] != 0) || (o[2] != 0)) {
            u = atan2(o[0], o[2]) / (2 *PI);
            du = lift::make_vector<double>( cos(u * 2 * PI), 0, sin(u * 2 * PI)
    );
        }
        v = o[1];
        break;

    case MT_TOROIDAL:   // Not much point without a torus!
        o[0] = m_Transformation.TransformLocation(o[0]);

        if((o[0] != 0) || (o[2] != 0)) {
            r = sqrt(o[0] *o[0] + o[1] *o[1]) - 1;
            if((r != 0) || (o[1] != 0)) {
                u = atan2(o[0], o[2]) / (2 *PI);
                v = atan2(o[1], r) / (2 *PI);

                // FIXUP: Vectors for torus...

            }
        }
        break;

    case MT_DISK:
        o[0] = m_Transformation.TransformLocation(o[0]);

        if((o[0] != 0) || (o[2] != 0)) {
            u = atan2(o[0], o[2]) / (2 *PI);
            du = lift::make_vector<double>( cos(u * 2 * PI), 0, sin(u * 2 * PI)
    );
            v = sqrt((o[0] * o[0]) + (o[2] * o[2]));
            dv = lift::make_vector<double>( o[0] / v, 0, o[2] / v );
        }

        break;

    default:
        return lift::make_vector<double>( 0, 0, 0 );
        break;
    }

    u *= PEEK_MAP(p_map)->frequency()[0];
    v *= PEEK_MAP(p_map)->frequency()[1];
    u += PEEK_MAP(p_map)->phase()[0];
    v += PEEK_MAP(p_map)->phase()[1];
    u += PEEK_MAP(p_map)->drift()[0] * o.t();
    v += PEEK_MAP(p_map)->drift()[1] * o.t();


    if(m_once) {
        if((u < 0) || (v < 0) || (u > 1) || (v > 1)) {
            return lift::make_vector<double>( 0, 0, 0 );
        }
    }

 */

  m_bitmap->GetBump(u, v, m_interpolate);

  // The purpose of u, v has changed radically

  Evaluate_Frame(o, f);

  du = ((f.du() * u) + (f.dv() * v)) * m_bump_size;

  return UnApply_Transforms(cEvent(du, o.t())).p();
}

bool cNormal::Test(pToken T) {
  switch (T->ID()) {
    case ID_normal:
    case ID_bump_map:
    case ID_map_type:
    case ID_tga:
    case ID_interpolate:
    case ID_bump_size:
    case ID_once:
    case ID_frequency:
    case ID_phase:
    case ID_drift:
      return true;
      break;
    default:
      return false;
  }
};

pToken cNormal::Parse(pToken T, bool& wasNormal) {
  wasNormal = true;
  switch (T->ID()) {
    case ID_normal:
    case ID_bump_map:
    case ID_map_type:
    case ID_tga:
    case ID_interpolate:
    case ID_bump_size:
    case ID_once:
    case ID_frequency:
    case ID_phase:
    case ID_drift:
      T = T->Next();
      break;
    default:
      wasNormal = false;
  }

  return T;
};

template <class DT>
_inline DT* ensure_ptr(DT*& w) {
  if (NULL == w) w = new DT;
  return w;
};

#define PEEK_MAP(x) ensure_ptr<cMap>(x)

pToken cNormal::Load(pToken T) {
  pToken S;
  long Depth;

  pMatrix p_mat = NULL;
  pMap p_map = NULL;
  ptMatrix p_transM = NULL;
  ptWarp p_transW = NULL;
  pTransform p_trans = NULL;

  // if we don't start with a texture keyword, there needs to be a brace!
  if (!Test(T)) {
    EXPECTED(T, LEFT_BRACE);
    Depth = 1;
  } else {
    Depth = 0;
  };

  // now look for the optional stuff
  do {
    S = T;

    // Parse the possible transform statements
    T = ctTransform::Parse(T, p_trans);
    if (NULL != p_trans) {
      T = p_trans->Load(T);

      // push it onto the transform stack, but keep a reference using p_transM
      buffer.push_back(p_trans);

      continue;
    };

    switch (T->ID()) {
      case ID_LEFT_BRACE:
        T = T->Next();
        Depth++;
        break;

      case ID_RIGHT_BRACE:
        if (Depth > 0) T = T->Next();
        Depth--;
        break;

      case ID_bump_map:
        T = T->Next();
        // We don't really need to worry too much about syntax,
        // so sweep it under the carpet...
        T = Load(T);
        break;

      case ID_map_type:
        ctTransform::Parse(T, p_trans);
        if (NULL != p_trans) buffer.push_back(p_trans);
        break;

      case ID_tga:
        T = T->Next();
        if (T->ID() != ID_STRING) {
          PrintToken(T);
          printf("found, expected \"filename.tga\" (\"%s\",%li)\n",
                 T->Filename(), T->Line());
          assert(false);
        }
        cBitmap::Request(&m_bitmap, (char*)T->Data());
        T = T->Next();

        if (m_map_type < 0) {
          m_map_type = 0;
        }

        break;

      case ID_interpolate:
        T = T->Next();
        EXPECT(T, DOUBLE);
        m_interpolate = (long int)*((double*)T->Data());
        T = T->Next();
        break;

      case ID_bump_size:
        T = T->Next();
        EXPECT(T, DOUBLE);
        m_bump_size = *((double*)T->Data());
        T = T->Next();
        break;

      case ID_once:
        T = T->Next();
        m_once = !m_once;
        break;

      case ID_frequency:
        T = T->Next();

        T = ::Load(m_frequency, T);
        break;

      case ID_phase:
        T = T->Next();

        T = ::Load(m_phase, T);
        break;

      case ID_drift:
        T = T->Next();

        T = ::Load(m_drift, T);
        break;
    };

    if (S == T) {
      // no tokens found
      if (Depth > 0) {
        PrintToken(T);
        printf("found in Texture definition(\"%s\",%li)\n", T->Filename(),
               T->Line());
        assert(false);
      }
    };

  } while (Depth > 0);

  return T;
}

//////////////////////////////////////////////////////////////////////////////////
////
////    cPigment implementation
////
//////////////////////////////////////////////////////////////////////////////////

bool cPigment::Test(pToken T) {
  switch (T->ID()) {
    case ID_pigment:

    case ID_image_map:  // sub entries
    case ID_map_type:
    case ID_tga:
    case ID_interpolate:
    case ID_doppler:
    case ID_intensity:
    case ID_once:
    case ID_frequency:
    case ID_phase:
    case ID_drift:

    case ID_colour:  // from cFColour sub-object
    case ID_color:
    case ID_red:
    case ID_blue:
    case ID_green:
    case ID_rgb:
    case ID_rgbf:
    case ID_rgbt:
    case ID_rgbft:
    case ID_rgbtf:
    case ID_filter:
    case ID_transmit:
      return true;
      break;
    default:
      return false;
  }
}

pToken cPigment::Parse(pToken T, bool& wasPigment) {
  wasPigment = true;
  switch (T->ID()) {
    case ID_pigment:
      T = T->Next();
      break;

    case ID_image_map:  // sub entries
    case ID_map_type:
    case ID_tga:
    case ID_interpolate:
    case ID_doppler:
    case ID_intensity:
    case ID_once:
    case ID_frequency:
    case ID_phase:
    case ID_drift:

    case ID_colour:  // from cFColour sub-object
    case ID_color:
    case ID_red:
    case ID_blue:
    case ID_green:
    case ID_rgb:
    case ID_rgbf:
    case ID_rgbt:
    case ID_rgbft:
    case ID_rgbtf:
    case ID_filter:
    case ID_transmit:
      break;

    default:
      wasPigment = false;
  }

  return T;
}

pToken cPigment::Load(pToken T) {
  pToken S;
  bool wasColour;
  long Depth;

  pMatrix p_mat = NULL;
  pMap p_map = NULL;
  ptMatrix p_transM = NULL;
  ptWarp p_transW = NULL;
  pTransform p_trans = NULL;

  // if we don't start with a texture keyword, there needs to be a brace!
  if (!Test(T)) {
    EXPECTED(T, LEFT_BRACE);
    Depth = 1;
  } else {
    Depth = 0;
  };

  // now look for the optional stuff
  do {
    S = T;

    // Parse the possible transform statements
    T = ctTransform::Parse(T, p_trans);
    if (S != T) {
      // ctTransform::Parse parses and loads ATM
      // T = p_trans->Load(T);

      // push it onto the transform stack, but keep a reference using p_transM
      buffer.push_back(p_trans);

      continue;
    };

    wasColour = m_fcolour.Test(T);
    if (wasColour) {
      T = m_fcolour.Load(T);

      continue;
    };

    switch (T->ID()) {
      case ID_LEFT_BRACE:
        Depth++;
        T = T->Next();
        break;

      case ID_RIGHT_BRACE:
        if (Depth > 0) T = T->Next();
        Depth--;
        break;

      case ID_image_map:
        T = T->Next();
        // We don't really need to worry too much about syntax,
        // so sweep it under the carpet...
        T = Load(T);
        break;

      case ID_map_type:
        ctTransform::Parse(T, p_trans);
        if (NULL != p_trans) buffer.push_back(p_trans);
        break;

      case ID_tga:
        T = T->Next();
        if (T->ID() != ID_STRING) {
          PrintToken(T);
          printf("found, expected \"filename.tga\" (\"%s\",%li)\n",
                 T->Filename(), T->Line());
          assert(false);
        }
        cBitmap::Request(&m_bitmap, (char*)T->Data());
        T = T->Next();

        if (m_map_type < 0) {
          m_map_type = WT_PLANAR;
        };  // default mapping
        break;

      case ID_interpolate:
        T = T->Next();
        EXPECT(T, DOUBLE);
        m_interpolate = (long int)*((double*)T->Data());
        T = T->Next();
        break;

      case ID_doppler:
        T = T->Next();
        EXPECT(T, DOUBLE);
        m_doppler = *((double*)T->Data());
        T = T->Next();
        break;

      case ID_intensity:
        T = T->Next();
        EXPECT(T, DOUBLE);
        m_intensity = *((double*)T->Data());
        T = T->Next();
        break;

      case ID_once:
        T = T->Next();
        m_once = !m_once;
        break;

      case ID_frequency:
        T = T->Next();

        T = ::Load(m_frequency, T);
        break;

      case ID_phase:
        T = T->Next();

        T = ::Load(m_phase, T);
        break;

      case ID_drift:
        T = T->Next();

        T = ::Load(m_drift, T);
        break;
    }

    if (S == T) {
      printf("Illegal ");
      PrintToken(T);
      printf(" in pigment definition (\"%s\",%li)\n", T->Filename(), T->Line());
      assert(false);
    };

  } while (Depth > 0);

  return T;
}

//////////////////////////////////////////////////////////////////////////////////
////
////    cFinish implementation
////
//////////////////////////////////////////////////////////////////////////////////

cFinish::cFinish() {
  m_ambient = cFColour(0, 0, 0, 0, 0);
  m_diffuse = 1;
  m_brilliance = 1;
  m_specular = 0;
  m_roughness = 0.1;
  m_reflection = cFColour(0, 0, 0, 0, 0);
  m_refraction = 0;
  m_ior = 1;
  m_metallic = 0;

  // Globals.MemoryUsed() += sizeof(cFinish);
};

cFinish::~cFinish() { Globals.MemoryUsed() -= sizeof(cFinish); }

bool cFinish::Test(pToken T) {
  switch (T->ID()) {
    case ID_finish:
    case ID_ambient:
    case ID_diffuse:
    case ID_brilliance:
    case ID_phong:
    case ID_specular:
    case ID_roughness:
    case ID_phong_size:
    case ID_reflection:
    case ID_metallic:
    case ID_refraction:
    case ID_ior:
      return true;
      break;
    default:
      return false;
  }
};

pToken cFinish::Parse(pToken T, bool& wasFinish) {
  wasFinish = true;
  switch (T->ID()) {
    case ID_finish:
    case ID_ambient:
    case ID_diffuse:
    case ID_brilliance:
    case ID_phong:
    case ID_specular:
    case ID_roughness:
    case ID_phong_size:
    case ID_reflection:
    case ID_metallic:
    case ID_refraction:
    case ID_ior:
      T = T->Next();
      break;
    default:
      wasFinish = false;
  }

  return T;
};

pToken cFinish::Load(pToken T) {
  pToken S;
  double Depth;

  // if we don't start with a texture keyword, there needs to be a brace!
  if (!Test(T)) {
    EXPECTED(T, LEFT_BRACE);
    Depth = 1;
  } else {
    Depth = 0;
  };

  // now look for the optional stuff
  do {
    S = T;
    switch (T->ID()) {
      case ID_LEFT_BRACE:
        T = T->Next();
        Depth++;
        break;

      case ID_RIGHT_BRACE:
        if (Depth > 0) T = T->Next();
        Depth--;
        break;

      case ID_ambient:
        T = T->Next();
        T = m_ambient.Load(T);
        break;

      case ID_diffuse:
        T = T->Next();
        EXPECT(T, DOUBLE)
        m_diffuse = *((double*)T->Data());
        T = T->Next();
        break;

      case ID_brilliance:
        T = T->Next();
        EXPECT(T, DOUBLE)
        m_brilliance = *((double*)T->Data());
        T = T->Next();
        break;

      case ID_phong:
      case ID_specular:
        T = T->Next();
        EXPECT(T, DOUBLE)
        m_specular = *((double*)T->Data());
        T = T->Next();
        break;

      case ID_roughness:
        T = T->Next();
        EXPECT(T, DOUBLE)
        m_roughness = *((double*)T->Data());
        T = T->Next();
        break;

      case ID_phong_size:
        T = T->Next();
        EXPECT(T, DOUBLE)
        m_roughness = 1 / (*((double*)T->Data()));
        T = T->Next();
        break;

      case ID_reflection:
        T = T->Next();
        T = m_reflection.Load(T);
        break;

      case ID_metallic:
        T = T->Next();
        EXPECT(T, DOUBLE)
        m_metallic = (*((double*)T->Data())) != 0;
        T = T->Next();
        break;

      case ID_refraction:
        T = T->Next();
        EXPECT(T, DOUBLE)
        m_refraction = *((double*)T->Data());
        T = T->Next();
        break;

      case ID_ior:
        T = T->Next();
        EXPECT(T, DOUBLE)
        m_ior = *((double*)T->Data());
        T = T->Next();
        break;
    }

    if (S == T) {
      printf("Illegal ");
      PrintToken(T);
      printf(" in pigment definition (\"%s\",%li)\n", T->Filename(), T->Line());
      assert(false);
    };

  } while (Depth > 0);

  return T;
};

//////////////////////////////////////////////////////////////////////////////////
////
////    cTexture implementation
////
//////////////////////////////////////////////////////////////////////////////////

cTexture::cTexture(){
    // Globals.MemoryUsed() += sizeof(cTexture);
};

cTexture::~cTexture() { Globals.MemoryUsed() -= sizeof(cTexture); };

void cTexture::Init() { *this = *Default_Texture; };

bool cTexture::Test(pToken T) {
  bool rv = true;

  switch (T->ID()) {
    case ID_texture:
    case ID_colour:
    case ID_color:
    case ID_pigment:
    case ID_normal:
    case ID_finish:
      break;

    default:
      rv = false;
  }

  rv = rv || m_P.Test(T) || m_F.Test(T) || m_N.Test(T);

  return rv;
};

pToken cTexture::Parse(pToken T, pTexture& tex) {
  switch (T->ID()) {
    case ID_texture:
    case ID_colour:
    case ID_color:
    case ID_pigment:
    case ID_normal:
    case ID_finish:
      tex = new cTexture;
      T = T->Next();
      break;

    default:
      tex = NULL;
  }

  return T;
};

pToken cTexture::Load(pToken T) {
  pToken S;
  pMatrix p_mat = NULL;
  bool wasFinish, wasPigment, wasNormal;
  long Depth;

  pTransform p_trans = NULL;

  Init();

  // if we don't start with a texture keyword, there needs to be a brace!
  if (!Test(T)) {
    EXPECTED(T, LEFT_BRACE);
    Depth = 1;
  } else {
    Depth = 0;
  };

  // now look for the optional stuff
  do {
    S = T;

    // Parse the possible transform statements
    T = ctTransform::Parse(T, p_trans);
    if (S != T) {
      T = p_trans->Load(T);

      // push it onto the transform stack, but keep a reference using p_transM
      buffer.push_back(p_trans);

      continue;
    };

    T = m_F.Parse(T, wasFinish);
    if (wasFinish) T = m_F.Load(T);

    T = m_N.Parse(T, wasNormal);
    if (wasNormal) T = m_N.Load(T);

    T = m_P.Parse(T, wasPigment);
    if (wasPigment) T = m_P.Load(T);

    switch (T->ID()) {
      case ID_LEFT_BRACE:
        T = T->Next();
        Depth++;
        break;

      case ID_RIGHT_BRACE:
        if (Depth > 0) T = T->Next();
        Depth--;
        break;
    };

    if (S == T) {
      // no tokens found
      if (Depth > 0) {
        PrintToken(T);
        printf("found in Texture definition (\"%s\",%li)\n", T->Filename(),
               T->Line());
        assert(false);
      }
    };

  } while (Depth > 0);

  return T;
}
