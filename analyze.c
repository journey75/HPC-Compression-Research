//Tripp Herlong
/Feb 2nd 2022
//binary analysis

#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
  FILE *fp1, *fp2;
  int i, j, mask;
  char c[33];
  
    
  //open files for reading and writing
  fp1 = fopen("output2.txt", "r");
  fp2 = fopen("organized.txt", "w");

  if(fp1 == NULL || fp2 == NULL)
  {
    printf("Error opening file\n");
    return 1;
  }

    
    
    
  //******FIX LATER
  //Check for repeating bytes and get rid of them
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

