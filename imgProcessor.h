#ifndef IMGPROCESSOR_H
#define IMGPROCESSOR_H
#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include"fileWriter.h"

using namespace cv;
class imgProcessor :public QThread
{
    Q_OBJECT

  private:
    bool stop;
    QMutex mutex;
    QWaitCondition condition;
    Mat frame;
    Mat frameout;
    Mat frameout1;
    Mat frameout2;
    int frameRate;
    VideoCapture *capture;
    QImage img;
    QImage img1;
    //all the trackbar relevant parameters
    int firstValue;
    int morph_elem;
    int morph_size;
    int morph_size2;
    int erosion_size;
    Point last;
    bool saveLog;
    bool showprogress;
    fileWriter *log;
    string pathname;
    int radius1;
    int radius2;
    int possibleradius;
    Point pt1;
    Point pt2;





signals:
    void processedImage(QImage image,QImage image1);
    void videofinish(int ok);


protected:
    void run();

    //void msleep(int ms);

public:
    imgProcessor(QObject *parent=0);
    ~imgProcessor();
    bool loadVideo(string filename);
    void Play();
    void Stop();
    bool isStopped() const;
    //set video properties
    void setCurrentFrame(int frameNumber);
   // find eyeball
    void findEyeball(Mat input, Mat src);
    //get video properties
    double getFrameRate();
    double getCurrentFrame();
    double getNumberOfFrames();

    //getter and setter for the trackbar
    int getfirstValue();
    void setfirstValue(int value);

    int getmorph_elem();
    void setmorph_elem(int value);

    int getmorph_size();
    void setmorph_size(int value);

    int getmorph_size2();
    void setmorph_size2(int value);

    int geterosion_size();
    void seterosion_size(int value);

    bool getsaveLog();
    void setsaveLog(bool saveorwaht);

    bool getshowProgress();
    void setshowProgress(bool showorwhat);
    string getName();
    void setName(string name);

    int getRadiu1();
    void setRadius1(int value);

    int getRadiu2();
    void setRadius2(int value);

    int getPossibleRadius();


    Point getllftc();
    void setllftc(Point llftc);

    Point geturgtc();
    void seturgtc(Point urgt);
    Point sp1;
    Point sp2;



};
#endif // IMGPROCESSOR_H
