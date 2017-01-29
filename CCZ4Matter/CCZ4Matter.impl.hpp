// Last edited K Clough 16.10.16

#if !defined(CCZ4MATTER_HPP_)
#error "This file should only be included through CCZ4Matter.hpp"
#endif


#ifndef CCZ4MATTER_IMPL_HPP_
#define CCZ4MATTER_IMPL_HPP_

#define COVARIANTZ4

//TODO KClough: inline??
template <class matter_t>
CCZ4Matter<matter_t>::CCZ4Matter(const FABDriverBase& driver,
    params_t params,
    const typename matter_t::matter_params_t matter_params,
    double dx,
    double sigma,
    int formulation,
    double G_Newton)
    : CCZ4(driver, params, dx, sigma, formulation, 0.0), //No cosmological const
      m_matter_params (matter_params) , m_G_Newton (G_Newton) {}


template <class matter_t>
template <class data_t>
void CCZ4Matter<matter_t>::compute(int ix, int iy, int iz)
{
  //copy data from chombo gridpoint into local variables
  vars_t<data_t> CCZ4_vars;
  m_driver.local_vars(CCZ4_vars);

  //work out first derivatives of variables on grid
  vars_t< tensor<1, data_t> > CCZ4_d1;
  FOR1(idir) m_deriv.diff1(CCZ4_d1, idir);

  // Repeated derivatives
  // Work out second derivatives of variables on grid
  vars_t< tensor<2,data_t> > CCZ4_d2;
  FOR1(idir) m_deriv.diff2(CCZ4_d2, idir);
  // Mixed derivatives
  // Note: no need to symmetrise explicitely, this is done in mixed_diff2
  m_deriv.mixed_diff2(CCZ4_d2, 1, 0);
  m_deriv.mixed_diff2(CCZ4_d2, 2, 0);
  m_deriv.mixed_diff2(CCZ4_d2, 2, 1);

  // Calculate advection terms
  vars_t<data_t> CCZ4_advec;
  CCZ4_advec.assign(0.);
  FOR1(idir) m_deriv.add_advection(CCZ4_advec, CCZ4_vars.shift[idir], idir);

  // Call CCZ4 RHS - work out RHS without matter, no dissipation
  vars_t<data_t> CCZ4_rhs = rhs_equation(CCZ4_vars, CCZ4_d1, CCZ4_d2, CCZ4_advec);

  // Rework RHS of variables to include matter
  // TODO KClough: Once CCZ4 is templated, won't need the two vars_t types, and so
  // won't need to calculate derivs twice. Will also add RHS terms here and
  // not in the matter_t class

  //copy data from chombo gridpoint into local variables
  typename matter_t::vars_t<data_t> vars;
  m_driver.local_vars(vars);

  //work out first derivatives of variables on grid
  typename matter_t::vars_t< tensor<1, data_t> > d1;
  FOR1(idir) m_deriv.diff1(d1, idir);

  // Repeated derivatives
  // work out second derivatives of variables on grid
  typename matter_t::vars_t< tensor<2,data_t> > d2;
  FOR1(idir) m_deriv.diff2(d2, idir);
  // Mixed derivatives
  // Note: no need to symmetrise explicitely, this is done in mixed_diff2
  m_deriv.mixed_diff2(d2, 1, 0);
  m_deriv.mixed_diff2(d2, 2, 0);
  m_deriv.mixed_diff2(d2, 2, 1);

  // Calculate advection terms
  typename matter_t::vars_t<data_t> advec;
  advec.assign(0.);
  FOR1(idir) m_deriv.add_advection(advec, vars.shift[idir], idir);

  //work out RHS matter terms
  typename matter_t::vars_t<data_t> matter_rhs;
  matter_rhs = matter_rhs_equation(vars, d1, d2, advec);

  // Combine the RHS terms and add matter evolution
  typename matter_t::vars_t<data_t> rhs_total;
  matter_t my_matter(m_matter_params);
  rhs_total = my_matter.compute_total_rhs(CCZ4_rhs, matter_rhs, vars, d1, d2, advec);

  //Add dissipation to all terms
  FOR1(idir) m_deriv.add_dissipation(rhs_total, m_sigma, idir);

  //Write the rhs into the output FArrayBox
  m_driver.store_vars(rhs_total);

}

// Function to add in matter terms to CCZ4, those depending on EM tensor
template <class matter_t>
template <class data_t>
typename matter_t::vars_t<data_t> CCZ4Matter<matter_t>::matter_rhs_equation(
    const typename matter_t::vars_t<data_t> &vars,
    const typename matter_t::vars_t< tensor<1,data_t> >& d1,
    const typename matter_t::vars_t< tensor<2,data_t> >& d2,
    const typename matter_t::vars_t<data_t> &advec) {

  typename matter_t::vars_t<data_t> add_rhs;
  add_rhs.assign(0.);

//  const data_t chi_regularised = simd_max(1e-6, vars.chi);

  using namespace TensorAlgebra;

  auto h_UU = compute_inverse(vars.h);
  auto chris = CCZ4Geometry::compute_christoffel(d1, h_UU);

  //Calculate elements of the decomposed stress energy tensor
  matter_t my_matter(m_matter_params);
  auto emtensor =  my_matter.compute_emtensor(vars, d1, h_UU, chris.ULL, advec);

  // Update RHS
  if (m_formulation == USE_BSSN) {
    add_rhs.K = 4.0*M_PI*m_G_Newton*vars.lapse*(emtensor.S + emtensor.rho);
    add_rhs.Theta = 0.0;
  } else {
    add_rhs.K = 4.0*M_PI*m_G_Newton*vars.lapse*(emtensor.S - 3*emtensor.rho);
    add_rhs.Theta = - 8.0*M_PI*m_G_Newton*vars.lapse*emtensor.rho;
  }

  tensor<2, data_t> Sij_TF = emtensor.Sij;
  make_trace_free(Sij_TF, vars.h, h_UU);

  FOR2(i,j)
  {
    add_rhs.A[i][j] =
        - 8.0*M_PI*m_G_Newton*vars.chi*vars.lapse*Sij_TF[i][j];
  }

  FOR1(i)
  {
    add_rhs.Gamma[i] = 0.0;
    add_rhs.B[i] = 0.0;
    FOR1(j)
    {
      add_rhs.Gamma[i] +=
          - 16.0*M_PI*m_G_Newton*vars.lapse*h_UU[i][j]*emtensor.Si[j];
    }

    add_rhs.B[i] += add_rhs.Gamma[i];

  }

  return add_rhs;
}

#endif /* CCZ4MATTER_IMPL_HPP_ */
