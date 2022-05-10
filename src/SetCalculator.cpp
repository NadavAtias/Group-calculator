//------------------------- Include Section --------------------------

#include "SetCalculator.h"
#include "Union.h"
#include "Intersection.h"
#include "Difference.h"
#include "Product.h"
#include "Comp.h"
#include "Identity.h"
#include <istream>
#include <ostream>
#include <algorithm>
#include <iostream>

//--------------------------------------------------------------------
namespace rng = std::ranges;


SetCalculator::SetCalculator(std::istream& istr, std::ostream& ostr, std::istringstream& stream)
    : m_actions(createActions()), m_operations(createOperations()), m_istr(istr), m_ostr(ostr),m_lineStream(stream)
{}

//====================================================================
//**************************** run Function **************************

// Main function that runs the main loop. 
// there several exaptions for input validation.
//====================================================================

void SetCalculator::run()
{
    try {
        if (!m_running)  defineSizeOepration();
        while (m_running)
        {
            
            print();
            input();
            const auto action = readAction();
            runAction(action);
        }
    }
    catch (std::invalid_argument& e) {
        printExceptionError(e.what());
    }
    catch (std::ios::failure& e) {
        printExceptionError("Eexcaption ! you have entered bad Input.\n");
    }
    catch (std::out_of_range& e) {
        printExceptionError(e.what());
    }

}
//====================================================================
//************************** Eval Operation **************************

// Function that exacute an operation from  list of operation. 
//====================================================================
void SetCalculator::eval()
{
    if (auto index = readOperationIndex(); index)
    {
        const auto& operation = m_operations[*index];
        auto inputs = std::vector<Set>();
        for (auto i = 0; i < operation->inputCount(); ++i)
        {
            input();
            inputs.push_back(Set(m_lineStream));
        }

        operation->print(m_ostr, inputs);
        m_ostr << " = " << operation->compute(inputs) << '\n';
    }

}
//====================================================================
//************************ Get Input from User ***********************

// Function that recives the commands from the user.
// handels 2 ways of input (via console or file).
//====================================================================

void SetCalculator::input()  {
    
    m_lineStream.clear();

    m_inputFile .clear();

   
    if (m_inputFile.is_open()) 
    {
        std::getline(m_inputFile, m_operationString);
        if (m_inputFile.eof()) m_inputFile.close();
    }
    else 
    {
        std::getline(m_istr, m_operationString);
    }

    m_lineStream = std::istringstream(m_operationString);
    m_lineStream.exceptions(std::ios::failbit | std::ios::badbit);
}

//====================================================================
//***************** Open File and read Operation's *******************

// Function that try's to open a file.
// If the file cannot open, exaption is throwen chatched by the 
// try block of "run" function 
//====================================================================

void SetCalculator::read()
{
 
    m_lineStream >> m_operationString;

    m_inputFile.exceptions(std::ios ::failbit | std::ios::badbit);
    
    m_inputFile.open(m_operationString) ;
    
}

//====================================================================
//************************  Exception`s printer *********************
// 
// Function that print diffrent massages of diffrent exaptions. 
//====================================================================

void SetCalculator::printExceptionError(const std::string & exceptionError)
{
    if (m_inputFile.is_open()) m_ostr << std::endl << m_operationString << std::endl;
    
    std::cerr << exceptionError;
    
    readNextOeprationInFile(); 

    run();
}

//====================================================================
//************************ Resize Oepration **************************
// 
// Function that defines (or not) new size ragarding amount of operation
// that the program supports.
// # Choice determend by user via console. 
// # Exepations can also throwen form here.
//====================================================================

void SetCalculator :: defineSizeOepration() {
    
    if (!m_running) {
        m_ostr << "Pls Enter size Of Operation (3 - 100)\n";
        input();
    }

    auto size = checkLimitVectorOepration();

    if (size < m_operations.size())
    {
        ChooseIfToResize(size);
    }
    else
    {
        m_running = true;
        m_sizeOperation = size;
    }
    
   
}
//----------------------------------------------------------------
int SetCalculator::checkLimitVectorOepration(){

    auto size = 0;
    m_lineStream >> size;

    if (size < MIN_OPERATION || size > MAX_OPERATION)
        throw(std::out_of_range("the size of Operation Out Of Range\n"));

    return size;
}
//====================================================================
//**************************** UI Dialog *****************************
// 
// Function that read user chices regarding qusetion followed by 
// a certain event.
//====================================================================
void SetCalculator::ChooseIfToResize(const int size) {
    
    bool choice = userChoice("\nWould you like to cancel the operation?\nY - yes for cancel / N - all operation until new limit will be deleted\n");
    if (!choice) m_operations.resize(size);

}
//-------------------------------------------------------------------
//********************  Read next line within file  ******************
// 
// Function that reads all data (line by line) from file.
// # If command "resize" appers, the user must choose how to continue.
//-------------------------------------------------------------------
void SetCalculator::readNextOeprationInFile()
{
    if (!m_inputFile.is_open()) return;

    bool choice = userChoice("\nwould you like to continue to read from file?\nY - yes / N - no\n");

    if (!choice)  m_inputFile.close();
}
//-------------------------------------------------------------------
bool SetCalculator::userChoice(const std::string& str) {

    while (true) 
    {
        m_ostr << str;
        
        std::getline(m_istr, m_operationString);
      
        if (m_operationString ==      "Y" || m_operationString == "y") {
            return true;
        }
        else if (m_operationString == "N" || m_operationString == "n") {
            return false;
        }
    }
    return false;
}

//====================================================================
//************************ delete Oepration **************************
// 
// Function that delets operations. 
// # Excaption can be throwen. 
//====================================================================

void SetCalculator::del()
{
        if (auto i = readOperationIndex(); i)
        {
            if (m_operations.size() > MIN_OPERATION) {
                m_operations.erase(m_operations.begin() + *i);
            }
            else
               throw(std::invalid_argument("You cannot delete more operations  !\nMin capacity is : 3")); 
        }
}

//====================================================================

void SetCalculator::help()
{
    m_ostr << "The available commands are:\n";
    for (const auto& action : m_actions)
    {
        m_ostr << "* " << action.command << action.description << '\n';
    }
    m_ostr << '\n';
}

//====================================================================

void SetCalculator::exit()
{
    m_ostr << "Goodbye!\n";
    m_running = false;
}
//====================================================================
//*********************** Print Function's  **************************
//====================================================================
void SetCalculator::print() const {
    printOperations();
    m_ostr << "Enter command ('help' for the list of available commands): ";
}
//----------------------------------------------------------------------
void SetCalculator::printOperations() const
{
    m_ostr << "\nList of available set operations:\n";
    for (decltype(m_operations.size()) i = 0; i < m_operations.size(); ++i)
    {
        m_ostr << i << ".\t";
        auto gen = NameGenerator();
        m_operations[i]->print(m_ostr, gen);
        m_ostr << '\n';
    }
    m_ostr << '\n';
}

//====================================================================
//************************ Oepration index  **************************
// 
// Function that retuns the operation list index . 
// # Excaption can be throwen. 
//====================================================================

std::optional<int> SetCalculator::readOperationIndex() const

{
    int i = 0;
    
    if(m_lineStream.eof()){
        throw(std::invalid_argument("Missing arguments ! try again\n"));
    }

    m_lineStream >> i;
    
    if (i >= m_operations.size() || i < 0 )
    {
        throw(std::invalid_argument("Operation #" + std::to_string(i) + " doesn't exist\n"));
    }
   
    return i;
}

//====================================================================
//*************************** Read Action  ***************************
// 
// Function that inputs command to exacute . 
// # Excaption can be throwen. 
//====================================================================

SetCalculator::Action SetCalculator::readAction() const
{
    auto action = std::string();
    m_lineStream >> action;

    const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);
        if (i == m_actions.end())
        {
            throw (std::invalid_argument("Command not found\n"));
        }
        return i->action;
  

    return Action::Invalid;
}
//====================================================================

void SetCalculator::runAction(Action action)
{
    switch (action)
    {
        default:
            m_ostr << "Unknown enum entry used!\n";
            break;

        case Action::Invalid:
            break;

        case Action::Eval:         eval();                     break;
        case Action::Union:        binaryFunc<Union>();        break;
        case Action::Intersection: binaryFunc<Intersection>(); break;
        case Action::Difference:   binaryFunc<Difference>();   break;
        case Action::Product:      binaryFunc<Product>();      break;
        case Action::Comp:         binaryFunc<Comp>();         break;
        case Action::Del:          del();                      break;
        case Action::Read:         read();                     break;
        case Action::Resize:       defineSizeOepration();      break;
        case Action::Help:         help();                     break;
        case Action::Exit:         exit();                     break;
    }
}

//----------------------------------------------------------------------
//          ***           Set Function           ***
//----------------------------------------------------------------------

SetCalculator::ActionMap SetCalculator::createActions()
{
    return ActionMap
    {
        {
            "eval",
            "(uate) num ... - compute the result of function #num on the "
            "following set(s); each set is prefixed with the count of numbers to"
            " read",
            Action::Eval
        },
        {
            "uni",
            "(on) num1 num2 - Creates an operation that is the union of "
            "operation #num1 and operation #num2",
            Action::Union
        },
        {
            "inter",
            "(section) num1 num2 - Creates an operation that is the "
            "intersection of operation #num1 and operation #num2",
            Action::Intersection
        },
        {
            "diff",
            "(erence) num1 num2 - Creates an operation that is the "
            "difference of operation #num1 and operation #num2",
            Action::Difference
        },
        {
            "prod",
            "(uct) num1 num2 - Creates an operation that returns the product of"
            " the items from the results of operation #num1 and operation #num2",
            Action::Product
        },
        {
            "comp",
            "(osite) num1 num2 - creates an operation that is the composition "
            "of operation #num1 and operation #num2",
            Action::Comp
        },
        {
            "del",
            "(ete) num - delete operation #num from the operation list",
            Action::Del
        },
        {
            "read",
            "read from txt file all the commands that needed to be performed.",
            Action::Read
        },
         {
            "resize",
            "resizing the amount of operations.",
            Action::Resize
        },
        {
            "help",
            " - print this command list",
            Action::Help
        },
        {
            "exit",
            " - exit the program",
            Action::Exit
        }
    };
}

//====================================================================

SetCalculator::OperationList SetCalculator::createOperations()
{
    return OperationList
    {
        std::make_shared<Union>(std::make_shared<Identity>(), std::make_shared<Identity>()),
        std::make_shared<Intersection>(std::make_shared<Identity>(), std::make_shared<Identity>()),
        std::make_shared<Difference>(std::make_shared<Identity>(), std::make_shared<Identity>())
    };
}
