
from paraview.simple import *
from paraview import coprocessing


#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# ParaView 5.3.0 64 bits


# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.3.0

      # ----------------------------------------------------------------
      # setup views used in the visualization
      # ----------------------------------------------------------------

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # Create a new 'Render View'
      renderView1 = CreateView('RenderView')
      renderView1.ViewSize = [1452, 891]
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.CenterOfRotation = [8.0, 8.0, 8.0]
      renderView1.StereoType = 0
      renderView1.CameraPosition = [8.0, 8.0, 61.53704343921971]
      renderView1.CameraFocalPoint = [8.0, 8.0, 8.0]
      renderView1.CameraParallelScale = 13.856406460551018
      renderView1.Background = [0.32, 0.34, 0.43]
      renderView1.OrientationAxesVisibility = 1
      renderView1.AxesGrid.Visibility = 1

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='image_%t.png', freq=1, fittoscreen=1, magnification=1, width=1452, height=891, cinema={})
      renderView1.ViewTime = datadescription.GetTime()

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'VisItChomboReader'
      # create a producer from a simulation input
      binaryBH_0000403dhdf5 = coprocessor.CreateProducer(datadescription, 'input')

      # create a new 'ExodusII Writer'
      exodusIIWriter1 = servermanager.writers.ExodusIIWriter(Input=binaryBH_0000403dhdf5)

      # register the writer with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the data, etc.
      coprocessor.RegisterWriter(exodusIIWriter1, filename='filename.e-s.%t', freq=1)

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from binaryBH_0000403dhdf5
      binaryBH_0000403dhdf5Display = Show(binaryBH_0000403dhdf5, renderView1)
      # trace defaults for the display properties.
      binaryBH_0000403dhdf5Display.Representation = 'AMR Blocks'
      binaryBH_0000403dhdf5Display.ColorArrayName = [None, '']
      binaryBH_0000403dhdf5Display.OSPRayScaleArray = 'A11'
      binaryBH_0000403dhdf5Display.OSPRayScaleFunction = 'PiecewiseFunction'
      binaryBH_0000403dhdf5Display.SelectOrientationVectors = 'None'
      binaryBH_0000403dhdf5Display.ScaleFactor = 1.6
      binaryBH_0000403dhdf5Display.SelectScaleArray = 'None'
      binaryBH_0000403dhdf5Display.GlyphType = 'Arrow'
      binaryBH_0000403dhdf5Display.PolarAxes = 'PolarAxesRepresentation'
      binaryBH_0000403dhdf5Display.ScalarOpacityUnitDistance = 0.6380929732329265

      # init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
      binaryBH_0000403dhdf5Display.OSPRayScaleFunction.Points = [0.0, 0.0, 0.5, 0.0, 8.0, 1.0, 0.5, 0.0]

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(exodusIIWriter1)
      # ----------------------------------------------------------------
    return Pipeline()

  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  freqs = {'input': [1, 1]}
  coprocessor.SetUpdateFrequencies(freqs)
  return coprocessor

#--------------------------------------------------------------
# Global variables that will hold the pipeline for each timestep
# Creating the CoProcessor object, doesn't actually create the ParaView pipeline.
# It will be automatically setup when coprocessor.UpdateProducers() is called the
# first time.
coprocessor = CreateCoProcessor()

#--------------------------------------------------------------
# Enable Live-Visualizaton with ParaView
coprocessor.EnableLiveVisualization(False, 1)


# ---------------------- Data Selection method ----------------------

def RequestDataDescription(datadescription):
    "Callback to populate the request for current timestep"
    global coprocessor
    if datadescription.GetForceOutput() == True:
        # We are just going to request all fields and meshes from the simulation
        # code/adaptor.
        for i in range(datadescription.GetNumberOfInputDescriptions()):
            datadescription.GetInputDescription(i).AllFieldsOn()
            datadescription.GetInputDescription(i).GenerateMeshOn()
        return

    # setup requests for all inputs based on the requirements of the
    # pipeline.
    coprocessor.LoadRequestedData(datadescription)

# ------------------------ Processing method ------------------------

def DoCoProcessing(datadescription):
    "Callback to do co-processing for current timestep"
    global coprocessor

    # Update the coprocessor by providing it the newly generated simulation data.
    # If the pipeline hasn't been setup yet, this will setup the pipeline.
    coprocessor.UpdateProducers(datadescription)

    # Write output data, if appropriate.
    coprocessor.WriteData(datadescription);

    # Write image capture (Last arg: rescale lookup table), if appropriate.
    coprocessor.WriteImages(datadescription, rescale_lookuptable=False)

    # Live Visualization, if enabled.
    coprocessor.DoLiveVisualization(datadescription, "localhost", 22222)
