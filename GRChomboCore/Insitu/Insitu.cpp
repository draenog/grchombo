#include "Insitu.hpp"
#include "../GRAMR.hpp"


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
	  
	    if(m_processor == NULL)
	      {
		m_processor = vtkCPProcessor::New();
		m_processor->Initialize();
	      }
	    else
	      {
		m_processor->RemoveAllPipelines();
	      }

	   
	    vtkCPPythonScriptPipeline* pipeline = vtkCPPythonScriptPipeline::New(); 
	    pipeline->Initialize(a_pyScript.c_str());
            m_processor->AddPipeline(pipeline);
            pipeline->Delete(); 

	   
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
  if(m_vtkGrid!=NULL)m_vtkGrid->Delete();

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  
  m_vtkGrid = vtkImageData::New();
  m_vtkGrid->SetExtent(rank,rank+1,0,10,0,10);
}
void Insitu::addArray(string a_arrayName, int a_arrayID)
{
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  vtkDoubleArray * arr = vtkDoubleArray::New();
  arr->SetNumberOfTuples(m_vtkGrid->GetNumberOfPoints());
  arr->SetName(a_arrayName.c_str());
  for(vtkIdType  i = 0;i<m_vtkGrid->GetNumberOfPoints();i++)
    {
      arr->SetValue(i,rank);
    }

  m_vtkGrid->GetPointData()->AddArray(arr);
  arr->Delete();

}
void Insitu::updateArrayValues(string a_arrayName, int a_arrayID)
{
  cout<<"call: Insitu::updateArrayValues"<<endl;
  

}
namespace
{
  double Time = 1;
  int TimeStep = 1;
}
void Insitu::coprocess()
{
  
  Time+=1;
  TimeStep+=1;

  int size;
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  cout<<"call: Insitu::coprocess"<<endl;
  
  vtkNew<vtkCPDataDescription> dataDescription;
  dataDescription->AddInput("input");
  dataDescription->SetTimeData(Time, TimeStep); // should get time and timestep from GRAMR
  
  dataDescription->ForceOutputOn();
  if (m_processor->RequestDataDescription(dataDescription.GetPointer()) != 0)
    {
      dataDescription->GetInputDescriptionByName("input")->SetGrid(m_vtkGrid);
      dataDescription->GetInputDescriptionByName("input")->SetWholeExtent(0,size,0,10,0,10);
      m_processor->CoProcess(dataDescription.GetPointer());
    }
}

void Insitu::finalise()
{
  cout<<"call: Insitu::finalyse"<<endl;

  if(m_vtkGrid)
    {
      m_vtkGrid->Delete();
      m_vtkGrid = NULL;
    }

  if(m_processor)
    {
      m_processor->Delete();
      m_processor = NULL;
    }
}
