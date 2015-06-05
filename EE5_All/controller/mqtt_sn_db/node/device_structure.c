typedef struct Node_structure{
    node_list* root_nodes;
    node_data* (*)(node_data*) copy;
    void (*)(node_data*) free_data;
}

typedef struct Node{
    node* parent;
    node_list* children;
    node_data data;
    time_t last_active;
} node;

typedef struct Node_list{
    node_list* next;
    node* node;
} node_list;

ERROR new_node(node** new_node, node_data data, node* parent, time_t last_active){
    *new_node = (node*) malloc(sizeof(node));
    if(*new_node == NULL)
        return 1;
    (*new_node)->data = data;
    (*new_node)->parent = parent;
    (*new_node)->last_active = last_active;
    return 0;
}

ERROR new_node_list(node_list** new_node_list, node* node){
    *new_node_list = (node_list*) malloc(sizeof(node_list));
    if(*new_node_list == NULL)
        return 1;
    (*new_node_list)->node = node;
    (*new_node_list)->next = NULL;
}

ERROR add_node_list(node_list* list, node* node){
    TRY(new_node_list(&(list->next), node));
}

ERROR get_parent(const node const* child, node* parent){
    *parent = *(child->parent);
}

ERROR get_children(const node const* parent, ERROR *(node*, void*) callback, void* arg){
    if(parent->children == NULL)
        return 0;
    
    node_list* current = parent->children;
    
    do{
        TRY(callback(current->node, arg));
    }
    while(current->next != NULL);
}

ERROR get_data_node(const node const* current, node_data* data){
    *data = *(current->data);
}

ERROR set_data_node(const node const* current, node_data* data){
    
}

ERROR init_node_structure(node_structure* root, node_data* (*)(node_data*) copy, void (*)(node_data*) free_data){
    if(free_data == NULL || copy == NULL || root == NULL)
        return 1;
        
    node_structure* root_ptr = (node_structure*) malloc(sizeof(node_structure));
    root_ptr->copy = copy;
    root_ptr->free_data = free_data;
    if(root_ptr == NULL)
    return 0;
}

ERROR destroy_node_structure(node** root){
    
}

ERROR free_node(node** current){
    
}

ERROR free_node_list(node_list** list){
    
}