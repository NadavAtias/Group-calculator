#pragma once
//------------------------- Include Section --------------------------
#include <vector>
#include <memory>
#include <string>
#include <iosfwd>
#include <fstream>
#include <sstream>
#include <cctype>
#include <optional>
//--------------------------------------------------------------------
class Operation;
const int MAX_OPERATION = 100;
const int MIN_OPERATION =   3;

//--------------------------- Class Section --------------------------

class SetCalculator
{
public:
    //----------------------------------------------------------------
    //         ***     constructors & deconstroctor   ***
    //----------------------------------------------------------------
    SetCalculator(std::istream& , std::ostream& , std::istringstream& );
    
    void run();

private:
    //-------------------------* Private *---------------------------- 

    void eval();
    void read();
    void del();
    void help();
    void exit();
    void readNextOeprationInFile ();
    void input                   ();
    void defineSizeOepration     ();
    void printExceptionError     (const std::string&);
    bool userChoice              (const std::string&);
    void ChooseIfToResize        (const int) ;
    int checkLimitVectorOepration();

    template <typename FuncType>
    void binaryFunc()
    {
        if (m_operations.size() == m_sizeOperation) 
        {
            throw (std::invalid_argument("You have reached max limitation of operation !\nMax capacity is: " + std::to_string(m_sizeOperation)));
        }
        if (auto f0 = readOperationIndex(), f1 = readOperationIndex(); f0 && f1)
        {
          m_operations.push_back(std::make_shared<FuncType>(m_operations[*f0], m_operations[*f1]));
        }
   
    }

    void printOperations () const;
    void print           () const;

    enum class Action
    {
        Invalid,
        Eval,
        Union,
        Intersection,
        Difference,
        Product,
        Comp,
        Del,
        Help,
        Exit,
        Read,
        Resize,
    };

    struct ActionDetails
    {
        std::string command;
        std::string description;
        Action action;
    };


    using ActionMap = std::vector<ActionDetails>;
    using OperationList = std::vector<std::shared_ptr<Operation>>;

    //-------------------------* Members *---------------------------- 

    const ActionMap m_actions;
    OperationList m_operations;
    bool m_running = false;
    int m_sizeOperation;
    std::istream& m_istr;
    std::istringstream& m_lineStream;
    std::ostream& m_ostr;
    std::ifstream m_inputFile;
    std::string   m_operationString;

    std::optional<int> readOperationIndex() const;
    Action readAction() const;
    void runAction(Action action);

    static ActionMap createActions();
    static OperationList createOperations();
};
