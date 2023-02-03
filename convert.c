//tripp Herlong
//jan 28 2023
//this will take in a bin file and convert the floating point contents to Uints


#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[]) 
{
    FILE *input = fopen("input.bin", "rb");  // Open binary file for reading
    FILE *output = fopen("output.txt", "w");  // Open text file for writing
    FILE *output2 = fopen("output2.txt", "w");


    //MAKE SURE the file names match up!!!
    //maybe fix the hardcoding and take in a command line argument
    if (!input || !output) 
    {
        printf("Error: failed to open files - check to make sure the names match\n");
        return 1;
    }

    //These are placeholders for reading and writing numbers
    float num;
    uint32_t converted;


    //This is where conversion takes place
    for (int i = 0; i < 10; i++) 
    {
        fread(&num, sizeof(float), 1, input);  // Read binary from file

        int32_t i = *(int32_t*) &num;  // Convert to int
        converted = (uint32_t) i;  // Convert to Uint

        fprintf(output, "%u\n", converted);  // Write to .txt file

	for (int j = 31; j >= 0; j--) 
	{
            fprintf(output2, "%d", (converted >> j) & 1);  // Write the binary representation to the text file
        }
        fprintf(output2, "\n");
    }

    fclose(input);
    fclose(output);
    fclose(output2);

    return 0;
}
