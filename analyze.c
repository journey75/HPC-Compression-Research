//#include<stdio.h>
//#include<stdlib.h>
//#include<string.h>
//#include<math.h>

//#define BUFSIZE 32

/*
int main(int argc, char *argv[]) {
    FILE *input, *output;
    char buffer[BUFSIZE];
    int mask = 0xffffffe0;

    input = fopen("output2.txt", "r");
    if (input == NULL) {
        fprintf(stderr, "Error: unable to open input file.\n");
        return 1;
    }
    output = fopen("organized.txt", "w");
    if (output == NULL) {
        fprintf(stderr, "Error: unable to open output file.\n");
        fclose(input);
        return 1;
    }

    while (fgets(buffer, BUFSIZE, input) != NULL) {
        int number = atoi(buffer);
        number &= mask;
        fprintf(output, "%d\n", number);
    }

    fclose(input);
    fclose(output);
    return 0;
}
*/


#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
  FILE *fp1, *fp2;
  int i, j, mask;
  char c[33];

  fp1 = fopen("output2.txt", "r");
  fp2 = fopen("organized.txt", "w");

  if(fp1 == NULL || fp2 == NULL){
    printf("Error opening file\n");
    return 1;
  }

  for(i = 0; i < 10; i++){
    fscanf(fp1, "%s", c);
    for(j = 0; j < 8; j++){
      mask = 1 << j + 24;
      fprintf(fp2, "%c", (c[j] == '1') ? '1' : '0');
    }
    fprintf(fp2, "|");
    for(j = 0; j < 8; j++){
      mask = 1 << j + 16;
      fprintf(fp2, "%c", (c[j + 8] == '1') ? '1' : '0');
    }
    fprintf(fp2, "|");
    for(j = 0; j < 8; j++){
      mask = 1 << j + 8;
      fprintf(fp2, "%c", (c[j + 16] == '1') ? '1' : '0');
    }
    fprintf(fp2, "|");
    for(j = 0; j < 3; j++){
      mask = 1 << j;
      fprintf(fp2, "%c", (c[j + 24] == '1') ? '1' : '0');
    }
    fprintf(fp2, "\n");
  }

  fclose(fp1);
  fclose(fp2);

  return 0;
}

