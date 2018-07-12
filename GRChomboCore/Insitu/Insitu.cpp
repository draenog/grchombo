#include "Insitu.hpp"
#include "../GRAMR.hpp"
#include "../GRAMRLevel.hpp"
#include <vtkAMRInformation.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>



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
  m_vtkGrid->Initialize(numLevels,blockPerLevel);
  double origin_global[3] = {0, 0, 0};
  m_vtkGrid->SetOrigin(origin_global);

  for(int ilevel=0; ilevel< numLevels; ilevel++)
    {
      GRAMRLevel *level = (GRAMRLevel *) m_amr->amrlevels(ilevel);
      const Real spacing = level->m_dx;
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
	  double origin[3];
	  origin[0]= (float)smallEnd[0] * spacing;
	  origin[1]= (float)smallEnd[1] * spacing;
	  origin[2]= (float)smallEnd[2] * spacing;
	  int extent[3];
	  extent[0] = bigEnd[0] - smallEnd[0] + 1;
	  extent[1] = bigEnd[1] - smallEnd[1] + 1;
	  extent[2] = bigEnd[2] - smallEnd[2] + 1;

	  //ug->SetOrigin(origin[0],origin[1],origin[2]);
	  ug->SetOrigin(0,0,0);
	  ug->SetSpacing(spacing, spacing, spacing);
	  //ug->SetExtent(0,extent[0],0,extent[1],0,extent[2]);
	  ug->SetExtent(smallEnd[0],bigEnd[0]+1,smallEnd[1],bigEnd[1]+1, smallEnd[2],bigEnd[2]+1);
          cerr << "BOUNDS: " << endl;
          double * bounds = ug->GetBounds();
          for(int i=0; i<6; i++) {
              cerr << "       " << bounds[i] << endl;
          }

          double spacing_arr[3] = {spacing, spacing, spacing};
	  //vtkAMRBox box(smallEnd[0],smallEnd[1],smallEnd[2], bigEnd[0] + 1,bigEnd[1] + 1,bigEnd[2] + 1);
          vtkAMRBox box(origin, extent, spacing_arr, origin_global);
	  
	  m_vtkGrid->SetAMRBox(ilevel,ibox,box);

	  //TODO loop ofer every non-local box BUT:
	  // if the box is not local:
	  // m_vtkGrid->SetDataSet(ilevel,ibox,NULL);
	  // else:
          cerr << "MAKING ONE" << ilevel << " " << ibox << endl;
	  m_vtkGrid->SetDataSet(ilevel,ibox,ug);
	  ug->Delete();


      }
      double spacing_arr[3] = {spacing, spacing, spacing};
      m_vtkGrid->SetRefinementRatio(ilevel,2);
      m_vtkGrid->SetSpacing(ilevel, spacing_arr);
    }
 cerr << "Printing ONE" << endl;
 cerr << "BBBBBBBBBBBBBBBBBBBBBBBBBB" << endl;
  //m_vtkGrid->GenerateParentChildInformation();
  //vtkAMRUtilities::BlankCells(m_vtkGrid);
  m_vtkGrid->PrintSelf(cerr, vtkIndent(0));
  cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA" << m_vtkGrid << endl;

 double bounds[6]; 
 m_vtkGrid->GetBounds(1,0,bounds);
 for(int i=0; i<6; i++) {
     cerr << "Bound AMR " << bounds[i] << endl;
 }

}
void Insitu::addArray(string a_arrayName, int a_arrayID)
{
  vtkAMRInformation * amrinfo = m_vtkGrid->GetAMRInfo();

  for(int ilevel = 0; ilevel < amrinfo->GetNumberOfLevels(); ilevel ++) {
    GRAMRLevel *level = (GRAMRLevel *) m_amr->amrlevels(ilevel);
    level->specificPostTimeStep();
    const DisjointBoxLayout& level_domain = level->m_state_new.disjointBoxLayout();
    const GRLevelData& boxes_with_ghosts = level->m_state_new;
    DataIterator diter = boxes_with_ghosts.dataIterator();
    cerr << "Add array " << amrinfo->GetNumberOfDataSets(ilevel) << " " << diter.size() << endl;
    for(int iblock = 0; iblock < amrinfo->GetNumberOfDataSets(ilevel); iblock++) {
        DataIndex di = diter[iblock];
        const Box& b = level_domain[di];
        const FArrayBox& b_ghost = boxes_with_ghosts[di];
        FArrayBox& stat_fab = level->m_state_new[di];
        cerr << "DDDDDDDDDD number of components " << stat_fab.nComp() << endl;
        const double* data = stat_fab.dataPtr(0);
        const IntVect lowcorner = b.smallEnd();
        const IntVect topcorner = b.bigEnd();
        const IntVect lowcorner_ghost = b_ghost.smallEnd();
        const IntVect topcorner_ghost = b_ghost.bigEnd();
        vtkUniformGrid * ug = m_vtkGrid->GetDataSet(ilevel, iblock);
        vtkDoubleArray * arr_psi4r = vtkDoubleArray::New();
        vtkDoubleArray * arr_psi4i = vtkDoubleArray::New();
        arr_psi4r->SetNumberOfTuples(ug->GetNumberOfCells());
        arr_psi4i->SetNumberOfTuples(ug->GetNumberOfCells());
        arr_psi4r->SetName("Psi4r");
        arr_psi4i->SetName("Psi4i");
        size_t index = 0;
        double maxval = -1.e299;
        pout() << "Corners: " << ilevel << " " << iblock << ": " << lowcorner[2] << " " << lowcorner[1] << " " << lowcorner[0] << endl;
        pout() << "Corners: " << topcorner[2] << " " << topcorner[1] << " " << topcorner[0] << endl;
        pout() << "Corners: " << ilevel << " " << iblock << ": " << lowcorner_ghost[2] << " " << lowcorner_ghost[1] << " " << lowcorner_ghost[0] << endl;
        pout() << "Corners: " << topcorner_ghost[2] << " " << topcorner_ghost[1] << " " << topcorner_ghost[0] << endl;
        for(int iz = lowcorner[2]; iz <= topcorner[2]; iz++) {
            for(int iy = lowcorner[1]; iy <= topcorner[1]; iy++) {
                for(int ix = lowcorner[0]; ix <= topcorner[0]; ix++) {
                    IntVect vect(ix, iy, iz);
                    arr_psi4r->SetValue(index, stat_fab(vect, c_Psi4r));
                    arr_psi4i->SetValue(index, stat_fab(vect, c_Psi4i));
                    maxval = std::max(maxval, stat_fab(vect, c_Psi4i));
                    index++;
                }
            }
        }
        ug->GetCellData()->AddArray(arr_psi4r);
        ug->GetCellData()->AddArray(arr_psi4i);
    }
  }
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

  pout() <<"call: Insitu::coprocess"<<endl;
  
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
  pout() <<"ending: Insitu::coprocess"<<endl;
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
