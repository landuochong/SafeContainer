#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <iostream>
#include <thread>
#include <QDateTime>
#include <windows.h>
#include <unordered_map>
#include <string>
#include <QTime>
#include <sstream>
#include <io.h>
#include<iomanip>
#include "windows.h"
#include <QDateTime>
#include <QTextCodec>
#include <QDir>
#include "EventBus/EventBus.hpp"
using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    thread_.Start();

    basic_comm_eventbus::EventBus::AddHandler<CustomEvent>(this);
}

Widget::~Widget(){
    basic_comm_eventbus::EventBus::RemoveHandler<CustomEvent>(this);
}

void blockingTask() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    qDebug() << "Running blocking task"<< QTime::currentTime().toString("hh:mm:ss.zzz");
}


void Widget::on_btn_thread_test_clicked(){
    qDebug()<<"on_btn_send_clicked "<< QTime::currentTime().msec();;

    thread_.PostTask([&]{
        std::this_thread::sleep_for(std::chrono::seconds(1));
        qDebug()<<"PostTask " << QTime::currentTime().toString("hh:mm:ss.zzz");
    });

    thread_.PostDelayedTask([&]{
        qDebug()<<"DelayedTask " << QTime::currentTime().toString("hh:mm:ss.zzz");
    }, 3500000);

    thread_.PostDelayedTask([&]{
        qDebug()<<"DelayedTask " << QTime::currentTime().toString("hh:mm:ss.zzz");
    }, 3'000'000);

    std::thread([this]() {
        qDebug()<<"start BlockingCall"<< QTime::currentTime().toString("hh:mm:ss.zzz");
        thread_.BlockingCall([]{
            blockingTask();
        });
    }).detach();
}

void Widget::on_btn_eventbus_clicked(){
    CustomEvent event("Send Custom Event");
    basic_comm_eventbus::EventBus::SendEvent(event);
}

void Widget::on_btn_net_clicked(){

}

void Widget::on_btn_media_clicked(){

}

void Widget::on_btn_connect_clicked(){

}

void Widget::test(int value){
    qDebug()<<"pool end id:"<<value;
}

void Widget::onEvent(CustomEvent& e){
    qDebug()<<e.GetMsg().c_str();
}
