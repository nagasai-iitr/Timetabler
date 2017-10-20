#include "time_tabler.h"

#include <vector>
#include <iostream>
#include <fstream>
#include "cclause.h"
#include "mtl/Vec.h"
#include "core/SolverTypes.h"
#include "tsolver.h"
#include "utils.h"
#include "MaxSATFormula.h"
#include "clauses.h"

using namespace Minisat;

TimeTabler::TimeTabler() {
    solver = new TSolver(1, _CARD_TOTALIZER_);
    formula = new MaxSATFormula();
    formula->setProblemType(_WEIGHTED_);
}

void TimeTabler::addClauses(std::vector<CClause> clauses) {
    std::cout << "Clause count : " << clauses.size() << std::endl;
    for(int i = 0; i < clauses.size(); i++) {
        vec<Lit> clauseVec;
        std::vector<Lit> clauseVector = clauses[i].getLits();
        for(int j = 0; j < clauseVector.size(); j++) {
            // if(sign(clauseVector[j])) { std::cout << "-"; }
            // std::cout << var(clauseVector[j]) << " ";
            clauseVec.push(clauseVector[j]);
        }
        // std::cout << std::endl;
        formula->addHardClause(clauseVec);
    }
}

void TimeTabler::addHighLevelClauses() {
    for(int i = 0; i < data.courses.size(); i++) {
        for(int j = 0; j < 6; j++) {
            // std::cout << "DHL : " << data.highLevelVars[i][j] << std::endl;
        }
    }
    std::vector<Var> highLevelVars = Utils::flattenVector<Var>(data.highLevelVars);
    for(int i = 0; i < highLevelVars.size(); i++) {
        // std::cout << "DHLF : " << highLevelVars[i] << std::endl;
    }
    for(int i = 0; i < highLevelVars.size(); i++) {
        vec<Lit> highLevelClause;
        highLevelClause.clear();
        highLevelClause.push(mkLit(highLevelVars[i],false));
        formula->addSoftClause(10, highLevelClause);
    }
}

void TimeTabler::addClauses(Clauses clauses) {
    addClauses(clauses.getClauses());
}

void TimeTabler::addSoftClauses(std::vector<CClause> clauses) {
    std::cout << "Soft Clause count : " << clauses.size() << std::endl;
    for(int i = 0; i < clauses.size(); i++) {
        vec<Lit> clauseVec;
        std::vector<Lit> clauseVector = clauses[i].getLits();
        for(int j = 0; j < clauseVector.size(); j++) {
            clauseVec.push(clauseVector[j]);
        }
        formula->addSoftClause(1, clauseVec);
    }    
}

void TimeTabler::addSoftClauses(Clauses clauses) {
    addSoftClauses(clauses.getClauses());
}

bool TimeTabler::solve() {
    solver->loadFormula(formula);
    if(formula->getProblemType() == _WEIGHTED_) {
        std::cout << "WEIGHTED" << std::endl;
    }
    // solver->search();
    model = solver->tSearch();
    if(checkAllTrue(Utils::flattenVector<Var>(data.highLevelVars))) {
        return true;
    } 
    return false;  
}

bool TimeTabler::checkAllTrue(std::vector<Var> inputs) {
    for(int i = 0; i < inputs.size(); i++) {
        if(model[inputs[i]] == l_False) {
            return false;
        }
    }
    return true;
}

bool TimeTabler::isVarTrue(Var v) {
    if (model[v] == l_False) {
        return false;
    }
    return true;
}

Var TimeTabler::newVar() {
    Var var = formula->nVars();
    formula->newVar();
    return var;
}

Lit TimeTabler::newLiteral(bool sign = false) {
    Lit p = mkLit(formula->nVars(), sign);
    formula->newVar();
    return p;
}

void TimeTabler::printResult() {
    if(checkAllTrue(Utils::flattenVector<Var>(data.highLevelVars))) {
        std::cout << "All high level clauses were satisfied" << std::endl;
        displayTimeTable();
    }
    else {
        std::cout << "Some high level clauses were not satisfied" << std::endl;
        displayUnsatisfiedOutputReasons();
    }
}

void TimeTabler::displayTimeTable() {
    for(int i = 0; i < data.courses.size(); i++) {
        std::cout << "Course : " << data.courses[i].getName() << std::endl;
        for(int j = 0; j < data.fieldValueVars[i][FieldType::slot].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::slot][j])) {
                std::cout << "Slot : " << data.slots[j].getName() << " " << data.fieldValueVars[i][FieldType::slot][j] << std::endl;
            }
        }
        for(int j = 0; j < data.fieldValueVars[i][FieldType::instructor].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::instructor][j])) {
                std::cout << "Instructor : " << data.instructors[j].getName() << " " << data.fieldValueVars[i][FieldType::instructor][j] << std::endl;
            }
        }
        for(int j = 0; j < data.fieldValueVars[i][FieldType::classroom].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::classroom][j])) {
                std::cout << "Classroom : " << data.classrooms[j].getName() << " " << data.fieldValueVars[i][FieldType::classroom][j] << std::endl;
            }
        }
        for(int j = 0; j < data.fieldValueVars[i][FieldType::segment].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::segment][j])) {
                std::cout << "Segment : " << data.segments[j].getName() << " " << data.fieldValueVars[i][FieldType::segment][j] << std::endl;
            }
        }
        for(int j = 0; j < data.fieldValueVars[i][FieldType::program].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::program][j])) {
                std::cout << "Program : " << data.programs[j/2].getName() << " ";
                if(j%2 == 0) {
                    std::cout << "Core" << std::endl;
                }
                else {
                    std::cout << "Elective" << std::endl;
                }
            }
        }
        std::cout << std::endl;
    }
}

void TimeTabler::writeOutput(std::string fileName) {
    std::ofstream fileObject;
    fileObject.open(fileName);
    fileObject << "name,class_size,instructor,segment,is_minor,";
    for(int i = 0; i < data.programs.size(); i+=2) {
        fileObject << data.programs[i].getName() << ",";
    }
    fileObject << "classroom,slot" << std::endl;
    for(int i = 0; i < data.courses.size(); i++) {
        fileObject << data.courses[i].getName() << "," << data.courses[i].getClassSize() << ",";
        for(int j = 0; j < data.fieldValueVars[i][FieldType::instructor].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::instructor][j])) {
                fileObject << data.instructors[j].getName() << ",";
            }
        }
        for(int j = 0; j < data.fieldValueVars[i][FieldType::segment].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::segment][j])) {
                fileObject << data.segments[j].getName() << ",";
            }
        }
        for(int j = 0; j < data.fieldValueVars[i][FieldType::isMinor].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::isMinor][j])) {
                fileObject << data.isMinors[j].getName() << ",";
            }
        }
        for(int j = 0; j < data.fieldValueVars[i][FieldType::program].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::program][j])) {
                fileObject << data.programs[j].getCourseTypeName() << ",";
            }
        }
        for(int j = 0; j < data.fieldValueVars[i][FieldType::classroom].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::classroom][j])) {
                fileObject << data.classrooms[j].getName() << ",";
            }
        }
        for(int j = 0; j < data.fieldValueVars[i][FieldType::slot].size(); j++) {
            if(isVarTrue(data.fieldValueVars[i][FieldType::slot][j])) {
                fileObject << data.slots[j].getName();
            }
        }
        fileObject << std::endl;
    }
    fileObject.close();
}

void TimeTabler::displayUnsatisfiedOutputReasons() {
    for(int i = 0; i < data.highLevelVars.size(); i++) {
        for(int j = 0; j < data.highLevelVars[i].size(); j++) {
            if(!isVarTrue(data.highLevelVars[i][j])) {
                std::cout << "Field : " << Utils::getFieldTypeName(FieldType(j));
                std::cout << " of Course : " << data.courses[i].getName();
                std::cout << " could not be satisfied" << std::endl;
            }
        }
    }
}
