#ifndef GRAMR_HPP
#define GRAMR_HPP

#include "AMR.H"

#ifdef USE_CATALYST
#include "Insitu/Insitu.hpp"
#endif


class GRAMR : public AMR
{

public:

  GRAMR * m_amr;
  void setDefaultValues();
  void conclude() const;


#ifdef USE_CATALYST
  void setupCatalyst(int a_num_pyScript = 0, string a_pyScript);
  Insitu m_insitu;
  int m_renderLevel;
#endif


};





#endif
