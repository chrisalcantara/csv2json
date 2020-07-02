#ifndef CSVPARSE_H
#define CSVPARSE_H

void make_rows(struct size *, struct row **, FILE *);
void check_if_in_quotes(char, char, int *);
void split_line(char *, char **collection);
void get_data_size(struct size *, FILE *);

#endif // CSVPARSE_H
