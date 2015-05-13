/**
  EyeTracking
  fileWriter.cpp
  Purpose:Processing all the file writing actions

  @author Hao Gao
  @version 1.0 01/12/2014
*/
#include "fileWriter.h"
#include <qdebug.h>
/**
 * @brief fileWriter::writeConf
 * this function is used to save the conf.value in a pre defined format
 * @param filename
 * @param first
 * @param morph_element
 * @param morph_size
 * @param morph_size2
 * @param erosion_size
 * @param radius1
 * @param radius2
 */
void fileWriter::writeConf(string filename, int first, int morph_element, int morph_size, int morph_size2, int erosion_size, int radius1, int radius2,Point pt1,Point pt2){

    //qDebug()<<QString::fromStdString(filename);
    configuration.open((filename+"_conf.txt"),ios::trunc);
    //qDebug()<<"i am here";
    configuration<<first<<"\n"<<morph_element<<"\n"<<morph_size<<"\n"<<morph_size2<<"\n"<<erosion_size<<"\n"<<radius1<<"\n"<<radius2<<"\n"<<pt1.x<<"\n"<<pt1.y<<"\n"<<pt2.x<<"\n"<<pt2.y;
    configuration.close();







}
/**
 * @brief fileWriter::wirteLog
 * this function is used to save the log files of the eye videos
 * @param filename
 * @param index
 * @param valid
 * @param x
 * @param y
 * @param area
 */
void fileWriter::wirteLog(string filename, int index, int valid, double x, double y, double area){

    journal.open((filename+"_journal.csv"),ios::app);
    //journal<<index<<";"<<valid<<";"<<x<<";"<<y<<";"<<area<<";\n";
    journal<<index<<";"<<x<<";"<<y<<";"<<area<<";"<<valid<<";\n";
    journal.close();




}
/**
 * @brief fileWriter::writeTitle
 * this function write the first line(header) to each log file
 * @param filename
 */
void fileWriter::writeTitle(string filename){
         //const char *title = "frames;valid;eye_x;eye_y;eye_a\n";
         const char *title = "index;eye_x;eye_y;eye_a;eye_valid\n";
         journal.open((filename+"_journal.csv"),ios::trunc);
         journal<<title;
         journal.close();


}

