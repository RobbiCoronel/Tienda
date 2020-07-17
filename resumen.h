#ifndef RESUMEN_H
#define RESUMEN_H

#include <QDialog>



namespace Ui {
class Resumen;
}

class Resumen : public QDialog
{
    Q_OBJECT

public:
    explicit Resumen(QWidget *parent = nullptr);
    ~Resumen();
    void setDatos(QString datos);
    void setSubtotal(QString precioS);
    void setIva(QString precioI);
    void setTotal(QString precioT);
    void setCantidad(QString);

private:
    Ui::Resumen *ui;
};

#endif // RESUMEN_H
