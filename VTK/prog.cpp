// New MVC-like high-level architecture
// 2 sliders (resolutions of Azimuth and Zenith angles)

#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkSliderWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation3D.h>
#include <vtkProperty.h>

/******** Simple string-processing class **********/
class String_ : public std::string {
  public:
  static bool includes(std::string s1, std::string s2) {
    int n1 = s1.length();
    int n2 = s2.length();
    bool flag = (n1 < n2);
    for (int cntr = 0; (cntr < n1) && !flag; cntr++) {
      flag = (s1.substr(cntr, n2) == s2);
    }
    return flag;
  }
};
/**************************************************/


/******** Templated call-back class ***************/
template <class T, int N>
class vtkSliderCallback : public vtkCommand
{
public:
  T* object;
  int index;
  vtkSliderWidget* sliderWidget;
  double sliderValue;
  
  static vtkSliderCallback *New() {
    return new vtkSliderCallback;
  }
  virtual void Execute(vtkObject *caller, unsigned long, void*) {
    sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    sliderValue = static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue();
    applyParam();
  }
  void applyParam() {
    std::string objectType = typeid(object).name();
    if (String_::includes(objectType, "SphereSource")) {
      if (N == 1) {
        this->object->SetPhiResolution(sliderValue / 2);
      } else if (N == 2) {
        this->object->SetThetaResolution(sliderValue);
      } else {
      }
    } else if (true) {
    } else {
    }
  }
  vtkSliderCallback():object(0) {
  }
};
/**************************************************/


/* Templated "Model" class (MVC-like architecture)*/
template <class T>
class ModelMVC : public vtkObject {
  public:
  static ModelMVC *New() {
    return new ModelMVC;
  }
  void setAll(vtkSmartPointer<T> object, int index) {
    setObject(object);
    trimObject();
    setIndex(index);
    instantiateMapper();
    setMapperInput();
  }
  vtkSmartPointer<vtkPolyDataMapper> getMapper() {
    return mapper;
  }
  
  private:
  int index;
  vtkSmartPointer<T> object;
  vtkSmartPointer<vtkPolyDataMapper> mapper;

  void setObject(vtkSmartPointer<T> object) {
    this->object = object;
  }
  void setIndex(int n) {
    this->index = n;
  }
  void instantiateMapper() {
    this->mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  }
  void setMapperInput() {
    this->mapper->SetInputConnection(this->object->GetOutputPort());
  }
  void trimObject() {
    std::string objectType = typeid(this->object).name();
    if (String_::includes(objectType, "SphereSource")) {
      this->object->SetCenter(0.0, 0.0, 0.0);
      this->object->SetRadius(4.0);
      this->object->SetPhiResolution(4);
      this->object->SetThetaResolution(8);
    } else if (true) {
    } else {
    }
  }
};
/**************************************************/


/****** "View" class (MVC-like architecture) ******/
class ViewMVC : public vtkObject {
  public:
  static ViewMVC *New() {
    return new ViewMVC;
  }
  void setAll(vtkSmartPointer<vtkPolyDataMapper> mapper) {
    setActor(mapper);
    setRenderer();
    setRendererWindow();
    setInteractor();
    addActors();
    render();
  }
  vtkSmartPointer<vtkRenderWindowInteractor> getRenderWindowInteractor() {
    return renderWindowInteractor;
  }
  void start() {
    this->renderWindowInteractor->Initialize();
    this->renderWindow->Render();
    this->renderWindowInteractor->Start();
  }
  
  private:
  vtkSmartPointer<vtkActor> actor;
  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkRenderWindow> renderWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;

  void setActor(vtkSmartPointer<vtkPolyDataMapper> mapper) {
    this->actor = vtkSmartPointer<vtkActor>::New();
    this->actor->SetMapper(mapper);
    this->actor->GetProperty()->SetInterpolationToFlat();
  }
  void setRenderer() {
    this->renderer = vtkSmartPointer<vtkRenderer>::New();
  }
  void setRendererWindow() {
    this->renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    this->renderWindow->AddRenderer(this->renderer);
  }
  void setInteractor() {
    this->renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    this->renderWindowInteractor->SetRenderWindow(this->renderWindow);
  }
  void addActors() {
    this->renderer->AddActor(this->actor);
  }
  void render() {
    this->renderWindow->Render();
  }
};
/**************************************************/


/*Templated Contorl class (MVC-like architecture)**/
template <class T, int N>
class ControllerMVC : public vtkObject {
  public:
  static ControllerMVC *New() {
    return new ControllerMVC;
  }
  void setAll(vtkSmartPointer<T> object, vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor) {
    instantiateSliderRep();
    setSliderRepParams();
    setSliderWidget(renderWindowInteractor);
    instantiateCallback();
    setCallback(object);
    addObserver();
  }
  void instantiateSliderRep() {
    this->sliderRep = vtkSmartPointer<vtkSliderRepresentation3D>::New();
  }
  void setSliderRepParams() {
    int index = N;
    this->sliderRep->SetMinimumValue(3.0);
    this->sliderRep->SetMaximumValue(50.0);
    //this->sliderRep->SetValue(sphereSource->GetThetaResolution());
    //this->sliderRep->SetTitleText("Sphere Resolution");
    this->sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
    this->sliderRep->GetPoint1Coordinate()->SetValue(-4 + 2 * index,6 + 2 * index,0);
    this->sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
    this->sliderRep->GetPoint2Coordinate()->SetValue(4 + 2 * index,6 + 2 * index,0);
    this->sliderRep->SetSliderLength(0.075);
    this->sliderRep->SetSliderWidth(0.05);
    this->sliderRep->SetEndCapLength(0.05);
  }
  void setSliderWidget(vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor) {
    this->sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
    this->sliderWidget->SetInteractor(renderWindowInteractor);
    this->sliderWidget->SetRepresentation(this->sliderRep);
    this->sliderWidget->SetAnimationModeToAnimate();
    this->sliderWidget->EnabledOn();
  }
  void instantiateCallback() {
    this->callback = vtkSmartPointer<vtkSliderCallback<T, N> >::New();
  }
  void setCallback(vtkSmartPointer<T> object) {
    this->callback->object = object;
    this->callback->index = N;
  }
  void addObserver() {
    this->sliderWidget->AddObserver(vtkCommand::InteractionEvent, this->callback);
  }
  
  private:
  vtkSmartPointer<vtkSliderRepresentation3D> sliderRep;
  vtkSmartPointer<vtkSliderWidget> sliderWidget;
  vtkSmartPointer<vtkSliderCallback<T, N> > callback;
};
/**************************************************/


/***Templated App class (using MVC-like classes)***/
template <class T>
class App : public vtkObject {
  public:
  vtkSmartPointer<ModelMVC<T> > modelMVC;
  vtkSmartPointer<ViewMVC> viewMVC;
  vtkSmartPointer<ControllerMVC<T, 1> > controllerMVC1;
  vtkSmartPointer<ControllerMVC<T, 2> > controllerMVC2;
  
  static App *New() {
    return new App;
  }
  void setMV_MVC(vtkSmartPointer<T> object) {
    setObject(object);
    setModel();
    setView();
  }
  void setC_MVC(int index) {
    this->index = index;
    setController();
  }
  void start() {
    this->viewMVC->start();
  }
  
  private:
  int index;
  vtkSmartPointer<T> object;
  
  void setObject(vtkSmartPointer<T> object) {
    this->object = object;
  }
  void setModel() {
    this->modelMVC = vtkSmartPointer<ModelMVC<T> >::New();
    this->modelMVC->setAll(this->object, this->index);
  }
  void setView() {
    this->viewMVC = vtkSmartPointer<ViewMVC>::New();
    this->viewMVC->setAll(this->modelMVC->getMapper());
  }
  void setController() {
    if (index == 1) {
      this->controllerMVC1 = vtkSmartPointer<ControllerMVC<T, 1> >::New();
      this->controllerMVC1->setAll(this->object, this->viewMVC->getRenderWindowInteractor());
    } else if (index == 2) {
      this->controllerMVC2 = vtkSmartPointer<ControllerMVC<T, 2> >::New();
      this->controllerMVC2->setAll(this->object, this->viewMVC->getRenderWindowInteractor());
    } else {
    }
  }
};
/**************************************************/


/************ Simple 'main' function **************/
int main (int, char *[])
{
  vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
  vtkSmartPointer< App<vtkSphereSource> > app = vtkSmartPointer< App<vtkSphereSource> >::New();
  app->setMV_MVC(sphereSource);
  app->setC_MVC(1);
  app->setC_MVC(2);
  app->start();
  
  return EXIT_SUCCESS;
}
/**************************************************/
