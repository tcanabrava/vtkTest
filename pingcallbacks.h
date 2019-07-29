#pragma once

#include <vtkCommand.h>

class PingVtkShowCommand : public vtkCommand {
public:
    static PingVtkShowCommand* New();
    virtual void Execute(vtkObject *caller, unsigned long identifier, void *data) override;
};
