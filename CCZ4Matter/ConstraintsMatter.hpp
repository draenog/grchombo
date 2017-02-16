// Last edited K Clough 16.02.17

#ifndef CONSTRAINTSMATTER_HPP_
#define CONSTRAINTSMATTER_HPP_

#include "FArrayBox.H"
#include "simd.hpp"
#include "tensor.hpp"
#include "GRUtils.hpp"
#include "FABDriverBase.hpp"
#include "FourthOrderDerivatives.hpp"
#include "CCZ4Geometry.hpp"
#include "ScalarField.hpp"
#include "Constraints.hpp"
#include <array>

//!  Calculates the Hamiltonain and Momentum constraints with matter fields
/*!
     The class calculates the Hamiltonian and Momentum constraints at each point in a
     box. It inherits from the Constraints class which calculates the constraints without
     the matter terms. It adds in the matter terms for a given matter class matter_t, which
     must provide it with the Energy Momentum tensor. For an example of a matter_t class
     see ScalarField.
     \sa Constraints(), ScalarField()
*/
template <class matter_t>
class ConstraintsMatter : public Constraints {
  //Use the variable definition in matter_t
  template<class data_t>
  using Vars=typename matter_t::template Vars<data_t>;
public:
  //! Constructor of class ConstraintsMatter
  /*!
       Takes in the box driver and the grid spacing, plus optionally the
       value of Newton's constant, which is set to one by default.
  */
  ConstraintsMatter(const FABDriverBase& driver,
                    const typename matter_t::params_t matter_params,
                    double dx, double G_Newton = 1.0);

  //! The compute member which calculates the constraints at each point in the box
  /*!
       \param ix the x index of the point on the level.
       \param iy the y index of the point on the level.
       \param iz the z index of the point on the level.
       \return is void - the constraints are written directly to the grid in the function.
  */
  template <class data_t>
  void compute(int x, int y, int z);

protected:
  const typename matter_t::params_t m_matter_params; //!< The matter params
  double m_G_Newton; //!< Newton's constant, set to one by default.

};

#include "ConstraintsMatter.impl.hpp"

#endif /* CONSTRAINTSMATTER_HPP_ */
