#ifndef CSVPARSE_H
#define CSVPARSE_H

void make_rows(struct size *, struct row **, FILE *);
void split_line(char *, char **collection);
void get_data_size(struct size *, FILE *);

#endif // CSVPARSE_H
