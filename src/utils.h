#ifndef UTILS_H
#define UTILS_H

void print_use(void);
void print_json(char *);
void read_input(char **, unsigned long *, char *);
void free_columns(int, char **);
void free_structs(struct row **, int);

void trim(char *);
void trim_whitespace(char *);
void check_if_in_quotes(char, char, bool *);

struct row **populate_rows(struct size *);

#endif // UTILS_H
