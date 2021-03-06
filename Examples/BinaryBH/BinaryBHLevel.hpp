#ifndef BINARYBHLEVEL_HPP_
#define BINARYBHLEVEL_HPP_

#include "GRAMRLevel.hpp"

class BinaryBHLevel : public GRAMRLevel
{
    friend class BinaryBHLevelFactory;
    //Inherit the contructors from GRAMRLevel
    using GRAMRLevel::GRAMRLevel;

    static const int s_num_comps_h;
    static const int s_num_comps_A;

    virtual
    void specificAdvance();

    // initialize data
    virtual
    void initialData();

    virtual
    void preCheckpointLevel();

    virtual
    void specificWritePlotHeader(std::vector<int> &plot_states) const;

    virtual
    void specificEvalRHS(GRLevelData& a_soln, GRLevelData& a_rhs, const double a_time);

    virtual
    void specificUpdateODE(GRLevelData& a_soln, const GRLevelData& a_rhs, Real a_dt);

#ifdef USE_CATALYST
    virtual
    void specificPostTimeStep();
#endif

    void getPsi4();


};

#include "BinaryBHLevel.impl.hpp"

#endif /* BINARYBHLEVEL_HPP_ */
