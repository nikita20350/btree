
# B-Tree Implementation in C

This project implements a B-tree in C with operations like insertion, deletion, and search.

## Functions

- **initTree**: Initializes the B-tree.
- **btree_createNode**: Creates a new B-tree node.
- **btree_createNodeWithVal**: Creates a new node with an initial value.
- **btree_insert**: Inserts a value into the B-tree.
- **btree_insertIntoNode**: Handles insertion inside a node.
- **btree_delete**: Deletes a value from the B-tree.
- **btree_deleteFromNode**: Handles deletion inside a node.
- **btree_search**: Searches for a value in the B-tree.
- **printBtree**: Prints the current state of the B-tree.
- **freeTree**: Frees the memory of the B-tree.

## Example Usage

```c
btree tree;
initTree(&tree);
```

```c
btree_insert(&tree, 10);
```

```c
printBtree(tree.root);
```

```c
btree_search(tree.root, 10);
```

```c
btree_delete(&tree, 10);
```

```c
printBtree(tree.root);
```

```c
btree_search(tree.root, 10);
```

```c
freeTree(tree.root);
```

## Compiling

```bash
gcc btree.c -o btree
./btree

