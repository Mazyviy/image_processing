#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QFileDialog>
#include <math.h>
#include <map>
#include <QMessageBox>

// показывает был ли выбор цвета на изображении (double click)
bool click_flag = false;
// цвет rgb при double click
float red_click, green_click, blue_click;
QString filename_imgOrigin;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_color_block->setVisible(false);
    ui->color_block->setVisible(false);
    ui->doubleSpinBox->setVisible(false);
    ui->spinBox->setValue(40);
    ui->spinBox->setMaximum(100);
    ui->spinBox->setMinimum(0);
    ui->spinBox->setSingleStep(5);
    ui->statusbar->addPermanentWidget(ui->progressBar);
    ui->progressBar->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// открывает файл и записывает имя изображения в filename_imgOrigin
// также создает папку в Pictures и сохраняет исходное изображение
// выводит изображение в окно
void MainWindow::on_buttonOpen_clicked()
{
    ui->label_color_block->clear();
    ui->color_block->setStyleSheet("");
    ui->label_img_left->clear();
    ui->label_img_right->clear();
    // создание папки если ее нет
    QDir dir(QDir::homePath()+"/Pictures/img_application");
    if (!dir.exists()) {
        QDir().mkpath(QDir::homePath()+"/Pictures/img_application");
    }
    QString filename = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath()+"/Pictures", "Images (*.jpg *.jpeg *.bmp)");
    if(!filename.isEmpty()) {
        filename_imgOrigin = filename;
        click_flag = false;
        ui->statusbar->showMessage(filename_imgOrigin,3000);
        QImage img = QImage(filename_imgOrigin);
        img.save(QDir::homePath()+"/Pictures/img_application/load.jpg");
        ui->statusbar->showMessage("Saved as "+QDir::homePath()+"/Pictures/img_application/load_origin.jpg", 3000);
        // вывод изображения в label
        QPixmap pix1 = QPixmap(filename_imgOrigin);
        pix1 = pix1.scaled(ui->label_img_left->width(), ui->label_img_left->height() ,Qt::KeepAspectRatio, Qt::FastTransformation);
        ui->label_img_left->setPixmap(pix1);
        ui->label_img_left->setScaledContents(false);
    }
    else {
        if(!filename_imgOrigin.isEmpty()) {
            click_flag = false;
            QMessageBox::warning(this, "Изображение не выбрано", "Новое изображение не выбрано");
            QPixmap pix1 = QPixmap(filename_imgOrigin);
            // если новое изображение не выбрано, выводит прежнее
            pix1 = pix1.scaled(ui->label_img_left->width(), ui->label_img_left->height() ,Qt::KeepAspectRatio, Qt::FastTransformation);
            ui->label_img_left->setPixmap(pix1);
            ui->label_img_left->setScaledContents(false);
        }
        else {
            click_flag = false;
            QMessageBox::warning(this, "Изображение не выбрано", "Изображение не выбрано!\nВыберите изображение");     
        }
    }
}

// выбирает rgb цвет на изображении и записывает значения в red_click, green_click, blue_click
void MainWindow::mouseDoubleClickEvent(QMouseEvent *)
{
    int index = ui->comboBox->currentIndex();
    if(index!=3 && ui->label_img_left->isVisible()) {
        ui->label_color_block->clear();
        ui->color_block->setStyleSheet("");
        // позиция курсора
        QPoint p = ui->label_img_left->mapFromGlobal(QCursor::pos());
        QImage img = QImage(filename_imgOrigin);
        img = img.scaled(ui->label_img_left->width(), ui->label_img_left->height(), Qt::KeepAspectRatio, Qt::FastTransformation);
        if(p.x()>img.width() || p.x()<0 || p.y()>img.height() || p.y()<0) {
            ui->label_color_block->setVisible(false);
            ui->color_block->setVisible(false);
            if (filename_imgOrigin.isEmpty()) QMessageBox::warning(this, "Изображение не выбрано", "Изображение не выбрано!\nВыберите изображение");
            else QMessageBox::critical(this,"Цвет не выбран", "Выберите цвет на изображении слева");
        }
        else {
            click_flag = true;
            QRgb rgblue_click = img.pixel(p.x(), p.y());
            red_click = qRed(rgblue_click);
            green_click = qGreen(rgblue_click);
            blue_click = qBlue(rgblue_click);
            ui->label_color_block->setVisible(true);
            ui->label_color_block->setText("Выбранный цвет:");
            ui->color_block->setVisible(true);
            ui->color_block->setStyleSheet(QString("background-color: %1").arg("rgb(" + QString::number(red_click) + "," + QString::number(green_click) + "," + QString::number(blue_click) + ")"));
        }
    }
}

// обрабатывает изображение
void MainWindow::on_buttonSearch_clicked()
{
    int index = ui->comboBox->currentIndex();
    int limitInt = ui->spinBox->value();
    double limitDouble = ui->doubleSpinBox->value();
    if(!filename_imgOrigin.isEmpty()) {
        QImage img = QImage(filename_imgOrigin);
        // евклидова метрика
        if(index==0) {
            if(click_flag==true) {
                for(int i=0; i<img.width(); i++)
                {
                    for(int j=0; j<img.height(); j++)
                    {
                        int r = qRed(img.pixel(i, j));
                        int g = qGreen(img.pixel(i, j));
                        int b = qBlue(img.pixel(i, j));

                        double distance = sqrt(pow(r-red_click,2)+pow(g-green_click,2)+pow(b-blue_click,2));
                        if(distance<limitInt) {
                            img.setPixel(i, j, qRgb(0, 255, 0));
                        }
                    }
                }
                QString filename_imgScaledModify = QDir::homePath()+"/Pictures/img_application/load_modify_em.jpg";
                img.save(filename_imgScaledModify);
                ui->statusbar->showMessage("Saved as " + QDir::homePath() + "/Pictures/img_application/load_modify_em.jpg", 3000);
                QPixmap pix2 = QPixmap(filename_imgScaledModify);
                ui->label_img_right->setPixmap(pix2.scaled(ui->label_img_left->width(), ui->label_img_left->height(), Qt::KeepAspectRatio, Qt::FastTransformation));
                ui->label_img_right->setScaledContents(false);
            }
            else QMessageBox::critical(this,"Цвет не выбран", "Выберите цвет на изображении слева");
        }
        // манхеттова метрика
        if(index==1) {
            if(click_flag==true) {
                for(int i=0; i<img.width(); i++)
                {
                    for(int j=0; j<img.height(); j++)
                    {
                        int r = qRed(img.pixel(i, j));
                        int g = qGreen(img.pixel(i, j));
                        int b = qBlue(img.pixel(i, j));

                        double distance = abs(r-red_click)+abs(g-green_click)+abs(b-blue_click);
                        if(distance<limitInt) {
                            img.setPixel(i, j, qRgb(0, 255, 0));
                        }
                    }
                }
                QString filename_imgScaledModify = QDir::homePath()+"/Pictures/img_application/load_modify_mm.jpg";
                img.save(filename_imgScaledModify);
                ui->statusbar->showMessage("Saved as " + QDir::homePath() + "/Pictures/img_application/load_modify_mm.jpg", 3000);
                QPixmap pix2 = QPixmap(filename_imgScaledModify);
                ui->label_img_right->setPixmap(pix2.scaled(ui->label_img_left->width(), ui->label_img_left->height(), Qt::KeepAspectRatio, Qt::FastTransformation));
                ui->label_img_right->setScaledContents(false);
            }
            else QMessageBox::critical(this,"Цвет не выбран", "Выберите цвет на изображении слева");
        }
        // косинус угла между векторами
        if(index==2) {
            if(click_flag==true) {
                for(int i=0; i<img.width(); i++)
                {
                    for(int j=0; j<img.height(); j++)
                    {
                        int r = qRed(img.pixel(i, j));
                        int g = qGreen(img.pixel(i, j));
                        int b = qBlue(img.pixel(i, j));

                        double distance = abs(1-((r*red_click+g*green_click+b*blue_click)/(sqrt(pow(red_click,2)+pow(green_click,2)+pow(blue_click,2))*sqrt(pow(r,2)+pow(g,2)+pow(b,2)))));
                        if(distance<limitDouble) {
                            img.setPixel(i, j, qRgb(0, 255, 0));
                        }
                    }
                }
                QString filename_imgScaledModify = QDir::homePath()+"/Pictures/img_application/load_modify_cos.jpg";
                img.save(filename_imgScaledModify);
                ui->statusbar->showMessage("Saved as "+QDir::homePath()+"/Pictures/img_application/load_modify_cos",3000);
                QPixmap pix2 = QPixmap(filename_imgScaledModify);
                ui->label_img_right->setPixmap(pix2.scaled(ui->label_img_left->width(), ui->label_img_left->height(), Qt::KeepAspectRatio, Qt::FastTransformation));
                ui->label_img_right->setScaledContents(false);
            }
            else QMessageBox::critical(this,"Цвет не выбран", "Выберите цвет на изображении слева");
        }
        // Поиск реже и чаще всего встречающегося цвета
        if(index==3) {
            ui->color_block->setStyleSheet("");
            ui->progressBar->setVisible(true);
            int max, min, count_max;
            QString str_max, str_min;
            QMap <QString,int> array_rgbAndCount;
            img = img.scaledToWidth(200, Qt::FastTransformation);

            for(int i=0; i<img.width(); i++)
            {
                for(int j=0; j<img.height(); j++)
                {
                    int r1 = qRed(img.pixel(i, j));
                    int g1 = qGreen(img.pixel(i, j));
                    int b1 = qBlue(img.pixel(i, j));
                    QString rgb=QString::number(r1)+","+ QString::number(g1)+","+QString::number(b1);
                    // записывает в array_rgbAndCount уникальный цвет и
                    // количество пикселей, которые похожи на него (Манхеттенова метрика)
                    count_max=0;
                    if(array_rgbAndCount.count(rgb)==0) {
                        for(int ii=0; ii<img.width(); ii++)
                        {
                            for(int jj=0; jj<img.height(); jj++)
                            {
                                if(i!=ii && j!=jj) {
                                    int r2 = qRed(img.pixel(ii, jj));
                                    int g2 = qGreen(img.pixel(ii, jj));
                                    int b2 = qBlue(img.pixel(ii, jj));

                                    if(abs(r2-r1)+abs(g2-g1)+abs(b2-b1)<50) count_max++;
                                }
                            }
                        }
                        array_rgbAndCount[rgb]=count_max;
                    }
                }
                ui->progressBar->setValue((i+1)*100/img.width());
            }
            ui->progressBar->setVisible(false);
            max=min=array_rgbAndCount.begin().value();
            for(auto iter=array_rgbAndCount.begin(); iter!=array_rgbAndCount.end(); iter++)
            {
               if(min>iter.value()) {
                   min = iter.value();
                   str_min = iter.key();
               }
               if(max<iter.value()) {
                   max = iter.value();
                   str_max = iter.key();
               }
            }

            ui->label_color_block->setVisible(true);
            ui->label_color_block->setText("Редкий/частый цвет:");
            ui->color_block->setVisible(true);
            QString background = "qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgb("+str_min+"),stop:0.49 rgb("+str_min+"),stop:0.5 rgb("+str_max+"), stop:1 rgb("+str_max+"))";
            ui->color_block->setStyleSheet(QString("background: %1").arg(background));
        }
    }
    else QMessageBox::warning(this, "Файл не выбран", "Файл не выбран!\nВыберите файл");
}

// очищает и скрывает элементы в окне в зависимости от выбранного элемента в comboBox
void MainWindow::on_comboBox_activated(int index)
{
    if(index==3) {
            click_flag = false;
            ui->label_color_block->clear();
            ui->label_img_right->clear();
            ui->color_block->setStyleSheet("");
            ui->doubleSpinBox->setVisible(false);
            ui->spinBox->setVisible(false);
        }
    else {
        if(click_flag==false) {
            ui->label_color_block->clear();
            ui->color_block->clear();
            ui->color_block->setStyleSheet("");
        }
        ui->label_img_right->clear();
        if(index==0) {
            ui->doubleSpinBox->setVisible(false);
            ui->spinBox->setVisible(true);
            ui->spinBox->setValue(40);
            ui->spinBox->setMaximum(100);
            ui->spinBox->setSingleStep(5);
        }
        if(index==1) {
            ui->doubleSpinBox->setVisible(false);
            ui->spinBox->setVisible(true);
            ui->spinBox->setValue(60);
            ui->spinBox->setMaximum(100);
            ui->spinBox->setSingleStep(5);
        }
        if(index==2) {
            ui->doubleSpinBox->setVisible(true);
            ui->spinBox->setVisible(false);
            ui->doubleSpinBox->setValue(0.0005);
        }
    }
}
