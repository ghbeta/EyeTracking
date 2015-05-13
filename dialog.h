#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "imgProcessor.h"
#include "fileWriter.h"
#include <QTime>
#include<qprogressdialog.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    QStringList list;
    int i;
    bool batch;



public slots:
    void updatePlayerUI(QImage input1,QImage input2);
    void batchLog();



private slots:
    void on_load_clicked();
/////////////////////////////////////////////////
    void on_frameSlider_sliderPressed();

    void on_frameSlider_sliderReleased();

    void on_frameSlider_sliderMoved(int position);
///////////////////////////////////////////////////////
    void on_firstSlider_sliderMoved(int position);

    void on_firstSlider_sliderPressed();

    void on_firstSlider_sliderReleased();
//////////////////////////////////////////////////////////
    void on_morph_size_sliderMoved(int position);

    void on_morph_size_sliderPressed();

    void on_morph_size_sliderReleased();
/////////////////////////////////////////////////////////
    void on_morph_size2_sliderMoved(int position);

    void on_morph_size2_sliderPressed();

    void on_morph_size2_sliderReleased();
//////////////////////////////////////////////////////////////
    void on_erosion_size_sliderMoved(int position);

    void on_erosion_size_sliderPressed();

    void on_erosion_size_sliderReleased();

///////////////////////////////////////////////////////////



    void on_morph_elem_sliderMoved(int position);

    void on_morph_elem_sliderPressed();

    void on_morph_elem_sliderReleased();

    void on_conf_clicked();

//    void on_log_clicked();



    void on_radius1_sliderMoved(int position);

    void on_radius1_sliderPressed();

    void on_radius1_sliderReleased();

    void on_radius2_sliderMoved(int position);

    void on_radius2_sliderPressed();

    void on_radius2_sliderReleased();



    void on_multipleLoad_clicked();

    void on_logs_clicked();



    void on_stop_clicked();

    void on_frameEdit_textEdited(const QString &arg1);









    void on_firstEdit_textEdited(const QString &arg1);

    void on_elementEdit_textEdited(const QString &arg1);

    void on_ele1Edit_textEdited(const QString &arg1);

    void on_ele2Edit_textEdited(const QString &arg1);

    void on_erosionEdit_textEdited(const QString &arg1);

    void on_radius1Edit_textEdited(const QString &arg1);

    void on_radius2Edit_textEdited(const QString &arg1);

    ///////////////////////////////////////////////////////////
    void on_left_clicked();
    void on_right_clicked();
    void on_up_clicked();
    void on_down_clicked();
    void on_bigger_clicked();
    void on_smaller_clicked();

private:
    Ui::Dialog *ui;
    imgProcessor* myPlayer;
    fileWriter* myFile;
    QProgressDialog* progressDlg;

};

#endif // DIALOG_H
