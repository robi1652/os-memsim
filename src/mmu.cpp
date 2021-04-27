#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->virtual_address = address;
    var->size = size;
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)
        }
    }
}

//Process Mmu::getProcessAtIndex(int index) {
//    return _processes[index];
//}

int Mmu::getIndexOfProc(int pid) {
    Process *proc = NULL;
    int i;
    for (i = 0; i < _processes.size(); i++) {
        if (_processes[i]->pid == pid) {
            return i;
        }
    }
    return -1;
}

int Mmu::getVarVectorLength(int pid) {
    Process *proc = _processes[getIndexOfProc(pid)];
    return proc->variables.size();
}

Variable Mmu::getVarAtIndex(int pid, int index) {
    Process *proc = _processes[getIndexOfProc(pid)]
    return proc->variables[index];
}

uint32_t Mmu::getVarAddress(Variable* var) {
    return var->virtual_address;
}

uint32_t Mmu::getLastVarAddress(int pid) {
    Process *proc = _processes[getIndexOfProc(pid)];
    return proc->variables[variables.size() - 1]->virtual_address;
}

uint32_t Mmu::getLastVarSize(int pid) {
    Process *proc = _processes[getIndexOfProc(pid)];
    return proc->variables[variables.size() - 1]->size;
}

Variable* Mmu::getVariable(uint32_t pid, std::string v_name) {
    Process *p = NULL;

    //for process in list of processes, find matching process id
    for (int i = 0; i < _processes.size(); i++) 
    {
        if (_processes[i]->pid == pid) p = _processes[i];
    }

    //given the process, serach it's variable for the match of the variable you sent in. return the match's types
    for (int i = 0; i < p->variables.size(); i++) 
    {
        if (p->variables[i]->name == v_name) return p->variables[i];
    }

    //should never reach here
    std::cout << "ERROR: logic in Mmu::getDataType did not work as expected. /mmu.cpp line 89" << std::endl;
    return NULL;
}
