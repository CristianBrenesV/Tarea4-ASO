#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <termios.h>
#include <fcntl.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#pragma region Variables
#define GB_TO_BYTES(gb) ((gb) * 1024 * 1024 * 1024)
int MemoriaGBh1 = 1;

int Mem1GBH4 = 1;
int Mem2GBH4 = 1;
int Mem3GBH4 = 1;
int Mem4GBH4 = 1;
int Mem5GBH4 = 1;
int Mem6GBH4 = 1;
int Mem7GBH4 = 1;
int Mem8GBH4 = 1;
int Mem9GBH4 = 1;
int Mem10GBH4 = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#pragma endregion Variables

//Variables globales 
struct ThreadData {
    int PosInicial;
    int PosFinal;
    int *MemoriaHilo2;
    int CantGB;
    std::string* textPtr;
};

//Función que detecta si se presionó la tecla
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    //Obtienen las configuraciones actuales del terminal
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    //Deshabilita la eco de los caracteres y el modo canónico
    newt.c_lflag &= ~(ICANON | ECHO);

    //Aplica la nueva configuración
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    //Establece el modo no bloqueante para la entrada estándar
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    //Restauran las configuraciones originales del terminal y se regresa el carácter
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
     return 0;
}


void* hilo1_textoingreso(void* arg) {

    uint64_t CantBytes = MemoriaGBh1 * static_cast<uint64_t>(1e9);

    std::cerr << "Hilo textoingreso creado" << std::endl;
    std::cerr << "Hilo textoingreso cargado \n" << std::endl;

    //Reserva la memoria dinámica
    void* MemoriaHilo1 = malloc(CantBytes);
    if (MemoriaHilo1 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria solicitada." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << MemoriaGBh1 << " Gb de memoria correctamente." << std::endl;

    }

    //Abre el archivo en modo lectura
    std::ifstream archivo("textoleer.txt"); 

    //Carga el archivo de texto
    std::ifstream file("textoleer.txt");
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo textoleer.txt." << std::endl;
    }
    std::string linea;
    while (std::getline(archivo, linea)) {
        std::cout << linea << std::endl; 
    }

    file.close(); 

    //Bloquea el hilo
    int result = pthread_mutex_trylock(&mutex);
    if (result == 0) {
        std::cout << "\nHilo textoingreso bloqueado correctamente.\n\n" << std::endl;
        pthread_mutex_unlock(&mutex);
    } else {
        std::cerr << "\nNo se pudo bloquear el hilo textoingreso." << std::endl;
    }

    return NULL;
}

void* hilo2_parametros(void* arg) {

    //Para traer las variables globales y guardar con los parámetros ingresados
    ThreadData* data = static_cast<ThreadData*>(arg);
    std::cerr << "Hilo parametros creado" << std::endl;
    std::cerr << "Hilo parametros cargado \n" << std::endl;

    std::cout << "\t\t\tMenu de opciones" << std::endl;

    //Solicita al usuario que ingrese la posición inicial
    do {
        std::cout << "Ingrese la posición inicial (entre 1 a 508): ";
        std::cin >> data->PosInicial;

        //Verifica si la posición inicial está dentro del rango deseado
        if (data->PosInicial <= 0 || data->PosInicial > 508) {
            std::cout << "La posición inicial debe ser mayor que 0 y menor que 504." << std::endl;
        } 
    } while (data->PosInicial <= 0 || data->PosInicial >= 508);

    //Solicita al usuario que ingrese la posición final
    do {
        std::cout << "Ingrese la posición final (mayor que la posición inicial y menor que 508): ";
        std::cin >> data->PosFinal;

        //Verifica si la posición final está dentro del rango deseado
        if (data->PosFinal <= data->PosInicial || data->PosFinal >= 508) {
            std::cout << "La posición final debe ser mayor que la posición inicial y menor que 508." << std::endl;
        } 
    } while (data->PosFinal <= data->PosInicial || data->PosFinal >= 508);
    
    //Solicita al usuario la cantidad de memoria en GB
    do {
        std::cout << "Ingrese la cantidad de memoria que desea reservar (entre 1 y 3): ";
        std::cin >> data->CantGB;

        // Validar la entrada del usuario
        if (data->CantGB < 1 || data->CantGB > 3) {
            std::cout << "La cantidad de memoria debe estar entre 1 y 3." << std::endl;
        }
    } while (data->CantGB < 1 || data->CantGB > 3);

    //Convierte GB a bytes
    uint64_t CantBytes = data->CantGB * static_cast<uint64_t>(1e9);

    //Reserva la memoria dinámica
    void* MemoriaHilo2 = malloc(CantBytes);
    if (MemoriaHilo2 == nullptr) {
        std::cout << "No se pudo reservar la memoria solicitada." << std::endl;
        pthread_exit(NULL);
    }

    std::cout << "\nSe han reservado los siguientes parámetros:" << std::endl;
    std::cout << "Posición inicial: " << data->PosInicial << std::endl;
    std::cout << "Posición inicial: " << data->PosFinal << std::endl;
    std::cout << "Memoria reservada: " << data->CantGB << " Gb \n" << std::endl;

    //Bloquea el hilo
    int result = pthread_mutex_trylock(&mutex);
    if (result == 0) {
        std::cout << "Hilo parametros bloqueado correctamente." << std::endl;
        pthread_mutex_unlock(&mutex);
    } else {
        std::cerr << "No se pudo bloquear el hilo parametros." << std::endl;
    }
    std::cout << "Presiona 'A' para continuar el programa." << std::endl;

    while (true) {
        //Verifica si se ha presionado una tecla
        if (kbhit()) {
            //Lee el carácter presionado
            char c = getchar();
            //Al presionar la tecla F termina el programa
            if (c == 'A' || c == 'a') {
                std::cout << "" << std::endl;
                break;
            }
        }
    }
    return NULL;
}

void* hilo3_extraccion(void* arg) {
    //Trae la variables globales con el dato ingresado
    ThreadData* data = reinterpret_cast<ThreadData*>(arg);
    std::string text = *reinterpret_cast<std::string*>(data);

    uint64_t CantBytes = data->CantGB * static_cast<uint64_t>(1e9);

    //Reservar memoria dinámica
    void* MemoriaHilo2 = malloc(CantBytes);
    if (MemoriaHilo2 == nullptr) {
        std::cout << "No se pudo reservar la memoria solicitada." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << data->CantGB << " Gb de memoria correctamente." << std::endl;
    }

    std::ifstream archivoEntrada("textoleer.txt");
    if (!archivoEntrada.is_open()) {
        std::cerr << "No se pudo abrir el archivo textoleer.txt." << std::endl;
        return NULL;
    }
    
    //Abre el archivo de texto de entrada
    std::ofstream archivoSalida("textosalida.txt", std::ios::app);
    if (!archivoSalida.is_open()) {
    std::cerr << "No se pudo abrir el archivo textosalida.txt." << std::endl;
    archivoEntrada.close();
    return NULL;
    }

    //Busca la posición inicial en el archivo de entrada
    archivoEntrada.seekg( data->PosInicial, std::ios::beg);

    std::cout << "\nTexto extraído y escrito en textosalida.txt correctamente.\n\n" << std::endl;

    //Extrae y escribir el texto en el archivo de salida
    char caracter;
    while ((caracter = archivoEntrada.get()) != EOF && data->PosInicial < data->PosFinal) {
    std::cout << "\033[34m" << caracter << "\033[0m";
    archivoSalida.put(caracter);
    data->PosInicial++;
    }
    archivoSalida.put('\n');

    //Cierra los archivos
    archivoEntrada.close();
    archivoSalida.close();

    std::cout << "\n" << std::endl;

    //Bloquea el hilo    
    int result = pthread_mutex_trylock(&mutex);
    if (result == 0) {
        std::cout << "Hilo extraccion bloqueado correctamente.\n" << std::endl;
        pthread_mutex_unlock(&mutex);
    } else {
        std::cerr << "No se pudo bloquear el hilo extraccion." << std::endl;
    }

    return NULL;
}

void* hilo4_LlamadasAlSistema(void* arg) {
    int result = pthread_mutex_trylock(&mutex);
    if (result == 0) {
        std::cout << "Hilo LlamadasAlSistema bloqueado correctamente.\n" << std::endl;
        pthread_mutex_unlock(&mutex);
    } else {
        std::cerr << "No se pudo bloquear el hilo LlamadasAlSistema." << std::endl;
    }

    #pragma region Memorias
    uint64_t CantBytes1 = Mem1GBH4 * static_cast<uint64_t>(1e9);
    uint64_t CantBytes2 = Mem2GBH4 * static_cast<uint64_t>(1e9);
    uint64_t CantBytes3 = Mem3GBH4 * static_cast<uint64_t>(1e9);
    uint64_t CantBytes4 = Mem4GBH4 * static_cast<uint64_t>(1e9);
    uint64_t CantBytes5 = Mem5GBH4 * static_cast<uint64_t>(1e9);
    uint64_t CantBytes6 = Mem6GBH4 * static_cast<uint64_t>(1e9);
    uint64_t CantBytes7 = Mem7GBH4 * static_cast<uint64_t>(1e9);
    uint64_t CantBytes8 = Mem8GBH4 * static_cast<uint64_t>(1e9);
    uint64_t CantBytes9 = Mem9GBH4 * static_cast<uint64_t>(1e9);
    uint64_t CantBytes10 = Mem10GBH4 * static_cast<uint64_t>(1e9);

    //Reserva la memoria dinámica
    void* MemGB1H4 = malloc(CantBytes1);
    if (MemGB1H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de marcas de tiempo." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem1GBH4 << " Gb de memoria de marcas de tiempo correctamente." << std::endl;
    }
    void* MemGB2H4 = malloc(CantBytes2);
    if (MemGB2H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de duraciones de tiempo ." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem2GBH4 << " Gb de memoria de duraciones de tiempo correctamente." << std::endl;
    }
    void* MemGB3H4 = malloc(CantBytes3);
    if (MemGB3H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de hora en microsegundos." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem3GBH4 << " Gb de memoria hora en microsegundos correctamente." << std::endl;
    }
    void* MemGB4H4 = malloc(CantBytes4);
    if (MemGB4H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de ID de cada proceso." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem4GBH4 << " Gb de memoria de ID de cada proceso correctamente." << std::endl;
    }
     void* MemGB5H4 = malloc(CantBytes5);
    if (MemGB5H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de informe sobre las llamadas." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem5GBH4 << " Gb de memoria de informe sobre las llamadas correctamente." << std::endl;
    }
    void* MemGB6H4 = malloc(CantBytes6);
    if (MemGB6H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de impresión de puntero." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem6GBH4 << " Gb de memoria de impresión de puntero correctamente." << std::endl;
    }
    void* MemGB7H4 = malloc(CantBytes7);
    if (MemGB7H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de hora del día ." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem7GBH4 << " Gb de memoria de hora del día correctamente." << std::endl;
    }    
    void* MemGB8H4 = malloc(CantBytes8);
    if (MemGB8H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de asignación de memoria." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem8GBH4 << " Gb de memoria de asignación de memoria correctamente." << std::endl;
    }       
    void* MemGB9H4 = malloc(CantBytes9);
    if (MemGB9H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de la información de depuración." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem9GBH4 << " Gb de memoria de la información de depuración correctamente." << std::endl;
    }   
    void* MemGB10H4 = malloc(CantBytes10);
    if (MemGB10H4 == nullptr) {
        std::cout << "Error: No se pudo reservar la memoria de ayuda strace." << std::endl;
        pthread_exit(NULL);
    }else{
        std::cout << "Se ha reservado " << Mem10GBH4 << " Gb de memoria de ayuda strace correctamente." << std::endl;
    }       
    #pragma endregion Memorias
   
    //system("strace -o tiempo.txt -t ./Tarea4");
    system("strace -o tiempo.txt -t ./Tarea4");
   // system("strace --debug ./Tarea4 > depurar.txt");
    system("strace --help ./Tarea4 > ayuda.txt");

    std::ifstream archivo;
    std::string linea;

    int opcion;
    bool salir = false;
    char key_pressed = '\0';

    do {
        // Mostrar las opciones del menú
        std::cout << "\n\t\t\tMenu de opciones:" << std::endl;
        std::cout << "\t\t1. Marcas de líneas" << std::endl;
        std::cout << "\t\t2. Tiempos" << std::endl;
        std::cout << "\t\t3. Horas y micro segundos" << std::endl;
        std::cout << "\t\t4. Identificador de procesos" << std::endl;
        std::cout << "\t\t5. Informes" << std::endl;
        std::cout << "\t\t6. Punteros" << std::endl;
        std::cout << "\t\t7. Hora líneas" << std::endl;
        std::cout << "\t\t8. Asignación de memoria" << std::endl;
        std::cout << "\t\t9. Terminar" << std::endl;
        std::cout << "\t\tIngrese su opción: ";
        std::cin >> opcion;

        // Realizar acciones según la opción seleccionada
        switch (opcion) {
            case 1:
                std::cout << "\n\t\t\t Archivo marcas de línea \n\n" << std::endl;

                archivo.open("tiempo.txt");
                if (archivo.is_open()) {
                    while (getline(archivo, linea)) {
                        std::cout << linea << std::endl;
                    }
                    archivo.close();
                } else {
                    std::cout << "No se pudo abrir el archivo." << std::endl;
                }
                std::cout << "\n\nDesea volver al menu principal? (S/N): \n";
                while (true) {
                        if (kbhit()) {
                            key_pressed = getchar();
                            if(key_pressed == 'S' || key_pressed == 's') {
                                system("clear"); 
                                break;
                            } else if (key_pressed == 'N' || key_pressed == 'n') {
                            //Libera la memoria reservada en el hilo 
                            std::cout << "\n\nSe han desbloqueado los hilos y memorias son libres." << std::endl;
                            return EXIT_SUCCESS;
                            }
                        } while (key_pressed == 'n' || key_pressed == 'n');
                }
                break;
            case 2:
                std::cout << "\n\t\t\t Archivo tiempos \n\n" << std::endl;

                archivo.open("Tiempo.txt");
                if (archivo.is_open()) {
                    while (getline(archivo, linea)) {
                        std::cout << linea << std::endl;
                    }
                    archivo.close();
                } else {
                    std::cout << "No se pudo abrir el archivo." << std::endl;
                }

                std::cout << "\n\nDesea volver al menu principal? (S/N): \n";
                while (true) {
                        if (kbhit()) {
                            key_pressed = getchar();
                            if(key_pressed == 'S' || key_pressed == 's') {
                                system("clear"); 
                                break;
                            } else if (key_pressed == 'N' || key_pressed == 'n') {
                            //Libera la memoria reservada en el hilo 
                            std::cout << "\n\nSe han desbloqueado los hilos y memorias son libres." << std::endl;
                            return EXIT_SUCCESS;
                            }
                        } while (key_pressed == 'n' || key_pressed == 'n');
                }
                break;
            case 3:
                std::cout << "\n\t\t\t Archivo horas y microsegundos \n\n" << std::endl;

                archivo.open("Tiempo.txt");
                if (archivo.is_open()) {
                    while (getline(archivo, linea)) {
                        std::cout << linea << std::endl;
                    }
                    archivo.close();
                } else {
                    std::cout << "No se pudo abrir el archivo." << std::endl;
                }

                std::cout << "\n\nDesea volver al menu principal? (S/N): \n";
                while (true) {
                        if (kbhit()) {
                            key_pressed = getchar();
                            if(key_pressed == 'S' || key_pressed == 's') {
                                system("clear"); 
                                break;
                            } else if (key_pressed == 'N' || key_pressed == 'n') {
                            //Libera la memoria reservada en el hilo 
                            std::cout << "\n\nSe han desbloqueado los hilos y memorias son libres." << std::endl;
                            return EXIT_SUCCESS;
                            }
                        } while (key_pressed == 'n' || key_pressed == 'n');
                }
                break;
            case 4:
                std::cout << "\n\t\t\t Archivo identificador de procesos \n\n" << std::endl;

                archivo.open("Tiempo.txt");
                if (archivo.is_open()) {
                    while (getline(archivo, linea)) {
                        std::cout << linea << std::endl;
                    }
                    archivo.close();
                } else {
                    std::cout << "No se pudo abrir el archivo." << std::endl;
                }
                std::cout << "\n\nDesea volver al menu principal? (S/N): \n";
                while (true) {
                        if (kbhit()) {
                            key_pressed = getchar();
                            if(key_pressed == 'S' || key_pressed == 's') {
                                system("clear"); 
                                break;
                            } else if (key_pressed == 'N' || key_pressed == 'n') {
                            //Libera la memoria reservada en el hilo 
                            std::cout << "\n\nSe han desbloqueado los hilos y memorias son libres." << std::endl;
                            return EXIT_SUCCESS;
                            }
                        } while (key_pressed == 'n' || key_pressed == 'n');
                }
                break;
            case 5:
                std::cout << "\n\t\t\t Archivo informes \n\n" << std::endl;

                archivo.open("Tiempo.txt");
                if (archivo.is_open()) {
                    while (getline(archivo, linea)) {
                        std::cout << linea << std::endl;
                    }
                    archivo.close();
                } else {
                    std::cout << "No se pudo abrir el archivo." << std::endl;
                }

                std::cout << "\n\nDesea volver al menu principal? (S/N): \n";
                while (true) {
                        if (kbhit()) {
                            key_pressed = getchar();
                            if(key_pressed == 'S' || key_pressed == 's') {
                                system("clear"); 
                                break;
                            } else if (key_pressed == 'N' || key_pressed == 'n') {
                            //Libera la memoria reservada en el hilo 
                            std::cout << "\n\nSe han desbloqueado los hilos y memorias son libres." << std::endl;
                            return EXIT_SUCCESS;
                            }
                        } while (key_pressed == 'n' || key_pressed == 'n');
                }
                break;
            case 6:
                std::cout << "\n\t\t\t Archivo punteros \n\n" << std::endl;

                archivo.open("Tiempo.txt");
                if (archivo.is_open()) {
                    while (getline(archivo, linea)) {
                        std::cout << linea << std::endl;
                    }
                    archivo.close();
                } else {
                    std::cout << "No se pudo abrir el archivo." << std::endl;
                }

                std::cout << "\n\nDesea volver al menu principal? (S/N): \n";
                while (true) {
                        if (kbhit()) {
                            key_pressed = getchar();
                            if(key_pressed == 'S' || key_pressed == 's') {
                                system("clear"); 
                                break;
                            } else if (key_pressed == 'N' || key_pressed == 'n') {
                            //Libera la memoria reservada en el hilo 
                            std::cout << "\n\nSe han desbloqueado los hilos y memorias son libres." << std::endl;
                            return EXIT_SUCCESS;
                            }
                        } while (key_pressed == 'n' || key_pressed == 'n');
                }
                break;
            case 7:
                std::cout << "\n\t\t\t Archivo hora línea \n\n" << std::endl;

                archivo.open("Tiempo.txt");
                if (archivo.is_open()) {
                    while (getline(archivo, linea)) {
                        std::cout << linea << std::endl;
                    }
                    archivo.close();
                } else {
                    std::cout << "No se pudo abrir el archivo." << std::endl;
                }

                std::cout << "\n\nDesea volver al menu principal? (S/N): \n";
                while (true) {
                        if (kbhit()) {
                            key_pressed = getchar();
                            if(key_pressed == 'S' || key_pressed == 's') {
                                system("clear"); 
                                break;
                            } else if (key_pressed == 'N' || key_pressed == 'n') {
                            //Libera la memoria reservada en el hilo 
                            std::cout << "\n\nSe han desbloqueado los hilos y memorias son libres." << std::endl;
                            return EXIT_SUCCESS;
                            }
                        } while (key_pressed == 'n' || key_pressed == 'n');
                }
                break;
            case 8:
                std::cout << "\n\t\t\t Archivo asignación de memoria \n\n" << std::endl;

                archivo.open("Tiempo.txt");
                if (archivo.is_open()) {
                    while (getline(archivo, linea)) {
                        std::cout << linea << std::endl;
                    }
                    archivo.close();
                } else {
                    std::cout << "No se pudo abrir el archivo." << std::endl;
                }

                std::cout << "\n\nDesea volver al menu principal? (S/N): \n";
                while (true) {
                        if (kbhit()) {
                            key_pressed = getchar();
                            if(key_pressed == 'S' || key_pressed == 's') {
                                system("clear"); 
                                break;
                            } else if (key_pressed == 'N' || key_pressed == 'n') {
                            //Libera la memoria reservada en el hilo 
                            std::cout << "\n\nSe han desbloqueado los hilos y memorias son libres." << std::endl;
                            return EXIT_SUCCESS;
                            }
                        } while (key_pressed == 'n' || key_pressed == 'n');
                }
                break;
            case 9:
                std::cout << "Saliendo del programa..." << std::endl;
                salir = true;
                break;
            default:
                std::cout << "Opción inválida. Por favor, ingrese una opción válida." << std::endl;
                break;
        }
    } while (!salir);; // Repetir hasta que se seleccione la opción de salir

    return 0;
}

int main() {

    char key_pressed = '\0';

    do {
    ThreadData data;

    pthread_t textoingreso, parametros, extraccion, LlamadasAlSistema;

    int terr = pthread_create(&textoingreso, NULL, hilo1_textoingreso, NULL);
    if (terr != 0) {
        std::cerr << "Error: No se pudo crear el hilo textoingreso." << std::endl;
        return EXIT_FAILURE;
    }
    
    // Esperar a que el hilo termine
    pthread_join(textoingreso, NULL);

    int perr = pthread_create(&parametros, NULL, hilo2_parametros, &data);
    if (perr != 0) {
        std::cerr << "Error: No se pudo crear el hilo parametros." << std::endl;
        return EXIT_FAILURE;
    } 
    
    // Esperar a que el hilo termine
     pthread_join(parametros, NULL);

    int eerr = pthread_create(&extraccion, NULL, hilo3_extraccion, &data);
    if (eerr != 0) {
        std::cerr << "Error: No se pudo crear el hilo extraccion." << std::endl;
        return EXIT_FAILURE;
    } else
    {
        std::cerr << "Hilo extraccion creado" << std::endl;
        std::cerr << "Hilo extraccion cargado" << std::endl;
    }
    
    // Esperar a que el hilo termine
     pthread_join(extraccion, NULL);

    std::cout << "Presiona 'R' para realizar otra extraccion, 'F' para salir o 'S' para ejecutar las llamadas al sistema con strace." << std::endl;

        while (true) {
            if (kbhit()) {
                key_pressed = getchar();
                if (key_pressed == 'R' || key_pressed == 'r') {
                    system("clear");
                    break; 
                } else if (key_pressed == 'F' || key_pressed == 'f') {
                    //Libera la memoria reservada en el hilo 
                    free(data.MemoriaHilo2);
                    //Desbloquea los hilos bloqueados
                    pthread_mutex_unlock(&mutex);

                    std::cout << "\n\nSe han desbloqueado los hilos y memorias son libres." << std::endl;
                    return EXIT_SUCCESS;
                } else if (key_pressed == 'S' || key_pressed == 's') {
                    int eerr = pthread_create(&LlamadasAlSistema, NULL, hilo4_LlamadasAlSistema, NULL);
                    if (eerr != 0) {
                        std::cerr << "Error: No se pudo crear el hilo LlamadasAlSistema." << std::endl;
                        return EXIT_FAILURE;
                    } else
                    {
                        std::cerr << "\n\nHilo LlamadasAlSistema creado" << std::endl;
                        std::cerr << "Hilo LlamadasAlSistema cargado" << std::endl;
                    }
                        // Esperar a que el hilo termine
                    pthread_join(LlamadasAlSistema, NULL);
                    return EXIT_SUCCESS;
                }

            }
        }
    } while (key_pressed == 'R' || key_pressed == 'r');


    return 0;
}
