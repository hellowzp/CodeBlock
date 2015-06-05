#ifndef CONTROLVIEW_H
#define CONTROLVIEW_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>

class MainWindow;

class ControlView : public QWidget
{
    Q_OBJECT

public:
    explicit ControlView(MainWindow *mainWindow);
    ~ControlView();

private:
    MainWindow* mainWindow;

    QSlider* hSlider;
    QSlider* fpsSlider;

    QPushButton* pause;
    QPushButton* stop;
    QCheckBox* itrct;

    bool paused;
    bool stopped;
    bool interactive;

    friend class MainWindow;

signals:
    void pauseChanged(bool);
    void stopChanged(bool);
    void interactChanged(bool);

public slots:
    void pauseClicked() {
        paused = !paused;
        pause->setText(paused?"Start":"Pause");
        emit pauseChanged(paused);

        stop->setEnabled(true);
    }

    void stopClicked() {
        stopped = !stopped;
        this->stop->setText(stopped?"Restart":"Stop");
        emit stopChanged(stopped);

        if(stopped) {
            pause->setDisabled(true);
            hSlider->setDisabled(true);
            fpsSlider->setDisabled(true);
        }else{
            pause->setEnabled(true);
            hSlider->setEnabled(true);
            fpsSlider->setEnabled(true);
        }
    }

    void itrctClicked(){
        interactive = !interactive;
        emit interactChanged(interactive);

        pause->setDisabled(interactive);
        if(!interactive) {
            pause->setText("Start");
            paused = true;
        }
    }

    void resetControls(){
        hSlider->setValue(10);
        fpsSlider->setValue(3);

        itrct->setChecked(false);
        pause->setText("Start");
        pause->setEnabled(true);
        paused = true;
        stop->setText("Stop");
        stopped = false;
        stop->setDisabled(true);
    }

};

#endif // CONTROLVIEW_H
