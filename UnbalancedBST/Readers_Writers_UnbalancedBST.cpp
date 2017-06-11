#include <bits/stdc++.h>
#include <pthread.h>

pthread_mutex_t full_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t writer_mutex = PTHREAD_MUTEX_INITIALIZER;
int readcount = 0 ;

using namespace std;

struct Node{
  int data;
  Node *left;
  Node *right;

};
Node *root;

struct thread_data{
    int type, key=0;
};

void* insert( void *threadarg){
    struct thread_data *my_data;


   my_data = (struct thread_data *) threadarg;
   int value =my_data->key;
   
   pthread_mutex_lock(&writer_mutex);

   cout << "high" << endl;

    Node *newNode = new Node;

   newNode -> data = value;
   newNode -> left = NULL; 
   newNode -> right = NULL;

   if(root == NULL) {
    root = newNode;
    pthread_mutex_unlock(&writer_mutex);
     pthread_exit(NULL);
    
   } //If the BST is empty
   
   else 
   {//The BST is not empty 
      Node *ptr = root; //points to the current Node
      Node *ptr_parent; //points to the parent Node

      while(ptr != NULL)
      {
         if((ptr -> data) > value)
         {   
            ptr_parent = ptr;    
            ptr = ptr -> left;
         }

        else  if((ptr -> data) < value)
         {
            ptr_parent = ptr;
            ptr = ptr -> right;
         }
      }
    

      ptr = newNode; //insert the newNode at the spot
      if((ptr_parent -> data) < value)
         ptr_parent -> right = newNode;
      else
         ptr_parent -> left = newNode; 


 }
   pthread_mutex_unlock(&writer_mutex);
    pthread_exit(NULL);
  
}

void * search(void *threadarg){
    struct thread_data *my_data;


   my_data = (struct thread_data *) threadarg;

   int data =my_data->key;

	 pthread_mutex_lock(&full_mutex);
     readcount++;
     if(readcount == 1){
      pthread_mutex_lock(&writer_mutex);
     }
    pthread_mutex_unlock(&full_mutex);

    Node *newNode = root;
   
    while(newNode!=NULL){
    	  if(data<newNode->data)
    	  	 newNode=newNode->left;
    	  	else if(data>newNode->data)
    	  		newNode=newNode->right;
    	  	else{
    	  		cout<<"Node found "<<endl;
    	  	    pthread_mutex_lock(&full_mutex);
               readcount--;
               if(readcount == 0){
                pthread_mutex_unlock(&writer_mutex);
               }
               pthread_mutex_unlock(&full_mutex);
    	  	         pthread_exit(NULL);
    	  	}
    }

    cout<<"Node Not Found"<<endl;
    pthread_mutex_lock(&full_mutex);
     readcount--;
     if(readcount == 0){
      pthread_mutex_unlock(&writer_mutex);
     }
     pthread_mutex_unlock(&full_mutex);
   pthread_exit(NULL);
}

void * delete_node(void *threadarg)
{
   struct thread_data *my_data;


   my_data = (struct thread_data *) threadarg;
   int val =my_data->key;

  pthread_mutex_lock(&full_mutex);
     readcount++;
     if(readcount == 1){
      pthread_mutex_lock(&writer_mutex);
     }
     pthread_mutex_unlock(&full_mutex);
  Node *ptr, *parent;
  Node *parent_replacement, *replacement;
  Node *child_ptr;

  int is_left = 0; // 1 for left. 0 for right.

  /* Find the node to be deleted */
  parent = NULL;
  ptr=root;
  while(ptr != NULL)
  {
     if (val == ptr->data)
         break; // Found it
     else if (val < ptr->data)
     {
         parent = ptr;
         is_left = 1; /* Left */
         ptr = ptr->left;
     }
     else
     {
         parent = ptr;
         is_left = 0; /* Right */
         ptr = ptr->right;
     }
  }

  /* ptr is either NULL or the node to be deleted */
  if (ptr == NULL)
  {
      printf("Node with value %d not found...\n", val);
      pthread_mutex_lock(&full_mutex);
     readcount--;
     if(readcount == 0){
      pthread_mutex_unlock(&writer_mutex);
     }
     pthread_mutex_unlock(&full_mutex);
        pthread_exit(NULL);
  }

  /* ptr is pointing to the node to be deleted */ 
  /* Three Cases
   * 1) It is leaf node
   * 2) It has one child
   * 3) It has two children
   */

   if ((ptr->left == NULL) && (ptr->right == NULL))
   {
       /* Case 1 : Leaf Node*/
       if (parent == NULL)
       {
            // The node being deleted is root itself!
            assert(ptr == root);
            free(ptr);
            root = NULL;
       }
       else
       {
             free(ptr);
             if (is_left)
                parent->left = NULL;
             else
                parent->right = NULL;
       }
   }
   else
   if ((ptr->left == NULL) || (ptr->right == NULL))
   {
      /* Case 2: The node has exactly one child */
      if (ptr->left != NULL)
          child_ptr = ptr->left;
      else
          child_ptr = ptr->right;

      if (parent == NULL) // root is being deleted 
      {
         assert(ptr == root);
         root = child_ptr;
         free(ptr);
      }
      else
      {
         // non-root node is being deleted. So, the parent of "to be deleted"
         // node will adopt the "to be deleted" node's child
         if (is_left)
             parent->left = child_ptr;
         else
             parent->right =  child_ptr;
         free(ptr);
      }
   }
   else
   {
       /* Case 3: Node to be deleted has both left and right children */
       
      /* Find replacement. i.e. the highest node less than the to-be-deleted
       *   It can be found as the largest member in the left subtree.
       *   So, we can take one left, and then keep moving right as far as
       *   possible (Note : Alternate approach was to take the minimum in
       *   the right subtree)
       */
      parent_replacement = ptr;
      replacement =  ptr->left;
      is_left = 1; /* Replacement is left child of parent */
      while(replacement->right != NULL)
      {
        parent_replacement = replacement;
        replacement = replacement->right;
        is_left = 0; // Replacement is right child of parent
      }


      /* Copy data */
      ptr->data = replacement->data;
      /* Two broad cases
       * i) Replacement is left child of ptr 
       *   (and could be having 0 or 1 children)
       * ii) Replacement is right (grand)child of ptr->left
       */

      if (is_left)
      {
            // case i : replacement is left child of ptr.
            assert(replacement->right == NULL);
            ptr->left = replacement->left;
            free(replacement);
      }
      else
      {
          // case ii : replacement is right grand(child of ptr->left
          parent_replacement->right = replacement->left;
          free(replacement);
      }
    }
    pthread_mutex_lock(&full_mutex);
     readcount--;
     if(readcount == 0){
      pthread_mutex_unlock(&writer_mutex);
     }
     pthread_mutex_unlock(&full_mutex);
      pthread_exit(NULL);   
}

void *MorrisTraversal(void *threadarg)
{

  pthread_mutex_lock(&full_mutex);
     readcount++;
     if(readcount == 1){
      pthread_mutex_lock(&writer_mutex);
     }
     pthread_mutex_unlock(&full_mutex);

  struct Node *current,*pre;
 
  if(root == NULL){
    pthread_mutex_lock(&full_mutex);
     readcount--;
     if(readcount == 0){
      pthread_mutex_unlock(&writer_mutex);
     }
     pthread_mutex_unlock(&full_mutex);
      pthread_exit(NULL); 
  }
 
  current = root;
  while(current != NULL)
  {                 
    if(current->left == NULL)
    {
      printf("%d ", current->data);
      current = current->right;      
    }    
    else
    {
      /* Find the inorder predecessor of current */
      pre = current->left;
      while(pre->right != NULL && pre->right != current)
        pre = pre->right;
 
      /* Make current as right child of its inorder predecessor */
      if(pre->right == NULL)
      {
        pre->right = current;
        current = current->left;
      }
             
      /* Revert the changes made in if part to restore the original 
        tree i.e., fix the right child of predecssor */   
      else 
      {
        pre->right = NULL;
        printf("%d ",current->data);
        current = current->right;      
      } /* End of if condition pre->right == NULL */
    } /* End of if condition current->left == NULL*/
  } 
   pthread_mutex_lock(&full_mutex);
     readcount--;
     if(readcount == 0){
      pthread_mutex_unlock(&writer_mutex);
     }
     pthread_mutex_unlock(&full_mutex);
                                /* End of while */
      pthread_exit(NULL);
}

void MorrisTraversal2()
{

  pthread_mutex_lock(&full_mutex);
  struct Node *current,*pre;
 
  if(root == NULL){
    pthread_mutex_unlock(&full_mutex);
      return ;
  }
 
  current = root;
  while(current != NULL)
  {                 
    if(current->left == NULL)
    {
      printf("%d ", current->data);
      current = current->right;      
    }    
    else
    {
      /* Find the inorder predecessor of current */
      pre = current->left;
      while(pre->right != NULL && pre->right != current)
        pre = pre->right;
 
      /* Make current as right child of its inorder predecessor */
      if(pre->right == NULL)
      {
        pre->right = current;
        current = current->left;
      }
             
      /* Revert the changes made in if part to restore the original 
        tree i.e., fix the right child of predecssor */   
      else 
      {
        pre->right = NULL;
        printf("%d ",current->data);
        current = current->right;      
      } /* End of if condition pre->right == NULL */
    } /* End of if condition current->left == NULL*/
  } 
   pthread_mutex_unlock(&full_mutex); 
                                
    return ;
}







/*void *func(void *threadarg){
    pthread_mutex_lock(&full_mutex);
    
     cout<<"worki"<<endl;
     struct td_para *my_data;

     my_data = (struct td_para*) threadarg;

     if(my_data->type == 1) insert(my_data->key);
     else if(my_data->type == 2) 
     	delete_node(my_data->key);
     else if(my_data->type == 3)
     	search(my_data->key);
     else if(my_data->type == 4)
     	MorrisTraversal();

     pthread_mutex_unlock(&full_mutex);

     pthread_exit(NULL);
}*/

int main()
{
    int ch,y=0;
    int n;
    cout<<"Enter number of threads you want to create ";
    cin>>n;
    pthread_t *threads;
    threads = new pthread_t[n];
    struct thread_data threads_para[n];
    

                cout<<"\n\t BINARY TREE " ;
                cout<<"\n 1. Insert in the tree ";
                cout<<"\n 2. Delete a node from the tree";
                cout<<"\n 3. Search for an element in the tree";
                cout<<"\n 4. Display the tree (Inorder) ";
                cout<<endl<<endl;
                cout<<"Enter Your choice for the function to be called and key"<<endl;


    for(int i=0;i<n;i++){
        cin>>threads_para[i].type;
        if(threads_para[i].type != 4){
            cin>>threads_para[i].key;
        }
    }
    for(int i=0;i<n;i++){
       if(threads_para[i].type==1){
          int rc = pthread_create(&threads[i], NULL, insert, (void *)&threads_para[i]);
         
          if (rc){
             cout << "Error:unable to create thread," << rc << endl;
             exit(-1);
          }
       }
       else if(threads_para[i].type==2){
          int rc = pthread_create(&threads[i], NULL, delete_node, (void *)&threads_para[i]);
         
          if (rc){
             cout << "Error:unable to create thread," << rc << endl;
             exit(-1);
          }
       }
       else if(threads_para[i].type==3){
          int rc = pthread_create(&threads[i], NULL, search, (void *)&threads_para[i]);
         
          if (rc){
             cout << "Error:unable to create thread," << rc << endl;
             exit(-1);
          }
       }
       else if(threads_para[i].type==4){
          int rc = pthread_create(&threads[i], NULL, MorrisTraversal, (void *)&threads_para[i]);
         
          if (rc){
             cout << "Error:unable to create thread," << rc << endl;
             exit(-1);
          }
       }

        
    }
    
    MorrisTraversal2();
   
    return 1;
  }
