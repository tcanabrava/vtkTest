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

#include <vtkCone.h>
#include <vtkConeSource.h>

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
    ~PingViewer() override = default;
    
private:
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkNew<vtkSphereSource> sphereSource;
    vtkNew<vtkPolyDataMapper> sphereMapper;
    vtkNew<vtkActor> sphereActor;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkConeSource> coneSource;
    vtkNew<vtkPolyDataMapper> coneMapper;
    vtkNew<vtkActor> coneActor;
    vtkNew<PingVtkShowCommand> showCallback;

    QThread simulatorThread;
};
