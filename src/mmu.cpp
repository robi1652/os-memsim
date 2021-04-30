#include "mmu.h"
#include <math.h>

const uint32_t STACK_SIZE = 67108864;

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
    var->type = DataType::FreeSpace;
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
    var->type = type;
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
            if (_processes[i]->variables[j]->name != "<FREE_SPACE>") 
            {
                 printf(" %4u | %-13s | 0x%08X   | %10u \n", _processes[i]->pid, _processes[i]->variables[j]->name.c_str(), 
                 _processes[i]->variables[j]->virtual_address, _processes[i]->variables[j]->size);
            }
        }
    }
}

int Mmu::getVarVectorLength(int pid) {
    Process *proc = _processes[getIndexOfProc(pid)];
    return proc->variables.size();
}

Variable* Mmu::getVarAtIndex(int pid, int index) {
    Process *proc = _processes[getIndexOfProc(pid)];
    return proc->variables[index];
}

uint32_t Mmu::getVarAddress(Variable* var) {
    return var->virtual_address;
}

uint32_t Mmu::getLastVarAddress(int pid) {
    Process *proc = _processes[getIndexOfProc(pid)];
    return proc->variables[proc->variables.size() - 1]->virtual_address;
}

uint32_t Mmu::getLastVarSize(int pid) {
    Process *proc = _processes[getIndexOfProc(pid)];
    return proc->variables[proc->variables.size() - 1]->size;
}

uint32_t Mmu::currentSize(int pid) {
    int sum = 0;
    Process *proc = _processes[getIndexOfProc(pid)];
    for (int i = 0; i < proc->variables.size(); i++) {
        sum += proc->variables[i]->size;
    }
    return sum;
}

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


Process* Mmu::getProcess(uint32_t pid) {
    Process *p = NULL;

    for (int i = 0; i < _processes.size(); i++) 
    {
        if (_processes[i]->pid == pid) return _processes[i];
    }

    //should never get here
    return NULL;
}


bool Mmu::removeVariable(uint32_t pid, std::string v_name) 
{
    bool variable_removed = false;

    for (int i = 0; i < _processes.size(); i++) 
    {
        if (_processes[i]->pid == pid) 
        {
            Process *p = _processes[i];
            for (int j = 0; j < p->variables.size(); j++) 
            {
                if (p->variables[j]->name == v_name) {
                    p->variables[j]->name = "<FREE_SPACE>";
                    variable_removed = true;
                    break;
                }
            }
        }

        if (variable_removed) break;
    }

    return variable_removed;
}


std::vector<uint32_t> Mmu::mergeAdjacentPartitions(uint32_t pid, uint32_t page_size) 
{
    Process *p = getProcess(pid);

    int stepper = 0;
    while (stepper < p->variables.size() - 1)  //-1 to avoid indexing out of vector
    {
        std::string v_name = p->variables[stepper]->name;
        std::string v_next_name = p->variables[stepper + 1]->name;
        if (v_name == "<FREE_SPACE>" && v_next_name == "<FREE_SPACE>") 
        {
            //if there are two adjacent freespaces, comibine their sizes and delete the cell on the right
            p->variables[stepper]->size += p->variables[stepper+1]->size;
            p->variables.erase(p->variables.begin()+ stepper + 1); //erase() takes an iterator as parameter
        } 
        else //note: stay at this indice until free spaces arent detected
        {
            stepper++; 
        }
    }

    //now need to adjust page table
    std::vector<uint32_t> pages_to_delete;
    for (int i = 0; i < p->variables.size(); i++) 
    {
        if (p->variables[i]->name == "<FREE_SPACE>") 
        {
            uint32_t start_page = floor((double)p->variables[i]->virtual_address / (double)page_size);
            uint32_t end_page = floor(((double)p->variables[i]->size + (double)p->variables[i]->virtual_address) / (double) page_size);

            if (start_page + 1 < end_page) 
            {
                for (int j = start_page + 1; j < end_page; j++) 
                {
                    pages_to_delete.push_back(j);
                }
            }

            //edge cases
            if (p->variables[i]->virtual_address % page_size == 0 && start_page != end_page) 
            {
                pages_to_delete.push_back(start_page);
            }
            if ((p->variables[i]->virtual_address + p->variables[i]->size) % page_size == 0 && start_page != end_page) 
            {
                pages_to_delete.push_back(end_page);
            }
        }
    }

    return pages_to_delete;
}

void Mmu::deleteProcess(int index) {
    _processes.erase(_processes.begin() + index - 1);
}