typedef struct state_list {
    state_t* state;
    struct state_list* next;
} state_list_t;


state_list_t*
atol_state(const node_t nodes[], size_t num_nodes) {
    if (states == NULL) {
        return NULL;
    }
    state_list_t* list = malloc(sizeof(node_list_t));

    state_list_t* elem = list;
    state_list_t* tail = NULL;

    for(size_t i = 0; i < num_states - 1; ++i, elem = elem->next) {
        elem->state = malloc(sizeof(state_t));
        *(elem->state) = nodes[i].state;
        elem->next = malloc(sizeof(node_list_t));
    }

    *(elem->state) = nodes[num_states - 1].state;
    elem->next = NULL;

    return list;
}

void
free_state_list(state_list_t* head) {
    state_list_t* current = head;
    state_list_t* next;
    while (current != NULL) {
        next = current->next;
        free(current->state);
        free(current);
        current = next;
    }
}