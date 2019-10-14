#include "Decision-Node.h"

Decision_Node::Decision_Node(Question* q, float gain, float impurity, 
        Decision_Node* t_branch, Decision_Node* f_branch){

    this->q            = q;
    this->gain         = gain;
    this->impurity     = impurity;

    true_branch  = t_branch;
    false_branch = f_branch;
    leaf_node    = NULL;
}

Decision_Node::Decision_Node(){
    impurity = 0.0;
    gain     = 0.0;
    q            = NULL;
    true_branch  = NULL;
    false_branch = NULL;
    leaf_node    = NULL;
}

Decision_Node::~Decision_Node(){
    delete true_branch;
    delete false_branch;
    delete leaf_node;
    delete q;
}

