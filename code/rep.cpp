#include "rep.h"

//every groupelem is an array of 5 inetegers.
//first 4 elements stand for each axis positions,
//last element stands for axis rotations

int is_valid_groupelem(int candidate[5]) {
  for (int i=0; i<4; i++) {
    if(candidate[i]>18 || candidate[i]<0) {
      return 0;
    }
  }
  if(candidate[4] >24 || candidate[4]<0) {
    return 0;
  }
  return 1;
}
void print_groupelem(int el[5]) {
  for(int i=0; i<5; i++) {
    printf("(%d)", el[i]);
  }
  printf("\n");
}

// the following fn maps n\in{0.. fact(n)-1} to a permutation of {0..n-1}
void itoperm(int p, int arr[4]) {
  int pt = p; 
  int filled[4] = {0,0,0,0};
  arr[0] = pt%4;
  filled[arr[0]] = 1;
  pt = floor(pt/4);
  for(int i=1; i<4; i++) {
    int pp1 = pt%(4-i);
    pt = floor(pt/(4-i));
    int j = 0, k = 0;
    while(k<pp1 || filled[j]==1) {
      if(filled[j]==0) {
	k++;
      }
      j++;
    }
    arr[i] = j; 
    filled[arr[i]] = 1;
  }
}

int PERMARRAY[4][4][4][4];
void init_PERMARRAY() {
  for (int i1 = 0; i1<4; i1++) {
    for(int i2=0; i2<4; i2++) {
      for(int i3=0; i3<4; i3++) {
	for(int i4=0; i4<4; i4++) {
	  PERMARRAY[i1][i2][i3][i4] = -1;
	}
      }
    }
  }
  for(int i=0; i<24; i++) {
    int pa[4];
    itoperm(i, pa);
    PERMARRAY[pa[0]][pa[1]][pa[2]][pa[3]] = i;
  }
}
// combination of two permutation/ groupop in permutation group
int perm_prod(int p1, int p2) {
  int pa1[4], pa2[4];
  itoperm(p1, pa1);
  itoperm(p2, pa2);
  int ra[4];
  for(int i=0; i<4; i++) {
    ra[i] = pa2[pa1[i]];
  }
  return PERMARRAY[ra[0]][ra[1]][ra[2]][ra[3]];
}

int is_same(int el1[5], int el2[5]) {
  for(int i=0; i<5; i++) {
    if(el1[i]!=el2[i]) {
      return 0;
    }
  }
  return 1;
}

//define the group op in our torus group
void perform_groupop(int el1[5], int el2[5], int result[5]) {
  if(!is_valid_groupelem(el1) || !is_valid_groupelem(el2)) {
    printf("CRITICAL ERROR at %s:%d\n", __FILE__, __LINE__);
    exit(-1);
  }
  //now define the group operation
  for(int i=0; i<4; i++) {
    int resor = 0;
    if((el1[i]<9 && el2[i]<9) || (el1[i]>8 && el2[i]>8)) {
      resor = 0;
    }
    else {
      resor = 1;
    }
    //orientation of res[i] is now determined.
    int val1 = el1[i]%9;
    int val2 = el2[i]%9;
    int resval = (val1+val2)%9;
    result[i] = (9*resor)+resval;
  }
  result[4] = perm_prod(el1[4], el2[4]);
  if(!is_valid_groupelem(result)) {
    printf("CRITICAL ERROR at %S:%d\n", __FILE__, __LINE__);
    exit(-1);
  }
}

int PERMINV_TABLE[24];
//store inverses of permutation group elems in a table
void init_PERMINV_TABLE() {
  for(int i=0; i<24; i++) {
    for(int j=0; j<24; j++) {
      if(perm_prod(i, j)==0) {
	PERMINV_TABLE[i] = j;
	// PERMINV_TABLE[j] = i;
      }
    }
  }
}

//inverse of an element in the torus group
void inverse(int el[5], int res[5]) {
  for(int i=0; i<4; i++) {
    int elor = 0;
    if(el[i]>8) {
      elor = 1;
    }
    res[i] = (9*elor) + (9-(el[i])%9)%9;
  }
  res[4] = PERMINV_TABLE[el[4]];
}

int fact(int n) {
  if(n==1) return 1;
  else return n*fact(n-1);
}

void init_rep() {
  init_PERMARRAY();
  init_PERMINV_TABLE();

}

