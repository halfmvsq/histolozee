#ifndef COMPUTER_BASE_H
#define COMPUTER_BASE_H

#include <QOpenGLFunctions_3_3_Core>

class ComputerBase : protected QOpenGLFunctions_3_3_Core
{
public:

    ComputerBase();
    ~ComputerBase() override = default;

    virtual void initialize() = 0;
    virtual void execute() = 0;
};

#endif // COMPUTER_BASE_H
