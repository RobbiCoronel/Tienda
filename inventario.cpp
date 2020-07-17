#include "inventario.h"
#include "ui_inventario.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>



Inventario::Inventario(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Inventario)
{
    ui->setupUi(this);
    connect(ui->btnAgregar, SIGNAL(clicked(bool)), this, SLOT(agregar()));
    connect(ui->btnEliminar, SIGNAL(clicked(bool)), this, SLOT(eliminar()));
    connect(ui->btnGuardar, SIGNAL(clicked(bool)), this, SLOT(guardar()));
    QStringList cabecera = {"Codigo", "Producto", "Precio"};
    ui->outProductos->setColumnCount(3);
    ui->outProductos->setHorizontalHeaderLabels(cabecera);

}

Inventario::~Inventario()
{
    delete ui;
}

void Inventario::setCabecera(QStringList cabecera)
{
    ui->outProductos->setHorizontalHeaderLabels(cabecera);
}

void Inventario::setProductos(int codigo, QString producto, float precio)
{
    int posicion = ui->outProductos->rowCount();
    ui->outProductos->insertRow(posicion);
    ui->outProductos->setItem(posicion,0,new QTableWidgetItem(QString::number(codigo)));
    ui->outProductos->setItem(posicion,1,new QTableWidgetItem(producto));
    ui->outProductos->setItem(posicion,2,new QTableWidgetItem(QString::number(precio)));
}

void Inventario::agregar()
{
    int posicion = ui->outProductos->rowCount();
    ui->outProductos->insertRow(posicion);
}

void Inventario::eliminar()
{
    int p = ui->outProductos->currentRow();
    ui->outProductos->removeRow(p);
}

void Inventario::guardar()
{
    QMessageBox msgBox;
    QString fileName = "productos.csv";
        if (fileName.isEmpty()) {
            return;
        }

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly |   QFile::Text)) {
            QMessageBox::information(this, tr("No se puede abrir el archivo"),
            file.errorString());
            return;
        }

        QTextStream out(&file);
        int rowCount = ui->outProductos->rowCount();
        int colCount = ui->outProductos->columnCount();

        for (int i = 0; i < rowCount; i++) {

            for (int j = 0; j < colCount; j++) {
                if (j > 0) {
                    out << ";";
                }
                QTableWidgetItem* item = ui->outProductos->item(i,j);
                out << item->data(Qt::DisplayRole).toString();
            }
            out << "\n";
        }
        msgBox.setText("Guardado en el archivo: " + fileName);
        msgBox.exec();

}
