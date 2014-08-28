#ifndef QTGLWEBCAMDEMO_H
#define QTGLWEBCAMDEMO_H

#include <QMainWindow>

#include <opencv2/highgui/highgui.hpp>

namespace Ui {
    class QtGLWebcamDemo;
}

class QtGLWebcamDemo : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtGLWebcamDemo(QWidget *parent = 0);
    ~QtGLWebcamDemo();

private slots:
    void on_actionStart_triggered();

private:
    Ui::QtGLWebcamDemo *ui;

    cv::VideoCapture mCapture;

protected:
    void timerEvent(QTimerEvent *event);
};

#endif // QTGLWEBCAMDEMO_H
