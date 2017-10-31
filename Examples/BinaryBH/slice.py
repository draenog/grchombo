
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
      renderView1.CenterOfRotation = [8.000000238418579, 8.000000596046448, 8.000000596046448]
      renderView1.StereoType = 0
      renderView1.CameraPosition = [36.108664328047055, -8.559285600314375, -6.5383391835064]
      renderView1.CameraFocalPoint = [8.000000238418579, 8.000000596046448, 8.000000596046448]
      renderView1.CameraViewUp = [0.47678486673133735, 0.0383131688453025, 0.8781846570904165]
      renderView1.CameraParallelScale = 17.12217184193382
      renderView1.Background = [0.32, 0.34, 0.43]

      # init the 'GridAxes3DActor' selected for 'AxesGrid'
      renderView1.AxesGrid.Visibility = 1

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='image_%t.png', freq=1, fittoscreen=1, magnification=1, width=632, height=602, cinema={})
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

      # get color transfer function/color map for 'chi'
      chiLUT = GetColorTransferFunction('chi')
      chiLUT.RGBPoints = [0.037834883915598305, 0.231373, 0.298039, 0.752941, 0.4030278320883695, 0.865003, 0.865003, 0.865003, 0.7682207802611407, 0.705882, 0.0156863, 0.14902]
      chiLUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'chi'
      chiPWF = GetOpacityTransferFunction('chi')
      chiPWF.Points = [0.037834883915598305, 0.0, 0.5, 0.0, 0.7682207802611407, 1.0, 0.5, 0.0]
      chiPWF.ScalarRangeInitialized = 1

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from binaryBH_0000003dhdf5
      binaryBH_0000003dhdf5Display = Show(binaryBH_0000003dhdf5, renderView1)
      # trace defaults for the display properties.
      binaryBH_0000003dhdf5Display.Representation = 'AMR Blocks'
      binaryBH_0000003dhdf5Display.ColorArrayName = ['CELLS', 'chi']
      binaryBH_0000003dhdf5Display.LookupTable = chiLUT
      binaryBH_0000003dhdf5Display.OSPRayScaleArray = 'chi'
      binaryBH_0000003dhdf5Display.OSPRayScaleFunction = 'PiecewiseFunction'
      binaryBH_0000003dhdf5Display.SelectOrientationVectors = 'None'
      binaryBH_0000003dhdf5Display.ScaleFactor = 1.6
      binaryBH_0000003dhdf5Display.SelectScaleArray = 'None'
      binaryBH_0000003dhdf5Display.GlyphType = 'Arrow'
      binaryBH_0000003dhdf5Display.GlyphTableIndexArray = 'None'
      binaryBH_0000003dhdf5Display.DataAxesGrid = 'GridAxesRepresentation'
      binaryBH_0000003dhdf5Display.PolarAxes = 'PolarAxesRepresentation'
      binaryBH_0000003dhdf5Display.ScalarOpacityUnitDistance = 0.610711830985561
      binaryBH_0000003dhdf5Display.ScalarOpacityFunction = chiPWF

      # show color legend
      binaryBH_0000003dhdf5Display.SetScalarBarVisibility(renderView1, True)

      # show data from slice1
      slice1Display = Show(slice1, renderView1)
      # trace defaults for the display properties.
      slice1Display.Representation = 'Wireframe'
      slice1Display.ColorArrayName = ['CELLS', 'chi']
      slice1Display.LookupTable = chiLUT
      slice1Display.OSPRayScaleArray = 'chi'
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

      # get color legend/bar for chiLUT in view renderView1
      chiLUTColorBar = GetScalarBar(chiLUT, renderView1)
      chiLUTColorBar.Title = 'chi'
      chiLUTColorBar.ComponentTitle = ''

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(slice1)
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
