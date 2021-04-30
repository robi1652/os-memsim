#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
std::vector<std::string> split_string(std::string str);
DataType to_DataType(std::string DataType_as_string);
void printVariable(void *memory, Variable *v, int physical_address);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint32_t mem_size = 67108864;
    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    while (command != "exit") {
        // Handle command
        // TODO: implement this!
        std::vector<std::string> command_parts = split_string(command);
        uint32_t text_size;
        uint32_t data_size;
        uint32_t number_of_elements;
        uint32_t pid;
        uint32_t offset;
        std::string var_name;

        //update command based on user parameter
        command = command_parts[0];

        if (command == "create") 
        {
            /*
            In this command, in vector command_parts: command_parts[0] = command_type, command_parts[1] = text_size, command_parts[2] = data_size
            */
            if (command_parts.size() != 3) 
            {
                std::cout << "ERROR: Invalid Input - 'create' Command Must be of form <command_type> <text_size> <data_size>";
            }
            else 
            {
                createProcess(std::stoi(command_parts[1]), std::stoi(command_parts[2]), mmu, page_table);
            }
        } 
        else if (command == "allocate") 
        {
            /*
            In this command, command parts is as follows: command_parts[0] = command_type, command_parts[1] = pid, command_parts[2] = var_name, command_parts[4] = DataType, command_parts[4] = num_elements
            */
            if (command_parts.size() != 5) 
            {
                std::cout << "ERROR: Invalid Input - 'allocate' Command Must be of form <command_type> <process_ID> <variable_name <data_type> <number_of_elements>";
            } 
            else 
            {
                allocateVariable(std::stoi(command_parts[1]), command_parts[2], to_DataType(command_parts[3]), std::stoi(command_parts[4]), mmu, page_table);   
            }
        } 
        else if (command == "set") 
        {
            /*
            Command is of form: command_parts[0] = set command_parts[1] = <PID> command_parts[2] = <var_name> command_parts[3] = <offset>  command_parts[,4,5,5...N] = <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)
            */
            if (command_parts.size() < 5) 
            {
                std::cout << "ERROR: Input is not valid for 'set' command -set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N>";
            }

            pid = std::stoi(command_parts[1]);
            var_name = command_parts[2];
            offset = std::stoi(command_parts[3]);
            uint32_t t_size;
            DataType data_type = mmu->getVariable(pid, var_name)->type;

            for (int i = 4; i < command_parts.size(); i++) 
            {
                void *value;
                if (data_type == DataType::Int || data_type == DataType::Float) 
                {
                    value = malloc(4);
                    value = (void*) std::stoi(command_parts[i]);
                    offset *= 4;
                    t_size = 4;
                }
                else if (data_type == DataType::Long || data_type == DataType::Double) 
                {
                    value = malloc(8);
                    long l_var = std::stol(command_parts[i]);
                    double d_var = std::stod(command_parts[i]);

                    data_type == DataType::Long ? memcpy(&value, &l_var, 8) : memcpy(&value, &d_var, 8);
                    
                    offset *= 8;
                    t_size = 8;
                }
                else if (data_type == DataType::Short) 
                {
                    value = malloc(2);
                    value = (void*) std::stoi(command_parts[i]);
                    offset *= 2;
                    t_size = 2;
                }
                else 
                {
                    value = malloc(1);
                    value = (void*) command_parts[i][0];
                    t_size = 1;
                }

                setVariable(pid, var_name, offset, value, mmu, page_table, memory);
            }
        }
        else if (command == "free") 
        {
            /*
            Command is of form: commad_parts[0] = command_type, command_parts[1] = var_name
            */
            freeVariable(std::stoi(command_parts[1]), command_parts[2], mmu, page_table);
        }
        else if (command == "print") 
        {
            if (command_parts[1] == "mmu") 
            {
                mmu->print();
            }
            else if (command_parts[1] == "page") 
            {
                page_table->print();
            }
            else if (command_parts[1] == "processes") 
            {
                std::vector<Process*> processes = mmu->getProcesses();
                
                for (Process* i : processes) 
                {
                    std::cout << i->pid << std::endl;
                }

            } else {
                bool colon_found = false;
                std::string pid = "";
                std::string name_of_variable = "";

                //check for colon found
                for (int i = 0; i < command_parts[1].size(); i++) {
                    if (command_parts[1][i] == ':') 
                    {
                        colon_found = true;
                    }
                }

                if (colon_found) 
                {
                    //get first arg
                    int stepper = 0;
                    while (command_parts[1][stepper] != ':') {

                        pid.push_back(command_parts[1][stepper]);
                        stepper++;
                    }

                    //skip colon
                    stepper++;
                    
                    int size = command_parts[1].size();
                    while(stepper < size) 
                    {
                        name_of_variable.push_back(command_parts[1][stepper]);
                        stepper++;
                    }

                    //Process *p = mmu->getProcess(std::stoi(pid));  
                    Variable *v = mmu->getVariable(std::stoi(pid), name_of_variable);
                    uint32_t physical_address = page_table->getPhysicalAddress(std::stoi(pid), v->virtual_address);
                    printVariable(memory, v, physical_address);
                }              
                else 
                {
                    std::cout << "Error: Not a valid input" << std::endl;
                    
                }


                
            }
        } 
        else 
        {

        }
        /*
        if (command == "create") {

        } else if (command == "allocate") {

        } else if (command == "set") {
            
        } else if (command == "free") {
            
        } else if (command == "terminate") {
            
        } else if (command == "print") {
            
        } else {

        }
        */
        // Get next command
        std::cout << "> ";
        std::getline (std::cin, command);
    }

    // Cean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - create new process in the MMU
    uint32_t newProcess = mmu->createProcess(); //::createProcess();
    
    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    allocateVariable(newProcess, "<TEXT>", DataType::Char, text_size, mmu, page_table);
    allocateVariable(newProcess, "<GLOBALS>", DataType::Char, data_size, mmu, page_table);
    allocateVariable(newProcess, "<STACK>", DataType::Char, 65536, mmu, page_table);

    //   - print pid
    printf("%d", newProcess);
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    // No idea how to look through the table for a page already allocated to the process
    //   - if no hole is large enough, allocate new page(s)

    bool foundFreeSpace = false;
    int mapIter = 0;
    uint32_t address;
    int typeSize = 0;
    uint32_t trueSize;
    if (type == DataType::Char) {
        trueSize = num_elements;
        typeSize = 1;
    } else if (type == DataType::Short) {
        trueSize = num_elements * 2;
        typeSize = 2;
    } else if (type == DataType::Int || type == DataType::Float) {
        trueSize = num_elements * 4;
        typeSize = 4;
    } else if (type == DataType::Long || type == DataType::Double) {
        trueSize = num_elements * 8;
        typeSize = 8;
    }

    bool checkIfAddEntry = false;

    for (int i = 0; i < mmu->getVarVectorLength(pid); i++) {
        // Can this be a pointer or should I push it back onto Variables
        Variable* var = mmu->getVarAtIndex(pid, i);
        if (var->name == "<FREE_SPACE>" && var->size <= trueSize) {
            foundFreeSpace = true;
            if (var->size == trueSize) {
                //uint32_t varAddr = getVarAddress(var);
                var->name = var_name;
                var->type = type;
            } else if  (var->size > trueSize) {
                checkIfAddEntry = true;

                // Update free space address to be N bytes bigger            
                uint32_t newVirtAdd = var->virtual_address;
                var->virtual_address += trueSize;
                // Update free space size to be N bytes smaller
                var->size -= trueSize;
                // N = num_elements

                // Need to check to make sure an element doesn't get split on a page break HERE, right?
                // How do I do that?
                // Current plan: 
                // 1. Figure out how much space is left on this current page
                int currentPage = var->virtual_address / page_table->getPageSize();
                int spaceLeft = var->virtual_address - (page_table->getPageSize() * currentPage);
                // 2. If space left < space needed, then a new page is needed. 
                if (spaceLeft < trueSize) {
                    newVirtAdd += (spaceLeft % typeSize);
                }
                //   --Mod space left by type size. If mod != 0, newVirtAdd += result of the mod? 

                //   - insert variable into MMU
                // Create the new variable              
                mmu->addVariableToProcess(pid, var_name, type, trueSize, newVirtAdd);
            }
        }
    }

    // Currently Confused: Don't know how adding pages/frames works. Here's my thought process
    // 1. Check to see if it fits on the current page, and if it does, then do nothing.
    //   -- If it doesn't then we need to addEntry

    int currentPage = page_table->pageCount(pid);

    if (checkIfAddEntry == true) {
        int currentSize = mmu->currentSize(pid);
        if (currentSize > (page_table->getPageSize() * currentPage)) {
            // Need to addEntry
            // Does this setup work? Or does it conflict with that second example?
            for (int i = page_table->getPageSize() * currentPage; i < currentSize; i += page_table->getPageSize()) {
                page_table->addEntry(pid, currentPage + 1);
            }
        }
    }
     
    // get address 
    //   - print virtual memory address 
    uint32_t oldFinal = page_table->getPhysicalAddress(pid, mmu->getLastVarAddress(pid));
    address =  oldFinal + mmu->getLastVarSize(pid);
    std::cout << address << std::endl;
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    // Need to know up in alloVar too, how do we get virtual address in main.cpp?

    //int physicalAddress;
    //physicalAddress = PageTable::getPhysicalAddress(pid, virtual_address);
    //   - insert `value` into `memory` at physical address

    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)
    Variable *v = mmu->getVariable(pid, var_name);
    DataType data_type = v->type;
    uint32_t physical_address;
    uint32_t index;
    
    /*
    Depending on datatype, we will need to adjust index and number of bytes. recall from class the indexing scheme we implemented
    */
    if (data_type == DataType::Char) 
    {
        index = v->virtual_address + offset;
        physical_address = page_table->getPhysicalAddress(pid, index);
        memcpy(memory + physical_address, value, 1);
    }
    else if (data_type == DataType::Short) 
    {
        index = v->virtual_address + offset * 2;
        physical_address = page_table->getPhysicalAddress(pid, index);
        memcpy(memory + physical_address, value, 2);
    } else if (data_type == DataType::Float || data_type == DataType::Int) 
    {
        index = v->virtual_address + offset * 4;
        physical_address = page_table->getPhysicalAddress(pid, index);
        memcpy(memory + physical_address, value, 4);
    } else if (data_type == DataType::Double || data_type == DataType::Long) {
        index = v->virtual_address + offset * 8;
        physical_address = page_table->getPhysicalAddress(pid, index);
        memcpy(memory + physical_address, value, 8);
    }
}

/*
From Lectrue:
-Dynamic Storage Allocation
    -Dynamically search memory for physical memory large enough for each request
    -Merge Adgacent partitions when deallocating memory
*/
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
    

    //get list of variables withing process
    Process *pid_process = mmu->getProcess(pid);
    std::vector<Variable*> v_list = pid_process->variables;
    uint32_t index = -1;

    for (int i = 0; i < v_list.size(); i++) 
    {
        if (v_list[i]->name == var_name) index = i;
    }

    //for debugging purposes
    if (index == -1) 
    {
        std::cout << "ERROR: variable was never found. main.cpp / freeVaraible" << std::endl;
        return;
    }

    mmu->removeVariable(pid, var_name);
    std::vector<uint32_t> pages_to_delete = mmu->mergeAdjacentPartitions(pid, page_table->getPageSize());
    for (uint32_t i : pages_to_delete) 
    {
        page_table->deleteEntry(pid, i);
    }

    return;
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    mmu->deleteProcess(mmu->getIndexOfProc(pid));
    
    //   - free all pages associated with given process
    page_table->deletePagesLoop(pid);
}


std::vector<std::string> split_string(std::string str) 
{
    std::vector<std::string> parts;
    std::string temp = "";

    for (int i = 0; i < str.size(); i++) 
    {
        if (str[i] == ' ') 
        {
            parts.push_back(temp);
            temp = "";
        } 
        else 
        {
            temp.push_back(str[i]);
        }
    }

    //don't forget to push last work
    parts.push_back(temp);
    return parts;
}

DataType to_DataType(std::string DataType_as_string)
{
    DataType data_type;
    if(DataType_as_string == "int")
    {
        data_type = DataType::Int;
    }
    else if(DataType_as_string == "char")
    {
        data_type = DataType::Char;
    }
    else if(DataType_as_string == "long")
    {
        data_type = DataType::Long;
    }
    else if(DataType_as_string == "short")
    {
        data_type = DataType::Short;
    }
    else if(DataType_as_string == "float")
    {
        data_type = DataType::Float;
    }
    else if(DataType_as_string == "double")
    {
       data_type = DataType::Double;
    }
    else
    {
        data_type = DataType::FreeSpace;
    }
    return data_type;
}

void printVariable(void *memory, Variable *v, int physical_address)
{

}

