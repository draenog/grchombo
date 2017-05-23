#include "Insitu.hpp"
#include "../GRAMR.hpp"

#include <vtkCPProcessor.h>

Insitu::Insitu()
{
  //  m_processor = NULL;
  //  m_vtkGrid = NULL;
}
Insitu::Insitu(GRAMR * a_amr, int a_num_pyScript,string a_pyScript)
{
  //  m_processor = NULL;                                                                                    
  //  m_vtkGrid = NULL; 
  this->initialise(a_amr, a_num_pyScript,a_pyScript);
}

Insitu::~Insitu()
{
}

void Insitu::initialise(GRAMR * a_amr, int a_num_pyScript, string a_pyScript)
{
  if(a_amr != NULL)
    {
      m_amr = a_amr;
      if(a_num_pyScript!=0)
	{
	  m_num_pyScript = a_num_pyScript;
	  m_pyScript = a_pyScript;

	  //TODO: initialise the coprocessor
	  /*
	    if(processor_ == NULL)
	    {
	    m_processor = vtkCPProcessor::New();
	    m_processor_->Initialize();
	    }
	    else
	    {
	    m_processor_->RemoveAllPipelines();
	    }

	    here how to do it?
	    

	    for(int i=paraview_scripts_offset;i<paraview_scripts_number+paraview_scripts_offset;i++)
	    {
	    vtkCPPythonScriptPipeline* pipeline = vtkCPPythonScriptPipeline::New();
	    pipeline->Initialize(argv[i]);
	    processor_->AddPipeline(pipeline);
	    pipeline->Delete();
	    }


	   */
	}
      else
	{
	  cout<<"insitu class need to be intialised passing a Paraview python script"<<endl;
	}
    }
  else
    {
      cout<<"insitu class need to be intialised passing an AMR object"<<endl;
    }
}

void Insitu::updateVTKgrid()
{
  cout<<"call: Insitu::updateVTKgrid"<<endl;
}
void Insitu::addArray(string a_arrayName, int a_arrayID)
{
  cout<<"call: Insitu::addArray"<<endl;
}
void Insitu::updateArrayValues(string a_arrayName, int a_arrayID)
{
  cout<<"call: Insitu::updateArrayValues"<<endl;
}
void Insitu::coprocess()
{
  cout<<"call: Insitu::coprocess"<<endl;
}

void Insitu::finalise()
{
  cout<<"call: Insitu::finalyse"<<endl;
}
