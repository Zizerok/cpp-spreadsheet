#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression) : ast_(ParseFormulaAST(expression)) {
        for (const Position& pos : ast_.GetCells()) {
            referenced_cells_.insert(pos);
        }
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute(sheet);
        } catch (const FormulaError& fe) {
            return fe;
        }
    }

    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> ans;
        for (const auto& pos : referenced_cells_) {
            ans.push_back(pos);
        }
        std::sort(ans.begin(), ans.end());
        return ans;
    }

    std::string GetExpression() const override {
        std::stringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }
    
    
    
private:
    std::unordered_set<Position, PositionHash> referenced_cells_;
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (...) {
        throw FormulaException("");
    }
}
