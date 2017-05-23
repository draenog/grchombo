#include "GRAMR.hpp"

#ifdef USE_CATALYST
#include "Insitu/Insitu.hpp"
#endif




void GRAMR::setDefaultValues()
{
  AMR::setDefaultValues();
#ifdef USE_CATALYST
  m_renderLevel = -1;
#endif
}

void GRAMR::conclude() const
{

  AMR::conclude();
#ifdef USE_CATALYST
  m_insitu.finalise();
#endif

}


#ifdef USE_CATALYST
void GRAMR::setupCatalyst(int a_num_pyScript = 0, string a_pyScript,int a_renderLevel);
{
  m_insitu.initialise(this, a_num_pyScript, a_pyScript);
  m_renderLevel = a_renderLevel;
}
#endif
