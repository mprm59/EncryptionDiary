#include "QFileDialog"
#include "QDate"
#include "QMessageBox"
#include "DearDiary.h"
#include "ui_DearDiary.h"
#include "QDir"
#include "iostream"

DearDiary::DearDiary(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DearDiary)
{
    ui->setupUi(this);
    QDir dir("C:\\Meow\\deardiary\\archive");
     QStringList archive = dir.entryList(QStringList("*.txt"));
     foreach (QString file, archive)
     ui->listWidget->addItem(file);
    QRegExp re( "^[A-Za-z0-9]*$" );
    QRegExpValidator *validator = new QRegExpValidator( re, 0 );
    ui->lineEdit->setValidator(validator);
    ui->lineEdit_3->setValidator(validator);
    ui->lineEdit_4->setValidator(validator);
    password = ui->lineEdit->text();

}

DearDiary::~DearDiary()
{
    delete ui;
}

void DearDiary::on_pushButton_clicked()
{
    ui->listWidget->clear();
    QDir dir("C:\\Meow\\deardiary\\archive");
     QStringList archive = dir.entryList(QStringList("*.txt"));
     foreach (QString file, archive)
     ui->listWidget->addItem(file);
}

void DearDiary::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->textEdit->clear();
    filename = "C:\\Meow\\deardiary\\archive\\" + item->text();
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QByteArray array = file.readAll();
    QString line(array);
    ui->textEdit->insertPlainText(line + "\n");
    file.close();
    viewDecryption();
}

void DearDiary::viewDecryption()
{
    ui->textEdit->clear();
    //--Ключ шифрования и его разбиение--------------------
    char s[32];
    for (unsigned int i=0; i<32; i++){
    std::string Password = password.toStdString().c_str();
        if (i<Password.length()) s[i]=Password[i];
        else s[i]='0';
    }
    unsigned int key[8];

    for(int i = 0; i<8; i++)
    {
        key[i] = (s[4*i]<<24) + (s[1+4*i]<<16) + (s[2+4*i]<<8) + s[3+4*i];  // Складывание каждых 4-х символов ключа в одно число
    }


    //--Открытие файла-------------------------------------
    FILE* f;            // Входной файл
    FILE* g;            // Выходной файл
    int fsize;          // Размер входного файла
    const char* Name = filename.toStdString().c_str();
    f = fopen(Name, "rb");

    fseek (f, 0, SEEK_END);         // Вычисление размера файла
    fsize = ftell(f);
    fseek (f, 0, SEEK_SET);

    g = fopen("TEMPDIRECTORY.txt", "wb");

    //--Основной цикл шифрования---------------------------
    while(fsize)
    {
        unsigned int A = 0;                 // Страший 32-битный блок
        unsigned int B = 0;                 // Младший 32-битный блок

        //------------------------------------
        if(fsize>=4)                    // Заполнение старшего блока...
        {
            fread(&A, 4, 1, f);         // ...символами...
            fsize -= 4;
        }
        else
        {
            fread(&A, fsize, 1, f);
            for(int i = 0; i<(4-fsize); i++)
            {
                A += (32<<(24-(i*8)));  // ... или пробелами, если символы кончились,
                fsize = 0;              //     а 64 бита еще не набралось
            }
        }
        //------------------------------------
        if(fsize>=4)                    // Заполнение младшего блока...
        {
            fread(&B, 4, 1, f);         // ...символами...
            fsize -= 4;
        }
        else
        {
            fread(&B, fsize, 1, f);
            for(int i = 0; i<(4-fsize); i++)
            {
                B += (32<<(24-(i*8)));  // ... или пробелами, если символы кончились,
                fsize = 0;              //     а 64 бита еще не набралось
            }
        }
        //------------------------------------
        unsigned int T;                 // Для промежуточных вычислений
        //--32 раунда--------------------------------------
        for(int i = 0; i<32; i++)
        {
            T = 0;

            if(i<8) T = (A+key[i%8])  % 0x100000000;    // суммирование с ключом в зависимости от раунда
            else T = (A+key[7-(i%8)]) % 0x100000000;

            unsigned int Fragments[8] =     // Разбиение на 4-битные фрагменты
            {
                (T & 0xF0000000)>>28,
                (T & 0xF000000)>>24,
                (T & 0xF00000)>>20,
                (T & 0xF0000)>>16,
                (T & 0xF000)>>12,
                (T & 0xF00)>>8,
                (T & 0xF0)>>4,
                (T & 0xF)
            };

            for(int j = 0; j<8; j++)
            {
                Fragments[j] = Sbox[j][Fragments[j]];     // Пропуск фрагментов через Sbox'ы
            }

            T = (Fragments[0]<<28) +        // Сборка фрагментов обратно в 32-битный подблок
                (Fragments[1]<<24) +
                (Fragments[2]<<20) +
                (Fragments[3]<<16) +
                (Fragments[4]<<12) +
                (Fragments[5]<< 8) +
                (Fragments[6]<< 4) +
                 Fragments[7];

            T = (T<<11)|(T>>21);            // Циклическое смещение влево на 11 бит
            T ^= B;                         // XOR с B

            if(i != 31)
            {
                B = A;                      // Перестановка подблоков 1-31 и 32 циклов
                A = T;
            }
            else
            {
                B = T;
            }
        }   //--Коненц 32 раундов---------------------------
        //--Вывод A и B в файл--------------------------
        fwrite(&A, 4, 1, g);
        fwrite(&B, 4, 1, g);
    }   // Конец файла
    fclose(f);
    fclose(g);
    QFile file("TEMPDIRECTORY.txt");
    file.open(QIODevice::ReadOnly);
    QByteArray array = file.readAll();
    QString line(array);
    ui->textEdit->insertPlainText(line + "\n");
    file.close();
    remove("TEMPDIRECTORY.txt");
}

void DearDiary::on_lineEdit_textChanged()
{
    password = ui->lineEdit->text();
    if (filename!=NULL) viewDecryption();
    }

void DearDiary::on_encrypt_new_clicked()
{
    encryptingfilename = QFileDialog::getOpenFileName(this,
         tr("Open Text File"), "C:\\Meow\\deardiary\\non encrypted examples", tr("Text Files (*.txt)"));
    ui->lineEdit_2->clear();
    ui->lineEdit_2->insert(encryptingfilename);
}


void DearDiary::on_lineEdit_2_textChanged()
{
    if (ui->lineEdit_2->text()!=NULL && ui->lineEdit_3->text()!=NULL && ui->lineEdit_4->text()!=NULL) ui->EncryptButton->setEnabled(true);
    else ui->EncryptButton->setEnabled(false);
}
void DearDiary::on_lineEdit_3_textChanged()
{
    if (ui->lineEdit_2->text()!=NULL && ui->lineEdit_3->text()!=NULL && ui->lineEdit_4->text()!=NULL) ui->EncryptButton->setEnabled(true);
    else ui->EncryptButton->setEnabled(false);
}
void DearDiary::on_lineEdit_4_textChanged()
{
    if (ui->lineEdit_2->text()!=NULL && ui->lineEdit_3->text()!=NULL && ui->lineEdit_4->text()!=NULL) ui->EncryptButton->setEnabled(true);
    else ui->EncryptButton->setEnabled(false);
}

void DearDiary::on_EncryptButton_clicked()
{
    //--Ключ шифрования и его разбиение--------------------

    char s[32];
    for (unsigned int i=0; i<32; i++){
    std::string Password = ui->lineEdit_3->text().toStdString().c_str();
        if (i<Password.length()) s[i]=Password[i];
        else s[i]='0';
    }

    unsigned int key[8];

    for(int i = 0; i<8; i++)
    {
        key[i] = (s[4*i]<<24) + (s[1+4*i]<<16) + (s[2+4*i]<<8) + s[3+4*i];  // Складывание каждых 4-х символов ключа в одно число
    }

    //--Открытие файла-------------------------------------
    FILE* f;            // Входной файл
    FILE* g;            // Выходной файл
    int fsize;          // Размер входного файла

    const char* Filename = ui->lineEdit_2->text().toStdString().c_str();
    f = fopen(Filename, "rb");

    fseek (f, 0, SEEK_END);         // Вычисление размера файла
    fsize = ftell(f);
    fseek (f, 0, SEEK_SET);
    QString path = "";
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    path = "C:\\Meow\\deardiary\\archive\\" + ui->lineEdit_4->text() + "_" + date.toString("dd.M.yy") + "-" + time.toString("hh.mm.ss") + ".txt";
    const char* c = path.toStdString().c_str();
    g = fopen(c, "wb");

        //--Основной цикл шифрования---------------------------
    while(fsize)
    {
        unsigned int A = 0;                 // Страший 32-битный блок
        unsigned int B = 0;                 // Младший 32-битный блок

        //------------------------------------
        if(fsize>=4)                    // Заполнение старшего блока...
        {
            fread(&A, 4, 1, f);         // ...символами...
            fsize -= 4;
        }
        else
        {
            fread(&A, fsize, 1, f);
            for(int i = 0; i<(4-fsize); i++)
            {
                A += (32<<(24-(i*8)));  // ... или пробелами, если символы кончились,
                fsize = 0;              //     а 64 бита еще не набралось
            }
        }

        //------------------------------------
        if(fsize>=4)                    // Заполнение младшего блока...
        {
            fread(&B, 4, 1, f);         // ...символами...
            fsize -= 4;
        }
        else
        {
            fread(&B, fsize, 1, f);
            for(int i = 0; i<(4-fsize); i++)
            {
                B += (32<<(24-(i*8)));  // ... или пробелами, если символы кончились,
                fsize = 0;              //     а 64 бита еще не набралось
            }
        }
        //------------------------------------

        unsigned int T;                 // Для промежуточных вычислений

        //--32 раунда--------------------------------------
        for(int i = 0; i<32; i++)
        {
            T = 0;

            if(i<24) T = (A+key[i%8]) % 0x100000000;    // суммирование с ключом в зависимости от раунда
            else T = (A+key[7-(i%8)]) % 0x100000000;


            unsigned int Fragments[8] =     // Разбиение на 4-битные фрагменты
            {
                (T & 0xF0000000)>>28,
                (T & 0xF000000)>>24,
                (T & 0xF00000)>>20,
                (T & 0xF0000)>>16,
                (T & 0xF000)>>12,
                (T & 0xF00)>>8,
                (T & 0xF0)>>4,
                (T & 0xF)
            };

            for(int j = 0; j<8; j++)
            {
                Fragments[j] = Sbox[j][Fragments[j]];     // Пропуск фрагментов через Sbox'ы
            }

            T = (Fragments[0]<<28) +        // Сборка фрагментов обратно в 32-битный подблок
                (Fragments[1]<<24) +
                (Fragments[2]<<20) +
                (Fragments[3]<<16) +
                (Fragments[4]<<12) +
                (Fragments[5]<< 8) +
                (Fragments[6]<< 4) +
                 Fragments[7];

            T = (T<<11)|(T>>21);            // Циклическое смещение влево на 11 бит

            T ^= B;                         // XOR с B

            if(i != 31)
            {
                B = A;                      // Перестановка подблоков 1-31 и 32 циклов
                A = T;
            }
            else
            {
                B = T;
            }
        }   //--Коненц 32 раундов---------------------------

        //--Вывод A и B в файл--------------------------
        fwrite(&A, 4, 1, g);
        fwrite(&B, 4, 1, g);

    }   // Конец файла
    fclose(f);
    fclose(g);
    QMessageBox::information(this,
                             tr("Encryption completed"),
                             tr("Your file has been sucesfully encrypted."
                                "\nTo view it, go to viewing mode."),
                             QMessageBox::Ok);
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
}




void DearDiary::on_pushButton_3_clicked()
{
    QMessageBox::information(this,
                             tr("About application."),
                             tr("This application allows you to select files from your device "
                                "end encrypt it with your own password to special directory.\n"
                                "After that you can decrypt and view your files correctly only"
                                "if you remember the password.\n"
                                "No decrypted files are saved, they are just viewed in the "
                                "window of application."),
                             QMessageBox::Ok);
}

void DearDiary::on_pushButton_4_clicked()
{
    DearDiary::on_pushButton_3_clicked();
}
