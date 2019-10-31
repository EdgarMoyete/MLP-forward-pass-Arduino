//Edgar Moises Hernandez Gonzalez
//Residente INAOE
//Programacion de BCI aplicando analisis multivariable y redes neuronales con el fin de ayudar a personas cuadriplejicas
//Asesora: Dra Maria del Pilar Gomez Gil
//Creacion: 16/11/17
//Modificacion: 30/11/17
//Salida de una red neuronal Feedforward que clasifica el parpadeo de ojos

//variables para micro SD
#include <SD.h> //biblioteca
File file; //objeto File
const int cs = 53; //pin cs (depende de la placa)
int posicionArchivo = 0; //posicion en el archivo

int contadorLinea = 0; //contador para recorrer cada linea cuando ya es String
int contadorValores = 0; //contador para el indice del arreglo de entradas (double)

//pins digitales de Arduino para los leds
const int derA = 12;
const int derB = 11;
const int izqA = 10;
const int izqB = 9;

//Variables para la Red Neuronal
const int N = 16; //entradas
const int L = 7; //neuronas capa oculta
const int M = 3; //salidas
double x[N]; //vector para las entradas
double w1[L][N] = {{0.0001, 5.6805e-05, -2.2471e-07, -3.8659e-11, -1.3335e-05, -1.6717e-08, 3.3817e-05, -5.6420e-08, -0.0001, 0.0001, -5.4787e-08, 9.3677e-12, -3.3624e-05, -1.1587e-07, -3.7817e-05, -7.0261e-08},
                  {0.0001, -5.6124e-05, -5.5122e-06, -4.1421e-08, 4.1296e-06, -2.3421e-05, 3.3628e-05, -1.0875e-05, 0.0001, 1.8292e-05, 5.5440e-07, 3.6376e-08, 7.2499e-07, -1.1013e-05, 2.2469e-05, -9.7555e-07},
                  {0.0001, -7.6922e-05, 3.9234e-06, 1.2338e-10, 8.9278e-06, -6.9971e-06, -4.0276e-06, -1.4026e-05, -0.0001, -9.9630e-05, -2.7411e-05, -1.6388e-10, -0.0002, 2.1088e-05, -4.1385e-05, 8.1051e-05},
                  {-6.6753e-05, -1.5459e-05, 2.4164e-07, 6.7555e-11, -6.7797e-05, -6.7948e-08, 2.8967e-05, -1.0327e-06, -6.5667e-06, 0.0002, -1.1734e-06, -1.8148e-10, -6.1738e-05, -1.0305e-07, 2.9937e-05, 1.1089e-06},
                  {-0.0001, -0.0001, 5.0014e-07, -2.0708e-10, 5.0528e-05, 4.7986e-08, -9.6080e-06, -3.3839e-06, -6.7568e-05, -1.8948e-05, -1.0121e-07, 2.4950e-10, -3.4510e-05, -1.8715e-07, 3.9714e-05, 8.5830e-06},
                  {4.0378e-05, -0.0001, -7.8792e-07, -4.4729e-10, 9.6519e-06, 2.3501e-08, 2.9144e-05, -9.2069e-07, 0.0002, -1.2940e-06, 5.7351e-07, 3.8368e-10, 3.8716e-05, 6.0198e-08, 5.4940e-06, 9.7990e-08},
                  {-1.5141e-05, -3.5072e-05, 3.1823e-06, -1.4587e-10, 9.1614e-05, -5.7232e-06, -4.4260e-05, -1.6507e-06, -0.0001, 7.3209e-05, -1.2927e-06, 1.4821e-10, -1.6752e-05, -1.1329e-07, -1.2356e-05, 4.8288e-07}}; //pesos que conectan a las entradas con la capa oculta
double w2[M][L] = {{0.1114, -0.2151, -2.6317, 3.4570, -3.7340, 2.5394, -1.8961},
                  {2.7113, -1.3589, 2.8865, 1.4373, 2.5404, -2.0089, 3.5829},
                  {-2.8879, -2.0047, -2.8191, -3.2042, -0.2147, 2.0089, 2.8753}}; //pesos que conectan a la capa oculta con la salida
double tet1[] = {-3.1748, -6.9916, 2.3465, -4.2177, 4.2726, -2.3282, 3.2664}; //bias capa entrada
double tet2[] = {-2.0912, -4.8953, -0.1525}; //bias capa oculta
double net1[L]; //net capa oculta
double net2[M]; //net capa de salida
double out1[L]; //salida capa oculta
double out2[M]; //salida de la red

String obtenerLineaSD() {
  String cad = "";
  file = SD.open("eeg.txt");//abrir archivo
  int totalBytes = file.size(); //total de bytes en el archivo
  if (file) {
    if (posicionArchivo >= totalBytes) //ya se recorrio todo el archivo
      posicionArchivo = 0; //volver al inicio del archivo
    file.seek(posicionArchivo); //ir a una posicion especifica en el archivo
    while (file.available()) { //leemos una linea, hay bytes disponibles para leer (num de bytes disponibles)
      char caracter = file.read(); //lee un byte
      cad += caracter;
      posicionArchivo = file.position(); //almacenamos la posicion del archivo
      if (caracter == 10) //si caracter es un salto de linea
        break;
    }
    file.close(); //cerrar archivo
  }
  else
    cad = "Error al abrir el archivo";
  return cad;
}

void obtenerArregloValores(String cadena) {
  String valores = "";
  while (contadorLinea < cadena.length()) { //recorrer cadena
    valores += cadena[contadorLinea]; //almacenar cada caracter en la cadena
    if (cadena[contadorLinea] == 32) { //si caracter es un espacio
      x[contadorValores] = valores.toDouble(); //almacenar en el arreglo los valores convertidos a flotantes
      contadorValores++; //contador del arreglo
      contadorLinea++; //se avanza en la cadena antes de que el break nos saque del ciclo
      break;
    }
    contadorLinea++; //se avanza en la cadena
  }
}

void calcularSalidaRed() {
  //Capa oculta
  for (int i = 0; i < L; i++) {
    net1[i] = 0.0;
    for (int j = 0; j < N; j++)
      net1[i] = net1[i] + w1[i][j] * x[j]; //sumatoria
    net1[i] = net1[i] + tet1[i]; //net
    out1[i] = 1.0 / (1.0 + exp(-net1[i])); //funcion de activacion sigmoide (logistica)
  }
  //Capa de salida
  for (int k = 0; k < M; k++) {
    net2[k] = 0.0;
    for (int i = 0; i < L; i++)
      net2[k] = net2[k] + w2[k][i] * out1[i]; //sumatoria
    net2[k] = net2[k] + tet2[k]; //net
    out2[k] = 1.0 / (1.0 + exp(-net2[k])); //funcion de activacion sigmoide (logistica)
  }
}

String imprimirArreglo(double vector[], int tamano) {
  String cad = "";
  for (int i = 0; i < tamano; i++) {
    cad += vector[i];
    cad += " ";
  }
  return cad;
}

int clasificar() {
  int clase = 0;
  double mayor = 0;
  for (int i = 0; i < M; i++) {
    if (out2[i] > mayor) {
      mayor = out2[i]; //mayor de out2
      clase = i + 1; //a que clase pertenece el mayor
    }
  }
  return clase;
}

void moverMotores(int valorClaseLeds) {
  switch (valorClaseLeds) {
    case 1:
      digitalWrite(derA,LOW);
      digitalWrite(derB,LOW);
      digitalWrite(izqA,HIGH);
      digitalWrite(izqB,LOW);
      break;
    case 2:
      digitalWrite(derA,HIGH);
      digitalWrite(derB,LOW);
      digitalWrite(izqA,LOW);
      digitalWrite(izqB,LOW);
      break;
    case 3:
      digitalWrite(derA,HIGH);
      digitalWrite(derB,LOW);
      digitalWrite(izqA,HIGH);
      digitalWrite(izqB,LOW);   
  }
}

void setup() {
  Serial.begin(9600);//Comunicacion Serial
  if (!SD.begin(cs)) //error al inicializar la biblioteca SD y la micro SD
    Serial.println("No se pudo inicializar la tarjeta SD");
  //declaramos los leds como salidas
  pinMode(derA, OUTPUT);
  pinMode(derB, OUTPUT);
  pinMode(izqA, OUTPUT);
  pinMode(izqB, OUTPUT);
}

void loop() {
  String cadenaLinea = obtenerLineaSD();
  contadorLinea = 0;
  contadorValores = 0;
  for (int i = 0; i < N; i++)
    obtenerArregloValores(cadenaLinea);
  calcularSalidaRed(); //calcular salida de la red
  int valorClase = clasificar();
  moverMotores(valorClase);
  Serial.print("Entradas: ");
  Serial.println(imprimirArreglo(x, N));
  Serial.print("Salidas: ");
  Serial.println(imprimirArreglo(out2, M));
  Serial.print("Clase: ");
  Serial.println(valorClase); //la clase
  Serial.println();
  delay(1000); //1 segundo
  //apagamos todos los leds
  digitalWrite(derA,LOW);
  digitalWrite(derB,LOW);
  digitalWrite(izqA,LOW);
  digitalWrite(izqB,LOW);
}
