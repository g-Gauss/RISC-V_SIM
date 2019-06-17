#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <pthread.h>
#include <queue>
#include <vector>
#include <QThread>

#include "cache.h"
#include "instructioncache.h"
#include "datacache.h"
#include "pcb.h"

class Processor: public QThread
{
    Q_OBJECT
    typedef struct
    {
        char opcode;
        int block;
    } message;

    typedef struct
    {
        short state;
        char processor[3];
    } directoryBlock;

    enum ProcessorStates
    {
        instructionFetch,
        dataFetch,
        execution,
        contextSwitch,
    };

private:
    size_t processorId;

    int pc;
    std::vector<int> registers;
    std::vector<int> instructionMemory;
    std::vector<int> dataMemory;

    std::vector<directoryBlock> directory;
    Processor* processors[3];

    std::queue<message> messages;
    pthread_mutex_t messagesMutex;

    pthread_barrier_t* barrier;
    size_t clock;
    ProcessorStates currentState;

    InstructionCache instructionsCache;
    DataCache dataCache;
    // Esto despues lo cambiamos por la lista enlazada circular con nodo centinela :V
    std::vector<Pcb> pcbQueue;
    int rl;

public:
    Processor(const size_t id);

    friend class InstructionCache;
    friend class DataCache;

    void run() override;

    inline bool isMemoryInstruction(int& instructionCode)
    {
        return instructionCode == 5 // lw
                || instructionCode == 37 // sw
                || instructionCode == 51 // lr
                || instructionCode == 52; // sc
    }

    void advanceClockCycle();

    inline void addi(unsigned destinationRegister, unsigned sourceRegister, int immediate)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister] + immediate;
    }
    inline void add(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] + this->registers[sourceRegister2];
    }
    inline void sub(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] - this->registers[sourceRegister2];
    }
    inline void mul(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] * this->registers[sourceRegister2];
    }
    inline void div(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] / this->registers[sourceRegister2];
    }

    inline std::vector<int>* getInstructionMemory() {return &this->instructionMemory;}

    void beq(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);
    void bne(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);

    void jal(unsigned destinationRegister, int immediate);
    void jalr(unsigned destinationRegister, unsigned sourceRegister, int immediate);
};

#endif // PROCESSOR_H
