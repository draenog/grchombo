#include "GRAMR.hpp"

#ifdef USE_CATALYST
#include "Insitu/Insitu.hpp"
#endif




void GRAMR::setDefaultValues()
{
  m_isDefined = false;
  m_isSetUp   = false;
  m_useSubcycling = true;
  m_max_level = -1;
  m_finest_level = -1;
  m_checkpoint_interval= -1;
  m_plot_interval=-1;
  m_checkForSteadyState = false;
  m_plot_period=-1.0;
  m_next_plot_time=-1.0;
  m_max_grid_size= 0;
  m_max_base_grid_size = m_max_grid_size;
  m_restart_step = -1;
  m_lastcheck_step = -1;
  m_cur_step =0;
  s_step = m_cur_step;
  m_maxDtGrow = 1.1;
  m_time_eps = 1.0e-6;
  m_dt_base = -1;
  m_amrlevels.resize(0);
  m_use_meshrefine=false;
  m_plotfile_prefix = string("pltstate");
  m_checkpointfile_prefix = string("chk");
  m_verbosity = 0;
  m_cur_time = 0;
  m_dt_tolerance_factor = 1.1;
  m_fixedDt = -1;
  m_blockFactor = 4;
#ifdef CH_USE_TIMER
  m_timer = NULL ;
#endif

#ifdef USE_CATALYST
  m_renderLevel = -1;
#endif

  m_scheduler = RefCountedPtr<Scheduler>();

}

void GRAMR::conclude() const
{

  AMR::conclude();
#ifdef USE_CATALYST
  m_insitu.finalise();
#endif

}


#ifdef USE_CATALYST
void GRAMR::setupCatalyst(int a_num_pyScript = 0, string a_pyScript);
{
  m_insitu.initialise(this, a_num_pyScript, a_pyScript);
}
#endif
