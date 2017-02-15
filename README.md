# TextEditor using balanced binary search tree

This implementation of text editor allows following opeartions - 

This tree structure supports the following operations - 
- text_t * create_text() creates an empty text, whose length is 0.

- int length_text( text_t *txt) returns the number of lines of the current text.

- char * get_line( text_t *txt, int index) gets the line of number index, if such a line exists, and  returns NULL else.

- void append_line( text_t *txt, char * new_line) appends new line as new last line.

- char * set_line( text_t *txt, int index, char * new_line) sets the line of number index, if such a line exists, to new line, and returns a pointer to the previous line of that number. If no line of that number exists, it does not change the structure and returns NULL.

- void insert_line( text_t *txt, int index, char * new_line) inserts the line before the line of number index, if such a line exists, to new line, renumbering all lines after that line. If no such line exists, it appends new line as new last line.

- char * delete_line( text_t *txt, int index) deletes the line of number index, renumbering all lines after that line, and returns a pointer to the deleted line.

Key mechanism in this implementation is changed so that we can easily when a line is inserted, all the following lines will be renumbered without visiting more than O(log n) nodes. Same goes for delete opeartion.

Contribution from each of us -

1) Pranav Nawathe - insert_line, set_line, get_line almost half of the implementation of standard tree such as left_rotation, right_rotation, etc

2) Omkar Joshi - delete_line, append_line, length_text along with remaining implementation of tree

/* Reference insert, delete,left_rotation,right_rotation Advanced Data Structures PETER BRASS */
