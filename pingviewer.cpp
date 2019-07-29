#include "pingviewer.h"
#include "pingcallbacks.h"

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

#include <vtkElevationFilter.h>

#include <vtkCone.h>
#include <vtkConeSource.h>
#include <QtMath>
#include <QTimer>
#include <QDebug>


void Ping360Simulator::generateFakeData() {
    static uint counter = 1;
    static const float numberOfSamples = 1200;
    static const int angularResolution = 400;

    const float stop1 = numberOfSamples / 2.0 - 10 * qSin(counter / 10.0);
    const float stop2 = 3 * numberOfSamples / 5.0 + 6 * qCos(counter / 5.5);

    QByteArray deviceData;
    deviceData.reserve(numberOfSamples);

    for (int i = 0; i < numberOfSamples; i++) {
        float point;
        if (i < stop1) {
            point = 0.1 * (qrand()%256);
        } else if (i < stop2) {
            point = 255 * ((-4.0 / qPow((stop2-stop1), 2.0)) * qPow((i - stop1 - ((stop2-stop1) / 2.0)), 2.0)  + 1.0);
        } else {
            point = 0.45 * (qrand()%256);
        }
        deviceData.append(point);
    }
    emit dataGenerated(deviceData);
}

void Ping1DSimulator::generateFakeData() {
    static const float numPoints = 200;
    static const float maxDepth = 70000;
    const float stop1 = numPoints / 2.0 - 10 * qSin(counter / 10.0);
    const float stop2 = 3 * numPoints / 5.0 + 6 * qCos(counter / 5.5);

    QByteArray data;
    data.reserve(numPoints);

    for (int i = 0; i < numPoints; i++) {
        float point;
        if (i < stop1) {
            point = 0.1 * (qrand()%256);
        } else if (i < stop2) {
            point = 255 * ((-4.0 / qPow((stop2-stop1), 2.0)) * qPow((i - stop1 - ((stop2-stop1) / 2.0)), 2.0)  + 1.0);
        } else {
            point = 0.45 * (qrand()%256);
        }
        data.append(point);
    }
    emit dataGenerated(data);
}


PingViewer::PingViewer () {
    coneSource->SetHeight(3.0);
    coneSource->SetRadius(1.0);
    coneSource->SetResolution(30);

    vtkNew<vtkElevationFilter> colorFilter;
    colorFilter->SetInputConnection(coneSource->GetOutputPort());
    coneMapper->SetInputConnection(colorFilter->GetOutputPort());

    coneActor->SetMapper(coneMapper);
    coneActor->SetPosition(-1, 0, 0);
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(colors->GetColor4d("Tomato").GetData());

    renderer->AddActor(sphereActor);
    renderer->AddActor(coneActor);
    renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());
    renderer->AddObserver(vtkCommand::StartEvent, showCallback);

    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("RenderWindowNoUIFile");

    SetRenderWindow(renderWindow);

    auto timer = new QTimer();
    connect(timer, &QTimer::timeout, this, [this]{
        static double rotation = 0;
        rotation += 0.01;
        coneActor->RotateZ(rotation);
        renderWindow->GetInteractor()->Render();
    });

    // 60fps
    timer->start(16);
}
