#include "pingviewer.h"
#include "pingcallbacks.h"

#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>

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

    auto resX = 200;
    auto resY = 200;
    planeSource->SetResolution(resX,resY);
    planeSource->SetCenter(1.0, 0.0, 0.0);
    planeSource->SetNormal(1.0, 0.0, 1.0);
    planeSource->Update();

    newCellData = vtkFloatArray::New();
    newCellData->SetNumberOfComponents(1);
    newCellData->SetNumberOfValues(200 * 200);

    // Create a lookup table to map cell data to colors
    vtkNew<vtkLookupTable> lut;
    auto tableSize = resX * resY + 1;
    lut->SetNumberOfTableValues(tableSize);
    lut->Build();

    // Fill in a few known colors, the rest will be generated if needed
    lut->SetTableValue(0     , 0     , 0     , 0, 1);  //Black
    lut->SetTableValue(1, 0.8900, 0.8100, 0.3400, 1); // Banana
    lut->SetTableValue(2, 1.0000, 0.3882, 0.2784, 1); // Tomato
    lut->SetTableValue(3, 0.9608, 0.8706, 0.7020, 1); // Wheat
    lut->SetTableValue(4, 0.9020, 0.9020, 0.9804, 1); // Lavender
    lut->SetTableValue(5, 1.0000, 0.4900, 0.2500, 1); // Flesh
    lut->SetTableValue(6, 0.5300, 0.1500, 0.3400, 1); // Raspberry
    lut->SetTableValue(7, 0.9804, 0.5020, 0.4471, 1); // Salmon
    lut->SetTableValue(8, 0.7400, 0.9900, 0.7900, 1); // Mint
    lut->SetTableValue(9, 0.2000, 0.6300, 0.7900, 1); // Peacock

    planeSource->Update();

    planeMapper->SetInputConnection(planeSource->GetOutputPort());
    planeMapper->SetScalarRange(0, tableSize);
    planeMapper->SetLookupTable(lut);

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

    // Horrible, but I did not managed to make the same cellData work.
    vtkFloatArray* cellData = vtkFloatArray::New();
    cellData->SetNumberOfComponents(1);
    cellData->SetNumberOfValues(2000 * 2000);

    for (vtkIdType i = 0, end = 200 * 200 - 200; i < end; i++) {
        cellData->SetValue(i + 200, newCellData->GetValue(i));
    }

    for (vtkIdType i = 0, end = 200; i < end; i++)
    {
        cellData->SetValue(i, data[(int)i]);
    }

    planeSource->GetOutput()->GetCellData()->SetScalars(cellData);
    renderWindow->GetInteractor()->Render();

    newCellData->Delete();
    newCellData = cellData;
}
