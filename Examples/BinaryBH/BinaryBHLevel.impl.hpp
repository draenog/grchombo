#if !defined(BINARYBHLEVEL_HPP_)
#error "This file should only be included through BinaryBHLevel.hpp"
#endif

#ifndef BINARYBHLEVEL_IMPL_HPP_
#define BINARYBHLEVEL_IMPL_HPP_

#include "BinaryBHLevel.hpp"
#include "FABDriver.hpp"
#include "EnforceTfA.hpp"
#include "PositiveChiAndAlpha.hpp"
#include "NanCheck.hpp"
#include "Constraints.hpp"
#include "CCZ4.hpp"
#include "PSI4_F.H"

//Initial data
#include "BinaryBH.hpp"
#include "GRAMR.hpp"


#include "FORT_PROTO.H"
#define CHF_FRAn(a, n, c) \
    a.dataPtr(n), \
    D_DECL6(&a.loVect()[0],  \
            &a.loVect()[1],  \
            &a.loVect()[2],  \
            &a.loVect()[3],  \
            &a.loVect()[4],  \
            &a.loVect()[5]), \
    D_DECL6(&a.hiVect()[0],  \
            &a.hiVect()[1],  \
            &a.hiVect()[2],  \
            &a.hiVect()[3],  \
            &a.hiVect()[4],  \
            &a.hiVect()[5]), \
    &c


#define CHF_CONST_FRAn(a, n, c) CHF_FRAn(a ,n, c)

const int BinaryBHLevel::s_num_comps_h = c_K-c_h;
const int BinaryBHLevel::s_num_comps_A = c_Theta-c_A;

void BinaryBHLevel::specificAdvance()
{
    //Enforce the trace free A_ij condition
    FABDriver<EnforceTfA>().execute(m_state_new, m_state_new, FILL_GHOST_CELLS);

    //Enforce positive chi and alpha
    FABDriver<PositiveChiAndAlpha>().execute(m_state_new, m_state_new, FILL_GHOST_CELLS);

    //Check for nan's
    if (m_p.nan_check) FABDriver<NanCheck>().execute(m_state_new, m_state_new, SKIP_GHOST_CELLS, no_simd_support());
}

void BinaryBHLevel::initialData()
{
    CH_TIME("BinaryBHLevel::initialData");
    if (m_verbosity) pout () << "BinaryBHLevel::initialData " << m_level << endl;

    //First set everything to zero ... we don't want undefined values in constraints etc
    m_state_new.setVal(0.);

    FABDriver<BinaryBH>(m_p.bh1_params, m_p.bh2_params, m_dx).execute(m_state_new, m_state_new, FILL_GHOST_CELLS);
}

void BinaryBHLevel::preCheckpointLevel()
{
    fillAllGhosts();
    FABDriver<Constraints>(m_dx).execute(m_state_new, m_state_new, SKIP_GHOST_CELLS);

    getPsi4();
}

void BinaryBHLevel::getPsi4()
{
// Copy caluclation of Psi4 from old code
    const DisjointBoxLayout& level_domain = m_state_new.disjointBoxLayout();
    DataIterator dit0 = level_domain.dataIterator();
    int nbox = dit0.size();
    // And compute Psi4...
  //#pragma omp parallel for default(shared) schedule(guided)
  //Moved the threading into boxes
    for(int ibox = 0; ibox < nbox; ++ibox)
    {
      DataIndex di = dit0[ibox];
      const Box& b = level_domain[di];
      FArrayBox& state_fab = m_state_new[di];
  
      Real centerX = (m_p.centerA[0] + m_p.centerB[0]) / 2.;
      Real centerY = (m_p.centerA[1] + m_p.centerB[1]) / 2.;
      Real centerZ = (m_p.centerA[2] + m_p.centerB[2]) / 2.;
  
      FORT_GETPSI4(CHF_FRA1(state_fab,c_Psi4r),
                         CHF_FRA1(state_fab,c_Psi4i),
                         CHF_CONST_FRA1(state_fab,c_chi),
                         CHF_CONST_FRAn(state_fab,c_h,s_num_comps_h),
                         CHF_CONST_FRA1(state_fab,c_K),
                         CHF_CONST_FRAn(state_fab,c_A,s_num_comps_A),
                         CHF_CONST_REAL(m_dx),
                         CHF_CONST_REAL(centerX),
                         CHF_CONST_REAL(centerY),
                         CHF_CONST_REAL(centerZ),
                         CHF_BOX(b));
    }

}

void BinaryBHLevel::specificEvalRHS(GRLevelData& a_soln, GRLevelData& a_rhs, const double a_time)
{
    FABDriver<EnforceTfA>().execute(a_soln, a_soln, FILL_GHOST_CELLS);

    //Enforce positive chi and alpha
    FABDriver<PositiveChiAndAlpha>().execute(a_soln, a_soln, FILL_GHOST_CELLS);

    //Calculate CCZ4 right hand side
    FABDriver<CCZ4>(m_p.ccz4Params, m_dx, m_p.sigma).execute(a_soln, a_rhs, SKIP_GHOST_CELLS);

    //We don't want undefined values floating around in the constraints
    a_rhs.setVal(0., Interval(c_Ham,c_Mom3));
}

void BinaryBHLevel::specificUpdateODE(GRLevelData& a_soln, const GRLevelData& a_rhs, Real a_dt)
{
    //Enforce the trace free A_ij condition
    FABDriver<EnforceTfA>().execute(a_soln, a_soln, FILL_GHOST_CELLS);
}

#endif
