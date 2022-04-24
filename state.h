#pragma once
#include "interface.h"
#include "model.h"
#include <iostream>


class StaticBlock : public __BaseState
{

public:
  
    void ReadBlock();

    void Exit() override;

    void EndBlock() override {}

    void StartBlock() override;


    ~StaticBlock() = default;
};

class DinamicBlock : public __BaseState
{
std::size_t block_count = 0;
public:
    
    void ReadBlock() override {}

    void Exit() override;

    void EndBlock();

    void StartBlock() override;


    ~DinamicBlock() = default;
};

