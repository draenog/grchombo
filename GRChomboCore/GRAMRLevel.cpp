#include "GRAMRLevel.hpp"
#include "LevelRK4.H"

//TODO: Remove this once tagCells is sorted out!
#include "FABDriver.hpp"
#include "ComputeModGrad.hpp"
#include "GRAMRLevel.hpp"

GRAMRLevel::GRAMRLevel (const SimulationParameters &a_p, int a_verbosity, ProfilingInfo * a_profilingInfo)
: m_num_ghosts (a_p.num_ghosts), m_p(a_p), m_verbosity (a_verbosity), m_profilingInfo(a_profilingInfo)
{
    if ( m_verbosity ) pout () << "GRAMRLevel default constructor" << endl;
}

GRAMRLevel::~GRAMRLevel ()
{
}

void
GRAMRLevel::define (AMRLevel* a_coarser_level_ptr,
                  const Box& a_problem_domain,
                  int a_level,
                  int a_ref_ratio)
{
    ProblemDomain physdomain(a_problem_domain);
    define(a_coarser_level_ptr, physdomain, a_level, a_ref_ratio);
}

void
GRAMRLevel::define (AMRLevel* a_coarser_level_ptr,
                  const ProblemDomain& a_problem_domain,
                  int a_level,
                  int a_ref_ratio)
{
    if ( m_verbosity ) pout () << "GRAMRLevel::define " << a_level << endl;

    AMRLevel::define (a_coarser_level_ptr,
                      a_problem_domain,
                      a_level,
                      a_ref_ratio);

    if (a_coarser_level_ptr)
    {
        GRAMRLevel *coarser_level_ptr = gr_cast(a_coarser_level_ptr);
        m_dx = coarser_level_ptr->m_dx / Real(coarser_level_ptr->m_ref_ratio);
    }
    else
    {
        m_dx = m_p.L / (a_problem_domain.domainBox().longside ());
    }
}

/// Do casting from AMRLevel to GRAMRLevel and stop if this isn't possible
GRAMRLevel* GRAMRLevel::gr_cast(AMRLevel* const amr_level_ptr)
{
    GRAMRLevel* gr_amr_level_ptr = dynamic_cast<GRAMRLevel*> (amr_level_ptr);
    if (gr_amr_level_ptr == nullptr)
    {
        MayDay::Error ("in GRAMRLevel::gr_cast: amr_level_ptr is not castable to GRAMRLevel*");
    }
    return gr_amr_level_ptr;
}

// advance by one timestep
Real
GRAMRLevel::advance ()
{
    CH_TIME("GRAMRLevel::advance");

    //Get information on number of boxes on this level (helps with better load balancing)
    const DisjointBoxLayout& level_domain = m_state_new.disjointBoxLayout();
    int nbox = level_domain.dataIterator().size();
    pout () << "GRAMRLevel::advance " << m_level << " at " << m_time <<
        ". Boxes on this rank: " << nbox << "." << endl;

    m_state_new.copyTo ( m_state_new.interval (),
                         m_state_old,
                         m_state_old.interval () );

    // The level classes take flux-register parameters, use dummy ones here
    LevelFluxRegister* coarser_fr = nullptr;
    LevelFluxRegister* finer_fr   = nullptr;
    // Default coarser level pointers to an empty LevelData
    // (Chombo usually checks isDefined() rather than != nullptr so we shouldn't use the nullptr)
    const GRLevelData null_gr_level_data;
    const GRLevelData* coarser_data_old = &null_gr_level_data;
    const GRLevelData* coarser_data_new = &null_gr_level_data;

    Real t_coarser_old = 0.0;
    Real t_coarser_new = 0.0;

    // A coarser level exists
    if (m_coarser_level_ptr != nullptr)
    {
        GRAMRLevel* coarser_gr_amr_level_ptr = gr_cast(m_coarser_level_ptr);
        coarser_data_old = &coarser_gr_amr_level_ptr->m_state_old;
        coarser_data_new = &coarser_gr_amr_level_ptr->m_state_new;

        t_coarser_new = coarser_gr_amr_level_ptr->m_time;
        t_coarser_old = t_coarser_new - coarser_gr_amr_level_ptr->m_dt;
    }

    if (m_finer_level_ptr != nullptr)
    {
        GRAMRLevel* fine_gr_amr_level_ptr = gr_cast(m_finer_level_ptr);
        RK4LevelAdvance(m_state_new, m_state_old,
                        fine_gr_amr_level_ptr->m_patcher.getTimeInterpolator(),
                        *coarser_data_old, t_coarser_old, *coarser_data_new, t_coarser_new,
                        *coarser_fr, *finer_fr, m_time, m_dt, *this);
    }
    else
    {
        RK4LevelAdvance(m_state_new, m_state_old, *coarser_data_old, t_coarser_old,
                        *coarser_data_new, t_coarser_new, *coarser_fr, *finer_fr, m_time, m_dt, *this);
    }

    specificAdvance();

    m_time += m_dt;
    return m_dt;
}

// things to do after a timestep
void
GRAMRLevel::postTimeStep ()
{
    if ( m_verbosity ) pout () << "GRAMRLevel::postTimeStep " << m_level << endl;

    if (m_finer_level_ptr != nullptr)
    {
        GRAMRLevel* finer_gr_amr_level_ptr = gr_cast(m_finer_level_ptr);
        finer_gr_amr_level_ptr->m_coarse_average.averageToCoarse (m_state_new, finer_gr_amr_level_ptr->m_state_new);
    }

    specificPostTimeStep();

    if ( m_verbosity ) pout () << "GRAMRLevel::postTimeStep " << m_level << " finished" << endl;
}


// create tags
void
GRAMRLevel::tagCells (IntVectSet& a_tags)
{
    CH_TIME("GRAMRLevel::tagCells");
    if ( m_verbosity ) pout () << "GRAMRLevel::tagCells " << m_level << endl;

    //TODO: Need to think about how to do the tagging nicely.
    //One possibility would be to pass a BaseFAB<bool> to the specific implementation
    //but for local tagging criteria this wastes memory as we save a whole BaseFAB.
    //In the meantime the user must override tagCells (it's virtual) with code similar
    //to the one below.

    fillAllGhosts(); //We need filled ghost cells to calculate gradients etc

    // Create tags based on undivided gradient of phi
    IntVectSet local_tags;

    const DisjointBoxLayout& level_domain = m_state_new.disjointBoxLayout();
    DataIterator dit0 = level_domain.dataIterator();
    int nbox = dit0.size();
    for(int ibox = 0; ibox < nbox; ++ibox)
    {
        DataIndex di = dit0[ibox];
        const Box& b = level_domain[di];
        const FArrayBox& state_fab = m_state_new[di];

        //mod gradient
        FArrayBox mod_grad_fab(b,c_NUM);
        FABDriver<ComputeModGrad>(m_dx).execute(state_fab, mod_grad_fab);

        const IntVect& smallEnd = b.smallEnd();
        const IntVect& bigEnd = b.bigEnd();

        const int xmin = smallEnd[0];
        const int ymin = smallEnd[1];
        const int zmin = smallEnd[2];

        const int xmax = bigEnd[0];
        const int ymax = bigEnd[1];
        const int zmax = bigEnd[2];

#pragma omp parallel for collapse(3) schedule(static) default(shared)
        for (int iz = zmin; iz <= zmax; ++iz)
        for (int iy = ymin; iy <= ymax; ++iy)
        for (int ix = xmin; ix <= xmax; ++ix)
        {
            IntVect iv(ix,iy,iz);
            //At the moment only base on gradient chi/chi^2
            if (m_dx * mod_grad_fab(iv,c_chi)/pow(state_fab(iv,c_chi),2)
                >= m_p.regrid_threshold)
            {
                // local_tags |= is not thread safe.
#pragma omp critical
                {
                    local_tags |= iv;
                }
            }
        }
    }

    local_tags.grow(m_p.tag_buffer_size);

    // Need to do this in two steps unless a IntVectSet::operator &=
    // (ProblemDomain) operator is defined
    Box local_tags_box = local_tags.minBox();
    local_tags_box &= m_problem_domain;
    local_tags &= local_tags_box;

    a_tags = local_tags;
}

// create tags at initialization
void
GRAMRLevel::tagCellsInit (IntVectSet& a_tags)
{
    //the default is to use the standard tagging function
    tagCells(a_tags);
}

// regrid
void
GRAMRLevel::regrid (const Vector<Box>& a_new_grids)
{
    CH_TIME("GRAMRLevel::regrid");

    if ( m_verbosity ) pout () << "GRAMRLevel::regrid " << m_level << endl;

    m_level_grids = a_new_grids;

    mortonOrdering(m_level_grids);
    const DisjointBoxLayout level_domain = m_grids = loadBalance (a_new_grids);

    // save data for later copy
    m_state_new.copyTo(m_state_new.interval(), m_state_old, m_state_old.interval());

    // reshape state with new grids
    IntVect iv_ghosts = m_num_ghosts*IntVect::Unit;
    m_state_new.define (level_domain, c_NUM, iv_ghosts);

    // maintain interlevel stuff
    m_exchange_copier.exchangeDefine(level_domain, iv_ghosts);
    m_coarse_average.define (level_domain, c_NUM, m_ref_ratio);
    m_fine_interp.define (level_domain, c_NUM, m_ref_ratio, m_problem_domain);

    if (m_coarser_level_ptr != nullptr)
    {
        GRAMRLevel* coarser_gr_amr_level_ptr = gr_cast(m_coarser_level_ptr);
        m_patcher.define(level_domain, coarser_gr_amr_level_ptr->m_grids,
                         c_NUM, coarser_gr_amr_level_ptr->problemDomain(),
                         m_ref_ratio, m_num_ghosts);

        // interpolate from coarser level
        m_fine_interp.interpToFine (m_state_new, coarser_gr_amr_level_ptr->m_state_new);
    }
    // copy from old state
    m_state_old.copyTo (m_state_old.interval(), m_state_new, m_state_new.interval() );
    m_state_old.define (level_domain, c_NUM, iv_ghosts);
}

// initialize grid
void
GRAMRLevel::initialGrid (const Vector<Box>& a_new_grids)
{
    CH_TIME("GRAMRLevel::initialGrid");

    if (m_verbosity) pout () << "GRAMRLevel::initialGrid " << m_level << endl;

    m_level_grids = a_new_grids;

    const DisjointBoxLayout level_domain = m_grids = loadBalance (a_new_grids);

    IntVect iv_ghosts = m_num_ghosts*IntVect::Unit;
    m_state_new.define (level_domain, c_NUM, iv_ghosts);
    m_state_old.define (level_domain, c_NUM, iv_ghosts);

    m_exchange_copier.exchangeDefine(level_domain, iv_ghosts);
    m_coarse_average.define (level_domain, c_NUM, m_ref_ratio);
    m_fine_interp.define (level_domain, c_NUM, m_ref_ratio, m_problem_domain);

    if (m_coarser_level_ptr != nullptr)
    {
        GRAMRLevel* coarser_gr_amr_level_ptr = gr_cast(m_coarser_level_ptr);
        m_patcher.define(level_domain, coarser_gr_amr_level_ptr->m_grids, c_NUM,
                         coarser_gr_amr_level_ptr->problemDomain(), m_ref_ratio, m_num_ghosts);
    }
}

// things to do after initialization
void
GRAMRLevel::postInitialize ()
{
}

// compute dt
Real
GRAMRLevel::computeDt ()
{
    if ( m_verbosity ) pout () << "GRAMRLevel::computeDt " << m_level << endl;
    return m_dt;
}


// compute dt with initial data
Real
GRAMRLevel::computeInitialDt ()
{
    if ( m_verbosity ) pout () << "GRAMRLevel::computeInitialDt " << m_level << endl;

    m_dt = m_initial_dt_multiplier * m_dx;
    return m_dt;
}

DisjointBoxLayout
GRAMRLevel::loadBalance(const Vector<Box>& a_grids)
{
    CH_TIME("GRAMRLevel::loadBalance");

    // load balance and create boxlayout
    Vector<int> procMap;

    // appears to be faster for all procs to do the loadbalance (ndk)
    LoadBalance(procMap,a_grids);

    if (m_verbosity)
    {
        pout() << "GRAMRLevel::::loadBalance: procesor map: " << endl;
        for (int igrid = 0; igrid < a_grids.size(); ++igrid)
        {
            pout() << igrid << ": " << procMap[igrid] << "  " << endl;
        }
        pout() << endl;
    }

    DisjointBoxLayout dbl(a_grids,procMap,m_problem_domain);
    dbl.close();

    return dbl;
}

// write checkpoint header
#ifdef CH_USE_HDF5
void
GRAMRLevel::writeCheckpointHeader (HDF5Handle& a_handle) const
{
    if ( m_verbosity ) pout () << "GRAMRLevel::writeCheckpointHeader" << endl;

    HDF5HeaderData header;
    header.m_int ["num_components"] = c_NUM;
    char comp_str[30];
    for (int comp = 0; comp < c_NUM; ++comp)
    {
        sprintf (comp_str, "component_%d", comp);
        header.m_string[comp_str] = UserVariables::variable_names[comp];
    }
    header.writeToFile(a_handle);

    if ( m_verbosity ) pout () << header << endl;
}


void
GRAMRLevel::writeCheckpointLevel (HDF5Handle& a_handle) const
{
    CH_TIME("GRAMRLevel::writeCheckpointLevel");

    if ( m_verbosity ) pout () << "GRAMRLevel::writeCheckpointLevel" << endl;

    char level_str[20];
    sprintf (level_str, "%d", m_level);
    const std::string label = std::string ("level_") + level_str;

    a_handle.setGroup (label);

    HDF5HeaderData header;

    header.m_int  ["ref_ratio"]   = m_ref_ratio;
    header.m_int  ["tag_buffer_size"] = m_p.tag_buffer_size;
    header.m_real ["dx"]          = m_dx;
    header.m_real ["dt"]          = m_dt;
    header.m_real ["time"]        = m_time;
    header.m_box  ["prob_domain"] = m_problem_domain.domainBox();

    // Setup the periodicity info
    for (int dir=0; dir<SpaceDim; ++dir)
    {
        char dir_str[20];
        sprintf (dir_str, "%d", dir);
        const std::string periodic_label = std::string ("is_periodic_") + dir_str;
        header.m_int[periodic_label] = m_problem_domain.isPeriodic(dir);
    }

    header.writeToFile(a_handle);

    if ( m_verbosity ) pout () << header << endl;

    write (a_handle, m_state_new.boxLayout ());
    write (a_handle, m_state_new, "data");
}

void
GRAMRLevel::readCheckpointHeader  (HDF5Handle& a_handle)
{
    CH_TIME("GRAMRLevel::readCheckpointHeader");

    if ( m_verbosity ) pout () << "GRAMRLevel::readCheckpointHeader" << endl;

    HDF5HeaderData header;
    header.readFromFile(a_handle);

    if ( m_verbosity ) pout () << "hdf5 header data:" << endl;
    if ( m_verbosity ) pout () << header << endl;

    // read number of components
    if (header.m_int.find ("num_components") == header.m_int.end())
    {
        MayDay::Error ("GRAMRLevel::readCheckpointHeader: checkpoint file does not have num_components");
    }
    int num_comps = header.m_int ["num_components"];
    if (num_comps != c_NUM)
    {
        MayDay::Error ("GRAMRLevel::readCheckpointHeader: num_components in checkpoint file does not match solver");
    }

    // read component names
    std::string state_name;
    char comp_str[60];
    for (int comp = 0; comp < c_NUM; ++comp)
    {
        sprintf (comp_str, "component_%d", comp);
        if (header.m_string.find (comp_str) == header.m_string.end())
        {
            MayDay::Error ("GRAMRLevel::readCheckpointHeader: checkpoint file does not have enough component names");
        }
        state_name = header.m_string [comp_str];
        if (state_name != UserVariables::variable_names[comp])
        {
            if (m_p.ignore_checkpoint_name_mismatch)
            {
                MayDay::Warning("GRAMRLevel::readCheckpointHeader: state_name mismatch error silenced by user.");
            }
            else MayDay::Error("GRAMRLevel::readCheckpointHeader: state_name in checkpoint does not match solver");
        }
    }
}

void
GRAMRLevel::readCheckpointLevel (HDF5Handle& a_handle)
{
    CH_TIME("GRAMRLevel::readCheckpointLevel");
    if ( m_verbosity ) pout () << "GRAMRLevel::readCheckpointLevel" << endl;

    char level_str[20];
    sprintf (level_str, "%d", m_level);
    const std::string label = std::string ("level_") + level_str;

    a_handle.setGroup (label);

    HDF5HeaderData header;
    header.readFromFile (a_handle);

    if ( m_verbosity ) pout () << "hdf5 header data:" << endl;
    if ( m_verbosity ) pout () << header << endl;

    // read refinement ratio
    if (header.m_int.find("ref_ratio") == header.m_int.end())
    {
        MayDay::Error("GRAMRLevel::readCheckpointLevel: file does not contain ref_ratio");
    }
    m_ref_ratio = header.m_int ["ref_ratio"];

    if ( m_verbosity ) pout () << "read ref_ratio = " << m_ref_ratio << endl;

    // read dx
    if (header.m_real.find("dx") == header.m_real.end())
    {
        MayDay::Error("GRAMRLevel::readCheckpointLevel: file does not contain dx");
    }
    m_dx = header.m_real ["dx"];

    if ( m_verbosity ) pout () << "read dx = " << m_dx << endl;

    //Since we have fixed time steping it is better to take dt from the parameter file
    computeInitialDt();
    if ( m_verbosity ) pout () << "dt = " << m_dt << endl;

    // read time
    if (header.m_real.find("time") == header.m_real.end())
    {
        MayDay::Error("GRAMRLevel::readCheckpointLevel: file does not contain time");
    }
    m_time = header.m_real ["time"];
    if ( m_verbosity ) pout () << "read time = " << m_time << endl;

    // read problem domain
    if (header.m_box.find("prob_domain") == header.m_box.end())
    {
        MayDay::Error("GRAMRLevel::readCheckpointLevel: file does not contain prob_domain");
    }
    Box domainBox = header.m_box ["prob_domain"];

    // Get the periodicity info
    bool isPeriodic[SpaceDim] = {false}; //default to false unless other info is available
    for (int dir=0; dir<SpaceDim; ++dir)
    {
        char dir_str[20];
        sprintf (dir_str, "%d", dir);
        const std::string periodic_label = std::string ("is_periodic_") + dir_str;
        if (!(header.m_int.find(periodic_label) == header.m_int.end()))
        {
            isPeriodic[dir] = (header.m_int[periodic_label] == true);
        }
    }
    m_problem_domain = ProblemDomain(domainBox, isPeriodic);

    // read grids
    Vector<Box> grids;
    const int grid_status = read (a_handle, grids);
    if (grid_status != 0)
    {
        MayDay::Error("GRAMRLevel::readCheckpointLevel: file does not contain a Vector<Box>");
    }

    // create level domain
    const DisjointBoxLayout level_domain = m_grids = loadBalance (grids);

    if ( m_verbosity ) pout () << "read level domain: " << endl;
    LayoutIterator lit = level_domain.layoutIterator();
    for (lit.begin(); lit.ok(); ++lit)
    {
        const Box& b = level_domain[lit()];
        if ( m_verbosity ) pout () << lit().intCode() << ": " << b << endl;
        m_level_grids.push_back(b);
    }
    if ( m_verbosity ) pout () << endl;

    // maintain interlevel stuff
    IntVect iv_ghosts = m_num_ghosts*IntVect::Unit;
    m_exchange_copier.exchangeDefine(level_domain, iv_ghosts);
    m_coarse_average.define (level_domain, c_NUM, m_ref_ratio);
    m_fine_interp.define (level_domain, c_NUM, m_ref_ratio, m_problem_domain);

    if (m_coarser_level_ptr != nullptr)
    {
        GRAMRLevel* coarser_gr_amr_level_ptr = gr_cast(m_coarser_level_ptr);
        m_patcher.define(level_domain, coarser_gr_amr_level_ptr->m_grids,
                         c_NUM, coarser_gr_amr_level_ptr->problemDomain(),
                         m_ref_ratio, m_num_ghosts);
    }

    // reshape state with new grids
    m_state_new.define (level_domain, c_NUM, iv_ghosts);
    const int data_status = read<FArrayBox> (a_handle, m_state_new, "data", level_domain);
    if (data_status != 0)
    {
        MayDay::Error("GRAMRLevel::readCheckpointLevel: file does not contain state data");
    }
    m_state_old.define (level_domain, c_NUM, iv_ghosts);
}

void
GRAMRLevel::writePlotLevel (HDF5Handle& a_handle) const
{
}

void
GRAMRLevel::writePlotHeader (HDF5Handle& a_handle) const
{
}
#endif /*ifdef CH_USE_HDF5*/

// methods used with LevelRK4
// evaluate d(soln)/dt at current time based on soln
void
GRAMRLevel::evalRHS(TSoln& rhs, // d(soln)/dt based on soln
                  TSoln& soln, // soln at current time
                  TFR& fineFR,  // flux register w/ finer level
                  TFR& crseFR,  // flux register w/ crse level
                  const TSoln& oldCrseSoln, // old-time crse solution
                  Real oldCrseTime,    // old crse time
                  const TSoln& newCrseSoln,  // new-time crse solution
                  Real newCrseTime,   // new crse time
                  Real time,   // current time centering of soln
                  Real fluxWeight // weight to apply to fluxRegister updates
                 )
{
    CH_TIME("GRAMRLevel::evalRHS");

    soln.exchange(m_exchange_copier);

    if (oldCrseSoln.isDefined())
    {
        // Fraction "a_time" falls between the old and the new coarse times
        Real alpha = (time - oldCrseTime) / (newCrseTime - oldCrseTime);

        // Truncate the fraction to the range [0,1] to remove floating-point
        // subtraction roundoff effects
        Real eps = 0.04 * m_dt / m_ref_ratio;
        if (Abs(alpha) < eps) alpha = 0.0;
        else if (Abs(1.0-alpha) < eps) alpha = 1.0;

        // Current time before old coarse time
        if (alpha < 0.0) MayDay::Error( "GRAMRLevel::evalRHS: alpha < 0.0");
        // Current time after new coarse time
        if (alpha > 1.0) MayDay::Error( "GRAMRLevel::evalRHS: alpha > 1.0");

        // Interpolate ghost cells from next coarser level in space and time
        m_patcher.fillInterp(soln,alpha,0,0,c_NUM);
    }
    //Time and count the RHS if possible
    if (m_profilingInfo != nullptr) m_profilingInfo->resetCounters();

    specificEvalRHS(soln, rhs, time); //Call the problem specific rhs

    if (m_profilingInfo != nullptr) m_profilingInfo->readCounters();
}

// implements soln += dt*rhs
void
GRAMRLevel::updateODE(TSoln& soln, const TSoln& rhs, Real dt)
{
    CH_TIME("GRAMRLevel::updateODE");
    soln.plus(rhs,dt);

    specificUpdateODE(soln, rhs, dt);
}

// define data holder newSoln based on existingSoln,
// including ghost cell specification
void
GRAMRLevel::defineSolnData(TSoln& newSoln,
                         const TSoln& existingSoln)
{
    newSoln.define(existingSoln.disjointBoxLayout(),existingSoln.nComp(),
                    existingSoln.ghostVect());
}

// define data holder for RHS based on existingSoln including ghost cell specification
// (which in most cases is no ghost cells)
void
GRAMRLevel::defineRHSData(TSoln& newRHS, const TSoln& existingSoln)
{
    newRHS.define(existingSoln.disjointBoxLayout(),existingSoln.nComp());
}

/// copy data in src into dest
void
GRAMRLevel::copySolnData(TSoln& dest, const TSoln& src)
{
    src.copyTo(src.interval(),dest,dest.interval());
}

void
GRAMRLevel::fillAllGhosts()
{
    // If there is a coarser level then interpolate undefined ghost cells
    if (m_coarser_level_ptr != nullptr)
    {
        GRAMRLevel* coarser_gr_amr_level_ptr = gr_cast(m_coarser_level_ptr);
        m_patcher.fillInterp(m_state_new, coarser_gr_amr_level_ptr->m_state_new, 0, 0, c_NUM);
    }
    fillIntralevelGhosts();
}

void
GRAMRLevel::fillIntralevelGhosts()
{
    m_state_new.exchange(m_exchange_copier);
    fillBdyGhosts();
}
