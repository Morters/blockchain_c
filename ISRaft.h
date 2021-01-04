#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h> 

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "data_containers/dict.h"
#include "data_containers/linked_list.h"

#include "nanomsg/include/nn.h"
#include "nanomsg/include/pipeline.h"

#define ERR_NULL 2
#define ERR_FILE 3
#define ERR_GENERAL 4

#define BLOCK_STR_SIZE 30000
#define SHORT_MESSAGE_LENGTH 300
#define MESSAGE_LENGTH 100000
#define TRANS_LIST_SIZE 20


//Message item structure
typedef struct message_item {
    char toWhom[300];
    char message[30000];
    unsigned int tries;
} message_item;

//Socket struct
typedef struct socket_item {
    int socket;
    unsigned int last_used;
} socket_item;

/////////////////////////////////////////////////////////
typedef struct block_header_t{
    uint32_t data_length;
    uint32_t timestamp;
    uint32_t nounce;

    unsigned char data_hash[32];
    unsigned char previous_hash[32];  
}block_header_t;

typedef struct block_t{
    void * body; 
    struct block_t* next;
    block_header_t header;
}block_t;

////////////// NEW DATA STRUCTURE /////////////////////////
//Transaction structure
typedef struct transaction {
    int time_of;
    char sender[500];
    char recipient[500];
    int amount;
    char signature[550];
} transaction;

typedef struct block{
    unsigned int index;
    unsigned int time;
    transaction trans_list[TRANS_LIST_SIZE];
    unsigned int trans_list_length;
    char previous_hash[32];
}block;

//Link in a datalog
typedef struct blink {
    block_t data;
    struct blink* next;
} blink;

////////////////////////////////////////////////////////////

//datalog structure
typedef struct datalog {
    block_t* head;
    int length;
} datalog;

//utils.c
datalog* new_chain();
block_t* create_new_block(block_t* prev, const char* data, uint32_t length);
int read_chain_from_file(datalog* in_chain, const char* filename);
int read_nodes_from_file(const char* filename, dict* dict_nodes);
int create_socket(const char* input);
void setup_message(message_item* in_message) ;

int ping(bt_node* in_dict, void* data);
int announce_existance(bt_node* in_dict, void* data);

int AppendEntries(bt_node* in_dict, void* data);

int announce_exit(bt_node* in_dict, void* data);
int announce_mined(bt_node* in_dict, void* data);

int discard_chain(datalog* in_chain);

//ISRaft.c
int hash256(const char *input_data, unsigned char *output_data);