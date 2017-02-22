#include <iostream>
#include <ctype.h>
#include <boost/thread/thread.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>


//--------------------------
// -----Class "Headers"-----
//--------------------------
class ActionListener {
  public:
  static int text_id;

  static void keyboardEventOccurred (const pcl::visualization::KeyboardEvent &event, void* viewer_void);
  static void mouseEventOccurred (const pcl::visualization::MouseEvent &event, void* viewer_void);
};
int ActionListener::text_id = 0;

class PureAbstrVis {
  private:

  public:
  PureAbstrVis() {}
  virtual void setPointCloud() = 0;
  virtual void setSurfaceNormals() = 0;
  virtual void initializeViewer() = 0;
  virtual void setViewer() = 0;
  virtual void loop() = 0;
  ~PureAbstrVis() {}
};

class AbstrVis : public PureAbstrVis {
  private:

  public:
  boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer;
  pcl::PointCloud<pcl::PointXYZ>::Ptr basic_cloud_ptr;
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr point_cloud_ptr;
  pcl::PointCloud<pcl::Normal>::Ptr cloud_normals1;
  pcl::PointCloud<pcl::Normal>::Ptr cloud_normals2;

  void setPointCloud();
  void setSurfaceNormals();
  void initializeViewer();
  virtual void setViewer() = 0;
  void loop();
};

class NullVis : public AbstrVis {
  public:
  void setViewer();
};

class SimpleVis : public AbstrVis {
  public:
  void setViewer();
};

class RGBVis : public AbstrVis {
  public:
  void setViewer();
};

class CustomColorVis : public AbstrVis {
  public:
  void setViewer();
};

class NormalsVis : public RGBVis {
  public:
  void setViewer();
};

class ShapesVis : public AbstrVis {
  public:
  void setViewer();
};

class TowerVis : public AbstrVis {
  public:
  void setViewer();
};

class ViewportsVis : public AbstrVis {
  public:
  void setViewer();
};

class InteractionVis : public AbstrVis {
  public:
  void setViewer();
};

class VisFactory {
  private:
  int argc;
  char** argv;
  int nOptions;
  char* visOptionCode;
  std::string* visOptionText;
  int selectedOption;
  
  void setVisOptions();
  void instantiateConcreteClass();
  void displayHelp();

  public:
  PureAbstrVis* concreteVis;

  VisFactory(int argc, char** argv);
  void loop();
  ~VisFactory();
};



// --------------
// -----Main-----
// --------------
int main (int argc, char** argv)
{
  VisFactory* vf = new VisFactory(argc, argv);
  vf->loop();
  delete vf;
}



//--------------------------
// -----Class Functions-----
//--------------------------
void ActionListener::keyboardEventOccurred (const pcl::visualization::KeyboardEvent &event, void* viewer_void) {
  pcl::visualization::PCLVisualizer *viewer = static_cast<pcl::visualization::PCLVisualizer *> (viewer_void);
  if (event.getKeySym () == "r" && event.keyDown ())
  {
    std::cout << "r was pressed => removing all text" << std::endl;

    char str[512];
    for (unsigned int i = 0; i < ActionListener::text_id; ++i)
    {
      sprintf (str, "text#%03d", i);
      viewer->removeShape (str);
    }
    ActionListener::text_id = 0;
  }
}
void ActionListener::mouseEventOccurred (const pcl::visualization::MouseEvent &event, void* viewer_void) {
  pcl::visualization::PCLVisualizer *viewer = static_cast<pcl::visualization::PCLVisualizer *> (viewer_void);
  if (event.getButton () == pcl::visualization::MouseEvent::LeftButton &&
      event.getType () == pcl::visualization::MouseEvent::MouseButtonRelease)
  {
    std::cout << "Left mouse button released at position (" << event.getX () << ", " << event.getY () << ")" << std::endl;

    char str[512];
    sprintf (str, "text#%03d", text_id ++);
    viewer->addText ("clicked here", event.getX (), event.getY (), str);
  }
}


void AbstrVis::setPointCloud() {
  pcl::PointCloud<pcl::PointXYZ>::Ptr basic_cloud_ptr (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr point_cloud_ptr (new pcl::PointCloud<pcl::PointXYZRGB>);

  std::cout << "Genarating example point clouds.\n\n";
  // We're going to make an ellipse extruded along the z-axis. The colour for
  // the XYZRGB cloud will gradually go from red to green to blue.
  uint8_t r(255), g(15), b(15);
  for (float z(-1.0); z <= 1.0; z += 0.05)
  {
    for (float angle(0.0); angle <= 360.0; angle += 5.0)
    {
      pcl::PointXYZ basic_point;
      basic_point.x = 0.5 * cosf (pcl::deg2rad(angle));
      basic_point.y = sinf (pcl::deg2rad(angle));
      basic_point.z = z;
      basic_cloud_ptr->points.push_back(basic_point);

      pcl::PointXYZRGB point;
      point.x = basic_point.x;
      point.y = basic_point.y;
      point.z = basic_point.z;
      uint32_t rgb = (static_cast<uint32_t>(r) << 16 |
              static_cast<uint32_t>(g) << 8 | static_cast<uint32_t>(b));
      point.rgb = *reinterpret_cast<float*>(&rgb);
      point_cloud_ptr->points.push_back (point);
    }
    if (z < 0.0)
    {
      r -= 12;
      g += 12;
    }
    else
    {
      g -= 12;
      b += 12;
    }
  }
  basic_cloud_ptr->width = (int) basic_cloud_ptr->points.size ();
  basic_cloud_ptr->height = 1;
  point_cloud_ptr->width = (int) point_cloud_ptr->points.size ();
  point_cloud_ptr->height = 1;
  
  this->basic_cloud_ptr = basic_cloud_ptr;
  this->point_cloud_ptr = point_cloud_ptr;
}
void AbstrVis::setSurfaceNormals() {
  // ------------------------------------
  // -----Create example point cloud-----
  // ------------------------------------

  // ----------------------------------------------------------------
  // -----Calculate surface normals with a search radius of 0.05-----
  // ----------------------------------------------------------------
  pcl::NormalEstimation<pcl::PointXYZRGB, pcl::Normal> ne;
  ne.setInputCloud (this->point_cloud_ptr);
  pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZRGB> ());
  ne.setSearchMethod (tree);
  pcl::PointCloud<pcl::Normal>::Ptr cloud_normals1 (new pcl::PointCloud<pcl::Normal>);
  ne.setRadiusSearch (0.05);
  ne.compute (*cloud_normals1);

  // ---------------------------------------------------------------
  // -----Calculate surface normals with a search radius of 0.1-----
  // ---------------------------------------------------------------
  pcl::PointCloud<pcl::Normal>::Ptr cloud_normals2 (new pcl::PointCloud<pcl::Normal>);
  ne.setRadiusSearch (0.1);
  ne.compute (*cloud_normals2);
  
  this->cloud_normals1 = cloud_normals1;
  this->cloud_normals2 = cloud_normals2;
}
void AbstrVis::initializeViewer() {
  boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
  this->viewer = viewer;
}
void AbstrVis::loop() {
  while (!viewer->wasStopped ())
  {
    viewer->spinOnce (100);
    boost::this_thread::sleep (boost::posix_time::microseconds (100000));
  }
}



void NullVis::setViewer() {
}
void SimpleVis::setViewer() {
    // --------------------------------------------
    // -----Open 3D viewer and add point cloud-----
    // --------------------------------------------
    viewer->addPointCloud<pcl::PointXYZ> (this->basic_cloud_ptr, "sample cloud");
    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
    viewer->addCoordinateSystem (1.0);
    viewer->initCameraParameters ();
}
void RGBVis::setViewer() {
    // --------------------------------------------
    // -----Open 3D viewer and add point cloud-----
    // --------------------------------------------
    pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(this->point_cloud_ptr);
    viewer->addPointCloud<pcl::PointXYZRGB> (this->point_cloud_ptr, rgb, "sample cloud");
    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
    viewer->addCoordinateSystem (1.0);
    viewer->initCameraParameters ();
}
void CustomColorVis::setViewer() {
    // --------------------------------------------
    // -----Open 3D viewer and add point cloud-----
    // --------------------------------------------
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color(this->basic_cloud_ptr, 0, 255, 0);
    viewer->addPointCloud<pcl::PointXYZ> (this->basic_cloud_ptr, single_color, "sample cloud");
    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
    viewer->addCoordinateSystem (1.0);
    viewer->initCameraParameters ();
}
void NormalsVis::setViewer() {
    // --------------------------------------------------------
    // -----Open 3D viewer and add point cloud and normals-----
    // --------------------------------------------------------
    RGBVis::setViewer();
    viewer->addPointCloudNormals<pcl::PointXYZRGB, pcl::Normal> (this->point_cloud_ptr, this->cloud_normals2, 10, 0.05, "normals");
}
void ShapesVis::setViewer() {
    // --------------------------------------------
    // -----Open 3D viewer and add point cloud-----
    // --------------------------------------------
    pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(this->point_cloud_ptr);
    viewer->addPointCloud<pcl::PointXYZRGB> (this->point_cloud_ptr, rgb, "sample cloud");
    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
    viewer->addCoordinateSystem (1.0);
    viewer->initCameraParameters ();
    //------------------------------------
    //-----Add shapes at cloud points-----
    //------------------------------------
    viewer->addLine<pcl::PointXYZRGB> (this->point_cloud_ptr->points[0],
                                       this->point_cloud_ptr->points[this->point_cloud_ptr->size() - 1], "line");
    viewer->addSphere (this->point_cloud_ptr->points[0], 0.2, 0.5, 0.5, 0.0, "sphere");
    //---------------------------------------
    //-----Add shapes at other locations-----
    //---------------------------------------
    pcl::ModelCoefficients coeffs;
    coeffs.values.push_back (0.0);
    coeffs.values.push_back (0.0);
    coeffs.values.push_back (1.0);
    coeffs.values.push_back (0.0);
    viewer->addPlane (coeffs, "plane");
    coeffs.values.clear ();
    coeffs.values.push_back (0.3);
    coeffs.values.push_back (0.3);
    coeffs.values.push_back (0.0);
    coeffs.values.push_back (0.0);
    coeffs.values.push_back (1.0);
    coeffs.values.push_back (0.0);
    coeffs.values.push_back (5.0);
    viewer->addCone (coeffs, "cone");
}
void TowerVis::setViewer() {
    // --------------------------------------------
    // -----Open 3D viewer and add point cloud-----
    // --------------------------------------------
    pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(this->point_cloud_ptr);
    viewer->addPointCloud<pcl::PointXYZRGB> (this->point_cloud_ptr, rgb, "sample cloud");
    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
    viewer->addCoordinateSystem (1.0);
    viewer->initCameraParameters ();
    //------------------------------------
    //-----Add shapes at cloud points-----
    //------------------------------------
    viewer->addLine<pcl::PointXYZRGB> (this->point_cloud_ptr->points[0],
                                       this->point_cloud_ptr->points[this->point_cloud_ptr->size() - 1], "line");
    //---------------------------------------------
    //-----Add cascaded cones along the z-axis-----
    //---------------------------------------------
    pcl::ModelCoefficients coeffs;
    std::string coneName("Cone_0");
    int nCones = 5;
    double coneSize = 6;
    double coneSizeExponent = 1.10;
    double coneLength = 0.20;
    double conePosition = 0;
    for (int cntr = 0; cntr < nCones; cntr++) {
        coneName.at(coneName.length() - 1) = (char) ('0' + cntr);

        coeffs.values.clear ();
        coeffs.values.push_back (0.0);
        coeffs.values.push_back (0.0);
        coeffs.values.push_back (- conePosition);
        coeffs.values.push_back (0.0);
        coeffs.values.push_back (0.0);
        coeffs.values.push_back (- (conePosition + coneLength));
        coeffs.values.push_back (coneSize);
        viewer->addCone (coeffs, coneName);
        
        coneSize *= coneSizeExponent;
        conePosition += (coneLength / 2.0);
    }
    
    //------------------------------------------------
    //-----Add a cube as the base of the cone-set-----
    //------------------------------------------------
    //viewer->addCube (0.0, 0.0, 0.0, 0.0, - (float) (conePosition + coneLength), - (float) conePosition, 0.53, 0.27, 0.07, "Cube");
    conePosition += (nCones - 1) * (coneLength / 2.0);
    coeffs.values.clear ();
    coeffs.values.push_back (0.0);  //Translation Tx, Ty, Tz
    coeffs.values.push_back (0.0);
    coeffs.values.push_back (- (conePosition + coneLength));
    coeffs.values.push_back (0.0);  //Quaternion of rotation Qx, Qy, Qz, Qt
    coeffs.values.push_back (0.0);
    coeffs.values.push_back (0.0);
    coeffs.values.push_back (1.0);
    coeffs.values.push_back (coneLength);
    coeffs.values.push_back (coneLength);
    coeffs.values.push_back (coneLength);
    viewer->addCube (coeffs, "Cube");
}
void ViewportsVis::setViewer() {
    // --------------------------------------------------------
    // -----Open 3D viewer and add point cloud and normals-----
    // --------------------------------------------------------
    viewer->initCameraParameters ();

    int v1(0);
    viewer->createViewPort(0.0, 0.0, 0.5, 1.0, v1);
    viewer->setBackgroundColor (0, 0, 0, v1);
    viewer->addText("Radius: 0.01", 10, 10, "v1 text", v1);
    pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(this->point_cloud_ptr);
    viewer->addPointCloud<pcl::PointXYZRGB> (this->point_cloud_ptr, rgb, "sample cloud1", v1);

    int v2(0);
    viewer->createViewPort(0.5, 0.0, 1.0, 1.0, v2);
    viewer->setBackgroundColor (0.3, 0.3, 0.3, v2);
    viewer->addText("Radius: 0.1", 10, 10, "v2 text", v2);
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZRGB> single_color(this->point_cloud_ptr, 0, 255, 0);
    viewer->addPointCloud<pcl::PointXYZRGB> (this->point_cloud_ptr, single_color, "sample cloud2", v2);

    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud1");
    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud2");
    viewer->addCoordinateSystem (1.0);

    viewer->addPointCloudNormals<pcl::PointXYZRGB, pcl::Normal> (this->point_cloud_ptr, this->cloud_normals1, 10, 0.05, "normals1", v1);
    viewer->addPointCloudNormals<pcl::PointXYZRGB, pcl::Normal> (this->point_cloud_ptr, this->cloud_normals2, 10, 0.05, "normals2", v2);
}
void InteractionVis::setViewer() {
    viewer->addCoordinateSystem (1.0);

    viewer->registerKeyboardCallback (ActionListener::keyboardEventOccurred, (void*)viewer.get ());
    viewer->registerMouseCallback (ActionListener::mouseEventOccurred, (void*)viewer.get ());
}


VisFactory::VisFactory(int argc, char** argv) {
  this->argc = argc;
  this->argv = argv;
  
  setVisOptions();
  instantiateConcreteClass();
  if (selectedOption < 1) {
    displayHelp();
  } else {
    concreteVis->setPointCloud();
    concreteVis->setSurfaceNormals();
  }
  concreteVis->initializeViewer();
  concreteVis->setViewer();
}
void VisFactory::setVisOptions() {
  int Cntr = 0;
  
  nOptions = 9;
  visOptionCode = new char[nOptions];
  visOptionText = new std::string[nOptions];

  visOptionText[Cntr++] = "Help text";
  visOptionText[Cntr++] = "Simple visualisation example";
  visOptionText[Cntr++] = "RGB colour visualisation example";
  visOptionText[Cntr++] = "Custom colour visualisation example";
  visOptionText[Cntr++] = "Normals visualisation example";
  visOptionText[Cntr++] = "Shapes visualisation example";
  visOptionText[Cntr++] = "Viewports example";
  visOptionText[Cntr++] = "Interaction Customization example";
  visOptionText[Cntr++] = "Tower visualisation example";
  
  for (Cntr = 0; Cntr < nOptions; Cntr++) {
    visOptionCode[Cntr] = tolower(visOptionText[Cntr].at(0));
  }
  visOptionCode[5] = 'a';
}
void VisFactory::instantiateConcreteClass() {
  const int falseOption = -1;
  char inputOption;
  int Cntr = 0;
  
  selectedOption = falseOption;
  if (argc != 2) {
    concreteVis = new NullVis();
    return;
  }
  if (*(argv[1] + 0) != '-') {
    concreteVis = new NullVis();
    return;
  }
  inputOption = tolower(*(argv[1] + 1));
  
  if (inputOption == visOptionCode[0]) {
    concreteVis = new NullVis();
    selectedOption = 0;
  } else if (inputOption == visOptionCode[1]) {
    concreteVis = new SimpleVis();
    selectedOption = 1;
  } else if (inputOption == visOptionCode[2]) {
    concreteVis = new RGBVis();
    selectedOption = 2;
  } else if (inputOption == visOptionCode[3]) {
    concreteVis = new CustomColorVis();
    selectedOption = 3;
  } else if (inputOption == visOptionCode[4]) {
    concreteVis = new NormalsVis();
    selectedOption = 4;
  } else if (inputOption == visOptionCode[5]) {
    concreteVis = new ShapesVis();
    selectedOption = 5;
  } else if (inputOption == visOptionCode[6]) {
    concreteVis = new ViewportsVis();
    selectedOption = 6;
  } else if (inputOption == visOptionCode[7]) {
    concreteVis = new InteractionVis();
    selectedOption = 7;
  } else if (inputOption == visOptionCode[8]) {
    concreteVis = new TowerVis();
    selectedOption = 8;
  } else {
    concreteVis = new NullVis();
    selectedOption = -1;
  }
}
void VisFactory::displayHelp() {
  std::cout << "\n\nUsage of the program " << argv[0] << ": [options]\n\n"
            << "Options:\n"
            << "-------------------------------------------\n";
  for (int cntr = 0; cntr < nOptions; cntr++) {
    std::cout << "-" << visOptionCode[cntr] << "           " << visOptionText[cntr] << "\n";
  }
  std::cout << "\n\n";
}
void VisFactory::loop() {
  if (selectedOption > 0) {
    concreteVis->loop();
  }
}
VisFactory::~VisFactory() {
  delete[] visOptionCode;
  delete[] visOptionText;
}
