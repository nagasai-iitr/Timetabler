class CClause {
private:
	std::vector<Var> vars;
public:
	CClause(std::vector<Var>);
	CClause(Var);
	CClause();
    std::vector<CClause> operator~();
    std::vector<CClause> operator&(const CClause&);
    CClause operator|(const CClause&);
    std::vector<CClause> operator->(const CClause&);
    void addVars(Var);
    void addVars(Var,Var);
    void addVars(Var,Var,Var);
    std::vector<Var> getVars();
};

class ConstraintType {
public:
    static std::vector<CClause> forAll(Variable,Variable,std::vector<CClause>);
    static
};

class FunctionType {
public:
	static 
};