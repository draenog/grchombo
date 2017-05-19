#ifndef BINARYBHLEVELFACTORY_HPP_
#define BINARYBHLEVELFACTORY_HPP_

//General includes:
#include "AMR.H"
#include "AMRLevelFactory.H"
#include "ProfilingInfo.hpp"
#include "SimulationParameters.hpp"

//Problem specific includes:
#include "BinaryBHLevel.hpp"

class BinaryBHLevelFactory : public AMRLevelFactory
{
public:
    BinaryBHLevelFactory(SimulationParameters& a_sim_params, ProfilingInfo * profilingInfo=nullptr);

    virtual
    AMRLevel* new_amrlevel(AMR * a_amr) const;

    virtual
    ~BinaryBHLevelFactory();

protected:
    SimulationParameters m_p;
    ProfilingInfo* m_profilingInfo;
};


BinaryBHLevelFactory::BinaryBHLevelFactory (SimulationParameters& a_sim_params, ProfilingInfo * a_profilingInfo):
    m_p (a_sim_params), m_profilingInfo (a_profilingInfo)
{
}

BinaryBHLevelFactory::~BinaryBHLevelFactory ()
{
}

// "virtual constructor"
AMRLevel*
BinaryBHLevelFactory::new_amrlevel(AMR * a_amr) const
{
    BinaryBHLevel* binary_bh_level_ptr = new BinaryBHLevel (a_amr,m_p, m_p.verbosity, m_profilingInfo);
    binary_bh_level_ptr->initialDtMultiplier(m_p.dt_multiplier);
    return (static_cast <AMRLevel*> (binary_bh_level_ptr));
}
#endif /* BINARYBHLEVELFACTORY_HPP_ */
