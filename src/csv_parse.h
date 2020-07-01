#ifndef CSVPARSE_H
#define CSVPARSE_H

void make_rows(struct size *, struct row **, char *);
void check_if_in_quotes(char, char, int *);
void split_line(char *, char **collection);
void get_data_size(struct size *, char *);

#endif // CSVPARSE_H
