// Last edited K Clough 16.10.16

#ifndef SCALARFIELDLEVEL_HPP_
#define SCALARFIELDLEVEL_HPP_

#include "GRAMRLevel.hpp"


//!  A class for the evolution of a scalar field, minimally coupled to gravity
/*!
     The class takes some initial data for a scalar field (variables phi and Pi)
     and evolves it using the CCZ4 equations. It is possible to specify an initial
     period of relaxation for the conformal factor chi, for non analytic initial
     conditions (for example, a general field configuration at a moment of time symmetry
     assuming conformal flatness).
     \sa CCZ4Matter(), ConstraintsMatter(), ScalarField(), RelaxationChi()
*/
class ScalarFieldLevel : public GRAMRLevel
{
    friend class ScalarFieldLevelFactory;
    //Inherit the contructors from GRAMRLevel
    using GRAMRLevel::GRAMRLevel;

    //! Things to do at the end of the advance step, after RK4 calculation
    virtual
    void specificAdvance();

    //! Initialize data for the field and metric variables
    virtual
    void initialData();

    //! routines to do before outputing checkpoint file
    virtual
    void preCheckpointLevel();

    //! RHS routines used at each RK4 step
    virtual
    void specificEvalRHS(GRLevelData& a_soln, GRLevelData& a_rhs, const double a_time);

    //! Things to do in UpdateODE step, after soln + rhs update
    virtual
    void specificUpdateODE(GRLevelData& a_soln, const GRLevelData& a_rhs, Real a_dt);

    //! Specify which variables to write at plot intervals
    virtual
    void specificWritePlotHeader(std::vector<int> &plot_states) const;

    //! Tell Chombo how to tag cells for regridding
    virtual
    void tagCells(IntVectSet& a_tags);

};

#include "ScalarFieldLevel.impl.hpp"

#endif /* SCALARFIELDLEVEL_HPP_ */
