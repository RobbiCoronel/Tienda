#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <QMainWindow>

#include "producto.h"

#include "resumen.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Principal; }
QT_END_NAMESPACE

class Principal : public QMainWindow
{
    Q_OBJECT

public:
    Principal(QWidget *parent = nullptr);
    ~Principal();
    QString acumulador;

public slots:
    void mostrarPrecio(int);
    void agregarProducto();
    void validarCedula();
    void finalizarDatos();
    void validarEmail();
    void validarNombre();
    void validarTelefono();
    void imprimirCabecera();
    void imprimirDatos();
    void consumidorFinal();
    void factura();

private:
    Ui::Principal *ui;
    QList<Producto*> m_productos;

    void inicializarDatos();
    void inicializarWidgets();
    float m_subtotal;

    void calcular(float subtotal);
    bool buscar(Producto *producto, int cantidad);

};
#endif // PRINCIPAL_H
