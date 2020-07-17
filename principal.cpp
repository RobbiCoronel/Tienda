#include "principal.h"
#include "ui_principal.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "inventario.h"
#include <QRegExp>
#include <QMessageBox>

Principal::Principal(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Principal)
{
    ui->setupUi(this);

    connect(ui->inEmail, SIGNAL(editingFinished()), this, SLOT(validarEmail()));
    connect(ui->inNombre, SIGNAL(editingFinished()), this, SLOT(validarNombre()));
    connect(ui->inTelefono, SIGNAL(editingFinished()), this, SLOT(validarTelefono()));
    connect(ui->btnFinalizar, SIGNAL(clicked(bool)), this, SLOT(finalizarDatos()));
    connect(ui->mnuProductos, SIGNAL(triggered(bool)), this, SLOT(imprimirDatos()));
    connect(ui->btnConsumidor, SIGNAL(clicked(bool)), this, SLOT(consumidorFinal()));
    connect(ui->btnFactura, SIGNAL(clicked(bool)), this, SLOT(factura()));
    ui->inTelefono->setValidator(new QIntValidator);
    ui->inNombre->setEnabled(false);
    ui->inTelefono->setEnabled(false);
    ui->inEmail->setEnabled(false);
    ui->inDireccion->setEnabled(false);
    inicializarDatos();
    m_subtotal = 0;
}

Principal::~Principal()
{
    delete ui;
}

void Principal::mostrarPrecio(int index)
{
    // Obtener el precio del producto seleccionado
    float precio = m_productos.at(index)->precio();

    // Colacar el precio en la etiqueta correspondiente
    ui->outPrecio->setText("$ " + QString::number(precio));
}

void Principal::agregarProducto()
{
    //Obteniendo datos desde la interfaz
    int index=ui->inProducto->currentIndex();
    Producto *p=m_productos.at(index);
    int cantidad =ui->inCantidad->value();
    if(cantidad==0){
        return;
    }
    //Calcular subtotal
    float subtotal=cantidad*p->precio();
    int posicion=ui->outDetalle->rowCount();
    //Buscar y actualizar productos repetidos
    if (!buscar(p,cantidad)){


        //agregar datos a la tabla
        ui->outDetalle->insertRow(posicion);
        ui->outDetalle->setItem(posicion,0,new QTableWidgetItem(QString::number(cantidad)));
        ui->outDetalle->setItem(posicion,1,new QTableWidgetItem(p->nombre()));
        ui->outDetalle->setItem(posicion,2,new QTableWidgetItem("$  "+QString::number(subtotal)));
    }

    //Limpiar Datos(esto en un metodo)

    ui->inCantidad->setValue(0);
    ui->inProducto->setFocus();


    //Calcular valores y mostrar
    calcular(subtotal);
    //validarCedula();

}

void Principal::finalizarDatos()
{

    QMessageBox msgBox;
    Resumen *dlgResumen = new Resumen() ;
    QString subtotal = ui->outSubtotal->text();
    QString iva = ui->outIva->text();
    QString total = ui->outTotal->text();
    QString nombre = ui->inNombre->text();
    QString cedula = ui->inCedula->text();
    QString telefono = ui->inTelefono->text();
    QString correo = ui->inEmail->text();
    QString direcion = ui->inDireccion->toPlainText();
    if(nombre == "\0" || cedula== "\0" || telefono== "\0" || correo== "\0" || direcion== "\0"){
        msgBox.setText("Por favor llene  todos los datos correctamente");
        msgBox.exec();
        return;
    }
    QString resultado = "Cedula: " + cedula + "\n";
    resultado += "Cliente: " + nombre + "\n";
    resultado += "Telefono: " + telefono + "\n";
    resultado += "E-mail: " + correo + "\n" +
            "Direccion: " + direcion;

    QStringList lista ;
    QString listaP;
    for(int columna = 0; columna<ui->outDetalle->columnCount(); ++columna){
        lista << ui->outDetalle->horizontalHeaderItem(columna)->data(Qt::DisplayRole).toString();
    }
    lista << "\n";
    for(int fila =0; fila<ui->outDetalle->rowCount();  ++fila){
        for (int columna = 0;columna<ui->outDetalle->columnCount(); ++columna) {
            lista <<  ui->outDetalle->item(fila, columna)->text();
            QTableWidgetItem *item = ui->outDetalle->item(fila, columna);
            if(!item || item->text().isEmpty()){
                ui->outDetalle->setItem(fila , columna, new QTableWidgetItem("0"));
            }
            listaP = lista.join("     ");
            lista << "   ";
        }
        lista << "\n";
        dlgResumen->setCantidad(listaP);
    }
    listaP = lista.join(";");
    dlgResumen->setDatos(resultado);
    dlgResumen->setSubtotal(subtotal);
    dlgResumen->setIva(iva);
    dlgResumen->setTotal(total);
    dlgResumen->show();
}

void Principal::validarEmail()
{
    QString value = ui->inEmail->text();
    QRegularExpression regex("[A-z0-9_\\.-]+@[A-z0-9\\.]+\\.[A-Za-z]{2,4}\\b");
    if(regex.match(value).hasMatch())
    {
        ui->inDireccion->setFocus();
        ui->inEmail->setStyleSheet("color: green");
    }else{
        ui->inEmail->setStyleSheet("color: red");
        ui->statusbar->showMessage("E-mail incorrecto",5000);
        ui->inEmail->selectAll();
    }
}

void Principal::validarNombre()
{
    QString nombre = ui->inNombre->text();
    QRegularExpression regex("^[a-zA-Z0-9]+$");
    if(!(regex.match(nombre).hasMatch()))
    {
        ui->inTelefono->setFocus();
        ui->inNombre->setStyleSheet("color: green");
    }else{
        ui->inNombre->setStyleSheet("color: red");
        ui->statusbar->showMessage("Nombre incorrecto",5000);
        ui->inNombre->selectAll();
    }
}

void Principal::validarTelefono()
{
    QString telefono = ui->inTelefono->text();
    if( telefono.size() < 10){
        ui->inTelefono->setStyleSheet("color : red");
        ui->statusbar->showMessage("Número de telefono incorrecto",5000);
        ui->inTelefono->selectAll();
    }else{
        ui->inTelefono->setStyleSheet("color : green");
        ui->inEmail->setFocus();
    }

}

void Principal::imprimirCabecera()
{
    Inventario *dlgInventario = new Inventario();
    QStringList cabecera = {"Cantidad", "Producto", "Sub Total"};
    dlgInventario->setCabecera(cabecera);
}

void Principal::imprimirDatos()
{
    imprimirCabecera();
    Inventario *dlgInventario = new Inventario();
    // abrir archivos
    QFile archivo("productos.csv");
    qDebug() << archivo.exists();

    // leer el archivo de productos
    if(archivo.open(QFile::ReadOnly)){
        QTextStream in(&archivo);
        while (!in.atEnd()){
            QString linea = in.readLine();
            //separar datos por ;
            QStringList datos = linea.split(';');
            int codigo = datos[0].toInt();
            QString producto = datos[1];
            float precio = datos[2].toFloat();
            qDebug() << codigo << producto << precio;
            dlgInventario->setProductos(codigo, producto, precio);
        }
    }else{
        qDebug() << "Error al abrir el archivo";
    }

    archivo.close();
    dlgInventario->show();
}
//INICIO DE LA FUNCION EXTRA
void Principal::consumidorFinal()
{
    ui->inCedula->setText("1234567890");
    ui->inNombre->setText("Cliente");
    ui->inTelefono->setText("9999999999");
    ui->inEmail->setText("consumidor@michita.com");
    ui->inDireccion->setPlainText("*****");
    ui->inCedula->setEnabled(false);
}

void Principal::factura()
{
    ui->inCedula->setEnabled(true);
    validarCedula();
    ui->inCedula->clear();
    validarEmail();
    ui->inEmail->clear();
    validarNombre();
    ui->inNombre->clear();
    validarTelefono();
    ui->inTelefono->clear();
    ui->inDireccion->clear();
}
//FIN DE LA FUNCION EXTRA

void Principal::calcular(float subtotal)
{
    m_subtotal+=subtotal;
    float iva= m_subtotal+0.12;
    float total = m_subtotal+iva;

    ui->outSubtotal->setText(QString::number(m_subtotal));
    ui->outIva->setText(QString::number(iva));
    ui->outTotal->setText(QString::number(total));
}

void Principal::validarCedula()
{
    QString parCedula = ui->inCedula->text();
    qDebug() << "";
    parCedula=ui->inCedula->text();
    QString digitoRegion=parCedula.mid(0,2);
    int digRegion=digitoRegion.toInt();
    int v_total=0,v_acu=0, v_decena=0,v_bandera=0;
    QString v_val;
    if (parCedula.size() == 10)
    {
        if(digRegion>0 && digRegion<25){
            v_bandera=1;
            // VALICIÓN DE CEDULA
            if (v_bandera==1){
                for (int i =0;i<=8;i++)
                {
                    QChar C_cedulaa=parCedula.at(i);
                    QString C_cedulaa2=C_cedulaa;
                    if(i%2==0)
                    {
                        if ((C_cedulaa2.toInt()*2)>9)
                        {
                            v_acu = v_acu+( C_cedulaa2.toInt()*2)-9;
                        }
                        else
                        {
                            v_acu= v_acu+ (C_cedulaa2.toInt()*2);
                        }
                    }
                    else
                    {
                        v_acu= v_acu+ (C_cedulaa2.toInt());
                    }
                }
                v_val= (QString::number(v_acu)).mid(0,1);
                v_decena= (v_val.toInt()+1)*10;
                v_total=v_decena-v_acu;


                if(v_total==(digitoRegion=parCedula.mid(9,10)).toInt())
                {
                    ui->inNombre->setEnabled(true);
                    ui->inEmail->setEnabled(true);
                    ui->inTelefono->setEnabled(true);
                    ui->inDireccion->setEnabled(true);
                    ui->inNombre->setFocus();
                    ui->inCedula->setStyleSheet("color: green");
                    ui->inCedula->setEnabled(false);

                }
                else
                {
                    ui->statusbar->showMessage("Número de cédula incorrecto",5000);
                    ui->inCedula->setStyleSheet("color: red");
                    ui->inCedula->selectAll();
                }
            }

        }else{

            ui->statusbar->showMessage("Número de cédula incorrecto",5000);
            ui->inCedula->setStyleSheet("color: red");
            ui->inCedula->selectAll();
        }


    }else{
        ui->statusbar->showMessage("Número de cédula incorrecto",5000);
        ui->inCedula->setStyleSheet("color: red");
        ui->inCedula->selectAll();
    }
}


void Principal::inicializarDatos()
{
    // abrir archivos
    QFile archivo("productos.csv");

    // leer el archivo de productos
    if(archivo.open(QFile::ReadOnly)){
        QTextStream in(&archivo);
        while (!in.atEnd()){
            QString linea = in.readLine();
            //separar datos por ;
            QStringList datos = linea.split(';');
            int codigo = datos[0].toInt();
            float precio = datos[2].toFloat();
            // Crear productos a la lista
            m_productos.append(new Producto(codigo,datos[1],precio));
        }

    }else{
        qDebug() << "Error al abrir el archivo";
    }

    archivo.close();

    // Crear y agregar productos a la lista
    //m_productos.append(new Producto(2,"Pan",0.15));

    // Invocar al metodo para inicializar los widgets
    inicializarWidgets();
}

void Principal::inicializarWidgets()
{
    // Agregar nombre de los productos al combo box
    for (int i = 0; i < m_productos.length(); ++i) {
        ui->inProducto->addItem(m_productos.at(i)->nombre());
    }

    // Colocar cabecera en la tabla
    QStringList cabecera = {"Cantidad", "Producto", "Sub Total"};
    ui->outDetalle->setColumnCount(3);
    ui->outDetalle->setHorizontalHeaderLabels(cabecera);

    // Eventos
    connect(ui->inProducto, SIGNAL(currentIndexChanged(int)), this, SLOT(mostrarPrecio(int)));
    connect(ui->cmdAgregar, SIGNAL(clicked(bool)), this, SLOT(agregarProducto()));
    connect(ui->inCedula,SIGNAL(editingFinished()),this,SLOT(validarCedula()));


    // Mostrar el precio del primer producto
    mostrarPrecio(0);
}

bool Principal::buscar(Producto *producto, int cantidad)
{
    //Recorrer la tabla
    int numFilas = ui->outDetalle->rowCount();
    for(int i=0;i<numFilas;i++){
        QTableWidgetItem *item =ui->outDetalle->item(i,1);
        QString dato=item->data(Qt::DisplayRole).toString();
        if(dato==producto->nombre()){
            //Obtener la cantidad del producto que ya esta en el detalle
            QTableWidgetItem *item= ui->outDetalle->item(i,0);
            int cantActual =item->data(Qt::DisplayRole).toInt();
            //sumar la cantidad
            int cantNueva = cantActual+cantidad;
            //Calcular nuevo sub total
            float subtotal = cantNueva*producto->precio();
            //Actualizar tabla
            ui->outDetalle->setItem(i,0,new QTableWidgetItem(QString::number(cantNueva)));

            ui->outDetalle->setItem(i,2,new QTableWidgetItem("$  "+QString::number(subtotal)));
            return true;

        }
    }
    return false;
}
