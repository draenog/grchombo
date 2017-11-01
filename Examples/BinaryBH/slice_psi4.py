
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
      renderView1.ViewSize = [632, 602]
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.CenterOfRotation = [8.000000357627869, 7.9999998807907104, 7.9999998807907104]
      renderView1.StereoType = 0
      renderView1.CameraPosition = [-35.21234350850901, 28.59417353183135, -16.782654637684114]
      renderView1.CameraFocalPoint = [8.000000357627878, 7.999999880790722, 7.999999880790728]
      renderView1.CameraViewUp = [-0.4820464441025623, 0.04167586691431631, 0.8751538995199732]
      renderView1.CameraParallelScale = 16.881071476306236
      renderView1.Background = [0.32, 0.34, 0.43]

      # init the 'GridAxes3DActor' selected for 'AxesGrid'
      renderView1.AxesGrid.Visibility = 1

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='image_%t.png', freq=1, fittoscreen=0, magnification=1, width=632, height=602, cinema={})
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

      # create a new 'Calculator'
      calculator1 = Calculator(Input=slice1)
      calculator1.AttributeMode = 'Cell Data'
      calculator1.ResultArrayName = 'Psi4_mag'
      calculator1.Function = 'sqrt(Psi4r^2 + Psi4i^2)'

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # get color transfer function/color map for 'Psi4_mag'
      psi4_magLUT = GetColorTransferFunction('Psi4_mag')
      psi4_magLUT.RGBPoints = [9.705938678127315e-05, 0.0, 0.0, 1.0, 0.04425295113860145, 1.0, 0.0, 0.0]
      psi4_magLUT.UseLogScale = 1
      psi4_magLUT.ColorSpace = 'HSV'
      psi4_magLUT.NanColor = [0.498039215686, 0.498039215686, 0.498039215686]
      psi4_magLUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'Psi4_mag'
      psi4_magPWF = GetOpacityTransferFunction('Psi4_mag')
      psi4_magPWF.Points = [9.70593867812732e-05, 0.0, 0.5, 0.0, 0.044252951138601414, 1.0, 0.5, 0.0]
      psi4_magPWF.ScalarRangeInitialized = 1

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from binaryBH_0000003dhdf5
      binaryBH_0000003dhdf5Display = Show(binaryBH_0000003dhdf5, renderView1)
      # trace defaults for the display properties.
      binaryBH_0000003dhdf5Display.Representation = 'AMR Blocks'
      binaryBH_0000003dhdf5Display.ColorArrayName = [None, '']
      binaryBH_0000003dhdf5Display.OSPRayScaleArray = 'Psi4i'
      binaryBH_0000003dhdf5Display.OSPRayScaleFunction = 'PiecewiseFunction'
      binaryBH_0000003dhdf5Display.SelectOrientationVectors = 'None'
      binaryBH_0000003dhdf5Display.ScaleFactor = 1.6
      binaryBH_0000003dhdf5Display.SelectScaleArray = 'None'
      binaryBH_0000003dhdf5Display.GlyphType = 'Arrow'
      binaryBH_0000003dhdf5Display.GlyphTableIndexArray = 'None'
      binaryBH_0000003dhdf5Display.DataAxesGrid = 'GridAxesRepresentation'
      binaryBH_0000003dhdf5Display.PolarAxes = 'PolarAxesRepresentation'
      binaryBH_0000003dhdf5Display.ScalarOpacityUnitDistance = 0.610711830985561

      # show data from calculator1
      calculator1Display = Show(calculator1, renderView1)
      # trace defaults for the display properties.
      calculator1Display.Representation = 'Surface'
      calculator1Display.ColorArrayName = ['CELLS', 'Psi4_mag']
      calculator1Display.LookupTable = psi4_magLUT
      calculator1Display.OSPRayScaleArray = 'Psi4i'
      calculator1Display.OSPRayScaleFunction = 'PiecewiseFunction'
      calculator1Display.SelectOrientationVectors = 'None'
      calculator1Display.ScaleFactor = 1.6
      calculator1Display.SelectScaleArray = 'None'
      calculator1Display.GlyphType = 'Arrow'
      calculator1Display.GlyphTableIndexArray = 'None'
      calculator1Display.DataAxesGrid = 'GridAxesRepresentation'
      calculator1Display.PolarAxes = 'PolarAxesRepresentation'
      calculator1Display.GaussianRadius = 0.8
      calculator1Display.SetScaleArray = [None, '']
      calculator1Display.ScaleTransferFunction = 'PiecewiseFunction'
      calculator1Display.OpacityArray = [None, '']
      calculator1Display.OpacityTransferFunction = 'PiecewiseFunction'

      # show color legend
      calculator1Display.SetScalarBarVisibility(renderView1, True)

      # setup the color legend parameters for each legend in this view

      # get color legend/bar for psi4_magLUT in view renderView1
      psi4_magLUTColorBar = GetScalarBar(psi4_magLUT, renderView1)
      psi4_magLUTColorBar.Title = 'Psi4_mag'
      psi4_magLUTColorBar.ComponentTitle = ''

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(calculator1)
      # ----------------------------------------------------------------
    return Pipeline()

  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  freqs = {'input': [1, 1, 1]}
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
