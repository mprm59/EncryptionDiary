#ifndef DearDiary_H
#define DearDiary_H

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui {
class DearDiary;
}

class DearDiary : public QMainWindow
{
    Q_OBJECT

public:
    explicit DearDiary(QWidget *parent = 0);
    ~DearDiary();

private slots:
    void on_pushButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_lineEdit_textChanged();

    void viewDecryption();

    void on_encrypt_new_clicked();

    void on_lineEdit_2_textChanged();
    void on_lineEdit_3_textChanged();
    void on_lineEdit_4_textChanged();

    void on_EncryptButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::DearDiary *ui;
    QString filename;
    QString password;
    QString encryptingfilename;
    
    unsigned int Sbox[8][16] =
    {
        {4,10,9,2,13,8,0,14,6,11,1,12,7,15,5,3},
        {14,11,4,12,6,13,15,10,2,3,8,1,0,7,5,9},
        {5,8,1,13,10,3,4,2,14,15,12,7,6,0,9,11},
        {7,13,10,1,0,8,9,15,14,4,6,12,11,2,5,3},
        {6,12,7,1,5,15,13,8,4,10,9,14,0,3,11,2},
        {4,11,10,0,7,2,1,13,3,6,8,5,9,12,15,14},
        {13,11,4,1,3,15,5,9,0,10,14,7,6,8,2,12},
        {1,15,13,0,5,7,10,4,9,2,3,14,6,11,8,12}
    };
};

#endif // DearDiary_H
