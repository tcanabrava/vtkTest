#include "pingviewer.h"
#include "pingcallbacks.h"

#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>

#include <QtMath>
#include <QTimer>
#include <QDebug>


void Ping360Simulator::generateFakeData() {
    static uint counter = 1;
    static const float numberOfSamples = 1200;

    float stop1 = numberOfSamples / 2.0 - 10 * qSin(counter / 10.0);
    float stop2 = 3 * numberOfSamples / 5.0 + 6 * qCos(counter / 5.5);

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
    counter += 1;
    emit dataGenerated(deviceData);
}

void Ping1DSimulator::generateFakeData() {
    static uint counter = 1;
    static const float numPoints = 200;

        float stop1 = numPoints / 2.0 - 10 * qSin(counter / 10.0);
        float stop2 = 3 * numPoints / 5.0 + 6 * qCos(counter / 5.5);

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
        counter += 1;
        emit dataGenerated(data);
}


PingViewer::PingViewer () {

    planeSource->SetCenter(1.0, 0.0, 0.0);
    planeSource->SetNormal(1.0, 0.0, 1.0);
    planeSource->Update();

    planeMapper->SetInputConnection(planeSource->GetOutputPort());

    planeActor->SetMapper(planeMapper);
    renderer->AddActor(planeActor);

    // Window.
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("RenderWindowNoUIFile");
    SetRenderWindow(renderWindow);

    ping360Sim = new Ping1DSimulator();
    ping360Sim->moveToThread(&simulatorThread);
    connect(ping360Sim, &Ping1DSimulator::dataGenerated, this, &PingViewer::handleData, Qt::QueuedConnection);

    simulatorThread.start();
    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, ping360Sim, &Ping1DSimulator::generateFakeData, Qt::QueuedConnection);
    timer->start(16);
}

PingViewer::~PingViewer() {
    simulatorThread.exit();
    simulatorThread.wait();
}

void PingViewer::handleData(const QByteArray& data)
{
    Q_UNUSED(data);
    static double rotation = 0;
    rotation += 0.01;
    renderWindow->GetInteractor()->Render();
}
