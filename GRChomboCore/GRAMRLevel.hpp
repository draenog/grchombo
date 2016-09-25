#ifndef GRAMRLEVEL_HPP_
#define GRAMRLEVEL_HPP_

#include "AMRLevel.H"
#include "SimulationParameters.hpp"
#include "ProfilingInfo.hpp"
#include "GRLevelData.hpp"
#include "LevelFluxRegister.H" //We don't actually use flux conservation but Chombo assumes we do
#include "FourthOrderFillPatch.H"
#include "CoarseAverage.H"

class GRAMRLevel : public AMRLevel
{
protected:
    GRAMRLevel(const SimulationParameters& a_p, int a_verbosity, ProfilingInfo * a_profilingInfo = NULL);

    virtual
    ~GRAMRLevel();

private:
    // define
    virtual
    void define(AMRLevel* a_coarser_level_ptr, const Box& a_problem_domain, int a_level, int a_ref_ratio);

    // define
    virtual
    void define(AMRLevel* a_coarser_level_ptr, const ProblemDomain& a_problem_domain,
                int a_level, int a_ref_ratio);

    /// Do casting from AMRLevel to GRAMRLevel and stop if this isn't possible
    GRAMRLevel* gr_cast(AMRLevel* const amr_level_ptr);

    // advance by one timestep
    virtual
    Real advance();

    // things to do after a timestep
    virtual
    void postTimeStep();

    // create tags
    virtual
    void tagCells(IntVectSet& a_tags) ;

    // create tags at initialization
    virtual
    void tagCellsInit(IntVectSet& a_tags) ;

    // regrid
    virtual
    void regrid(const Vector<Box>& a_new_grids);

    // initialize grids
    virtual
    void initialGrid(const Vector<Box>& a_new_grids);

    // things to do after initialization
    virtual
    void postInitialize();

    // compute dt
    virtual
    Real computeDt();

    // compute dt with initial data
    virtual
    Real computeInitialDt();

    DisjointBoxLayout loadBalance(const Vector<Box>& a_grids);

#ifdef CH_USE_HDF5
    virtual
    void writeCheckpointHeader(HDF5Handle& a_handle) const;

    virtual
    void writeCheckpointLevel(HDF5Handle& a_handle) const;

    virtual
    void readCheckpointHeader(HDF5Handle& a_handle);

    virtual
    void readCheckpointLevel(HDF5Handle& a_handle);

    virtual
    void writePlotHeader(HDF5Handle& a_handle) const;

    virtual
    void writePlotLevel(HDF5Handle& a_handle) const;
#endif

public:
    // methods used with LevelRK4:
    typedef GRLevelData TSoln;
    typedef LevelFluxRegister TFR;

    // evaluate d(soln)/dt at current time based on soln
    void evalRHS(TSoln& rhs, // d(soln)/dt based on soln
                 TSoln& soln, // soln at current time
                 TFR& fineFR,  // flux register w/ finer level
                 TFR& crseFR,  // flux register w/ crse level
                 const TSoln& oldCrseSoln, // old-time crse solution
                 Real oldCrseTime,    // old crse time
                 const TSoln& newCrseSoln,  // new-time crse solution
                 Real newCrseTime,   // new crse time
                 Real time,   // current time centering of soln
                 Real fluxWeight // weight to apply to fluxRegister updates
                );

    // implements soln += dt*rhs
    void updateODE(TSoln& soln, const TSoln& rhs, Real dt);

    // define data holder newSoln based on existingSoln, including ghost cell specification
    void defineSolnData(TSoln& newSoln, const TSoln& existingSoln);

    // define data holder for RHS based on existingSoln including ghost cell specification
    // (which in most cases is no ghost cells)
    void defineRHSData(TSoln& newRHS, const TSoln& existingSoln);

    /// copy data in src into dest
    void copySolnData(TSoln& dest, const TSoln& src);

    //Pure virtual functions for the specific implementation
    virtual
    void specificAdvance() = 0;

    virtual
    void specificPostTimeStep() = 0;

    // initialize data
    virtual
    void initialData() = 0;

    virtual
    void fillBdyGhosts() = 0;

#ifdef CH_USE_HDF5
    virtual
    void preCheckpointLevel() = 0;
#endif

    virtual
    void specificEvalRHS(GRLevelData& a_soln, GRLevelData& a_rhs, const double a_time) = 0;

    virtual
    void specificUpdateODE(GRLevelData& a_soln, const GRLevelData& a_rhs, Real a_dt) = 0;

protected:
    virtual
    void fillAllGhosts();

    virtual
    void fillIntralevelGhosts();


public:
    const int m_num_ghosts;
    // periodicity information
    //  static const int s_periodicity[SpaceDim];

protected:
    // state vector at old time
    GRLevelData m_state_old;
    // state vector at new time
    GRLevelData m_state_new;
    // grid spacing
    Real m_dx;

    // params
    SimulationParameters m_p;
    int m_verbosity;

    //Profiling info
    ProfilingInfo * m_profilingInfo;

    // exchange copier
    Copier m_exchange_copier;

    // interpolator for filling in ghost cells from the next coarser level
    FourthOrderFillPatch m_patcher;

    CoarseAverage m_coarse_average;
    FourthOrderFineInterp m_fine_interp;
    DisjointBoxLayout m_grids;
};

#include "GRAMRLevel.impl.hpp"

#endif /* GRAMRLEVEL_HPP_ */