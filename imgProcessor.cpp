/**
  EyeTracking
  imgProcessor.cpp
  Purpose:process every frame of the eye video and passing the result to the GUI thread

  @author Hao Gao
  @version 1.0 01/12/2014
*/
#include"imgProcessor.h"
#include<qdebug.h>
imgProcessor::imgProcessor(QObject *parent)
    :QThread(parent)
{
    log=new fileWriter();
    stop=true;
}

bool imgProcessor::loadVideo(string filename){

    //capture.open(filename);
    pathname=filename;
    capture=new VideoCapture(filename);

    if(capture->isOpened()){

        frameRate = (int) capture->get(CV_CAP_PROP_FPS);
        int middle_width = (int) capture->get(CV_CAP_PROP_FRAME_WIDTH)/2;
        int middle_hight = (int) capture->get(CV_CAP_PROP_FRAME_HEIGHT)/2;
        sp1.x=middle_width-50;
        sp2.x=middle_width+50;
        sp1.y=middle_hight-25;
        sp2.y=middle_hight+25;
        return true;


    }
    else
        return false;



}

void imgProcessor::Play(){

    if(!isRunning()){

    if(isStopped()){


        stop=false;

    }

        start(LowPriority);

    }

    }
/**
 * @brief imgProcessor::run
 * process every frame of the eye video and save the result as the QImage to show in the GUI thread
 */
void imgProcessor::run(){

    //int delay = (1/frameRate);
    log->writeTitle(pathname);

    while(!stop){

//        if(!capture->read(frame)){

//            stop=true;
//            emit videofinish(0);
//            return;

//        }
//        else{

         if(!capture->read(frame)){
             stop=true;

             //emit videofinish(0);
             capture->release();
             frame.release();
             frameout.release();
             frameout1.release();
             frameout2.release();
             delete capture;   ///< improve ram usage
             emit videofinish(0);
             return;
         }
         else{

        cvtColor(frame,frameout,CV_BGR2GRAY);///< change the frame into the gray image
        threshold(frameout,frameout,firstValue,255,CV_THRESH_BINARY);///< change the gray image into a bitmap


        Mat element=getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );//morph_elem
        Mat element2 = getStructuringElement( morph_elem, Size( 2*morph_size2 + 1, 2*morph_size2+1 ), Point(morph_size2, morph_size2 ) );//morph_size2
        //qDebug()<<QString::number(morph_size);
                morphologyEx(frameout,frameout,2,element2);
                morphologyEx(frameout,frameout,1,element);
                morphologyEx(frameout,frameout,2,element2);
                morphologyEx(frameout,frameout,1,element);
                morphologyEx(frameout,frameout,2,element2);
                morphologyEx(frameout,frameout,1,element);
                morphologyEx(frameout,frameout,2,element2);

        Mat element1 = getStructuringElement( MORPH_ELLIPSE,Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                                            Point( erosion_size, erosion_size ) );
        erode(frameout,frameout1,element1);
        erode(frameout,frameout2,element1);///<using the morphology and erode to isolate the eye ball.
        img1=QImage((uchar*)(frameout1.data),frameout1.cols,frameout1.rows,QImage::Format_Indexed8);

        findEyeball(frameout2,frame);///< the result image should be used to draw the eyeball in the function findEyeball

        Scalar color=Scalar(255, 255, 0);
        rectangle(frame, pt1, pt2, color, 2, 8,0);// showing the bbox within the source image, adjust needed later
        qDebug()<<QString::number(pt1.y);
        qDebug()<<QString::number(pt2.y);
        img=QImage((uchar*)(frame.data),frame.cols,frame.rows,QImage::Format_RGB888);


        //
       // if(capture->get(CV_CAP_PROP_POS_FRAMES)==(capture->get(CV_CAP_PROP_FRAME_COUNT))-2){emit videofinish(0);}


       // }

   }
        emit processedImage(img1,img);
         //frame.release();
        this->msleep(long(25*2.5));

    }

 //return ;

}
/**
 * @brief imgProcessor::findEyeball
 * this function is to find the eye ball and draw the eye ball to each frame also save the location of the eye ball into the journal files
 * @param input
 * processed image from the function run
 * @param src
 * ogrinal image from the eye videos
 */
void imgProcessor::findEyeball(Mat input, Mat src){

      vector<vector<Point> > contours;
      vector<Vec4i> hierarchy;
      Scalar color=Scalar(255, 255, 0);
      //threshold( iinput, iinput, 100, 255, THRESH_BINARY );
      findContours( input, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0) );

        vector<RotatedRect> minEllipse( contours.size() );
        vector<Point2f>center( contours.size() );
          vector<float>radius( contours.size() );
          vector<double> area(contours.size());
          for( int i = 0; i < contours.size(); i++ )
               {
                   if(contours[i].size()<5)
                       continue;
                     minEllipse[i]=fitEllipse(Mat(contours[i]));

                 center[i]=minEllipse[i].center;

                area[i]= 3.14*(minEllipse[i].size.height/2)*(minEllipse[i].size.width/2);

                 radius[i]=sqrt(area[i]/3.14);

               }

          if(contours.size()==2){
            //radius
            if((int)radius[1]<=radius2&&(int)radius[1]>radius1&&center[1].x<=pt2.x&&center[1].x>=pt1.x&&center[1].y<=pt2.y&&center[1].y>=pt1.y){
                possibleradius=(int)radius[1];
                ellipse(src,minEllipse[1],color,2,8);

                last=center[1];

               // cout<<"find "<<index<<endl;
              //  filewriter(index,center[1].x,center[1].y,area[1]);
                if(saveLog){

                    log->wirteLog(pathname,getCurrentFrame(),1,center[1].x,center[1].y,area[1]);

                }
            }
            else{
               // cout<<"frame:"<<index<<" not found"<<endl;
               // filewriter(index,0,0,0);
                if(saveLog){

                    log->wirteLog(pathname,getCurrentFrame(),0,0,0,0);

                }

            }
            }///< if there are only two contours in the image, and one contours is in the possible radius range, this countour should be the eye ball
            else{

                vector<double> find;//collector von distanz

                if(contours.size()<=1){
                  //  cout<<"frame:"<<index<<" not found"<<endl;
                //filewriter(index,0,0,0);  //wenn keine contours gefunden
                    if(saveLog){

                        log->wirteLog(pathname,getCurrentFrame(),0,0,0,0);

                    }

                return;///< if there is only one contour, this must be the faulty one should be ignored.
                }
                for(int i=1;i<contours.size();i++){

                    double distance = sqrt((last.x-center[i].x)*(last.x-center[i].x)+(last.y-center[i].y)*(last.y-center[i].y));//distanz berechnen
                    find.push_back(distance);//speichern die distanz im Vector
                }
                double found = *min_element(find.begin(),find.end());//kleinste werte zu bestimmen

                for(int j=1;j<contours.size();j++){

                    //radius
                    if(found==find[j-1]&&(int)radius[j]<radius2&&(int)radius[j]>radius1&&center[j].x<=pt2.x&&center[j].x>=pt1.x&&center[j].y<=pt2.y&&center[j].y>=pt1.y){
                       possibleradius=(int)radius[j];
                       ellipse(src,minEllipse[j],color,2,8);

                        last=center[j];
                     //   cout<<"find "<<index<<endl;
                    //filewriter(index,center[j].x,center[j].y,area[j]);
                        if(saveLog){

                            log->wirteLog(pathname,getCurrentFrame(),1,center[j].x,center[j].y,area[j]);

                        }

                    find.clear();
                    break;
                    }

                    else if(j==contours.size()-1)
                    {

                      // cout<<"frame:"<<index<<" not found"<<endl;
                      // filewriter(index,0,0,0);
                        if(saveLog){

                            log->wirteLog(pathname,getCurrentFrame(),0,0,0,0);

                        }


                    }
                }






            }///< if there are more than two contours are found, we should then find the nearest countour to the last countour which was found in the last frame(calculate all the distance from this contours to the last found countours, and choose the nearst one)











}

imgProcessor::~imgProcessor(){

    mutex.lock();
    stop=true;
    capture->release();
    delete capture;
    condition.wakeOne();
    mutex.unlock();
    wait();


}

void imgProcessor::Stop(){

    stop=true;


}



bool imgProcessor::isStopped() const{

    return this->stop;

}

double imgProcessor::getCurrentFrame(){

    return capture->get(CV_CAP_PROP_POS_FRAMES);


}

double imgProcessor::getNumberOfFrames(){
    return capture->get(CV_CAP_PROP_FRAME_COUNT);


}

double imgProcessor::getFrameRate(){
    return frameRate;
}

void imgProcessor::setCurrentFrame(int frameNumber){

    capture->set(CV_CAP_PROP_POS_FRAMES, frameNumber);

}

void imgProcessor::setfirstValue(int value){

    firstValue=value;
}

int imgProcessor::getfirstValue(){
    //qDebug()<<firstValue;
    return firstValue;

}

int imgProcessor::getmorph_elem(){

    return morph_elem;

}
void imgProcessor::setmorph_elem(int value){

    morph_elem=value;
}
int imgProcessor::getmorph_size(){

    return morph_size;
}
void imgProcessor::setmorph_size(int value){
    morph_size=value;


}
int imgProcessor::getmorph_size2(){

    return morph_size2;

}
void imgProcessor::setmorph_size2(int value){

    morph_size2=value;
}
int imgProcessor::geterosion_size(){
    return erosion_size;

}
void imgProcessor::seterosion_size(int value){
    erosion_size=value;

}

bool imgProcessor::getsaveLog(){
    return saveLog;
}

void imgProcessor::setsaveLog(bool saveorwaht){


    saveLog=saveorwaht;
}

string imgProcessor::getName(){
    return pathname;
}

void imgProcessor::setName(string name){
    pathname=name;
}

int imgProcessor::getRadiu1(){

    return radius1;
}

void imgProcessor::setRadius1(int value){

    radius1=value;
}

int imgProcessor::getRadiu2(){

    return radius2;
}

void imgProcessor::setRadius2(int value){

    radius2=value;
}

int imgProcessor::getPossibleRadius(){

    return possibleradius;
}

bool imgProcessor::getshowProgress(){
    return showprogress;
}

void imgProcessor::setshowProgress(bool showorwhat){
    showprogress=showorwhat;

}

Point imgProcessor::getllftc(){
    return pt1;
}

void imgProcessor::setllftc(Point llftc){
    pt1 = llftc;
}

Point imgProcessor::geturgtc(){
    return pt2;
}

void imgProcessor::seturgtc(Point urgtc){
    pt2 = urgtc;
}


