#include "MIP_Fileparser.h"
#include "Constraint.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>

using namespace std;

// local function declarations
void trim(string& s);
bool checkForConstraint(const vector<string>& line_split);
bool variablesBegin(vector<string>& line_split);
bool marker(const vector<string>& line_split);
bool intOrgCheck(const vector<string>& line_split);
bool intEndCheck(vector<string>& line_split);
bool objFnCheck(const string& word, const string& objFnSymbol);
bool RHSSectionCheck(vector<string>& line_split);
bool BoundsSectionCheck(vector<string>& line_split);
bool ObjFnSymbolCheck(vector<string>& line_split);
string extractObjSymbol(vector<string>& line_split);

void MIP_Fileparser::parserMps(string filename)
{
    ifstream input(filename);
    unsigned int current_constraint_number = 0u;
    unsigned int current_var_number = 0u;
    bool variableLines = false;
    bool RHSLines = false;
    bool Bounds = false;
    bool int_var = false;
    unsigned int cont_var_count = 0u;
    unsigned int marker_count = 0u;
    if (input.is_open()) {
        while (!input.eof()) {
            string line;
            getline(input, line);
            if (line.empty()) {
                continue;
            }
            if (line[line.size() - 1] == '\r') {
                line.erase(line.size() - 1);
            }
            trim(line);
            vector<string> line_split;
            boost::split(line_split, line, boost::is_any_of(" \t"), boost::token_compress_on);

            // ENDATA
            if (line_split[0].find("ENDATA") != std::string::npos) {
                break;
            }

            // COLUMNS
            //check for objective function column
            if (ObjFnSymbolCheck(line_split) == true) {
                obj_function_symbol = extractObjSymbol(line_split);
            }
            // check for constraints - Exact, Less, Greater
            if (checkForConstraint(line_split) == true) {
                createConstraint(line_split, current_constraint_number);
            }
            // ROWS
            // variables start after COLUMNS line
            if (variablesBegin(line_split) == true) {
                variableLines = true;
                continue;
            }
            // if RHS has begun
            if (RHSSectionCheck(line_split) == true && (RHSLines == false)) {
                cout << "checking RHS section" << endl;
                variableLines = false;
                RHSLines = true;
                continue;
            }

            // BOUNDS Section Check
            if (BoundsSectionCheck(line_split) == true) {
                cout << "checking bounds section" << endl;
                RHSLines = false;
                Bounds = true;
                continue;
            }

            if (variableLines == true) {
                // marker indicates integer variables, otherwise it's just a continuous variable
                // RHS constraints Section

                // If MARKER is found with INTORG, this indicates variables are Integer variables
                // once this section finishes, variables are continuous
                if (marker(line_split) == true && intOrgCheck(line_split) == true) {
                    // if (marker(line_split) == true) {
                    // marker_count++;
                    // cout << "int section found" << endl;
                    int_var = true;
                    // second marker reached with INTEND flagged, finished integer variable section
                    // read in the the next line after the first marker
                    continue;
                } else if (marker(line_split) == true && intEndCheck(line_split) == true) {
                    int_var = false;
                    // cout << "int section finished" << endl;
                    // read in the next line after the second marker
                    continue;
                }

                string variable_name = line_split[0];
                // check if variable name has been seen - create new variable if not
                if (MII.varNameExists(variable_name) == false) {
                    MII.addVariableName(variable_name, current_var_number);
                    MII.addVariableIdx(current_var_number, variable_name);
                    if (int_var) {
                        Variable v(current_var_number, Int);
                        MP.variables.push_back(v);
                    } else {
                        cont_var_count++;
                        Variable v(current_var_number, Cont);
                        MP.variables.push_back(v);
                    }
                    current_var_number++;
                }
                extractVariableInfo(line_split, variable_name);
            }

            if (RHSLines == true) {
                extractRHSInfo(line_split);
            }

            if (Bounds == true) {
                extractBoundsInfo(line_split);
            }
        }
    }
    MP.number_variables = current_var_number;
}

void trim(string& s)
{
    size_t p = s.find_first_not_of(" \t");
    s.erase(0, p);

    p = s.find_last_not_of(" \t");
    if (string::npos != p)
        s.erase(p + 1);
}

void MIP_Fileparser::parse(file_type ft, string filename)
{

    if (ft == USER) {
        parserUser(filename);
    } else if (ft == MPS) {
        parserMps(filename);
    } else {
        cout << "incorrect file type provided for Hypergraph initialisation" << endl;
    }
}


// user defined format
void MIP_Fileparser::parserUser(string filename)
{
   
    return;
}


bool checkForConstraint(const vector<string>& line_split)
{

    if ((line_split[0].compare("E") == 0) || (line_split[0].compare("L") == 0)
        || (line_split[0].compare("G")) == 0) {
        return true;
    }
    return false;
}

bool variablesBegin(vector<string>& line_split)
{
    for (auto& word : line_split) {
        if (word.find("COLUMNS") != string::npos) {
            return true;
        }
    }
    return false;
}

bool marker(const vector<string>& line_split)
{

    for (auto& word : line_split) {
        if (word.find("MARKER") != string::npos) {
            // for (auto& word : line_split) {
            //     cout << word << " ";
            // }
            // cout << endl;
            return true;
        }
    }
    return false;
}

bool intOrgCheck(const vector<string>& line_split)
{
    for (auto& word : line_split) {
        if (word.find("INTORG") != string::npos) {
            // cout << "found INTORG" << endl;
            return true;
        }
    }
    return false;
}

bool intEndCheck(vector<string>& line_split)
{
    for (auto& word : line_split) {
        if (word.find("INTEND") != string::npos) {
            return true;
        }
    }
    return false;
}



bool objFnCheck(const string& word, const string& objFnSymbol)
{
    if (word.compare(objFnSymbol) == 0) {
        return true;
    }
    return false;
}

bool RHSSectionCheck(vector<string>& line_split)
{

    for (auto& word : line_split) {
        if (word.compare("RHS") == 0) {
            return true;
        }
    }
    return false;
}

bool BoundsSectionCheck(vector<string>& line_split)
{
    for (auto& word : line_split) {
        if (word.compare("BOUNDS") == 0) {
            return true;
        }
    }
    return false;
}

bool ObjFnSymbolCheck(vector<string>& line_split)
{
    if (line_split[0].compare("N") == 0) {
        return true;
    }
    return false;
}

string extractObjSymbol(vector<string>& line_split)
{

    return line_split[1];
}

void MIP_Fileparser::createConstraint(const vector<string>& line_split, unsigned int& current_constraint_number)
{
    string constraint_name = line_split[1];
    MII.addConstraintName(constraint_name, current_constraint_number);
    MII.addConstraintIdx(current_constraint_number, constraint_name);
    Constraint constraint(current_constraint_number);
    // greater, lesser or equal to
    if (line_split[0].compare("G") == 0) {
        constraint.setBoundType(Greater);
    } else if (line_split[0].compare("E") == 0) {
        constraint.setBoundType(Equal);
    } else if (line_split[0].compare("L") == 0) {
        constraint.setBoundType(Less);
    }
    MP.addConstraint(constraint);
    ++current_constraint_number;
}

void MIP_Fileparser::extractVariableInfo(vector<string>& line_split, const string& variable_name)
{
    for (int i = 1; i < line_split.size(); i++) {
        // does line contain constraint information
        if (MII.constraintNameExists(line_split[i])) {
            string constraint_name = line_split[i];
            int edge_constraint_idx = MII.getConstraintIdx(constraint_name);
            try {
                double var_coeff = stod(line_split[i + 1]);
                if (var_coeff == 0.00) {
                    for (const auto& str : line_split)
                        cout << str << " " << endl;
                }
                int var_idx = MII.getVariableIdx(variable_name);
                pair<int, double> con_term = { var_idx, var_coeff };
                MP.constraints[edge_constraint_idx].addConTerm(con_term);
                MP.constraints[edge_constraint_idx].addVarIdx(var_idx);
                MP.constraints[edge_constraint_idx].addVarCoeff(var_coeff);
            } catch (...) {
                cout << "error with var coeff parsing " << endl;
                cout << "line_split[i+1] = " << line_split[i + 1] << endl;
                exit(EXIT_FAILURE);
            }
        }

        if (objFnCheck(line_split[i], obj_function_symbol)) {
            int obj_var_idx = MII.getVariableIdx(variable_name);
            try {
                double obj_var_coeff = stod(line_split[i + 1]);
                pair<int, double> objective_term = { obj_var_idx, obj_var_coeff };
                MP.addObjTerm(objective_term);
            } catch (...) {
                cout << "error with parsing objective term " << line_split[i + 1] << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}

void MIP_Fileparser::extractRHSInfo(const vector<string>& line_split)
{

    for (int i = 1; i < line_split.size(); i++) {
        // does line contain constraint information
        if (MII.constraintNameExists(line_split[i])) {
            string constraint_name = line_split[i];

            int constraint_idx = MII.getConstraintIdx(constraint_name);

            try {
                double bound = stod(line_split[i + 1]);
                MP.constraints[constraint_idx].setRHS(bound);
            } catch (...) {
                cout << "error with parsing in RHS bound = " << line_split[i + 1] << endl;
            }
        }
    }
}

void MIP_Fileparser::extractBoundsInfo(const vector<string>& line_split)
{
    Bounds_type bt;
    if (line_split[0].find("PL") != std::string::npos) {
        // PL is already the default bound for variables (0<=x<INF
        return;
    }
    if (line_split[0].find("UP") != std::string::npos || line_split[0].find("UI") != std::string::npos) {
        bt = Upper;
    } else if (line_split[0].find("LO") != std::string::npos || line_split[0].find("LI") != std::string::npos) {
        bt = Lower;
    } else if (line_split[0].find("BV") != std::string::npos) {
        bt = Bool;
    } else if (line_split[0].find("FX") != std::string::npos) {
        bt = Fix;
    } else if (line_split[0].find("FR") != std::string::npos) {
        bt = Free;
    } else if (line_split[0].find("MI") != std::string::npos) {
        bt = FreeNegative;
    }
    // search through line for the variable name. The position afterwards contains the bound value
    for (int i = 1; i < line_split.size(); ++i) {
        if (MII.varNameExists(line_split[i])) {
            string var_name = line_split[i];
            int var_idx = MII.getVariableIdx(var_name);
            if (bt == Bool) {
                MP.variables[var_idx].setUB(1);
                MP.variables[var_idx].setLB(0);
                MP.variables[var_idx].setVarType(Bin);
            } else if (bt == Free) {
                // set lower bound. By default upper bound is already max
                if (MP.variables[var_idx].getVarType() == Int) {
                    MP.variables[var_idx].setMinLB();
                }
            } else if (bt == FreeNegative) {
                if (MP.variables[var_idx].getVarType() == Int) {
                    MP.variables[var_idx].setMinLB();
                    MP.variables[var_idx].setUB(0);
                } else if (MP.variables[var_idx].getVarType() == Cont) {
                    MP.variables[var_idx].setMinLB();
                    MP.variables[var_idx].setUB(0.00);
                }
            } else {
                try {
                    double bound = stod(line_split[i + 1]);
                    if (bt == Upper) {
                        MP.variables[var_idx].setUB(bound);

                        // cout << "Upper Bound Set" << endl;
                    } else if (bt == Lower) {
                        MP.variables[var_idx].setLB(bound);
                        // cout << "Lower Bound Set" << endl;
                    } else if (bt == Fix) {
                        MP.variables[var_idx].setUB(bound);
                        MP.variables[var_idx].setLB(bound);
                    }
                } catch (...) {
                    cout << "error reading in the bound value at position line_split[i+1" << endl;
                    cout << "line_split[i+1] = " << line_split[i + 1] << endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

void MIP_Fileparser::printConstraints()
{
    for (auto& it : MII.constraint_to_idx) {
        cout << "Constraint Name - " << it.first << " Constraint IDX - " << it.second << endl;
    }
}

void MIP_Fileparser::printVariables()
{
    for (auto& it : MII.var_to_idx) {
        cout << "Variable Name - " << it.first << " Variable IDX - " << it.second << endl;
    }
}
