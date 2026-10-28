#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stack>
#include <queue>
#include <iomanip>
#include <ctime>

using namespace std;

// ==========================================
// JERARQUIA DE PROGRAMAS ACADEMICOS (POLIMORFISMO)
// ==========================================

class ProgramaAcademico {
protected:
    string nombre;
    string codigo;
public:
    ProgramaAcademico(string n, string c) : nombre(n), codigo(c) {}
    virtual ~ProgramaAcademico() {}
    string getNombre() const { return nombre; }
    string getCodigo() const { return codigo; }
    virtual bool evaluarAprobacion(const vector<double>& notas) const = 0;
};

class Curso : public ProgramaAcademico {
public:
    Curso() : ProgramaAcademico("Curso", "CUR-01") {}
    bool evaluarAprobacion(const vector<double>& notas) const override {
        if (notas.empty()) return false;
        double s = 0;
        for (double n : notas) s += n;
        return (s / notas.size()) >= 10.0;
    }
};

class Diplomado : public ProgramaAcademico {
public:
    Diplomado() : ProgramaAcademico("Diplomado", "DIP-01") {}
    bool evaluarAprobacion(const vector<double>& notas) const override {
        if (notas.empty()) return false;
        double s = 0;
        for (double n : notas) s += n;
        return (s / notas.size()) >= 14.0;
    }
};

class Bootcamp : public ProgramaAcademico {
public:
    Bootcamp() : ProgramaAcademico("Bootcamp", "BTC-01") {}
    bool evaluarAprobacion(const vector<double>& notas) const override {
        if (notas.empty()) return false;
        for (double n : notas) {
            if (n < 14.0) return false;
        }
        return true;
    }
};

// ==========================================
// JERARQUIA DE PERSONAS
// ==========================================

class Persona {
private:
    string cedula;
    string nombreCompleto;
    string correoElectronico;
public:
    Persona(string c, string n, string corr) : cedula(c), nombreCompleto(n), correoElectronico(corr) {}
    virtual ~Persona() {}
    string getCedula() const { return cedula; }
    string getNombreCompleto() const { return nombreCompleto; }
    string getCorreoElectronico() const { return correoElectronico; }
    virtual void mostrarInfo() const = 0;
};

class Alumno : public Persona {
private:
    vector<double> notas;
    ProgramaAcademico* programaAsignado;
public:
    Alumno(string c, string n, string corr, ProgramaAcademico* p)
        : Persona(c, n, corr), programaAsignado(p) {}

    ~Alumno() {
        delete programaAsignado;
    }

    bool agregarNota(double n) {
        if (notas.size() < 3) {
            notas.push_back(n);
            return true;
        }
        return false;
    }

    void eliminarUltimaNota() {
        if (!notas.empty()) notas.pop_back();
    }

    const vector<double>& getNotas() const { return notas; }
    ProgramaAcademico* getProgramaAsignado() const { return programaAsignado; }

    double getPromedio() const {
        if (notas.empty()) return 0.0;
        double s = 0;
        for (double n : notas) s += n;
        return s / notas.size();
    }

    bool estaAprobado() const {
        return programaAsignado->evaluarAprobacion(notas);
    }

    void mostrarInfo() const override {
        string st = estaAprobado() ? "APROBADO" : "REPROBADO";
        cout << "[Alumno] Cedula: " << getCedula() << " | Nombre: " << getNombreCompleto()
             << " | Prog: " << programaAsignado->getNombre() << " | Prom: " << fixed << setprecision(1) << getPromedio()
             << " | Estatus: " << st << endl;
    }
};

class Profesor : public Persona {
private:
    string especialidadAcademica;
    string materiaAsignada;
public:
    Profesor(string c, string n, string corr, string esp, string mat)
        : Persona(c, n, corr), especialidadAcademica(esp), materiaAsignada(mat) {}

    string getEspecialidadAcademica() const { return especialidadAcademica; }
    string getMateriaAsignada() const { return materiaAsignada; }

    void mostrarInfo() const override {
        cout << "[Profesor] Cedula: " << getCedula() << " | Nombre: " << getNombreCompleto()
             << " | Esp: " << especialidadAcademica << " | Materia: " << materiaAsignada << endl;
    }
};

// ==========================================
// REGISTRO DE ACCION PARA PILA (LIFO)
// ==========================================

struct AccionRegistro {
    string cedula;
    double nota;
};

// ==========================================
// CLASE GESTORA (SISTEMA)
// ==========================================

class Sistema {
private:
    vector<Alumno*> alumnos;
    vector<Profesor*> profesores;
    stack<AccionRegistro> pilaDeshacer;
    queue<Alumno*> colaCertificados;

    const string archivoAlumnos = "Data/alumnos.txt";
    const string archivoProfesores = "Data/profesores.txt";
    const string archivoCertificados = "Data/certificados_pendientes.txt";

    void crearCarpetaData() {
        #ifdef _WIN32
            system("mkdir Data 2>nul");
        #else
            system("mkdir -p Data 2>/dev/null");
        #endif
    }

    string obtenerFechaActual() {
        time_t now = time(0);
        struct tm* tiempo = localtime(&now);
        char buffer[100];
        strftime(buffer, sizeof(buffer), "%d de %B de %Y", tiempo);
        return string(buffer);
    }

public:
    Sistema() {
        crearCarpetaData();
        cargarAlumnos();
        cargarProfesores();
    }

    ~Sistema() {
        for (auto a : alumnos) delete a;
        for (auto p : profesores) delete p;
    }

    void registrarAlumno() {
        string c, n, cor, op;
        cout << "\n--- Registro de Alumno ---" << endl;
        cout << "Cedula: "; getline(cin, c);
        cout << "Nombre Completo: "; getline(cin, n);
        cout << "Correo: "; getline(cin, cor);
        cout << "Programa (1. Curso | 2. Diplomado | 3. Bootcamp): "; getline(cin, op);

        ProgramaAcademico* p = nullptr;
        if (op == "1") {
            p = new Curso();
        } else if (op == "2") {
            p = new Diplomado();
        } else if (op == "3") {
            p = new Bootcamp();
        } else {
            cout << "Seleccion no valida." << endl;
            return;
        }

        alumnos.push_back(new Alumno(c, n, cor, p));
        guardarAlumnos();
        cout << "Alumno matriculado." << endl;
    }

    void registrarProfesor() {
        string c, n, cor, esp, mat;
        cout << "\n--- Registro de Profesor ---" << endl;
        cout << "Cedula: "; getline(cin, c);
        cout << "Nombre Completo: "; getline(cin, n);
        cout << "Correo: "; getline(cin, cor);
        cout << "Especialidad: "; getline(cin, esp);
        cout << "Materia: "; getline(cin, mat);

        profesores.push_back(new Profesor(c, n, cor, esp, mat));
        guardarProfesores();
        cout << "Docente registrado." << endl;
    }

    void registrarNota() {
        cout << "\n--- Carga de Calificaciones ---" << endl;
        string c;
        cout << "Cedula del Alumno: "; getline(cin, c);

        Alumno* al = nullptr;
        for (auto a : alumnos) {
            if (a->getCedula() == c) {
                al = a;
                break;
            }
        }

        if (!al) {
            cout << "Alumno no encontrado." << endl;
            return;
        }

        try {
            cout << "Nota (0 a 20): ";
            string input;
            getline(cin, input);
            double v = stod(input);

            if (v < 0 || v > 20) {
                cout << "La nota esta fuera de rango." << endl;
                return;
            }

            if (al->agregarNota(v)) {
                pilaDeshacer.push({c, v});
                guardarAlumnos();
                cout << "Calificacion registrada." << endl;
            } else {
                cout << "El alumno ya posee el limite de 3 notas." << endl;
            }
        } catch (...) {
            cout << "Error: Debe ingresar una cifra numerica." << endl;
        }
    }

    void deshacerNota() {
        cout << "\n--- Reversion de Ultima Nota (LIFO) ---" << endl;
        if (pilaDeshacer.empty()) {
            cout << "No existen registros para revertir." << endl;
            return;
        }

        AccionRegistro reg = pilaDeshacer.top();
        pilaDeshacer.pop();

        for (auto a : alumnos) {
            if (a->getCedula() == reg.cedula) {
                a->eliminarUltimaNota();
                guardarAlumnos();
                cout << "Se elimino la calificacion " << reg.nota
                     << " asignada a " << a->getNombreCompleto() << "." << endl;
                break;
            }
        }
    }

    void generarCertificados() {
        cout << "\n--- Emision de Cola de Certificados (FIFO) ---" << endl;

        while (!colaCertificados.empty()) colaCertificados.pop();

        for (auto a : alumnos) {
            if (a->estaAprobado()) {
                colaCertificados.push(a);
            }
        }

        guardarCertificados();
        cout << "Archivo 'certificados_pendientes.txt' exportado correctamente." << endl;
    }

    void mostrarReporteGeneral() {
        cout << "\n=========================================" << endl;
        cout << "          MONITOR ACADEMICO SGA-DO       " << endl;
        cout << "=========================================" << endl;
        cout << "\n[DOCENTES REGISTRADOS]" << endl;
        for (auto p : profesores) {
            p->mostrarInfo();
        }
        cout << "\n[ESTUDIANTES MATRICULADOS]" << endl;
        for (auto a : alumnos) {
            a->mostrarInfo();
        }
        cout << "=========================================" << endl;
    }

    void guardarAlumnos() {
        crearCarpetaData();

        ofstream f(archivoAlumnos);
        if (f.is_open()) {
            for (auto a : alumnos) {
                const auto& n = a->getNotas();
                double n1 = n.size() > 0 ? n[0] : 0;
                double n2 = n.size() > 1 ? n[1] : 0;
                double n3 = n.size() > 2 ? n[2] : 0;
                f << a->getCedula() << "," << a->getNombreCompleto() << ","
                  << a->getCorreoElectronico() << ","
                  << a->getProgramaAsignado()->getNombre() << ","
                  << n1 << "," << n2 << "," << n3 << "\n";
            }
            f.close();
        }
    }

    void guardarProfesores() {
        crearCarpetaData();

        ofstream f(archivoProfesores);
        if (f.is_open()) {
            for (auto p : profesores) {
                f << p->getCedula() << "," << p->getNombreCompleto() << ","
                  << p->getCorreoElectronico() << ","
                  << p->getEspecialidadAcademica() << ","
                  << p->getMateriaAsignada() << "\n";
            }
            f.close();
        }
    }

    void guardarCertificados() {
        crearCarpetaData();

        string separador = "============================================================";
        string fecha = obtenerFechaActual();

        ofstream f(archivoCertificados);
        if (f.is_open()) {
            f << separador << "\n";
            f << "              SISTEMA DE GESTION ACADEMICA - SGA-DO\n";
            f << "              CERTIFICADOS PENDIENTES DE GRADUACION\n";
            f << separador << "\n";
            f << "Total de graduandos en cola: " << colaCertificados.size() << "\n";
            f << separador << "\n\n";

            if (colaCertificados.empty()) {
                f << "No hay certificados pendientes por generar.\n";
            } else {
                int idx = 1;
                queue<Alumno*> temp = colaCertificados;
                while (!temp.empty()) {
                    Alumno* a = temp.front();
                    temp.pop();

                    f << "Certificado #" << idx << "\n";
                    f << "  Cedula: " << a->getCedula() << "\n";
                    f << "  Nombre: " << a->getNombreCompleto() << "\n";
                    f << "  Programa: " << a->getProgramaAsignado()->getNombre() << "\n";
                    f << "  Promedio Final: " << fixed << setprecision(1) << a->getPromedio() << "\n";
                    f << "  Estatus: APROBADO\n";
                    f << "  Fecha de certificacion: Pendiente\n";
                    f << "------------------------------------------------------------\n";
                    idx++;
                }
            }

            f << "\n" << separador << "\n";
            f << "Total de certificados pendientes: " << colaCertificados.size() << "\n";
            f << "* Este reporte debe ser enviado al Departamento de Certificados\n";
            f << "* Generado automaticamente por SGA-DO - DiplomadosOnline.com\n";
            f << separador << "\n";
            f.close();
        }
    }

    void cargarAlumnos() {
        crearCarpetaData();

        ifstream f(archivoAlumnos);
        if (f.is_open()) {
            string line;
            while (getline(f, line)) {
                if (line.find(",") != string::npos && line.find("=") == string::npos && line.find("*") == string::npos) {
                    stringstream ss(line);
                    string c, n, cor, pr, s1, s2, s3;
                    if (getline(ss, c, ',') && getline(ss, n, ',') && getline(ss, cor, ',') &&
                        getline(ss, pr, ',') && getline(ss, s1, ',') && getline(ss, s2, ',') && getline(ss, s3, ',')) {
                        try {
                            ProgramaAcademico* p;
                            if (pr == "Curso") {
                                p = new Curso();
                            } else if (pr == "Diplomado") {
                                p = new Diplomado();
                            } else {
                                p = new Bootcamp();
                            }

                            Alumno* al = new Alumno(c, n, cor, p);
                            double v1 = stod(s1), v2 = stod(s2), v3 = stod(s3);
                            if (v1 > 0) al->agregarNota(v1);
                            if (v2 > 0) al->agregarNota(v2);
                            if (v3 > 0) al->agregarNota(v3);
                            alumnos.push_back(al);
                        } catch (...) {}
                    }
                }
            }
            f.close();
        }
    }

    void cargarProfesores() {
        crearCarpetaData();

        ifstream f(archivoProfesores);
        if (f.is_open()) {
            string line;
            while (getline(f, line)) {
                if (line.find(",") != string::npos && line.find("=") == string::npos && line.find("*") == string::npos) {
                    stringstream ss(line);
                    string c, n, cor, esp, mat;
                    if (getline(ss, c, ',') && getline(ss, n, ',') && getline(ss, cor, ',') &&
                        getline(ss, esp, ',') && getline(ss, mat, ',')) {
                        try {
                            profesores.push_back(new Profesor(c, n, cor, esp, mat));
                        } catch (...) {}
                    }
                }
            }
            f.close();
        }
    }
};

// ==========================================
// MENU INTERACTIVO PRINCIPAL
// ==========================================

int main() {
    Sistema sys;
    string opc;

    while (true) {
        cout << "\n=========================================" << endl;
        cout << "     SGA-DO: SISTEMA DIPLOMADOSONLINE    " << endl;
        cout << "=========================================" << endl;
        cout << "1. Registrar Alumno" << endl;
        cout << "2. Registrar Profesor" << endl;
        cout << "3. Registrar Notas a un Alumno" << endl;
        cout << "4. Deshacer Ultimo Registro de Nota" << endl;
        cout << "5. Generar Cola de Certificados" << endl;
        cout << "6. Mostrar Reporte General" << endl;
        cout << "7. Salir" << endl;
        cout << "Seleccione (1-7): ";
        getline(cin, opc);

        if (opc == "1") {
            sys.registrarAlumno();
        } else if (opc == "2") {
            sys.registrarProfesor();
        } else if (opc == "3") {
            sys.registrarNota();
        } else if (opc == "4") {
            sys.deshacerNota();
        } else if (opc == "5") {
            sys.generarCertificados();
        } else if (opc == "6") {
            sys.mostrarReporteGeneral();
        } else if (opc == "7") {
            cout << "\nFinalizando sesion del sistema..." << endl;
            break;
        } else {
            cout << "Opcion no valida." << endl;
        }
    }

    return 0;
}