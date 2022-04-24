#pragma once
#include <memory>

#define EXITSUCES  0
#define EXITFAILURE 1 


class Model;

class __BaseState
{
protected:

    Model *_modelContext;
    
public:
    
    void setModelContext(Model *model)
    {   
        _modelContext = model;
    }
    virtual void ReadBlock() = 0;
    virtual void EndBlock() = 0;
    virtual void StartBlock() = 0;
    virtual void Exit() = 0;
    ~__BaseState() = default;
};

