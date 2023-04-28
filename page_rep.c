#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


typedef struct Node{
  struct Node *next;
  char rw;
  int page_number;
} Node;

#define LINE_LEN 256

struct Node *head;     // head of the linked list of frames
struct Node *tail;     // tail of frames

int *lru;              // array tracking the order of the most recently used nodes
int page_ref = 0;      // total number of page references
int page_miss = 0;     // total number of page faults
int time_miss = 0;     // total time units for page misses
int time_write = 0;    // time units spent on evicting dirty pages
int count_node = 0;    // page counter

// Update the lru array and return the least recently used page
// new_page: page number of the new page
// array_len: number of frames
int lru_array(int new_page, int array_len){
  int index = 0;

  // grab the least recent used
  int least_recent = lru[0];

  // find the index of the least recently used
  for (int x = 0; x < array_len; x++){
    if (lru[x] == new_page){
      index = x;
    }
  }
  
  // shift elements to left starting at the index of lru + 1
  for (int y = index + 1; y < array_len; y++){
    // if the cell is -1 means empty
    if (lru[y] == -1){
      lru[y-1] = new_page;
      return least_recent;
    }
    lru[y-1] = lru[y];
  }
  lru[array_len - 1] = new_page;

  return least_recent;
}

// page replacement with LRU algorithm
// rw: whether the current reference is read or write
// page_number: page number of the current reference
// num_frames: total number of frames available
void LRU(char rw, int page_number, int num_frames){
  int hit = 0;
 
  int least_recent = 0;
  
  //  check for hit
  struct Node *temp = NULL;
  temp = head; 
  while (temp != NULL) {
    if(temp->page_number == page_number){
      hit = 1;
      
      if(temp->rw == 'R' && rw == 'W'){
        temp->rw = 'W';
      }
      
      break;
    }
    temp = temp->next;
  }

  if(hit == 0){
    page_miss++;
    time_miss += 5;

    //create a new node
    struct Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->page_number = page_number;
    new_node->rw = rw;
    new_node->next = NULL;

    if (count_node < num_frames){

      //if head is empty
      if (head == NULL){
       tail = new_node; 
       head = new_node;

      // add at the end of the linked list
      } else {
        tail->next = new_node;
        tail = new_node;
      }

      // add the page number into the array
      lru[count_node] = page_number;
      count_node++;
    
    } else { // page fault and frames full
      
      least_recent = lru_array(page_number, num_frames);
 
      struct Node *temp_head = NULL;
      struct Node *ptr = head;

      // if head is LRU
      if(head->page_number == least_recent){
        temp_head = head;
        head = head->next;
        temp_head->next = NULL;
        tail->next = new_node;
        tail = new_node;
      
      } else {
        
        while (ptr->next != NULL){
   
          if (ptr->next->page_number == least_recent){
            temp_head = ptr->next;

            //middle case
            if(ptr->next->next != NULL){
              ptr->next = ptr->next->next;
              tail->next = new_node;
              tail = new_node;
              
            } else{ //tail case
              ptr->next = new_node;
              tail = new_node;
            }
            temp_head->next = NULL;
            break;
          }
          ptr = ptr->next;
        }
      }
      if (temp_head->rw == 'W'){
        time_write += 10;
      }
      free(temp_head);
    }
  } else {
    
    least_recent = lru_array(page_number, num_frames);

  }
  return;
}

// page replacement with FIFO algorithm
// rw: whether the current reference is read or write
// page_number: page number of the current reference
// num_frames: total number of frames available
void FIFO(char rw, int page_number, int num_frames){
  int hit = 0;
  struct Node *temp = NULL;

  // check for page hit
  temp = head; 
  while (temp != NULL) {
    if(temp->page_number == page_number){
      hit = 1;
      if(temp->rw == 'R' && rw == 'W'){
        temp->rw = 'W';
      }
    }
    temp = temp->next;
  }


  if(hit == 0){         // page fault case
    page_miss++;
    time_miss += 5;

    // create a new node
    struct Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->page_number = page_number;
    new_node->rw = rw;
    new_node->next = NULL;

    if (count_node < num_frames){     // page fault and frames not full
      if (head == NULL){              // first page case
       tail = new_node; 
       head = new_node;

      } else {                        // append the node to the end of list 
        tail->next = new_node;
        tail = new_node;
      }
      count_node++;
    
    } else {                           // page fault and frames are full
      // if the head (evicted) has "W" increment time_write      
      if(head->rw == 'W') {
        time_write += 10;
      }

      // remove the head
      struct Node *temp_head = NULL;
      temp_head = head;
      head = head->next;
      temp_head->next = NULL;
      tail->next = new_node;
      tail = new_node;
      free(temp_head);
    }
  }
  return;
}

// open the file and read each line and pass it to the function
void read_file(char * algo, int row_size, char * input_file){
  FILE * fp = fopen((char *) input_file, "r");
  if (fp == NULL){
    printf("cannot open file\n");
    exit(1);
  }

  char line[LINE_LEN];
  while (fgets(line, LINE_LEN, fp) != NULL) {
    
    char *RW = strtok(line, " ");            // read or write
    char *page_num = strtok(NULL, " ");      // page number
    page_num[strcspn(page_num, "\n")] = '\0';

    // convert the page number str into an integer
    int page_number = atoi(page_num);
    char rw = RW[0];
    page_ref++;

    // checks which algorithm to do
    if (strcmp(algo, "FIFO") == 0) {
      FIFO(rw, page_number, row_size);
    } else if (strcmp(algo, "LRU") == 0){
      LRU(rw, page_number, row_size);
    }
  }
  fclose(fp);
}

int main(int argc, char * argv[]){
  if ( argc < 4 ){
    return 0;
  }

  //save all commandline arguments
  char *algo = argv[1];
  int row_size = atoi(argv[2]);
  char *input_file = argv[3];

  // allocate memory for the lru array and intialize all elements to -1
  lru = (int *)malloc(sizeof(int *) * row_size);
  for(int x = 0; x < row_size ; x++){
    lru[x] = -1;
  }

  //size of the inner array
	read_file(algo, row_size, input_file);
  printf("total references: %d\n", page_ref);
  printf("page miss: %d\n", page_miss);
  printf("time miss: %d\n", time_miss + time_write);
  printf("time write: %d\n", time_write);

  // free all the allocated memory
  struct Node *ptr;
  while (head != NULL){
    ptr = head;
    head = head->next;
    free(ptr);
  }
  free(lru);

	return 0;
}