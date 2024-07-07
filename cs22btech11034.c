#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define N0_OF_CONFIG_LINES 10
#define CONFIG_LINE_LEN 10
#define ADDRESS_LINES 6000
#define RW_LEN 5
#define ADD_LEN 32

int log2Ceil(int num)
{

   int result = 0;
   num--;

   while (num > 0)
   {
      num >>= 1;
      result++;
   }

   return result;
}
uint64_t binaryToDecimal(const char *binaryString)
{
   size_t len = strlen(binaryString);

   uint64_t decimalValue = 0;
   uint64_t powerOfTwo = 1;

   for (size_t i = len; i > 0; i--)
   {
      if (binaryString[i - 1] == '1')
      {
         decimalValue += powerOfTwo;
      }
      powerOfTwo *= 2;
   }

   return decimalValue;
}

int stringToInt(const char *str)
{
   int result = 0;
   int sign = 1;

   if (*str == '-')
   {
      sign = -1;
      str++;
   }

   while (*str >= '0' && *str <= '9')
   {
      result = result * 10 + (*str - '0');
      str++;
   }

   return result * sign;
}
void sliceString(const char *input, char *output)
{
   const char *prefix = "0x";
   size_t prefixLength = strlen(prefix);
   size_t inputLength = strlen(input);

   if (inputLength <= prefixLength || strncmp(input, prefix, prefixLength) != 0)
   {
      fprintf(stderr, "Error: Input string is not in the expected format.\n");
      return;
   }

   strncpy(output, input + prefixLength, inputLength - prefixLength);
   output[inputLength - prefixLength] = '\0';
}
char *hexToBinary(const char *hexString)
{
   size_t len = strlen(hexString);
   size_t binaryLen = len * 4;

   // Allocate memory for the binary string
   char *binaryString = (char *)malloc(binaryLen + 1);

   if (binaryString == NULL)
   {
      perror("Memory allocation error");
      exit(EXIT_FAILURE);
   }

   binaryString[binaryLen] = '\0';

   for (size_t i = 0; i < len; i++)
   {
      char hexDigit = hexString[i];

      switch (hexDigit)
      {
      case '0':
         strcat(binaryString + i * 4, "0000");
         break;
      case '1':
         strcat(binaryString + i * 4, "0001");
         break;
      case '2':
         strcat(binaryString + i * 4, "0010");
         break;
      case '3':
         strcat(binaryString + i * 4, "0011");
         break;
      case '4':
         strcat(binaryString + i * 4, "0100");
         break;
      case '5':
         strcat(binaryString + i * 4, "0101");
         break;
      case '6':
         strcat(binaryString + i * 4, "0110");
         break;
      case '7':
         strcat(binaryString + i * 4, "0111");
         break;
      case '8':
         strcat(binaryString + i * 4, "1000");
         break;
      case '9':
         strcat(binaryString + i * 4, "1001");
         break;
      case 'A':
      case 'a':
         strcat(binaryString + i * 4, "1010");
         break;
      case 'B':
      case 'b':
         strcat(binaryString + i * 4, "1011");
         break;
      case 'C':
      case 'c':
         strcat(binaryString + i * 4, "1100");
         break;
      case 'D':
      case 'd':
         strcat(binaryString + i * 4, "1101");
         break;
      case 'E':
      case 'e':
         strcat(binaryString + i * 4, "1110");
         break;
      case 'F':
      case 'f':
         strcat(binaryString + i * 4, "1111");
         break;
      default:
         fprintf(stderr, "Invalid hex digit: %c\n", hexDigit);
         exit(EXIT_FAILURE);
      }
   }

   return binaryString;
}

char *sliceBinaryString(const char *binaryString, size_t index1, size_t index2)
{
   size_t len = strlen(binaryString);

   if (index1 > len || index2 > len || index1 > index2)
   {
      fprintf(stderr, "Invalid indices\n");
      exit(EXIT_FAILURE);
   }

   size_t slicedLen = index2 - index1 + 1;

   char *slicedString = (char *)malloc(slicedLen + 1);

   if (slicedString == NULL)
   {
      perror("Memory allocation error");
      exit(EXIT_FAILURE);
   }

   slicedString[slicedLen] = '\0';

   strncpy(slicedString, binaryString + len - index2 - 1, slicedLen);

   return slicedString;
}
void updateArray(uint64_t arr[], size_t size, size_t k)
{

   for (size_t i = k; i < size - 1; i++)
   {
      arr[i] = arr[i + 1];
   }
}
int main()
{

   FILE *file = fopen("instruct.txt", "r");

   if (file == NULL)
   {
      perror("Error opening file");
      return 1;
   }

   char lines[N0_OF_CONFIG_LINES][CONFIG_LINE_LEN];

   int lineCount = 0;
   while (fgets(lines[lineCount], CONFIG_LINE_LEN, file) != NULL)
   {

      size_t length = strlen(lines[lineCount]);
      if (lines[lineCount][length - 1] == '\n')
      {
         lines[lineCount][length - 1] = '\0';
      }

      lineCount++;

      if (lineCount >= N0_OF_CONFIG_LINES)
      {
         fprintf(stderr, "Exceeded maximum number of lines (%d)\n", N0_OF_CONFIG_LINES);
         break;
      }
   }

   // Closing the file
   fclose(file);
   int cachesize;
   int blocksize;
   int totalblocks;
   int rows;
   char *a = lines[2];
   int columns = stringToInt(a);
   if (columns != 0)
   {
      a = lines[0];
      cachesize = stringToInt(a);
      a = lines[1];
      blocksize = stringToInt(a);
      totalblocks = cachesize / blocksize;
      rows = totalblocks / columns;
   }
   else
   {
      a = lines[0];
      cachesize = stringToInt(a);
      a = lines[1];
      blocksize = stringToInt(a);
      totalblocks = cachesize / blocksize;
      rows = 1;
      columns = totalblocks;
   }
   int replace_type;
   int writeBack;
   if (strcmp(lines[3], "FIFO") == 0)
      replace_type = 1;
   else if (strcmp(lines[3], "LRU") == 0)
      replace_type = 2;
   else if (strcmp(lines[3], "RANDOM") == 0)
      replace_type = 3;
   else
      printf("replacement type is not specified proprely\n");

   if (strcmp(lines[4], "WB") == 0)
      writeBack = 1;
   else if (strcmp(lines[4], "WT") == 0)
      writeBack = 2;
   else
      printf("READ or WTITE BACK type is not specified proprely\n");

   int offset_len = log2Ceil(blocksize);
   int rows_index_len = log2Ceil(rows);
   if (rows_index_len == 0)
      rows_index_len = 1;

   FILE *file2 = fopen("trace.txt", "r");

   if (file2 == NULL)
   {
      perror("Error opening file2");
      return 1;
   }

   char readOrWrite[ADDRESS_LINES][2];
   char add[ADDRESS_LINES][ADD_LEN];
   char setIndeces[ADDRESS_LINES][10];

   char tags[ADDRESS_LINES][ADD_LEN];
   uint64_t tagDecimal[ADDRESS_LINES];
   uint64_t setDecimal[ADDRESS_LINES];

   int lineCount2 = 0;
   while (fscanf(file, "%1s : %13s\n", readOrWrite[lineCount2], add[lineCount2]) == 2)
   {

      lineCount2++;
   }
   for (int i = 0; i < lineCount2; ++i)
   {

      const char *input = add[i];
      char output[9];

      sliceString(input, output);

      strcpy(add[i], output);
      input = add[i];
      size_t index1 = offset_len;
      size_t index2 = offset_len + rows_index_len - 1;
      char *binaryString = hexToBinary(input);
      char *slicedString = sliceBinaryString(binaryString, index1, index2);
      strcpy(setIndeces[i], slicedString);
      setDecimal[i] = binaryToDecimal(setIndeces[i]);
      index1 = offset_len + rows_index_len;
      index2 = strlen(binaryString) - 1;
      slicedString = sliceBinaryString(binaryString, index1, index2);
      strcpy(tags[i], slicedString);
      tagDecimal[i] = binaryToDecimal(tags[i]);
   }

   // closing the file2
   fclose(file2);

   uint64_t arrayOfCache[rows][columns];
   int no_of_hits = 0;
   bool hitOrMiss[ADDRESS_LINES];
   int no_of_miss = 0;
   for (int i = 0; i < rows; ++i)
   {
      for (int j = 0; j < columns; ++j)
      {
         arrayOfCache[i][j] = 0;
      }
   }
   if (writeBack == 1)
   {
      if (replace_type == 1)
      {
         for (int i = 0; i < lineCount2; ++i)
         {
            uint64_t rowindex = setDecimal[i];
            for (int columnIndex = 0; columnIndex < columns; ++columnIndex)
            {

               if (columnIndex == (columns - 1))
               {
                  updateArray(arrayOfCache[rowindex], columns, 0);
                  arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  hitOrMiss[i] = false;
                  no_of_miss++;
               }

               if (arrayOfCache[rowindex][columnIndex] == tagDecimal[i])
               {
                  hitOrMiss[i] = true;
                  no_of_hits++;
                  break;
               }

               if (arrayOfCache[rowindex][columnIndex] == 0)
               {
                  arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  hitOrMiss[i] = false;
                  no_of_miss++;
                  break;
               }
            }
         }
      }
      else if (replace_type == 2)
      {
         for (int i = 0; i < lineCount2; ++i)
         {
            uint64_t rowindex = setDecimal[i];
            for (int columnIndex = 0; columnIndex < columns; ++columnIndex)
            {
               if (columnIndex == (columns - 1))
               {
                  updateArray(arrayOfCache[rowindex], columns, 0);
                  arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  hitOrMiss[i] = false;
                  no_of_miss++;
               }

               if (arrayOfCache[rowindex][columnIndex] == tagDecimal[i])
               {
                  hitOrMiss[i] = true;
                  no_of_hits++;
                  for (int x = columnIndex; x < columns; x++)
                  {
                     if (arrayOfCache[rowindex][x] == 0)
                     {
                        updateArray(arrayOfCache[rowindex], x + 1, columnIndex);
                        arrayOfCache[rowindex][x] = tagDecimal[i];
                        break;
                     }
                     else if (x == (columns - 1))
                     {
                        updateArray(arrayOfCache[rowindex], columns, columnIndex);
                        arrayOfCache[rowindex][x] = tagDecimal[i];
                     }
                  }

                  break;
               }

               if (arrayOfCache[rowindex][columnIndex] == 0)
               {
                  arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  hitOrMiss[i] = false;
                  no_of_miss++;
                  break;
               }
            }
         }
      }
      else
      {
         for (int i = 0; i < lineCount2; ++i)
         {
            uint64_t rowindex = setDecimal[i];
            for (int columnIndex = 0; columnIndex < columns; ++columnIndex)
            {
               if (columnIndex == (columns - 1))
               {
                  int random_index;
                  srand(time(NULL));
                  random_index = rand() % (columns);

                  updateArray(arrayOfCache[rowindex], columns, random_index);
                  arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  hitOrMiss[i] = false;
                  no_of_miss++;
               }

               if (arrayOfCache[rowindex][columnIndex] == tagDecimal[i])
               {
                  no_of_hits++;
                  hitOrMiss[i] = true;
                  break;
               }

               if (arrayOfCache[rowindex][columnIndex] == 0)
               {
                  arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  hitOrMiss[i] = false;
                  no_of_miss++;
                  break;
               }
            }
         }
      }
   }
   else
   {
      if (replace_type == 1)
      {
         for (int i = 0; i < lineCount2; ++i)
         {
            uint64_t rowindex = setDecimal[i];
            for (int columnIndex = 0; columnIndex < columns; ++columnIndex)
            {
               if (columnIndex == (columns - 1))
               {
                  if (strcmp(readOrWrite[i], "R") == 0)
                  {
                     updateArray(arrayOfCache[rowindex], columns, 0);
                     arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  }
                  no_of_miss++;
                  hitOrMiss[i] = false;
               }
               if (arrayOfCache[rowindex][columnIndex] == tagDecimal[i])
               {
                  no_of_hits++;
                  hitOrMiss[i] = true;
                  break;
               }

               if (arrayOfCache[rowindex][columnIndex] == 0)
               {
                  if (strcmp(readOrWrite[i], "R") == 0)
                     arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  no_of_miss++;
                  hitOrMiss[i] = false;
                  break;
               }
            }
         }
      }
      else if (replace_type == 2)
      {
         for (int i = 0; i < lineCount2; ++i)
         {
            uint64_t rowindex = setDecimal[i];
            for (int columnIndex = 0; columnIndex < columns; ++columnIndex)
            {
               if (columnIndex == (columns - 1))
               {
                  if (strcmp(readOrWrite[i], "R") == 0)
                  {
                     updateArray(arrayOfCache[rowindex], columns, 0);
                     arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  }
                  no_of_miss++;
                  hitOrMiss[i] = false;
               }

               if (arrayOfCache[rowindex][columnIndex] == tagDecimal[i])
               {
                  no_of_hits++;
                  hitOrMiss[i] = true;
                  for (int x = columnIndex; x < columns; x++)
                  {
                     if (arrayOfCache[rowindex][x] == 0)
                     {
                        updateArray(arrayOfCache[rowindex], x + 1, columnIndex);
                        arrayOfCache[rowindex][x] = tagDecimal[i];
                        break;
                     }
                     else if (x == (columns - 1))
                     {
                        updateArray(arrayOfCache[rowindex], columns, columnIndex);
                        arrayOfCache[rowindex][x] = tagDecimal[i];
                     }
                  }

                  break;
               }

               if (arrayOfCache[rowindex][columnIndex] == 0)
               {
                  if (strcmp(readOrWrite[i], "R") == 0)
                     arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  no_of_miss++;
                  hitOrMiss[i] = false;
                  break;
               }
            }
         }
      }
      else
      {
         for (int i = 0; i < lineCount2; ++i)
         {
            uint64_t rowindex = setDecimal[i];
            for (int columnIndex = 0; columnIndex < columns; ++columnIndex)
            {
               if (columnIndex == (columns - 1))
               {
                  if (strcmp(readOrWrite[i], "R") == 0)
                  {
                     int random_index;
                     srand(time(NULL));
                     random_index = rand() % (columns);

                     updateArray(arrayOfCache[rowindex], columns, random_index);
                     arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  }
                  no_of_miss++;
                  hitOrMiss[i] = false;
               }
               if (arrayOfCache[rowindex][columnIndex] == tagDecimal[i])
               {
                  no_of_hits++;
                  hitOrMiss[i] = true;
                  break;
               }

               if (arrayOfCache[rowindex][columnIndex] == 0)
               {
                  if (strcmp(readOrWrite[i], "R") == 0)
                     arrayOfCache[rowindex][columnIndex] = tagDecimal[i];
                  no_of_miss++;
                  hitOrMiss[i] = false;
                  break;
               }
            }
         }
      }
   }
   for (int i = 0; i < lineCount2; ++i)
   {
      if (hitOrMiss[i])
         printf("Address: 0x%s, SET: 0x%lx, HIT, TAG: 0x%lx\n", add[i], setDecimal[i], tagDecimal[i]);
      else
         printf("Address: 0x%s, SET: 0x%lx, MISS, TAG: 0x%lx\n", add[i], setDecimal[i], tagDecimal[i]);
   }
   printf("%d hits, %d misses  \n", no_of_hits, no_of_miss);

   return 0;
}
