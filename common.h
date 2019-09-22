#ifndef THREADPOOL_COMMON_H
#define THREADPOOL_COMMON_H

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define GUESS_LEN 4 // Can be increased up to 9. With 10 and above, check_answer will return a non 4 length of string
#define CHECK_LEN 4 // Will ALWAYS be 4 because of the format 0A0B and the numbers are up to 9
#define GAME_ID_LEN 4
#define NEW_GAME_CMD "0000" // Must be the same length of the GAME_ID_LEN

#define INVALID_GUESS_MSG "-_-_-_-_The guess is invalid" // Because we need it to be after GAME_ID_LEN+CHECK_LEN chars
#define SERVER_ERROR "-_-_-_-_Internal Server Error"  //  Same
#define OLD_GAME_ERROR "-_-_-_-_Game does not exist anymore"  //  Same
#define GAME_OVER_MSG "\nYou WON!"

#define PORT 8080


#endif // THREADPOOL_COMMON_H