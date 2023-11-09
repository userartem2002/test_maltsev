#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QSignalMapper"
#include "QListWidgetItem"
#include "QVBoxLayout"
#include "QPushButton"
#include "QMessageBox"
#include "QLabel"
#include "QLineEdit"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    //Переменные для окна ввода пин кода
    QWidget *pin_window;
    QLineEdit *input_pincode;

    //Хранилище
    QList<QPushButton *> game_btn_list; //Хранение кнопок
    int list_of_number[9]; //Хранение рандомных чисел
    int count_of_click = 0; //Количество нажатий
    int sum = 0; // Общая сумма

    //Функции
    QPushButton* create_game_btn(int id);//Функция создание одной кнопки
    QSignalMapper *game_mapper;//отслеживание кнопки копирования логина
    void show_pin_check_window();//Функция показа окна для ввода pincode
    int show_game_window();//Функция показа основного игрового окна
    int reset();//Функция сброса игрового поля

    unsigned char *key = (unsigned char *)"81dc9bdb52d04dc20036dbd8313ed055";
    unsigned char *iv = (unsigned char *)"0123456789012345";


    int encrypt(unsigned char *text, int text_len, unsigned char *key,unsigned char *iv, unsigned char *ciphertext);
    int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,unsigned char *iv, unsigned char *decryptext);
    QByteArray encrypt_num(int num);
    int decrypt_num(int btn_id);
    int crypt_error(void);

    QByteArray list_of_encrypt_number[9];

private slots:
    int check_pin_code();
    int on_click_game_btn (int id);
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
