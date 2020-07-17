#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <QDialog>

#include "principal.h"

namespace Ui {
class Inventario;
}

class Inventario : public QDialog
{
    Q_OBJECT

public:
    explicit Inventario(QWidget *parent = nullptr);
    ~Inventario();
     void setCabecera(QStringList);
     void setProductos(int, QString, float);
     QString archivoExistente;

public slots:
     void agregar();
     void eliminar();
     void guardar();

private:

    Ui::Inventario *ui;
};

#endif // INVENTARIO_H
