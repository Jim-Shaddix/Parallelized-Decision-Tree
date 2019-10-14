
#ifndef DECISION-NODE_H
#define DECISION-NODE_H

using namespace std;

class Decision_Node { 
// {{{        
    /* This class just stores some general information about each node. The 
     * only real important information for the algorithem is the true/false
     * branches.*/
    public:
        float impurity;
        float gain;
        Decision_Node* true_branch;
        Decision_Node* false_branch;

        Leaf* leaf_node;
        
        Question* q;

        Decision_Node();
        Decision_Node(Question*, float, float, Decision_Node*, Decision_Node*);
        ~Decision_Node();
}; // }}}

#endif /* DECISION-NODE_H */
