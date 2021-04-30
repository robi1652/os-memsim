#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>

enum DataType : uint8_t {FreeSpace, Char, Short, Int, Float, Long, Double};

typedef struct Variable {
    std::string name;
    DataType type;
    uint32_t virtual_address;
    uint32_t size;
} Variable;

typedef struct Process {
    uint32_t pid;
    std::vector<Variable*> variables;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    uint32_t _max_size;
    std::vector<Process*> _processes;

public:
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    void addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address);
    void print();
    std::vector<uint32_t> mergeAdjacentPartitions(uint32_t pid, uint32_t page_size);
    int getVarVectorLength(int pid);
    uint32_t getVarAddress(Variable* var);
    Variable* getVariable(uint32_t pid, std::string v_name);
    Process* getProcess(uint32_t pid);
    bool removeVariable(uint32_t pid, std::string v_name); 
    Variable* getVariableAtIndex(int index, uint32_t pid);
    uint32_t currentSize(int pid);
    uint32_t getLastVarAddress(int pid);    
    uint32_t getLastVarSize(int pid);
    Variable* getVarAtIndex(int pid, int index);
    int getIndexOfProc(int pid);
    void deleteProcess(int pid);
    std::vector<Process*> getProcesses();
    //Variable* getVariable(uint32_t pid, std::string v_name);
};

#endif // __MMU_H_
