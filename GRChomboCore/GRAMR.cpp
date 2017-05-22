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

void AMR::conclude() const
{
  CH_TIME("AMR::conclude");

  CH_assert(isDefined());
  CH_assert(isSetUp());

  if (m_verbosity >= 3)
    {
      pout() << "AMR::conclude" << endl;
    }

  if ((m_plot_interval >= 0) || (m_plot_period >= 0.0))
    {
      writePlotFile();
    }

  if ((m_checkpoint_interval >= 0)     &&
      (m_lastcheck_step != m_cur_step) &&
      (m_restart_step != m_cur_step))
    {
      writeCheckpointFile();
    }

  // Call any scheduled functions. This is placed after plotting and
  // checkpointing so that the plotting functions can congeal plot files.
  if (!m_scheduler.isNull())
    {
      m_scheduler->conclude(m_cur_step, m_cur_time);
    }

  // Call the hooks for the AMRLevel objects.                                      
  for (int level = 0; level <= m_finest_level; ++level)
    m_amrlevels[level]->conclude(m_cur_step);

  if (m_verbosity >= 2)
    {
      long long total_cell_updates = 0;
      for (int ll = 0; ll < m_max_level+1; ll++)
        {
          total_cell_updates += m_cell_updates[ll];

#ifdef CH_OSF1
          pout() << "number of points updated at level " << ll <<" = " <<
	    (Real)m_cell_updates[ll] << endl;
#else
          pout() << "number of points updated at level " << ll <<" = " <<
	    m_cell_updates[ll] << endl;
#endif
        }
#ifdef CH_OSF1
      pout() << "total number of points updated = " << (Real)total_cell_updates << endl;
#else
      pout() << "total number of points updated = " << total_cell_updates << endl;
#endif
    }

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
