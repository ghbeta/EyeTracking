#ifndef FILEWRITER_H
#define FILEWRITER_H
#include<stdio.h>
#include<fstream>
#include<iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
using namespace std;
using namespace cv;
class fileWriter
{



public:
ofstream configuration;
ofstream journal;
ifstream reading;

void writeConf(string filename,int first,int morph_element,int morph_size,int morph_size2,int erosion_size,int radius1,int radius2,Point pt1,Point pt2);
void wirteLog(string filename, int index, int valid, double x, double y, double area);
void writeTitle(string filename);







};
#endif // FILEWRITER_H
