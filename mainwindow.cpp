#include <QSqlQueryModel>
#include <QThread>
#include <QProgressDialog>
#include <QSqlDatabase>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlQueryModel* model = new QSqlQueryModel;
    ui->tableView->setModel(model);

    connect(ui->lineEdit, &QLineEdit::editingFinished, this, [this, model]
    {
        if(this->ui->lineEdit->text().isEmpty())
        {
            model->clear();
        }
        else {
            model->setQuery(QString("SELECT student.name, disci"
                                    "phline.title, mark.global_grade_value, mark.nati"
                                    "onal_grade_value FROM `Студент` AS student INNE"
                                    "R JOIN `Група` as gr ON student.group_id = gr.id AND LOWER(gr.title) = LOWER('%1') I"
                                    "NNER JOIN `ГрупаДисциплінаManyToMany` as gr_disc ON gr_disc.group_id = gr.id INNER JOIN `Дисципліна` as discip"
                                    "hline ON disciphline.id = gr_disc.discipline_id LEFT JOIN `Оцінка` AS mark ON mark.st"
                                    "udent_id = student.id AND mark.discipline_id = disciphline.id ORDER "
                                    "BY student.name, disciphline.title").
                            arg(this->ui->lineEdit->text()));
        }
    });
}

/*
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,%&@@*,,,
,,,,,,,,,,,,,,,,,,%../#@%,,,/,&,,,,,%&@&,,,
,,,,,,,,,,,,,,,,,,,*,......%.,,,,,,,,%@@*,,
,,,,,,,,,,,,,,,,,,,*..........@,,,,,,*@#,,,
,,,,,,,,,,,,,,,,,(#,...........@@&,,,,@,,,,
,,,,,,,,,,,,,,,,/#&@@%....%@%(/#@(,,/,,,,,,
,,,,,,,,,,,,,/@@@@@@& .      ,,,,,,/#&,,,,,
,,,,,,,,,,,@@@@@@&@@@@@%%&(@@@/*,*,&%,,,,,,
,,,,,,,,,,@@@%(((((((%@@@%#&@@#*,*&&,,,,,,,
,,,,,,,,,*@@&((&(((&%(#@@@#####%@@@*,,,,,,,
,,,,,,,,,(@@%(%@(@@(#((@@########%/,,,,,,
,,,,,**,,,@@@(((@(@(((@&@##########&@*,,,,,
%/////////@@@@@@(((&@@@@%@&@######@@(,,,,,,
/////////////(&@@@@@@@#****(@@@@@@@(***,,,,
//////////////(((((@&******@@(#%(&@(**,,,,,
///////////////(/((((((/@&(@@((#(&@/**,,,,,
*/

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Init()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("db4free.net");
    db.setDatabaseName("gosiv12");
    db.setUserName("gosiv12");
    db.setPassword("gZwiV6mFZ6SBDeZ");

    QThread* worker = QThread::create([this]
    {
        bool ok = db.open();

        if(!ok)
        {
            emit ConnectionFailed();
        }
    });

    QProgressDialog* progress = new QProgressDialog("Підключення до бази даних", "", 0, 0, this);
    progress->setWindowModality(Qt::WindowModal);
    progress->setCancelButton(nullptr);
    connect(worker, &QThread::finished, progress, &QProgressDialog::close);
    connect(worker, &QThread::finished, worker, &QThread::deleteLater);
    connect(worker, &QThread::finished, progress, &QProgressDialog::deleteLater);
    connect(this, &MainWindow::ConnectionFailed, this, [this]
    {
        QMessageBox::critical(this, "Помилка", "Помилка при підключенні до бази даних");
        qApp->quit();
    });

    progress->show();
    worker->start();
}
