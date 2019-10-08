#ifndef C_THREAD_POOL_GAME_LIST_H
#define C_THREAD_POOL_GAME_LIST_H

typedef struct GameNode {
    int answer;
    int game_id;
    int last_guess;
    struct GameNode *_next;
} GameNode;

typedef struct GameList {
    GameNode *_head;
    GameNode *_tail;
    int count;
} GameList;

extern GameList game;

GameNode *createGameNode(int answer, int game_id);

GameNode *createGame(int answer, int game_id, int guess);

int deleteGame(int game_id);

GameNode *getGame(int game_id, int guess);

void listGames(void);


#endif //C_THREAD_POOL_GAME_LIST_H