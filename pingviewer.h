#pragma once

#include <QVTKOpenGLNativeWidget.h>

#include <vtkActor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectBase.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphere.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>
#include <vtkCone.h>
#include <vtkConeSource.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkElevationFilter.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>

#include <QThread>

#include "pingcallbacks.h"

class Ping360Simulator : public QObject {
    Q_OBJECT
public:
    void generateFakeData();
signals:
    void dataGenerated(const QByteArray& data);
};

class Ping1DSimulator : public QObject {
    Q_OBJECT
public:
    void generateFakeData();
signals:
    void dataGenerated(const QByteArray& data);
};

class PingViewer : public QVTKOpenGLNativeWidget {
    Q_OBJECT
public:
    PingViewer();
    ~PingViewer() override;
    void handleData(const QByteArray& data);

signals:
    void startSimulation();

private:
    // Window, rendereres and etc.
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkNew<vtkRenderer> renderer;

    // Plane
    vtkNew<vtkPlaneSource> planeSource;
    vtkNew<vtkPolyDataMapper> planeMapper;
    vtkNew<vtkActor> planeActor;
    vtkNew<vtkColorSeries> colorSeries;
    vtkNew<vtkColorTransferFunction> colorTransferFunction;

    // vtk callbacks
    vtkNew<PingVtkShowCommand> showCallback;

    // Qt
    QThread simulatorThread;

    // internal
    Ping1DSimulator *ping360Sim;

};
