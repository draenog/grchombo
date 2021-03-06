
from paraview.simple import *
from paraview import coprocessing


#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# ParaView 5.4.1 64 bits

#--------------------------------------------------------------
# Global screenshot output options
imageFileNamePadding=0
rescale_lookuptable=False


# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.4.1

      # ----------------------------------------------------------------
      # setup views used in the visualization
      # ----------------------------------------------------------------

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # Create a new 'Render View'
      renderView1 = CreateView('RenderView')
      renderView1.ViewSize = [1028, 602]
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.CenterOfRotation = [8.000000357627869, 7.9999998807907104, 7.9999998807907104]
      renderView1.StereoType = 0
      renderView1.CameraPosition = [73.22345201521156, 7.9999998807907104, 7.9999998807907104]
      renderView1.CameraFocalPoint = [8.000000357627869, 7.9999998807907104, 7.9999998807907104]
      renderView1.CameraViewUp = [0.0, 0.0, 1.0]
      renderView1.CameraParallelScale = 16.881071476306236
      renderView1.Background = [0.32, 0.34, 0.43]

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='image_%t.png', freq=1, fittoscreen=1, magnification=1, width=1028, height=602, cinema={})
      renderView1.ViewTime = datadescription.GetTime()

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'VisItChomboReader'
      # create a producer from a simulation input
      binaryBH_0000003dhdf5 = coprocessor.CreateProducer(datadescription, 'input')

      # create a new 'Slice'
      slice1 = Slice(Input=binaryBH_0000003dhdf5)
      slice1.SliceType = 'Plane'
      slice1.SliceOffsetValues = [0.0]

      # init the 'Plane' selected for 'SliceType'
      slice1.SliceType.Origin = [8.0, 8.0, 8.0]

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # get color transfer function/color map for 'Psi4i'
      psi4iLUT = GetColorTransferFunction('Psi4i')
      psi4iLUT.RGBPoints = [-0.04266900966712457, 0.231373, 0.298039, 0.752941, -0.0013345777004220571, 0.865003, 0.865003, 0.865003, 0.03999985426628046, 0.705882, 0.0156863, 0.14902]
      psi4iLUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'Psi4i'
      psi4iPWF = GetOpacityTransferFunction('Psi4i')
      psi4iPWF.Points = [-0.04266900966712457, 0.0, 0.5, 0.0, 0.03999985426628046, 1.0, 0.5, 0.0]
      psi4iPWF.ScalarRangeInitialized = 1

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from slice1
      slice1Display = Show(slice1, renderView1)
      # trace defaults for the display properties.
      slice1Display.Representation = 'Surface'
      slice1Display.ColorArrayName = ['CELLS', 'Psi4i']
      slice1Display.LookupTable = psi4iLUT
      slice1Display.OSPRayScaleArray = 'Psi4i'
      slice1Display.OSPRayScaleFunction = 'PiecewiseFunction'
      slice1Display.SelectOrientationVectors = 'None'
      slice1Display.ScaleFactor = 1.6
      slice1Display.SelectScaleArray = 'None'
      slice1Display.GlyphType = 'Arrow'
      slice1Display.GlyphTableIndexArray = 'None'
      slice1Display.DataAxesGrid = 'GridAxesRepresentation'
      slice1Display.PolarAxes = 'PolarAxesRepresentation'
      slice1Display.GaussianRadius = 0.8
      slice1Display.SetScaleArray = [None, '']
      slice1Display.ScaleTransferFunction = 'PiecewiseFunction'
      slice1Display.OpacityArray = [None, '']
      slice1Display.OpacityTransferFunction = 'PiecewiseFunction'

      # show color legend
      slice1Display.SetScalarBarVisibility(renderView1, True)

      # setup the color legend parameters for each legend in this view

      # get color legend/bar for psi4iLUT in view renderView1
      psi4iLUTColorBar = GetScalarBar(psi4iLUT, renderView1)
      psi4iLUTColorBar.Title = 'Psi4i'
      psi4iLUTColorBar.ComponentTitle = ''

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(binaryBH_0000003dhdf5)
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
# Global variable that will hold the pipeline for each timestep
# Creating the CoProcessor object, doesn't actually create the ParaView pipeline.
# It will be automatically setup when coprocessor.UpdateProducers() is called the
# first time.
coprocessor = CreateCoProcessor()

#--------------------------------------------------------------
# Enable Live-Visualizaton with ParaView and the update frequency
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
    coprocessor.WriteImages(datadescription, rescale_lookuptable=rescale_lookuptable,
        image_quality=0, padding_amount=imageFileNamePadding)

    # Live Visualization, if enabled.
    coprocessor.DoLiveVisualization(datadescription, "localhost", 22222)
