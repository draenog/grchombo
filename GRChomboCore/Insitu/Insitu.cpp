#include "Insitu.hpp"
#include "../GRAMR.hpp"
#include "../GRAMRLevel.hpp"


Insitu::Insitu()
{
  m_processor = NULL;
  m_vtkGrid = NULL;
}
Insitu::Insitu(GRAMR * a_amr, int a_num_pyScript,string a_pyScript)
{
  m_processor = NULL;      
  m_vtkGrid = NULL; 
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
  /*
  int no_levels = m_amr->finest_level() + 1;
  for(int ilevel=0; ilevel< no_levels; ilevel++) {
      GRAMRLevel *level = (GRAMRLevel *) m_amr->amrlevels(ilevel);
      const DisjointBoxLayout& level_domain = level->m_state_new.disjointBoxLayout();
      DataIterator diter(level_domain);
      int nbox = diter.size();
      pout() << "updateVTKgrid: Number of boxex " << nbox << endl;
      for(int ibox = 0; ibox < nbox; ++ibox)
      {
          DataIndex di = diter[ibox];
          const Box& b = level_domain[di];
          const IntVect& smallEnd = b.smallEnd();
          const IntVect& bigEnd = b.bigEnd();
          pout () << "updateVTKgrid: Number of boxex " << "Box no " << ibox << " small end " << smallEnd << endl;
          pout () << "updateVTKgrid: Number of boxex " << "Box no " << ibox << " big end " << bigEnd << endl;
      }
  }
  */
  m_vtkGrid = vtkOverlappingAMR::New();

  int numLevels = m_amr->finest_level() + 1; 

  int * blockPerLevel;
  blockPerLevel = new int[numLevels];
  for(int ilevel=0; ilevel< numLevels; ilevel++)
    {
      GRAMRLevel *level = (GRAMRLevel *) m_amr->amrlevels(ilevel);
      const DisjointBoxLayout& level_domain = level->m_state_new.disjointBoxLayout();   
      DataIterator diter(level_domain); 
      int nbox = diter.size(); //TODO change to the correct iterator
      blockPerLevel[ilevel] = nbox;
    }
  blockPerLevel[0] = 1;
  //m_vtkGrid->Initialize(numLevels,blockPerLevel);
  m_vtkGrid->Initialize(1,blockPerLevel);

  int power2 =1;
  float spacing=1.0;
  for(int ilevel=0; ilevel< numLevels; ilevel++)
    {
      GRAMRLevel *level = (GRAMRLevel *) m_amr->amrlevels(ilevel);
      const DisjointBoxLayout& level_domain = level->m_state_new.disjointBoxLayout();
      DataIterator diter(level_domain);
      int nbox = diter.size();
      for(int ibox = 0; ibox < nbox; ++ibox)
      {
          DataIndex di = diter[ibox];
          const Box& b = level_domain[di];
          const IntVect& smallEnd = b.smallEnd();
          const IntVect& bigEnd = b.bigEnd();
          pout () << "updateVTKgrid: Number of boxex " << "Box no " << ibox << " small end " << smallEnd << endl;
          pout () << "updateVTKgrid: Number of boxex " << "Box no " << ibox << " big end " << bigEnd << endl;
      
	  vtkUniformGrid * ug = vtkUniformGrid::New();
	  float origin[3];
	  origin[0]= (float)smallEnd[0] / power2;
	  origin[1]= (float)smallEnd[1] / power2;
	  origin[2]= (float)smallEnd[2] / power2;
	  int extent[3];
	  extent[0] = bigEnd[0] - smallEnd[0] + 1;
	  extent[1] = bigEnd[1] - smallEnd[1] + 1;
	  extent[2] = bigEnd[2] - smallEnd[2] + 1;

	  //ug->SetOrigin(origin[0],origin[1],origin[2]);
	  ug->SetOrigin(0, 0, 0);
	  ug->SetSpacing(spacing,spacing,spacing);
	  //ug->SetExtent(0,extent[0],0,extent[1],0,extent[2]);
	  ug->SetExtent(0,10,0,10,0,10);
          cerr << "BOUNDS: " << endl;
          double * bounds = ug->GetBounds();
          for(int i=0; i<6; i++) {
              cerr << "       " << bounds[i] << endl;
          }


	  vtkAMRBox box(smallEnd[0],smallEnd[1],smallEnd[2], bigEnd[0] + 1,bigEnd[1] + 1,bigEnd[2] + 1);
	  
	  if (ilevel==0 && ibox==0)
          {
	  //m_vtkGrid->SetAMRBox(ilevel,ibox,box);
	  m_vtkGrid->SetAMRBox(0,0,box);
          }

	  //TODO loop ofer every non-local box BUT:
	  // if the box is not local:
	  // m_vtkGrid->SetDataSet(ilevel,ibox,NULL);
	  // else:
	  if (ilevel==0 && ibox==0)
          {
          cerr << "MAKING ONE" << ilevel << " " << ibox << endl;
	  m_vtkGrid->SetDataSet(0,0,ug);
          }
	  //ug->Delete();


      }
      power2 *= 2;
      spacing /= 2.0;

      m_vtkGrid->SetRefinementRatio(0,2.0);
    }
  cerr << "BOUNDS: " << endl;
  double * bounds;
  m_vtkGrid->GetBounds(bounds);
  for(int i=0; i<6; i++) {
      cerr << "       " << bounds[i] << endl;
  }
 cerr << "Printing ONE" << endl;
  m_vtkGrid->GenerateParentChildInformation();
  //vtkAMRUtilities::BlankCells(m_vtkGrid);
  m_vtkGrid->PrintSelf(cerr, vtkIndent(0));
  cerr << "------------------- " << m_vtkGrid << endl;

}
void Insitu::addArray(string a_arrayName, int a_arrayID)
{
  /*
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
  */
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
