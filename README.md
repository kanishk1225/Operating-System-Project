# Operating-System-Project

# Project Problem
Implement a set of thread-safe functions that update and search an unbalanced binary tree. This library should include functions (with the obvious purposes) of the following form: 


 initialize(tree); 
 add(tree, char *key, void *value); 
 delete(tree, char *key);
 Boolean lookup(char *key, void **value);
 etc. 

In the above prototypes, the tree is a structure that points to the root of the tree (you will need to define a suitable structure for this purpose). Each element of the tree holds a key-value pair. You will also need to define the structure for each element to include a mutex that protects that element so that only one thread at a time can access it. 
#After completing the above task you need to perform the same operation with balanced binary search tree. 
Then, perform the time and memory comparison between them


