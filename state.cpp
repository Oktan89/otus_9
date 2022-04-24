#include "state.h"

void StaticBlock::ReadBlock()
{
    if(_modelContext->getData().size() == 1)
        _modelContext->setTimeStartBlock();
    if (_modelContext->getData().size() == _modelContext->get_batch_size())
    {
        _modelContext->notify();
        _modelContext->getData().clear();
    }
}

void StaticBlock::StartBlock()
{
    if(_modelContext->getData().size() != 0)
    {
        _modelContext->notify();
        _modelContext->getData().clear();
    }

    std::unique_ptr<__BaseState> state_dinamic = std::make_unique<DinamicBlock>();
    _modelContext->TransitionTo(state_dinamic);
}

void DinamicBlock::StartBlock()
{
    if(block_count == 0)
        _modelContext->setTimeStartBlock();
    ++block_count;
}

void StaticBlock::Exit()
{
    if(_modelContext->getData().size() != 0)
        _modelContext->notify();
    _modelContext->setStatus(false);
}

void DinamicBlock::Exit()
{
    _modelContext->setStatus(false);
}

void DinamicBlock::EndBlock()
{
    if (block_count == 0)
    {
        if(_modelContext->getData().size() != 0) 
            _modelContext->notify();
        _modelContext->getData().clear();

        std::unique_ptr<__BaseState> state_static = std::make_unique<StaticBlock>();
        _modelContext->TransitionTo(state_static);
        block_count = 0;
    }
    else --block_count;
}