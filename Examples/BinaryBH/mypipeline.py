
from paraview.simple import *
from paraview import coprocessing


#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# paraview version 5.5.2

#--------------------------------------------------------------
# Global screenshot output options
imageFileNamePadding=0
rescale_lookuptable=False


# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.5.2

      # ----------------------------------------------------------------
      # setup views used in the visualization
      # ----------------------------------------------------------------

      # trace generated using paraview version 5.5.2

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # get the material library
      materialLibrary1 = GetMaterialLibrary()

      # Create a new 'Render View'
      renderView1 = CreateView('RenderView')
      renderView1.ViewSize = [1107, 645]
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.CenterOfRotation = [256.0, 256.0, 256.0]
      renderView1.StereoType = 0
      renderView1.CameraPosition = [188.02054550291314, -849.130273625168, 1563.314060500172]
      renderView1.CameraFocalPoint = [256.0, 256.0, 256.0]
      renderView1.CameraViewUp = [-0.010802768101508832, 0.7639234377257922, 0.6452164609606243]
      renderView1.CameraParallelScale = 443.40500673763256
      renderView1.Background = [0.32, 0.34, 0.43]
      renderView1.OSPRayMaterialLibrary = materialLibrary1

      # init the 'GridAxes3DActor' selected for 'AxesGrid'
      renderView1.AxesGrid.XTitleFontFile = ''
      renderView1.AxesGrid.YTitleFontFile = ''
      renderView1.AxesGrid.ZTitleFontFile = ''
      renderView1.AxesGrid.XLabelFontFile = ''
      renderView1.AxesGrid.YLabelFontFile = ''
      renderView1.AxesGrid.ZLabelFontFile = ''

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='image_%t.png', freq=1, fittoscreen=0, magnification=1, width=1107, height=645, cinema={})
      renderView1.ViewTime = datadescription.GetTime()

      # ----------------------------------------------------------------
      # restore active view
      SetActiveView(renderView1)
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'VisItChomboReader'
      # create a producer from a simulation input
      plot_wo_catalyst_0000003dhdf5 = coprocessor.CreateProducer(datadescription, 'input')

      # create a new 'Slice'
      slice1 = Slice(Input=plot_wo_catalyst_0000003dhdf5)
      slice1.SliceType = 'Plane'
      slice1.SliceOffsetValues = [0.0]

      # init the 'Plane' selected for 'SliceType'
      slice1.SliceType.Origin = [256.0, 256.0, 256.0]
      slice1.SliceType.Normal = [0.0, 0.0, 1.0]

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from plot_wo_catalyst_0000003dhdf5
      plot_wo_catalyst_0000003dhdf5Display = Show(plot_wo_catalyst_0000003dhdf5, renderView1)

      # trace defaults for the display properties.
      plot_wo_catalyst_0000003dhdf5Display.Representation = 'Outline'
      plot_wo_catalyst_0000003dhdf5Display.ColorArrayName = [None, '']
      plot_wo_catalyst_0000003dhdf5Display.OSPRayScaleFunction = 'PiecewiseFunction'
      plot_wo_catalyst_0000003dhdf5Display.SelectOrientationVectors = 'None'
      plot_wo_catalyst_0000003dhdf5Display.ScaleFactor = 51.2
      plot_wo_catalyst_0000003dhdf5Display.SelectScaleArray = 'None'
      plot_wo_catalyst_0000003dhdf5Display.GlyphType = 'Arrow'
      plot_wo_catalyst_0000003dhdf5Display.GlyphTableIndexArray = 'None'
      plot_wo_catalyst_0000003dhdf5Display.GaussianRadius = 2.56
      plot_wo_catalyst_0000003dhdf5Display.SetScaleArray = [None, '']
      plot_wo_catalyst_0000003dhdf5Display.ScaleTransferFunction = 'PiecewiseFunction'
      plot_wo_catalyst_0000003dhdf5Display.OpacityArray = [None, '']
      plot_wo_catalyst_0000003dhdf5Display.OpacityTransferFunction = 'PiecewiseFunction'
      plot_wo_catalyst_0000003dhdf5Display.DataAxesGrid = 'GridAxesRepresentation'
      plot_wo_catalyst_0000003dhdf5Display.SelectionCellLabelFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.SelectionPointLabelFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.PolarAxes = 'PolarAxesRepresentation'
      plot_wo_catalyst_0000003dhdf5Display.ScalarOpacityUnitDistance = 6.913326157867665

      # init the 'GridAxesRepresentation' selected for 'DataAxesGrid'
      plot_wo_catalyst_0000003dhdf5Display.DataAxesGrid.XTitleFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.DataAxesGrid.YTitleFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.DataAxesGrid.ZTitleFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.DataAxesGrid.XLabelFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.DataAxesGrid.YLabelFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.DataAxesGrid.ZLabelFontFile = ''

      # init the 'PolarAxesRepresentation' selected for 'PolarAxes'
      plot_wo_catalyst_0000003dhdf5Display.PolarAxes.PolarAxisTitleFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.PolarAxes.PolarAxisLabelFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.PolarAxes.LastRadialAxisTextFontFile = ''
      plot_wo_catalyst_0000003dhdf5Display.PolarAxes.SecondaryRadialAxesTextFontFile = ''

      # show data from slice1
      slice1Display = Show(slice1, renderView1)

      # get color transfer function/color map for 'chi'
      chiLUT = GetColorTransferFunction('chi')
      chiLUT.RGBPoints = [0.0009853932076415149, 0.231373, 0.298039, 0.752941, 0.49542285684288034, 0.865003, 0.865003, 0.865003, 0.9898603204781191, 0.705882, 0.0156863, 0.14902]
      chiLUT.ScalarRangeInitialized = 1.0

      # trace defaults for the display properties.
      slice1Display.Representation = 'Surface'
      slice1Display.ColorArrayName = ['CELLS', 'chi']
      slice1Display.LookupTable = chiLUT
      slice1Display.OSPRayScaleFunction = 'PiecewiseFunction'
      slice1Display.SelectOrientationVectors = 'None'
      slice1Display.ScaleFactor = 51.2
      slice1Display.SelectScaleArray = 'None'
      slice1Display.GlyphType = 'Arrow'
      slice1Display.GlyphTableIndexArray = 'None'
      slice1Display.GaussianRadius = 2.56
      slice1Display.SetScaleArray = [None, '']
      slice1Display.ScaleTransferFunction = 'PiecewiseFunction'
      slice1Display.OpacityArray = [None, '']
      slice1Display.OpacityTransferFunction = 'PiecewiseFunction'
      slice1Display.DataAxesGrid = 'GridAxesRepresentation'
      slice1Display.SelectionCellLabelFontFile = ''
      slice1Display.SelectionPointLabelFontFile = ''
      slice1Display.PolarAxes = 'PolarAxesRepresentation'

      # init the 'GridAxesRepresentation' selected for 'DataAxesGrid'
      slice1Display.DataAxesGrid.XTitleFontFile = ''
      slice1Display.DataAxesGrid.YTitleFontFile = ''
      slice1Display.DataAxesGrid.ZTitleFontFile = ''
      slice1Display.DataAxesGrid.XLabelFontFile = ''
      slice1Display.DataAxesGrid.YLabelFontFile = ''
      slice1Display.DataAxesGrid.ZLabelFontFile = ''

      # init the 'PolarAxesRepresentation' selected for 'PolarAxes'
      slice1Display.PolarAxes.PolarAxisTitleFontFile = ''
      slice1Display.PolarAxes.PolarAxisLabelFontFile = ''
      slice1Display.PolarAxes.LastRadialAxisTextFontFile = ''
      slice1Display.PolarAxes.SecondaryRadialAxesTextFontFile = ''

      # setup the color legend parameters for each legend in this view

      # get color legend/bar for chiLUT in view renderView1
      chiLUTColorBar = GetScalarBar(chiLUT, renderView1)
      chiLUTColorBar.Title = 'chi'
      chiLUTColorBar.ComponentTitle = ''
      chiLUTColorBar.TitleFontFile = ''
      chiLUTColorBar.LabelFontFile = ''

      # set color bar visibility
      chiLUTColorBar.Visibility = 1

      # show color legend
      slice1Display.SetScalarBarVisibility(renderView1, True)

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # get opacity transfer function/opacity map for 'chi'
      chiPWF = GetOpacityTransferFunction('chi')
      chiPWF.Points = [0.0009853932076415149, 0.0, 0.5, 0.0, 0.9898603204781191, 1.0, 0.5, 0.0]
      chiPWF.ScalarRangeInitialized = 1

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
  freqs = {'MyBHPipeline': [1, 1]}
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
