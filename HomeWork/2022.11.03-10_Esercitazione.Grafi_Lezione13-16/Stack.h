typedef struct n{
	int index;
	struct n * next;
} StackNode;

typedef StackNode* Stack;

void push(Stack*, int n);
int pop(Stack*);

 