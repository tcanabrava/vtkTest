#include "pingcallbacks.h"
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include <QDebug>

PingVtkShowCommand *PingVtkShowCommand::New() {
    return new PingVtkShowCommand;
}

void PingVtkShowCommand::Execute(vtkObject* caller, unsigned long identifier, void* data)
{
    Q_UNUSED(identifier);
    Q_UNUSED(data);
    auto renderer = dynamic_cast<vtkRenderer*>(caller);
    const auto activeCam = renderer->GetActiveCamera();
    const auto pos = activeCam->GetPosition();
}
