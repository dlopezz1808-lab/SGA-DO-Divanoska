import java.io.*;
import java.util.*;

// ==========================================
// JERARQUIA DE PROGRAMAS ACADEMICOS (POLIMORFISMO)
// ==========================================

abstract class ProgramaAcademico {
    protected String nombre;
    protected String codigo;

    public ProgramaAcademico(String nombre, String codigo) {
        this.nombre = nombre;
        this.codigo = codigo;
    }

    public String getNombre() { return nombre; }
    public String getCodigo() { return codigo; }
    public abstract boolean evaluarAprobacion(List<Double> notas);
}

class Curso extends ProgramaAcademico {
    public Curso() { super("Curso", "CUR-01"); }

    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        if (notas.isEmpty()) return false;
        double s = 0;
        for (double n : notas) s += n;
        return (s / notas.size()) >= 10.0;
    }
}

class Diplomado extends ProgramaAcademico {
    public Diplomado() { super("Diplomado", "DIP-01"); }

    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        if (notas.isEmpty()) return false;
        double s = 0;
        for (double n : notas) s += n;
        return (s / notas.size()) >= 14.0;
    }
}

class Bootcamp extends ProgramaAcademico {
    public Bootcamp() { super("Bootcamp", "BTC-01"); }

    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        if (notas.isEmpty()) return false;
        for (double n : notas) {
            if (n < 14.0) return false;
        }
        return true;
    }
}

// ==========================================
// JERARQUIA DE PERSONAS
// ==========================================

abstract class Persona {
    private String cedula;
    private String nombreCompleto;
    private String correoElectronico;

    public Persona(String cedula, String nombreCompleto, String correoElectronico) {
        this.cedula = cedula;
        this.nombreCompleto = nombreCompleto;
        this.correoElectronico = correoElectronico;
    }

    public String getCedula() { return cedula; }
    public String getNombreCompleto() { return nombreCompleto; }
    public String getCorreoElectronico() { return correoElectronico; }

    public abstract void mostrarInfo();
}

class Alumno extends Persona {
    private List<Double> notas;
    private ProgramaAcademico programaAsignado;

    public Alumno(String cedula, String nombreCompleto, String correoElectronico, ProgramaAcademico programaAsignado) {
        super(cedula, nombreCompleto, correoElectronico);
        this.programaAsignado = programaAsignado;
        this.notas = new ArrayList<>();
    }

    public boolean agregarNota(double nota) {
        if (notas.size() < 3) {
            notas.add(nota);
            return true;
        }
        return false;
    }

    public Double eliminarUltimaNota() {
        if (!notas.isEmpty()) {
            return notas.remove(notas.size() - 1);
        }
        return null;
    }

    public List<Double> getNotas() { return notas; }
    public ProgramaAcademico getProgramaAsignado() { return programaAsignado; }

    public double getPromedio() {
        if (notas.isEmpty()) return 0.0;
        double sum = 0;
        for (double n : notas) sum += n;
        return sum / notas.size();
    }

    public boolean estaAprobado() {
        return programaAsignado.evaluarAprobacion(notas);
    }

    @Override
    public void mostrarInfo() {
        String st = estaAprobado() ? "APROBADO" : "REPROBADO";
        System.out.printf("[Alumno] Cedula: %s | Nombre: %s | Prog: %s | Prom: %.1f | Estatus: %s\n",
                getCedula(), getNombreCompleto(), programaAsignado.getNombre(), getPromedio(), st);
    }
}

class Profesor extends Persona {
    private String especialidadAcademica;
    private String materiaAsignada;

    public Profesor(String cedula, String nombreCompleto, String correoElectronico, String esp, String mat) {
        super(cedula, nombreCompleto, correoElectronico);
        this.especialidadAcademica = esp;
        this.materiaAsignada = mat;
    }

    public String getEspecialidadAcademica() { return especialidadAcademica; }
    public String getMateriaAsignada() { return materiaAsignada; }

    @Override
    public void mostrarInfo() {
        System.out.printf("[Profesor] Cedula: %s | Nombre: %s | Esp: %s | Materia: %s\n",
                getCedula(), getNombreCompleto(), especialidadAcademica, materiaAsignada);
    }
}

// ==========================================
// REGISTRO DE ACCION PARA PILA (LIFO)
// ==========================================

class RegistroAccion {
    String cedula;
    double nota;

    public RegistroAccion(String cedula, double nota) {
        this.cedula = cedula;
        this.nota = nota;
    }
}

// ==========================================
// CLASE GESTORA (SISTEMA)
// ==========================================

class SistemaSGA {
    private List<Alumno> alumnos;
    private List<Profesor> profesores;
    private Stack<RegistroAccion> pilaDeshacer;
    private Queue<Alumno> colaCertificados;

    private final String archivoAlumnos = "Data/alumnos.txt";
    private final String archivoProfesores = "Data/profesores.txt";
    private final String archivoCertificados = "Data/certificados_pendientes.txt";

    public SistemaSGA() {
        alumnos = new ArrayList<>();
        profesores = new ArrayList<>();
        pilaDeshacer = new Stack<>();
        colaCertificados = new LinkedList<>();
        cargarAlumnos();
        cargarProfesores();
    }

    public void registrarAlumno(Scanner sc) {
        System.out.print("Cedula: ");
        String c = sc.nextLine().trim();
        System.out.print("Nombre Completo: ");
        String n = sc.nextLine().trim();
        System.out.print("Correo: ");
        String cor = sc.nextLine().trim();
        System.out.print("Programa (1. Curso | 2. Diplomado | 3. Bootcamp): ");
        String op = sc.nextLine().trim();

        ProgramaAcademico p;
        if (op.equals("1")) {
            p = new Curso();
        } else if (op.equals("2")) {
            p = new Diplomado();
        } else {
            p = new Bootcamp();
        }

        alumnos.add(new Alumno(c, n, cor, p));
        guardarAlumnos();
        System.out.println("Estudiante registrado.");
    }

    public void registrarProfesor(Scanner sc) {
        System.out.print("Cedula: ");
        String c = sc.nextLine().trim();
        System.out.print("Nombre Completo: ");
        String n = sc.nextLine().trim();
        System.out.print("Correo: ");
        String cor = sc.nextLine().trim();
        System.out.print("Especialidad: ");
        String esp = sc.nextLine().trim();
        System.out.print("Materia: ");
        String mat = sc.nextLine().trim();

        profesores.add(new Profesor(c, n, cor, esp, mat));
        guardarProfesores();
        System.out.println("Docente registrado.");
    }

    public void registrarNota(Scanner sc) {
        System.out.print("Cedula del Alumno: ");
        String c = sc.nextLine().trim();
        Alumno al = null;
        for (Alumno a : alumnos) {
            if (a.getCedula().equals(c)) {
                al = a;
                break;
            }
        }

        if (al == null) {
            System.out.println("Estudiante no localizado.");
            return;
        }

        try {
            System.out.print("Calificacion (0-20): ");
            double nota = Double.parseDouble(sc.nextLine().trim());
            if (nota < 0 || nota > 20) {
                System.out.println("La nota debe estar entre 0 y 20.");
                return;
            }

            if (al.agregarNota(nota)) {
                pilaDeshacer.push(new RegistroAccion(c, nota));
                guardarAlumnos();
                System.out.println("Nota ingresada.");
            } else {
                System.out.println("Limite de 3 notas alcanzado.");
            }
        } catch (Exception e) {
            System.out.println("Error de formato numerico.");
        }
    }

    public void deshacerNota() {
        if (pilaDeshacer.isEmpty()) {
            System.out.println("Pila vacia.");
            return;
        }

        RegistroAccion reg = pilaDeshacer.pop();
        for (Alumno a : alumnos) {
            if (a.getCedula().equals(reg.cedula)) {
                a.eliminarUltimaNota();
                guardarAlumnos();
                System.out.println("Calificacion de " + reg.nota + " removida.");
                break;
            }
        }
    }

    public void generarCertificados() {
        colaCertificados.clear();
        for (Alumno a : alumnos) {
            if (a.estaAprobado()) {
                colaCertificados.add(a);
            }
        }

        guardarCertificados();
        System.out.println("Archivo exportado.");
    }

    public void mostrarReporteGeneral() {
        System.out.println("\n=========================================");
        System.out.println("          MONITOR ACADEMICO SGA-DO       ");
        System.out.println("=========================================");
        System.out.println("\n--- PROFESORES ---");
        for (Profesor p : profesores) {
            p.mostrarInfo();
        }
        System.out.println("\n--- ALUMNOS ---");
        for (Alumno a : alumnos) {
            a.mostrarInfo();
        }
        System.out.println("=========================================\n");
    }

    private void guardarAlumnos() {
        File dataDir = new File("Data");
        if (!dataDir.exists()) {
            dataDir.mkdir();
        }

        try (PrintWriter pw = new PrintWriter(new FileWriter(archivoAlumnos))) {
            for (Alumno a : alumnos) {
                List<Double> n = a.getNotas();
                double n1 = n.size() > 0 ? n.get(0) : 0;
                double n2 = n.size() > 1 ? n.get(1) : 0;
                double n3 = n.size() > 2 ? n.get(2) : 0;
                pw.println(a.getCedula() + "," + a.getNombreCompleto() + "," +
                        a.getCorreoElectronico() + "," +
                        a.getProgramaAsignado().getNombre() + "," +
                        n1 + "," + n2 + "," + n3);
            }
        } catch (Exception ignored) {}
    }

    private void guardarProfesores() {
        File dataDir = new File("Data");
        if (!dataDir.exists()) {
            dataDir.mkdir();
        }

        try (PrintWriter pw = new PrintWriter(new FileWriter(archivoProfesores))) {
            for (Profesor p : profesores) {
                pw.println(p.getCedula() + "," + p.getNombreCompleto() + "," +
                        p.getCorreoElectronico() + "," +
                        p.getEspecialidadAcademica() + "," +
                        p.getMateriaAsignada());
            }
        } catch (Exception ignored) {}
    }

    private void guardarCertificados() {
        File dataDir = new File("Data");
        if (!dataDir.exists()) {
            dataDir.mkdir();
        }

        String separador = "============================================================";

        try (PrintWriter pw = new PrintWriter(new FileWriter(archivoCertificados))) {
            pw.println(separador);
            pw.println("              SISTEMA DE GESTION ACADEMICA - SGA-DO");
            pw.println("              CERTIFICADOS PENDIENTES DE GRADUACION");
            pw.println(separador);
            pw.println("Total de graduandos en cola: " + colaCertificados.size());
            pw.println(separador);
            pw.println();

            if (colaCertificados.isEmpty()) {
                pw.println("No hay certificados pendientes por generar.");
            } else {
                int idx = 1;
                for (Alumno a : colaCertificados) {
                    pw.println("Certificado #" + idx);
                    pw.println("  Cedula: " + a.getCedula());
                    pw.println("  Nombre: " + a.getNombreCompleto());
                    pw.println("  Programa: " + a.getProgramaAsignado().getNombre());
                    pw.println("  Promedio Final: " + String.format("%.1f", a.getPromedio()));
                    pw.println("  Estatus: APROBADO");
                    pw.println("  Fecha de certificacion: Pendiente");
                    pw.println("------------------------------------------------------------");
                    idx++;
                }
            }

            pw.println();
            pw.println(separador);
            pw.println("Total de certificados pendientes: " + colaCertificados.size());
            pw.println("* Este reporte debe ser enviado al Departamento de Certificados");
            pw.println("* Generado automaticamente por SGA-DO - DiplomadosOnline.com");
            pw.println(separador);

        } catch (Exception ignored) {}
    }

    private void cargarAlumnos() {
        File dataDir = new File("Data");
        if (!dataDir.exists()) {
            dataDir.mkdir();
        }

        File f = new File(archivoAlumnos);
        if (!f.exists()) return;

        try (BufferedReader br = new BufferedReader(new FileReader(f))) {
            String l;
            while ((l = br.readLine()) != null) {
                if (l.contains(",") && !l.startsWith("=") && !l.startsWith("*")) {
                    String[] p = l.split(",");
                    if (p.length >= 7) {
                        try {
                            ProgramaAcademico prog;
                            if (p[3].equals("Curso")) {
                                prog = new Curso();
                            } else if (p[3].equals("Diplomado")) {
                                prog = new Diplomado();
                            } else {
                                prog = new Bootcamp();
                            }

                            Alumno a = new Alumno(p[0], p[1], p[2], prog);
                            for (int i = 4; i <= 6; i++) {
                                double v = Double.parseDouble(p[i]);
                                if (v > 0) a.agregarNota(v);
                            }
                            alumnos.add(a);
                        } catch (Exception ignored) {}
                    }
                }
            }
        } catch (Exception ignored) {}
    }

    private void cargarProfesores() {
        File dataDir = new File("Data");
        if (!dataDir.exists()) {
            dataDir.mkdir();
        }

        File f = new File(archivoProfesores);
        if (!f.exists()) return;

        try (BufferedReader br = new BufferedReader(new FileReader(f))) {
            String l;
            while ((l = br.readLine()) != null) {
                if (l.contains(",") && !l.startsWith("=") && !l.startsWith("*")) {
                    String[] p = l.split(",");
                    if (p.length >= 5) {
                        try {
                            profesores.add(new Profesor(p[0], p[1], p[2], p[3], p[4]));
                        } catch (Exception ignored) {}
                    }
                }
            }
        } catch (Exception ignored) {}
    }

    public static void main(String[] args) {
        SistemaSGA s = new SistemaSGA();
        Scanner sc = new Scanner(System.in);

        while (true) {
            System.out.println("\n=========================================");
            System.out.println("     SGA-DO: SISTEMA DIPLOMADOSONLINE    ");
            System.out.println("=========================================");
            System.out.println("1. Registrar Alumno");
            System.out.println("2. Registrar Profesor");
            System.out.println("3. Registrar Notas a un Alumno");
            System.out.println("4. Deshacer Ultimo Registro de Nota");
            System.out.println("5. Generar Cola de Certificados");
            System.out.println("6. Mostrar Reporte General");
            System.out.println("7. Salir");
            System.out.print("Seleccione una opcion (1-7): ");

            String op = sc.nextLine().trim();

            if (op.equals("1")) {
                s.registrarAlumno(sc);
            } else if (op.equals("2")) {
                s.registrarProfesor(sc);
            } else if (op.equals("3")) {
                s.registrarNota(sc);
            } else if (op.equals("4")) {
                s.deshacerNota();
            } else if (op.equals("5")) {
                s.generarCertificados();
            } else if (op.equals("6")) {
                s.mostrarReporteGeneral();
            } else if (op.equals("7")) {
                System.out.println("\nFinalizando sesion del sistema...");
                break;
            } else {
                System.out.println("Opcion no valida.");
            }
        }
        sc.close();
    }
}