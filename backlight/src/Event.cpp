//
//  cEvent functions
//

#include "Event.hpp"

cEvent cEvent::LorentzTransform(lift::vector<double, 3> &v,
                                lift::vector<double, 3> &u) {
  // FROM frame with velocity v (relative to base reference frame)
  // TO frame with velocity u (relative to base reference frame)

  cEvent e;
  lift::vector<double, 3> nu, nv;
  double gu, gv;

  if (!Globals.Relativity()) {
    e.m_pos = m_pos + (v * m_t) - (u * m_t);
    e.m_t = m_t;
    return e;
  }

  if (abs(u) != 0) {
    nu = u / abs(u);
  } else {
    nu = u;
  }

  if (abs(v) != 0) {
    nv = v / abs(v);
  } else {
    nv = v;
  }

  gu = gamma(u);
  gv = gamma(v);

  // Derived in obvious fashion; calculate m_pos|| = m_pos.nv, and transform
  // this
  // with m_t in the traditional fashion, then reconstruct m_pos'. Applied twice
  // this gives a transformation between general frames. Uses -v as we
  // transform FROM v-frame.

  e.m_t =
      gu * (gv * (m_t * (1 - inner_product(u, v)) + inner_product(v, m_pos)) +
            inner_product(u, nv) * inner_product(nv, m_pos) * (1 - gv) -
            inner_product(u, m_pos));
  e.m_pos = m_pos + nv * (gv - 1) * inner_product(nv, m_pos) + v * gv * m_t +
            nu * (gu - 1) *
                (inner_product(nu, m_pos) + inner_product(nu, v) * gv * m_t +
                 inner_product(nu, nv) * inner_product(nv, m_pos) * (gv - 1)) -
            u * gu * gv * (inner_product(v, m_pos) + m_t);

  return e;
}
