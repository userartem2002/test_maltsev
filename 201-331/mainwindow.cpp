#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPushButton"
#include "QMessageBox"
#include "QRandomGenerator"
#include <openssl/evp.h>
#include "QByteArray"
#include "QFile"
#include "QJsonDocument"
#include "QJsonObject"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show_pin_check_window() {

    pin_window = new QWidget(); //Создание виджета как окна для ввода пин кода
    pin_window->setGeometry(683,350,270,120);

    QVBoxLayout *layout_pin = new QVBoxLayout;

    QLabel *pin_code_label = new QLabel("Пин код"); //Лейбл для пинкода
    input_pincode = new QLineEdit(); // Инпут для пинкода
    input_pincode->setEchoMode(QLineEdit::Password);

    QPushButton *check_pin_btn = new QPushButton("Вход"); //Кнопка для проверки
    connect(check_pin_btn, SIGNAL(clicked()), this, SLOT(check_pin_code())); //При нажатии на кнопку отправляемся в функцию проверки

    //Добавляем в окно лейбл кнопку и инпут
    layout_pin->addWidget(pin_code_label);
    layout_pin->addWidget(input_pincode);
    layout_pin->addWidget(check_pin_btn);
    pin_window->setLayout(layout_pin);

    pin_window->show();
}

int MainWindow::check_pin_code() {
    const QString orig_code = "1234";
    QString code = input_pincode->text();

    if (orig_code == code) {
        delete[] pin_window;
        this->show_game_window(); //запускаем основное окно
        this->show();
        return 0;

    }
    else {
        QMessageBox::critical(NULL,QObject::tr("Ошибка"),tr("Ошибка ввода пин-кода"));
        return 0;
    }
}


int MainWindow::show_game_window() {

    //Создание маперов для кнопок
    game_mapper = new QSignalMapper(this);
    QObject::connect(game_mapper,SIGNAL(mappedInt(int)),this,SLOT(on_click_game_btn(int))); //сигнал передачу id кнопки при нажатии на кнопку

    list_of_number[8]; //Массив хранит в себе значения рандомных чисел

    //Создание кнопок
    for (int i = 0; i < 9; i++) {
        QPushButton* game_btn = create_game_btn(i); //Создание кнопки

        game_btn_list.push_back(game_btn); // Добавление в общий лист

        qint32 number = QRandomGenerator::global()->bounded(-1000,1000); //Создание рандомного числа

        //list_of_number[i] = (int)number;//Добавление в масив рандомных чисел

        QByteArray chipr_num = encrypt_num((int)number); //шифрование числа

        qDebug() << "Encrypt number is " << chipr_num;

        list_of_encrypt_number[i] = chipr_num;//Вставка шифрованного числа в соответствующий слот

    }

    //Добавление кнопок в графическое окно
    for (int i = 0; i < game_btn_list.length();) {
        QHBoxLayout *Hlayout = new QHBoxLayout;
        for (int j = 0; j < 3; j++, i++) {
            Hlayout->addWidget(game_btn_list[i]);//Добавляем в гориз лайаут кнопку из листа
        }
        ui->verticalLayout->addLayout(Hlayout);
    }

    return 0;
}

QPushButton* MainWindow::create_game_btn(int id) {

    QPushButton *current_game_btn = new QPushButton("Click");//Создание кнопки

    game_mapper->setMapping(current_game_btn, id); //Вместе с кнопкой передаем id
    connect(current_game_btn,SIGNAL(clicked()),game_mapper,SLOT(map())); //слот на передачу при нажатии

    return current_game_btn;
}

int MainWindow::on_click_game_btn (int id) {

    int decrypt_number = decrypt_num(id);
    qDebug() << "Decrypt number is " << decrypt_number;

    count_of_click = count_of_click + 1; //Счет кликов

    sum = ui->label_of_sum->text().toInt();// Получаем переменную суммы

    game_btn_list[id]->setText(QString::number(decrypt_number));//Вскрываем число кнопки

    //ui->label_of_sum->setText(QString::number(sum + list_of_number[id])); // Обновляем переменную sum

    ui->label_of_sum->setText(QString::number(sum + decrypt_number)); // Обновляем переменную sum

    //Если 3 нажатия на кнопку
    if (count_of_click >= 3) {
        //Вывод суммы в отдельном окне
        QMessageBox::information(this, "Information", "Your sum is " + QString::number(ui->label_of_sum->text().toInt()));

        //Сбрасываем поле
        reset();
        return 0;
    }

    return 0;
}

int MainWindow::reset() {
    qDebug() << "***Reset***";
    // Обнуление счетчика
    count_of_click = 0;

    //Обнуление суммы
    sum = 0;
    ui->label_of_sum->setText(QString::number(sum));

    //Удаление всех кнопок
    for (int var = 0; var < game_btn_list.length(); ++var) {
        delete[] game_btn_list[var];
    }
    game_btn_list.clear();

    //Вызов функции создание и отрисовки кнопок
    show_game_window();
    return 0;
}


void MainWindow::on_pushButton_clicked()
{
    reset();
}

QByteArray MainWindow::encrypt_num(int num){
    unsigned char chipr[512]; //Создание буфера, который будет хранить зашифрованную строчку
    QByteArray number_bit;//Создание буфера байтов для числа
    number_bit.setNum(num);//Переносим число в буфер
    int encrypt_number = encrypt((unsigned char *)number_bit.data(), number_bit.length(), key, iv, chipr);//Шифрование
    QByteArray chipr_bit =  QByteArray::fromRawData((const char*)chipr, encrypt_number).toBase64();//Перевод в байты

    return chipr_bit;
}

int MainWindow::decrypt_num(int btn_id){
    //Расшифровка
    unsigned char decrypt_number[512];
    QByteArray::fromBase64(list_of_encrypt_number[btn_id].data());
    int decrypt_len = decrypt((unsigned char*)QByteArray::fromBase64(list_of_encrypt_number[btn_id]).data(), list_of_encrypt_number[btn_id].length(),key,iv,decrypt_number);
    decrypt_number[decrypt_len] = '\0';
    QByteArray decrypt_bit =  QByteArray::fromRawData((const char*)decrypt_number, decrypt_len);
    //qDebug() << "decrypt_number_bit --- " << decrypt_bit.toInt();

    return decrypt_bit.toInt();
}

int MainWindow::encrypt(unsigned char *text, int text_len, unsigned char *key,
                        unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;


    if(!(ctx = EVP_CIPHER_CTX_new()))
        crypt_error();


    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        crypt_error();


    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, text, text_len))
        crypt_error();
    ciphertext_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        crypt_error();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int MainWindow::decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                        unsigned char *iv, unsigned char *decryptext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int decryptext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        crypt_error();


    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        crypt_error();


    if(1 != EVP_DecryptUpdate(ctx, decryptext, &len, ciphertext, ciphertext_len))
        crypt_error();
    decryptext_len = len;


    if(1 != EVP_DecryptFinal_ex(ctx, decryptext + len, &len))
        crypt_error();
    decryptext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return decryptext_len;
}

int MainWindow::crypt_error(void) {
    return 1;
}



