#include "resumen.h"
#include "ui_resumen.h"
#include "principal.h"
#include "producto.h"
#include <QDebug>


Resumen::Resumen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Resumen)
{
    ui->setupUi(this);
}

Resumen::~Resumen()
{
    delete ui;
}

void Resumen::setDatos(QString datos)
{
    ui->outDatos->setPlainText(datos);
}
void Resumen::setSubtotal(QString precioS)
{
    ui->outSubtotal->setText(precioS);
}

void Resumen::setIva(QString precioI)
{
    ui->outIva->setText(precioI);
}

void Resumen::setTotal(QString precioT)
{
    ui->outTotal->setText(precioT);
}

void Resumen::setCantidad(QString cantidad)
{
    ui->outDetalle->setPlainText(cantidad);

}





