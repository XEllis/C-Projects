/**
   @file pie.c
   @author Xiaohui Z Ellis (xzheng6)

   This program draws a pie chart using a very simple image format.
   As input, the program expects relative sizes of three sections of the pie chart.
   It draws one section in red, another in green and another in blue.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

/** The number of pie slice input from user. */
#define SLICES 3

/** The size of the image, 100 pixels by 100 pixels. */
#define SIZE 100

/** RGB values in the range of 0 to 255. */
#define RGB 255

/** Red slice will drawn first. */
#define RED 1

/** Green slice will be drawn second. */
#define GREEN 2

/** Blue slice will be drawn third. */
#define BLUE 3

/** The pie chart will be centered at 49.5, 49.5. */
#define CENTER 49.5

/** The pie chart will be drawn in a circle of radius 45. */
#define PIE_RADIUS 45

/** The pie chart will have a 3-pixel border around it. */
#define BORDER 3

/** A mathematical constant. The ratio of a circle's circumference to its diameter. */
#define PI 3.14159265358979

/** Exit code for invalid input. */
#define EXIT_UNSUCCESS 100

/** Track the color of each pixel. */
int color = 0;

/**
   Return true if the given pixel location is inside a circle of radius rad centered at 49.5, 49.5.
   @param x pixle X coordinate.
   @param y pixle Y coordinate.
   @param rad radius of a circle.
   @return true if the given pixel location is inside a circle of radius rad centered at 49.5, 49.5.
 */
bool insideCircle(int x, int y, int rad)
{
  return pow(x-CENTER, 2) + pow(y-CENTER, 2) <= pow(rad, 2);
}

/**
   Return true.
   Update the global variable color
   if the vector from the center of the pie chart to the pixel's location
   is inside a particular slice of the pie chart (red, green, or blue).
   @param x pixle X coordinate.
   @param y pixle Y coordinate.
   @param red relative size of the red slice of the pie chart.
   @param green relative size of the green slice of the pie chart.
   @param blue relative size of the blue slice of the pie chart.
   @return true.
 */
bool insideSlice(int x, int y, int red, int green, int blue)
{
  double first = 2*PI*((double) red)/((double) (red+green+blue))-PI;
  double second = 2*PI*((double) (red+green))/((double) (red+green+blue))-PI;
  if (atan2(y-CENTER, x-CENTER) >= -PI && atan2(y-CENTER, x-CENTER) <= first) {
    color = RED;
  }
  else if (atan2(y-CENTER, x-CENTER) >= first && atan2(y-CENTER, x-CENTER) <= second) {
    color =GREEN;
  }
  else {
    color = BLUE;
  }
  return true;
}

/**
   Print out the color intensity in a 3-character field, followed by a space.
   @param color the color needed to be printed.
 */
void printcolor(int color)
{
  if (color == RED) {
    printf("%3d %3d %3d ", RGB, 0, 0);
  }
  else if (color == GREEN) {
    printf("%3d %3d %3d ", 0, RGB, 0);
  }
  else {
    printf("%3d %3d %3d ", 0, 0, RGB);
  }
}

/**
   Starting point for the program,
   it reads the sizes of three pie slices from standard input, red, then green then blue.
   Figures out the color of each pixel and writes the image out to the standard output.
   @return exit exit status for the program.
 */
int main()
{
  int red;
  int green;
  int blue;

  // Handle invalid input.
  int matches = scanf("%d%d%d", &red, &green, &blue);
  if (matches != SLICES) {
    printf("Invalid input\n");
    exit(EXIT_UNSUCCESS);
  }
  if (red < 0 || green < 0 || blue < 0 || (red == 0 && green == 0 && blue == 0)) {
    printf("Invalid input\n");
    exit(EXIT_UNSUCCESS);
  }

  // Header.
  printf("P3\n");
  printf("%d %d\n", SIZE, SIZE);
  printf("%d\n", RGB);

  // Figure out the color of each pixel and write out its color intensity.
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      // Check if the pixel is inside the pie chart.
      if (insideCircle(j, i, PIE_RADIUS)) {
        // Check which slide the pixel is part of and write out the color.
        if (insideSlice(j, i, red, green, blue)) {
          printcolor(color);
        }
      }
      // Check if the pixel is inside the black border.
      else if (insideCircle(j, i, PIE_RADIUS+BORDER)) {
        printf("%3d %3d %3d ", 0, 0, 0);
      }
      else {
        printf("%3d %3d %3d ", RGB, RGB, RGB);
      }
    }
    printf("\n");
  }
  return EXIT_SUCCESS;
}
