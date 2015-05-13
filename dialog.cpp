/**
    Eye Tracking
    dialog.cpp
    Purpose: Present the GUI of this Program,communicating with the processing thread in imgProcessor.cpp
    @author Hao Gao
    @version 1.0 01/12/2014
*/
#include "dialog.h"
#include "ui_dialog.h"
#include <QtCore>
#include <qfiledialog.h>
#include<qmessagebox.h>


/// Constructor
/// define the GUI Layout and init. Object myPlayer (ImgProcessor)
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{


    ui->setupUi(this);
    ui->frameSlider->setEnabled(false);///< frameSlider is the Slider to control the frame number of the video
    ui->firstSlider->setEnabled(false);///< firstSlider is the Slider to control the Contrast of the video
    ui->morph_elem->setEnabled(false);///< morph_elem is the Slider to control the Shapes for processing image
    ui->morph_size->setEnabled(false);///< morph_size is the Slider to control the isolation value
    ui->erosion_size->setEnabled(false);///< erosion_size is the Slider to control the erode value
    ui->morph_size2->setEnabled(false);///< morph_size2 is the Slider to control the noise reduction
    ui->conf->setEnabled(false);///< conf defines the button Save conf.
    //ui->log->setEnabled(false);
    ui->radius1->setEnabled(false);///< radius1 is the Slider to control the lower bound of the radius
    ui->radius2->setEnabled(false);///< radius2 is the Slider to control the upper bound of the radius
    ui->logs->setEnabled(false);

    myPlayer= new imgProcessor();
    ui->label->setToolTip("The lower bound of the Radius interval");
    ui->label_2->setToolTip("Control the contrast of the image");
    ui->label_3->setToolTip("The shapes for processing image\n0:Rectangular box\n1:Ellipse\n2:Cross");
    ui->label_4->setToolTip("Isolate the pupil");
    ui->label_5->setToolTip("Lower the noise on the pupil");
    ui->label_6->setToolTip("Fill the pupil, in order to eliminate the interspace");
    ui->label_7->setToolTip("The upper bound of the Radius interval");



    QObject::connect(myPlayer,SIGNAL(processedImage(QImage,QImage)),this,SLOT(updatePlayerUI(QImage,QImage)));///< communicate with the thread imgProcessor, update the UI according to the result of the imgProcessor.


   QObject::connect(myPlayer,SIGNAL(videofinish(int)),this,SLOT(batchLog()));///< if video is finished,the GUI part of this programm should stop the playback
   QObject::connect(ui->logs,SIGNAL(released()),this,SLOT(batchLog()));///< Save Multiple Logs button released means the function batchLog() should excuted.

}

Dialog::~Dialog()
{
    delete ui;
}

/**
 * @brief Dialog::updatePlayerUI
 * this function shows the result of the thread imgProcessor on to the GUI.
 * @param input1 is the source image of the video
 * @param input2 is the result image from imgProcessor
 */
void Dialog::updatePlayerUI(QImage input1,QImage input2){

    if(myPlayer->getshowProgress()){

        progressDlg->setValue(myPlayer->getCurrentFrame());
                qDebug()<<QString::number(myPlayer->getCurrentFrame());
                    if(progressDlg->wasCanceled())
                       { myPlayer->setshowProgress(false);
                        myPlayer->Stop();
                        progressDlg->close();
                        delete progressDlg;                ///< this part controls the popup progress bar
                        }

    }
    //else{progressDlg->close();}
    if(!myPlayer->isStopped()){

        ui->stop->setText("Stop");


    }
    else
    {ui->stop->setText("Continue");

    }


    ui->source->setPixmap(QPixmap::fromImage(input1).scaled(ui->source->size(),Qt::KeepAspectRatio,Qt::FastTransformation));
    ui->result->setPixmap(QPixmap::fromImage(input2).scaled(ui->result->size(),Qt::KeepAspectRatio,Qt::FastTransformation));
    ui->frameSlider->setValue(myPlayer->getCurrentFrame());
    ui->frameEdit->setText(QString::number(myPlayer->getCurrentFrame()));


    ui->firstSlider->setValue(myPlayer->getfirstValue());
    ui->firstEdit->setText(QString::number(myPlayer->getfirstValue()));

    ui->morph_elem->setValue(myPlayer->getmorph_elem());
    ui->elementEdit->setText(QString::number(myPlayer->getmorph_elem()));

    ui->morph_size->setValue(myPlayer->getmorph_size());
    ui->ele1Edit->setText(QString::number(myPlayer->getmorph_size()));

    ui->morph_size2->setValue(myPlayer->getmorph_size2());
    ui->ele2Edit->setText(QString::number(myPlayer->getmorph_size2()));

    ui->erosion_size->setValue(myPlayer->geterosion_size());
    ui->erosionEdit->setText(QString::number(myPlayer->geterosion_size()));

    ui->radius1->setValue(myPlayer->getRadiu1());
    ui->radius1Edit->setText(QString::number(myPlayer->getRadiu1()));

    ui->radius2->setValue(myPlayer->getRadiu2());
    ui->radius2Edit->setText(QString::number(myPlayer->getRadiu2()));


    ui->txtoutput->appendPlainText("Possible radius range: "+QString::number(myPlayer->getPossibleRadius()));
   // myPlayer->framedelete();//frame wieder frei geben


}


/**
 * @brief Dialog::on_load_clicked
 * this function is used to load a video file or multiple video files from the file system
 */
void Dialog::on_load_clicked()
{
  myPlayer->setsaveLog(false);
  myPlayer->setshowProgress(false);
  batch=false;
  QString path = QFileDialog::getOpenFileName(this,tr("Open Video"),".",tr("Video Files (*.avi *.mpg *.mp4)"));
  if(path.isEmpty()){

      QMessageBox::information(NULL,QString("Notice"),QString("At least one Video file should be selected!"));///< when no files was choosed,programm should noticed
      return;


  }
  else
  {string filename = path.toStdString();
  qDebug()<<path;

     string settings;
     ifstream readconf;
     int firstc=0;
     int secondc=0;
     int thirdc=0;
     int fourthc=0;///< firstc to sevenc are used to save the value which are read from the conf file
     int fifthc=0;
     int sixc=0;
     int sevenc=0;
     Point p1;
     Point p2;

     int indexconf=0;///< indicats the line number in the conf. file indexconf=0 means the first line and the value of the firstline should be saved in firstc
     string confname = path.toStdString();
     readconf.open(confname+"_conf.txt");

     if(readconf.good()){
     while(getline(readconf,settings)){
        // qDebug()<<QString::fromStdString(word);
         if(indexconf==0){firstc=stoi(settings);}
         if(indexconf==1){secondc=stoi(settings);}
         if(indexconf==2){thirdc=stoi(settings);}
         if(indexconf==3){fourthc=stoi(settings);}
         if(indexconf==4){fifthc=stoi(settings);}
         if(indexconf==5){sixc=stoi(settings);}
         if(indexconf==6){sevenc=stoi(settings);}
         if(indexconf == 7){p1.x=stoi(settings);}
         if(indexconf == 8){p1.y=stoi(settings);}
         if(indexconf == 9){p2.x=stoi(settings);}
         if(indexconf == 10){p2.y=stoi(settings);}
         indexconf++;

     }
     myPlayer->setfirstValue(firstc);
     myPlayer->setmorph_elem(secondc);
     myPlayer->setmorph_size(thirdc);
     myPlayer->setmorph_size2(fourthc);
     myPlayer->seterosion_size(fifthc);
     myPlayer->setRadius1(sixc);
     myPlayer->setRadius2(sevenc);///< reading the conf. value from the videos conf.file and set the slider bar accordingly.
     myPlayer->setllftc(p1);
     myPlayer->seturgtc(p2);
     readconf.close();}


  else{


  ///
  //QFileInfo name = filename;
  myPlayer->setfirstValue(0);
  myPlayer->setmorph_elem(1);//tot legen
  myPlayer->setmorph_size(0);
  myPlayer->setmorph_size2(0);
  myPlayer->seterosion_size(0);
  myPlayer->setRadius1(0);
  myPlayer->setRadius2(300);
  myPlayer->setllftc(myPlayer->sp1);
  myPlayer->seturgtc(myPlayer->sp2);}
  myPlayer->setsaveLog(false);
  myPlayer->loadVideo(filename);
  this->setWindowTitle(path);
              //ui->pushButton_2->setEnabled(true);
  ////////////////////////////////////////////////////////////////////////
              ui->frameSlider->setEnabled(true);
              ui->frameSlider->setMaximum(myPlayer->getNumberOfFrames());
              ui->frameSlider->setToolTip(QString::number(myPlayer->getNumberOfFrames()));
              //ui->frameEdit->setText(QString::number(myPlayer->getCurrentFrame()) );
  ////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
              ui->firstSlider->setEnabled(true);
              ui->firstSlider->setMaximum(255);
              ui->firstSlider->setToolTip("255");
/////////////////////////////////////////////////////////////////////////////
              ui->morph_elem->setEnabled(false);
              ui->morph_elem->setMaximum(2);
              ui->morph_elem->setToolTip("2");

///////////////////////////////////////////////////////////////////////////////
              ui->morph_size->setEnabled(true);
              ui->morph_size->setMaximum(21);
              ui->morph_size->setToolTip("21");

              ui->erosion_size->setEnabled(true);
              ui->erosion_size->setMaximum(21);
              ui->erosion_size->setToolTip("21");

              ui->morph_size2->setEnabled(true);
              ui->morph_size2->setMaximum(21);
              ui->morph_size2->setToolTip("21");

              ui->radius1->setEnabled(true);
              ui->radius1->setMaximum(300);
              ui->radius1->setToolTip("300");

              ui->radius2->setEnabled(true);
              ui->radius2->setMaximum(300);
              ui->radius2->setToolTip("300");

qDebug()<<"here";
  /////////////////////////////////////////////////////////////////////////
  myPlayer->Play();

  ui->conf->setEnabled(true);}
  //ui->log->setEnabled(true);
}

void Dialog::on_frameSlider_sliderPressed()
{
     myPlayer->Stop();
}

void Dialog::on_frameSlider_sliderReleased()
{
     myPlayer->Play();
}

void Dialog::on_frameSlider_sliderMoved(int position)
{
    myPlayer->setCurrentFrame(position);
        ui->frameEdit->setText( QString::number(myPlayer->getCurrentFrame()) );
}
///////////////////////////////////////////////////////////////////////////////////
void Dialog::on_firstSlider_sliderMoved(int position)
{
    myPlayer->setfirstValue(position);
    ui->firstEdit->setText(QString::number(myPlayer->getfirstValue()));
}

void Dialog::on_firstSlider_sliderPressed()
{
     myPlayer->Stop();
}

void Dialog::on_firstSlider_sliderReleased()
{
    myPlayer->Play();
}


//////////////////////////////////////////////////////////////////////
void Dialog::on_morph_size_sliderMoved(int position)
{
    myPlayer->setmorph_size(position);
    ui->ele1Edit->setText(QString::number(myPlayer->getmorph_size()));
}

void Dialog::on_morph_size_sliderPressed()
{
    myPlayer->Stop();
}

void Dialog::on_morph_size_sliderReleased()
{
    myPlayer->Play();
}
///////////////////////////////////////////////////////////////////////
void Dialog::on_morph_size2_sliderMoved(int position)
{
    myPlayer->setmorph_size2(position);
    ui->ele2Edit->setText(QString::number(myPlayer->getmorph_size2()));
}

void Dialog::on_morph_size2_sliderPressed()
{
    myPlayer->Stop();
}

void Dialog::on_morph_size2_sliderReleased()
{
     myPlayer->Play();
}
////////////////////////////////////////////////////////////////////////
void Dialog::on_erosion_size_sliderMoved(int position)
{
    myPlayer->seterosion_size(position);
    ui->erosionEdit->setText(QString::number(myPlayer->geterosion_size()));
}

void Dialog::on_erosion_size_sliderPressed()
{
    myPlayer->Stop();
}

void Dialog::on_erosion_size_sliderReleased()
{
    myPlayer->Play();
}
//////////////////////////////////////////////////////////////////////////////



void Dialog::on_morph_elem_sliderMoved(int position)
{
    myPlayer->setmorph_elem(position);
    ui->elementEdit->setText(QString::number(myPlayer->getmorph_elem()));
}

void Dialog::on_morph_elem_sliderPressed()
{
    myPlayer->Stop();
}

void Dialog::on_morph_elem_sliderReleased()
{
   myPlayer->Play();

}

void Dialog::on_conf_clicked()
{
    myPlayer->Stop();
   // ui->log->setEnabled(true);
    QMessageBox message(QMessageBox::Warning,
                        "Notice","Really to save the Configuration File? You can still change it later",
                        QMessageBox::Yes|QMessageBox::No,
                        NULL);

    if(message.exec()==QMessageBox::Yes){

     myFile = new fileWriter();
     myFile->writeConf(myPlayer->getName(),
                       myPlayer->getfirstValue(),
                       myPlayer->getmorph_elem(),
                       myPlayer->getmorph_size(),
                       myPlayer->getmorph_size2(),
                       myPlayer->geterosion_size(),
                       myPlayer->getRadiu1(),
                       myPlayer->getRadiu2(),
                       myPlayer->getllftc(),
                       myPlayer->geturgtc());
         myPlayer->Play();
    //ui->log->setEnabled(true);
    }
    else
    {
        myPlayer->Play();
    }


}

//void Dialog::on_log_clicked()
//{

//   int first=0;
//   int second=0;
//   int third=0;
//   int fourth=0;
//   int fifth=0;
//   int six=0;
//   int seven=0;
//   int index=0;
//   ifstream reading;


//   reading.open(myPlayer->getName()+"_conf.txt");
//   qDebug()<<QString::fromStdString(myPlayer->getName());
//   string word;
//   while(getline(reading,word)){
//       qDebug()<<QString::fromStdString(word);
//       if(index==0){first=stoi(word);}
//      if(index==1){second=stoi(word);}
//     if(index==2){third=stoi(word);}
//       if(index==3){fourth=stoi(word);}
//       if(index==4){fifth=stoi(word);}
//       if(index==5){six=stoi(word);}
//       if(index==6){seven=stoi(word);}
//       index++;

//   }
//   reading.close();
//   myPlayer->setCurrentFrame(1);
//   myPlayer->setfirstValue(first);
//   myPlayer->setmorph_elem(second);
//   myPlayer->setmorph_size(third);
//   myPlayer->setmorph_size2(fourth);
//   myPlayer->seterosion_size(fifth);
//   myPlayer->setRadius1(six);
//   myPlayer->setRadius2(seven);
//   //////////////////////////////////////////////////////////////////



//   //////////////////////////////////////////////////////////////////
//   myPlayer->setsaveLog(true);
//   myPlayer->setshowProgress(true);
//   if(myPlayer->isStopped())
//  {myPlayer->Play();}
//           progressDlg = new QProgressDialog();
//           progressDlg->setWindowModality(Qt::WindowModal);
//           progressDlg->setMinimumDuration(5);
//           progressDlg->setWindowTitle(tr("please wait"));
//           progressDlg->setLabelText(tr("Writing journal files......      "));
//           progressDlg->setCancelButtonText(tr("Cancel"));
//           progressDlg->setRange(0,myPlayer->getNumberOfFrames());
//           progressDlg->show();



//   ui->txtoutput->appendPlainText("Loading video with the correct parameters and start saving the jounal file");

//   //myFile->writeTitle(myPlayer->getName());


//}


void Dialog::on_radius1_sliderMoved(int position)
{
    myPlayer->setRadius1(position);
    ui->radius1Edit->setText(QString::number(myPlayer->getRadiu1()));
}

void Dialog::on_radius1_sliderPressed()
{
    myPlayer->Stop();
}

void Dialog::on_radius1_sliderReleased()
{
    myPlayer->Play();
}

void Dialog::on_radius2_sliderMoved(int position)
{
    myPlayer->setRadius2(position);
    ui->radius2Edit->setText(QString::number(myPlayer->getRadiu2()));
}

void Dialog::on_radius2_sliderPressed()
{
    myPlayer->Stop();
}

void Dialog::on_radius2_sliderReleased()
{
    myPlayer->Play();
}



void Dialog::on_multipleLoad_clicked()
{
    ui->logs->setEnabled(true);
    batch=true;
    QStringList pathList = QFileDialog::getOpenFileNames(this,tr("Select only configured Video files"),".",tr("Video Files (*.avi *.mpg *.mp4)"));
    list=pathList;

    i=0;
    ui->txtoutput->appendPlainText("The following files are chosen: ");
    for(int j=0;j<pathList.length();j++){

        ui->txtoutput->appendPlainText(pathList[j]);





    }


}
/**
 * @brief Dialog::batchLog
 * this function is used to process the batch operations,so that the programm can run multiple files automatically.
 */
void Dialog::batchLog(){


    if(batch){

    if(i<list.length()){
    int first=0;
    int second=0;
    int third=0;
    int fourth=0;
    int fifth=0;
    int six=0;
    int seven=0;
    int index=0;
    Point p1;
    Point p2;
    string word;
    ifstream reading;





       string filename = list[i].toStdString();
       reading.open(filename+"_conf.txt");
       if(reading.good()){
       while(getline(reading,word)){
           qDebug()<<QString::fromStdString(word);
           if(index==0){first=stoi(word);}
          if(index==1){second=stoi(word);}
         if(index==2){third=stoi(word);}
           if(index==3){fourth=stoi(word);}
           if(index==4){fifth=stoi(word);}
           if(index==5){six=stoi(word);}
           if(index==6){seven=stoi(word);}
           if(index == 7){p1.x=stoi(word);}
           if(index == 8){p1.y=stoi(word);}
           if(index == 9){p2.x=stoi(word);}
           if(index == 10){p2.y=stoi(word);}
           index++;

       }
       reading.close();


       myPlayer->setfirstValue(first);
       myPlayer->setmorph_elem(second);
       myPlayer->setmorph_size(third);
       myPlayer->setmorph_size2(fourth);
       myPlayer->seterosion_size(fifth);
       myPlayer->setRadius1(six);
       myPlayer->setRadius2(seven);
       myPlayer->setllftc(p1);
       myPlayer->seturgtc(p2);
       myPlayer->setsaveLog(true);
       myPlayer->loadVideo(filename);
       this->setWindowTitle(QString::fromStdString(filename));
       ui->frameSlider->setEnabled(true);
       ui->frameSlider->setMaximum(myPlayer->getNumberOfFrames());
        qDebug()<<QString::fromStdString(filename);

        ui->firstSlider->setMaximum(255);


        ui->morph_elem->setMaximum(2);


        ui->morph_size->setMaximum(21);


        ui->erosion_size->setMaximum(21);


        ui->morph_size2->setMaximum(21);


        ui->radius1->setMaximum(300);


        ui->radius2->setMaximum(300);



        myPlayer->Play();
        myPlayer->setshowProgress(true);
                qDebug()<<QString::number(myPlayer->getNumberOfFrames());
                progressDlg = new QProgressDialog();
                progressDlg->setWindowModality(Qt::WindowModal);
                progressDlg->setMinimumDuration(5);
                progressDlg->setWindowTitle(tr("please wait"));
                progressDlg->setLabelText(tr("Writing journal files......      "));
                progressDlg->setCancelButtonText(tr("Cancel"));
                progressDlg->setMaximum(myPlayer->getNumberOfFrames());
                progressDlg->setRange(1,myPlayer->getNumberOfFrames());
                progressDlg->show();


         i++;
         //myPlayer->capturedelete();//hier
        //QApplication::processEvents();


 }
       else
       {ui->txtoutput->appendPlainText("The chosen Video: "+ QString::fromStdString(filename)+ " does not have the conf file Please configure the Video first");

        QMessageBox::information(NULL,QString("Notice"),QString(QString::fromStdString(filename)+" does not have the conf file, Please configure the Video first"));

       }

}
    else{
     myPlayer->setshowProgress(false);
     QMessageBox::information(NULL,QString("Notice"),QString("All Videos are finished"));


    }}



    else
    {
        QMessageBox::information(NULL,QString("Notice"),QString("Video is finished"));}
}

void Dialog::on_logs_clicked()
{
   //batch=true;

}



void Dialog::on_stop_clicked()
{
    if(myPlayer->isStopped()){

        ui->stop->setText("Stop");
        myPlayer->Play();

    }
    else
    {ui->stop->setText("Continue");
     myPlayer->Stop();
    }
}

void Dialog::on_frameEdit_textEdited(const QString &arg1)
{
    qDebug()<<arg1
              ;

    if(ui->frameEdit->hasFocus()){
        myPlayer->Stop();
        ui->stop->setText("continue");
    }

    myPlayer->setCurrentFrame(arg1.toInt());
}








void Dialog::on_firstEdit_textEdited(const QString &arg1)
{
    if(ui->firstEdit->hasFocus()){
        myPlayer->Stop();
        ui->stop->setText("continue");
    }

    myPlayer->setfirstValue(arg1.toInt());
}

void Dialog::on_elementEdit_textEdited(const QString &arg1)
{
    if(ui->elementEdit->hasFocus()){
        myPlayer->Stop();
        ui->stop->setText("continue");
    }
    if(arg1.toInt()<3){
    myPlayer->setmorph_elem(arg1.toInt());}
    else
    {
        QMessageBox::critical(this,"Warning","You can only choose a number between 0 and 2",QMessageBox::Ok);
    }
}

void Dialog::on_ele1Edit_textEdited(const QString &arg1)
{
    if(ui->ele1Edit->hasFocus()){
        myPlayer->Stop();
        ui->stop->setText("continue");
    }

    myPlayer->setmorph_size(arg1.toInt());
}

void Dialog::on_ele2Edit_textEdited(const QString &arg1)
{
    if(ui->ele2Edit->hasFocus()){
        myPlayer->Stop();
        ui->stop->setText("continue");
    }

    myPlayer->setmorph_size2(arg1.toInt());
}

void Dialog::on_erosionEdit_textEdited(const QString &arg1)
{
    if(ui->erosionEdit->hasFocus()){
        myPlayer->Stop();
        ui->stop->setText("continue");
    }

    myPlayer->seterosion_size(arg1.toInt());
}

void Dialog::on_radius1Edit_textEdited(const QString &arg1)
{
    if(ui->radius1Edit->hasFocus()){
        myPlayer->Stop();
        ui->stop->setText("continue");
    }

    myPlayer->setRadius1(arg1.toInt());
}

void Dialog::on_radius2Edit_textEdited(const QString &arg1)
{
    if(ui->radius2Edit->hasFocus()){
        myPlayer->Stop();
        ui->stop->setText("continue");
    }

    myPlayer->setRadius2(arg1.toInt());
}

void Dialog::on_left_clicked(){

    Point llft = myPlayer->getllftc();
    Point urgt = myPlayer->geturgtc();
    llft.x=llft.x-10;
    urgt.x=urgt.x-10;
    myPlayer->setllftc(llft);
    myPlayer->seturgtc(urgt);

}

void Dialog::on_right_clicked(){
Point llft = myPlayer->getllftc();
Point urgt = myPlayer->geturgtc();
llft.x=llft.x+10;
urgt.x=urgt.x+10;
myPlayer->setllftc(llft);
myPlayer->seturgtc(urgt);
}

void Dialog:: on_up_clicked(){
    Point llft = myPlayer->getllftc();
    Point urgt = myPlayer->geturgtc();
    llft.y=llft.y-10;
    urgt.y=urgt.y-10;
    myPlayer->setllftc(llft);
    myPlayer->seturgtc(urgt);
}

void Dialog:: on_down_clicked(){
    Point llft = myPlayer->getllftc();
    Point urgt = myPlayer->geturgtc();
    llft.y=llft.y+10;
    urgt.y=urgt.y+10;
    myPlayer->setllftc(llft);
    myPlayer->seturgtc(urgt);
}

void Dialog:: on_bigger_clicked(){
    Point llft = myPlayer->getllftc();
    Point urgt = myPlayer->geturgtc();
    llft.x=llft.x-10;
    urgt.x=urgt.x+10;
    llft.y=llft.y-10;
    urgt.y=urgt.y+10;
    myPlayer->setllftc(llft);
    myPlayer->seturgtc(urgt);
}

void Dialog:: on_smaller_clicked(){
    Point llft = myPlayer->getllftc();
    Point urgt = myPlayer->geturgtc();
    llft.x=llft.x+10;
    urgt.x=urgt.x-10;
    llft.y=llft.y+10;
    urgt.y=urgt.y-10;
    myPlayer->setllftc(llft);
    myPlayer->seturgtc(urgt);
}
