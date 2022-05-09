#ifndef HEADER_FILE
#define HEADER_FILE


struct element {
	int type; //Machine type
	int time; //Using time
};

typedef struct queue {
	struct element *elem;
	//maximum number of elements
    int max_elements;
	//current queue size
	int current_size;
    int head;
    int tail;

}queue;

queue* queue_init (int max_elements);
int queue_destroy (queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);

#endif