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
    uint32_t newProcess = Mmu::createProcess();

    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    allocateVariable(newProcess, "<TEXT>", DataType::Char, text_size,*mmu, *page_table);
    allocateVariable(newProcess, "<GLOBALS>", DataType::Char, data_size,*mmu, *page_table);
    allocateVariable(newProcess, "<STACK>", DataType::Char, 65536,*mmu, *page_table);

    //   - print pid
     std::cout << newProcess << std::end;
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    // No idea how to look through the table for a page already allocated to the process
    //   - if no hole is large enough, allocate new page(s)

    bool foundFreeSpace = false;
    int mapIter = 0;
    for (int i = 0; i < mmu.getVariableVectorLength(); i++) {
        // Can this be a pointer or should I push it back onto Variables
        Variable* var = mmu.getVariableAtIndex(i);
        if (var->name == "<FREE_SPACE>" && var->size <= num_elements) {
            foundFreeSpace = true;
            if (var->size == num_elements) {
                uint32_t varAddr = getVarAddress(var);
                //deleteVar();
                mmu.addVariableToProcess(pid, var_name, type, num_elements, varAddr);
            } else if  (var->size > num_elements) {
                // Update free space address to be N bytes bigger
                // Update free space size to be N bytes smaller
                // N = num_elements
            }
        }
    }

    if (foundFreeSpace == false) {
        // addEntry to create new page
        // get address 
    }
    

    //   - insert variable into MMU
    mmu.addVariableToProcess(pid, var_name, type, num_elements, address);

    //   - print virtual memory address 
    std::cout << address << std::endl;
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    // Need to know up in alloVar too, how do we get virtual address in main.cpp?

    int physicalAddress;
    physicalAddress = PageTable::getPhysicalAddress(pid, virtual_address);
    //   - insert `value` into `memory` at physical address

    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array) 
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
}
