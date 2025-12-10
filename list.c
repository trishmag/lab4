#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// 1. Allocate a new list
list_t* list_alloc() {
    list_t* new_list = (list_t*) malloc(sizeof(list_t));
    if (new_list != NULL) {
        new_list->head = NULL;
    }
    return new_list;
}

// 2. Free the list and all nodes
void list_free(list_t* l) {
    if (l == NULL) return;
    
    node_t* current = l->head;
    node_t* next_node;
    
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    free(l);
}

// 3. Convert list to string (e.g., "1 -> 2 -> 3 -> NULL")
char* listToString(list_t* l) {
    char* buf = (char*) malloc(1024 * sizeof(char)); // Allocate buffer
    if (l == NULL || l->head == NULL) {
        strcpy(buf, "NULL");
        return buf;
    }
    
    buf[0] = '\0'; // Start with empty string
    char temp[32]; 
    
    node_t* current = l->head;
    while(current != NULL) {
        sprintf(temp, "%d -> ", current->value);
        strcat(buf, temp);
        current = current->next;
    }
    strcat(buf, "NULL");
    return buf;
}

// 4. Get length
int list_length(list_t* l) {
    if (l == NULL) return 0;
    int count = 0;
    node_t* current = l->head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// 5. Add to front
void list_add_to_front(list_t* l, int value) {
    if (l == NULL) return;
    node_t* new_node = (node_t*) malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = l->head;
    l->head = new_node;
}

// 6. Add to back
void list_add_to_back(list_t* l, int value) {
    if (l == NULL) return;
    node_t* new_node = (node_t*) malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = NULL;
    
    if (l->head == NULL) {
        l->head = new_node;
    } else {
        node_t* current = l->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

// 7. Add at index
void list_add_at_index(list_t* l, int value, int index) {
    if (l == NULL) return;
    
    // If index is 0, reuse add_to_front
    if (index == 0) {
        list_add_to_front(l, value);
        return;
    }
    
    node_t* current = l->head;
    int i = 0;
    
    // Traverse to the node JUST BEFORE the index
    while (current != NULL && i < index - 1) {
        current = current->next;
        i++;
    }
    
    // If index is out of bounds
    if (current == NULL) return;

    node_t* new_node = (node_t*) malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = current->next;
    current->next = new_node;
}

// 8. Remove from front
int list_remove_from_front(list_t* l) {
    if (l == NULL || l->head == NULL) return -1;
    
    node_t* temp = l->head;
    int val = temp->value;
    
    l->head = l->head->next;
    free(temp);
    
    return val;
}

// 9. Remove from back
int list_remove_from_back(list_t* l) {
    if (l == NULL || l->head == NULL) return -1;
    
    // Only one item in list
    if (l->head->next == NULL) {
        int val = l->head->value;
        free(l->head);
        l->head = NULL;
        return val;
    }
    
    node_t* current = l->head;
    // Stop at second to last node
    while (current->next->next != NULL) {
        current = current->next;
    }
    
    int val = current->next->value;
    free(current->next);
    current->next = NULL;
    return val;
}

// 10. Remove at index
int list_remove_at_index(list_t* l, int index) {
    if (l == NULL || l->head == NULL) return -1;
    
    if (index == 0) {
        return list_remove_from_front(l);
    }
    
    node_t* current = l->head;
    int i = 0;
    
    // Traverse to node BEFORE target
    while (current != NULL && i < index - 1) {
        current = current->next;
        i++;
    }
    
    // If out of bounds or target is NULL
    if (current == NULL || current->next == NULL) return -1;
    
    node_t* temp = current->next;
    int val = temp->value;
    
    current->next = temp->next;
    free(temp);
    
    return val;
}

// 11. Get element at index
int list_get_elem_at(list_t* l, int index) {
    if (l == NULL || l->head == NULL) return -1;
    
    node_t* current = l->head;
    int i = 0;
    
    while (current != NULL) {
        if (i == index) return current->value;
        current = current->next;
        i++;
    }
    return -1; // Not found
}
