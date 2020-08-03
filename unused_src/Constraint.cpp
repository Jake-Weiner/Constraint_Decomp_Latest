#include "Constraint.h"


 void Constraint::addVarIdx(const int& node_idx_to_add){
      // if node already exists return
    for (auto& var : variable_indxs){
        if (var == node_idx_to_add){
            return;
        }
    }
    variable_indxs.push_back(node_idx_to_add);
 }

