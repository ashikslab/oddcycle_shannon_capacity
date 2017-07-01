#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>

int is_valid_groupelem(int candidate[5]);
void print_groupelem(int el[5]);

void init_PERMARRAY();
int perm_prod(int p1, int p2) ;
int is_same(int el1[5], int el2[5]);
void perform_groupop(int el1[5], int el2[5], int result[5]);

void init_PERMINV_TABLE();
void inverse(int el[5], int res[5]);
int fact(int n);
void init_rep();
