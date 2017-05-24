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

void GRAMR::conclude()
{

  AMR::conclude();
#ifdef USE_CATALYST
  m_insitu->finalise();
#endif

}


#ifdef USE_CATALYST
void GRAMR::setupCatalyst(int a_num_pyScript, string a_pyScript,int a_renderLevel)
{
  pout () << "Setup Catalyst: " << a_pyScript << endl;
  m_insitu = new Insitu;
  m_insitu->initialise(this, a_num_pyScript, a_pyScript);
  m_renderLevel = a_renderLevel;
}
#endif
