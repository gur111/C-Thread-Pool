#include <stdlib.h>
#include <stdio.h>
#include "game_list.h"

/* We allow ourselves to create only a single list and put it in a global
 * variable to allow access everywhere in the server */
GameList game = {0};

// For local use only
GameNode *createGameNode(int answer, int game_id) {
    GameNode *node;
    node = (GameNode *) malloc(sizeof(GameNode));
    if (!node) {
        perror("Allocation");
        return NULL;
    }

    node->answer = answer;
    node->game_id = game_id;
    node->last_guess = -1;
    node->_next = NULL;

    return node;
}

// Add new item at the end of list.
GameNode *createGame(int answer, int game_id, int guess) {
    GameNode *new_node;

    new_node = createGameNode(answer, game_id);

    if (!new_node) {
        perror("Allocation");
        return NULL;
    }

    // If list is empty.
    if (game._head == NULL) {
        game._head = game._tail = new_node;
    } else {
        game._tail->_next = new_node;
        game._tail = new_node;
    }
    new_node->last_guess = guess;
    return createGameNode(answer, game_id);
}

//Delete item from the end of list.
int deleteGame(int game_id) {
    GameNode *tmp, *to_delete;

    // Check list isn't empty
    if (game._head == NULL) {
        perror("Can't delete from an empty list");
        return -1;
    }

    // Check if game_id is in head
    if (game._head->game_id == game_id) {
        tmp = game._head;
        game._head = game._head->_next;
        free(tmp);
        return 0;
    }

    tmp = game._head;
    while (tmp->_next && (tmp->_next->game_id != game_id)) {
        tmp = tmp->_next;
    }

    if (tmp->_next == NULL) {
        perror("game_id isn't an existing game");
        return -1;
    } else {
        to_delete = tmp->_next;
        tmp->_next = to_delete->_next;

        // Check if tail needs to be updated
        if (to_delete == game._tail) {
            game._tail = tmp;
        }
        free(to_delete);
        return 0;
    }
} // End deleteNode


GameNode *getGame(int game_id, int guess) {
    GameNode *cur = game._head, *tmp;

    while (cur && (cur->game_id != game_id)) {
        cur = cur->_next;
    }
    if(!cur)
        return NULL;

    cur->last_guess = guess;
    tmp = createGameNode(cur->answer, cur->game_id);
    tmp->last_guess = cur->last_guess;

    return tmp;
}


//To print list from start to end of the list.
void listGames(void) {
    GameNode *node = game._head;

    if (node == NULL) {
        printf("Empty List\n");
        return;
    }

    printf("Games in Progress: \n");
    while (node != NULL) {
        printf("\tGame %d - Number: %d, Last Guess: %d\n", node->game_id, node->answer, node->last_guess);
        node = node->_next;
    }
}