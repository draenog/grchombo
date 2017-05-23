#ifndef INSITU_HPP
#define INSITU_HPP

#include "../GRAMR.hpp"

#include <vtkCPProcessor.h>
#include <vtkImageData.h>
#include <vtkCPDataDescription.h>
#include <vtkCPInputDataDescription.h>
#include <vtkCPPythonScriptPipeline.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkNew.h>


class GRAMR;


class Insitu
{
public:
  Insitu();
  Insitu(GRAMR * a_amr, int a_num_pyScript, string a_pyScript);
  ~Insitu();

  void initialise(GRAMR * a_amr, int a_num_pyScript, string a_pyScript);
  void finalise();


  void updateVTKgrid();
  void addArray(string a_arrayName, int a_arrayID);
  void updateArrayValues(string a_arrayName, int a_arrayID);
  void coprocess();


private:

  GRAMR * m_amr;
  int m_num_pyScript;
  string m_pyScript;

  vtkCPProcessor* m_processor; // static data
  vtkImageData * m_vtkGrid;


};

#endif
