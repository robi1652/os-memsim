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

    std::vector<uint32_t> pages_to_delete = mmu->mergeAdjacentPartitions(pid, page_table->getPageSize());
    for (int i = 0; i <pages_to_delete.size(); i++) 
    {
        //delete entry here
    }


}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
}

std::vector<std::string> split_string(std::string str) 
{
    std::vector<std::string> return_vector;
    char space_delimeter = ' ';

    int left_pointer = 0;
    int right_pointer = 0;
    int end_index = str.size();

    while (right_pointer < end_index) 
    {   

        if (str.at(right_pointer) == space_delimeter && right_pointer != left_pointer) 
        {
            std::string temp = str.substr(left_pointer, right_pointer-1);
            return_vector.push_back(temp);
            while(str.at(right_pointer) == space_delimeter && right_pointer < end_index) right_pointer++; //find start of word
            left_pointer = right_pointer;
        } 
        else 
        {
            right_pointer++;
        }
    }
    return return_vector;
}

