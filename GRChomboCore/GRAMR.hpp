#ifndef GRAMR_HPP
#define GRAMR_HPP

#include "AMR.H"

#ifdef USE_CATALYST
#include "Insitu/Insitu.hpp"
#endif

class Insitu;

class GRAMR : public AMR
{

public:

  GRAMR * m_amr;
  void setDefaultValues();
  void conclude();


#ifdef USE_CATALYST
  void setupCatalyst(int a_num_pyScript, string a_pyScript, int a_renderLevel);
  Insitu * m_insitu;
  int m_renderLevel;
#endif

  int finest_level() {
    return m_finest_level;
  }
 
  AMRLevel* amrlevels(int i) {
    return m_amrlevels[i];
  }
};





#endif
