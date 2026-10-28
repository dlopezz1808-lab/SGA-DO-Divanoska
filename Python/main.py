import os
from datetime import datetime

# ==========================================
# JERARQUIA DE PROGRAMAS ACADEMICOS (POLIMORFISMO)
# ==========================================

class ProgramaAcademico:
    def __init__(self, nombre, codigo):
        self._nombre = nombre
        self._codigo = codigo

    def get_nombre(self):
        return self._nombre

    def get_codigo(self):
        return self._codigo

    def evaluar_aprobacion(self, notas):
        raise NotImplementedError("Metodo abstracto debe ser implementado en la subclase")


class Curso(ProgramaAcademico):
    def __init__(self):
        super().__init__("Curso", "CUR-01")

    def evaluar_aprobacion(self, notas):
        if not notas:
            return False
        promedio = sum(notas) / len(notas)
        return promedio >= 10.0


class Diplomado(ProgramaAcademico):
    def __init__(self):
        super().__init__("Diplomado", "DIP-01")

    def evaluar_aprobacion(self, notas):
        if not notas:
            return False
        promedio = sum(notas) / len(notas)
        return promedio >= 14.0


class Bootcamp(ProgramaAcademico):
    def __init__(self):
        super().__init__("Bootcamp", "BTC-01")

    def evaluar_aprobacion(self, notas):
        if not notas:
            return False
        return all(nota >= 14.0 for nota in notas)


# ==========================================
# JERARQUIA DE PERSONAS
# ==========================================

class Persona:
    def __init__(self, cedula, nombre_completo, correo_electronico):
        self._cedula = cedula
        self._nombre_completo = nombre_completo
        self._correo_electronico = correo_electronico

    def get_cedula(self):
        return self._cedula

    def get_nombre_completo(self):
        return self._nombre_completo

    def get_correo_electronico(self):
        return self._correo_electronico

    def mostrar_info(self):
        return f"Cedula: {self._cedula} | Nombre: {self._nombre_completo} | Correo: {self._correo_electronico}"


class Alumno(Persona):
    def __init__(self, cedula, nombre_completo, correo_electronico, programa_asignado):
        super().__init__(cedula, nombre_completo, correo_electronico)
        self._programa_asignado = programa_asignado
        self._notas = []

    def agregar_nota(self, nota):
        if len(self._notas) < 3:
            self._notas.append(nota)
            return True
        return False

    def eliminar_ultima_nota(self):
        if self._notas:
            return self._notas.pop()
        return None

    def get_notas(self):
        return self._notas

    def get_programa_asignado(self):
        return self._programa_asignado

    def obtener_promedio(self):
        if not self._notas:
            return 0.0
        return sum(self._notas) / len(self._notas)

    def esta_aprobado(self):
        return self._programa_asignado.evaluar_aprobacion(self._notas)

    def mostrar_info(self):
        estatus = "APROBADO" if self.esta_aprobado() else "REPROBADO"
        notas_str = ", ".join(map(str, self._notas)) if self._notas else "Sin notas"
        return (f"[Alumno] {super().mostrar_info()} | "
                f"Prog: {self._programa_asignado.get_nombre()} | "
                f"Notas: [{notas_str}] | Prom: {self.obtener_promedio():.1f} | Estatus: {estatus}")


class Profesor(Persona):
    def __init__(self, cedula, nombre_completo, correo_electronico, especialidad_academica, materia_asignada):
        super().__init__(cedula, nombre_completo, correo_electronico)
        self._especialidad_academica = especialidad_academica
        self._materia_asignada = materia_asignada

    def get_especialidad_academica(self):
        return self._especialidad_academica

    def get_materia_asignada(self):
        return self._materia_asignada

    def mostrar_info(self):
        return (f"[Profesor] {super().mostrar_info()} | "
                f"Esp: {self._especialidad_academica} | Materia: {self._materia_asignada}")


# ==========================================
# CLASE GESTORA (SISTEMA)
# ==========================================

class Sistema:
    def __init__(self):
        self.alumnos = []
        self.profesores = []
        self.pila_deshacer = []
        self.cola_certificados = []

        # Rutas de archivos en carpeta Data/
        self.archivo_alumnos = "Data/alumnos.txt"
        self.archivo_profesores = "Data/profesores.txt"
        self.archivo_certificados = "Data/certificados_pendientes.txt"

        self.cargar_alumnos()
        self.cargar_profesores()

    def registrar_alumno(self):
        print("\n--- Registro de Alumno ---")
        ced = input("Cedula: ").strip()
        nom = input("Nombre Completo: ").strip()
        cor = input("Correo Electronico: ").strip()
        print("Programa: 1. Curso | 2. Diplomado | 3. Bootcamp")
        opc = input("Opcion (1-3): ").strip()

        if opc == "1":
            prog = Curso()
        elif opc == "2":
            prog = Diplomado()
        elif opc == "3":
            prog = Bootcamp()
        else:
            print("Seleccion de programa no valida.")
            return

        self.alumnos.append(Alumno(ced, nom, cor, prog))
        self.guardar_alumnos()
        print("Registro completado exitosamente.")

    def registrar_profesor(self):
        print("\n--- Registro de Profesor ---")
        ced = input("Cedula: ").strip()
        nom = input("Nombre Completo: ").strip()
        cor = input("Correo Electronico: ").strip()
        esp = input("Especialidad: ").strip()
        mat = input("Materia Asignada: ").strip()

        self.profesores.append(Profesor(ced, nom, cor, esp, mat))
        self.guardar_profesores()
        print("Registro de profesor completado.")

    def registrar_nota(self):
        print("\n--- Carga de Calificaciones ---")
        ced = input("Ingrese Cedula del Alumno: ").strip()
        al = next((a for a in self.alumnos if a.get_cedula() == ced), None)

        if not al:
            print("Alumno no encontrado.")
            return

        try:
            val = float(input("Nota (0 a 20): "))
            if 0 <= val <= 20:
                if al.agregar_nota(val):
                    self.pila_deshacer.append((ced, val))
                    self.guardar_alumnos()
                    print("Calificacion registrada.")
                else:
                    print("El alumno ya posee el limite de 3 notas.")
            else:
                print("La nota ingresada esta fuera de rango.")
        except ValueError:
            print("Error: Debe ingresar una cifra numerica.")

    def deshacer_nota(self):
        print("\n--- Reversion de Ultima Nota (LIFO) ---")
        if not self.pila_deshacer:
            print("No existen registros para revertir.")
            return

        ced, val = self.pila_deshacer.pop()
        al = next((a for a in self.alumnos if a.get_cedula() == ced), None)
        if al:
            al.eliminar_ultima_nota()
            self.guardar_alumnos()
            print(f"Se elimino la calificacion {val} asignada a {al.get_nombre_completo()}.")

    def generar_certificados(self):
        print("\n--- Emision de Cola de Certificados (FIFO) ---")
        self.cola_certificados = [a for a in self.alumnos if a.esta_aprobado()]

        self.guardar_certificados()
        print(f"Archivo '{self.archivo_certificados}' exportado correctamente.")

    def mostrar_reporte_general(self):
        print("\n=========================================")
        print("          MONITOR ACADEMICO SGA-DO       ")
        print("=========================================")
        print("\n[DOCENTES REGISTRADOS]")
        for p in self.profesores:
            print(p.mostrar_info())
        print("\n[ESTUDIANTES MATRICULADOS]")
        for a in self.alumnos:
            print(a.mostrar_info())
        print("=========================================\n")

    def guardar_alumnos(self):
        if not os.path.exists("Data"):
            os.makedirs("Data")

        with open(self.archivo_alumnos, "w", encoding="utf-8") as f:
            for a in self.alumnos:
                n = a.get_notas()
                n1 = n[0] if len(n) > 0 else 0
                n2 = n[1] if len(n) > 1 else 0
                n3 = n[2] if len(n) > 2 else 0
                f.write(f"{a.get_cedula()},{a.get_nombre_completo()},{a.get_correo_electronico()},{a.get_programa_asignado().get_nombre()},{n1},{n2},{n3}\n")

    def guardar_profesores(self):
        if not os.path.exists("Data"):
            os.makedirs("Data")

        with open(self.archivo_profesores, "w", encoding="utf-8") as f:
            for p in self.profesores:
                f.write(f"{p.get_cedula()},{p.get_nombre_completo()},{p.get_correo_electronico()},{p._especialidad_academica},{p._materia_asignada}\n")

    def guardar_certificados(self):
        if not os.path.exists("Data"):
            os.makedirs("Data")

        fecha_actual = datetime.now().strftime("%d de %B de %Y")
        separador = "=" * 60

        with open(self.archivo_certificados, "w", encoding="utf-8") as f:
            f.write(f"{separador}\n")
            f.write("              SISTEMA DE GESTION ACADEMICA - SGA-DO\n")
            f.write("              CERTIFICADOS PENDIENTES DE GRADUACION\n")
            f.write(f"{separador}\n")
            f.write(f"Fecha de generacion: {fecha_actual}\n")
            f.write(f"Total de graduandos en cola: {len(self.cola_certificados)}\n")
            f.write(f"{separador}\n\n")

            if not self.cola_certificados:
                f.write("No hay certificados pendientes por generar.\n")
            else:
                for idx, al in enumerate(self.cola_certificados, 1):
                    notas = al.get_notas()
                    notas_str = ", ".join(map(str, notas)) if notas else "Sin notas"
                    prom = al.obtener_promedio()

                    f.write(f"Certificado #{idx}\n")
                    f.write(f"  Cedula: {al.get_cedula()}\n")
                    f.write(f"  Nombre: {al.get_nombre_completo()}\n")
                    f.write(f"  Programa: {al.get_programa_asignado().get_nombre()}\n")
                    f.write(f"  Notas: [{notas_str}]\n")
                    f.write(f"  Promedio Final: {prom:.1f}\n")
                    f.write(f"  Estatus: APROBADO\n")
                    f.write(f"  Fecha de certificacion: Pendiente\n")
                    f.write(f"{'-' * 60}\n")

            f.write(f"\n{separador}\n")
            f.write(f"Total de certificados pendientes: {len(self.cola_certificados)}\n")
            f.write("* Este reporte debe ser enviado al Departamento de Certificados\n")
            f.write("* Generado automaticamente por SGA-DO - DiplomadosOnline.com\n")
            f.write(f"{separador}\n")

    def cargar_alumnos(self):
        if not os.path.exists("Data"):
            os.makedirs("Data")

        if os.path.exists(self.archivo_alumnos):
            with open(self.archivo_alumnos, "r", encoding="utf-8") as f:
                for line in f:
                    if "," in line and not line.startswith("=") and not line.startswith("*"):
                        p = line.strip().split(",")
                        if len(p) >= 7:
                            try:
                                prog = Curso() if p[3] == "Curso" else (Diplomado() if p[3] == "Diplomado" else Bootcamp())
                                al = Alumno(p[0], p[1], p[2], prog)
                                for val in [float(p[4]), float(p[5]), float(p[6])]:
                                    if val > 0:
                                        al.agregar_nota(val)
                                self.alumnos.append(al)
                            except:
                                pass

    def cargar_profesores(self):
        if not os.path.exists("Data"):
            os.makedirs("Data")

        if os.path.exists(self.archivo_profesores):
            with open(self.archivo_profesores, "r", encoding="utf-8") as f:
                for line in f:
                    if "," in line and not line.startswith("=") and not line.startswith("*"):
                        p = line.strip().split(",")
                        if len(p) >= 5:
                            try:
                                self.profesores.append(Profesor(p[0], p[1], p[2], p[3], p[4]))
                            except:
                                pass


# ==========================================
# MENU INTERACTIVO PRINCIPAL
# ==========================================

def iniciar_menu():
    sys = Sistema()
    while True:
        print("\n=== SGA-DO: CONTROL ACADEMICO ===")
        print("1. Registrar Alumno")
        print("2. Registrar Profesor")
        print("3. Registrar Notas a un Alumno")
        print("4. Deshacer Ultimo Registro de Nota")
        print("5. Generar Cola de Certificados")
        print("6. Mostrar Reporte General")
        print("7. Salir")
        opc = input("Seleccione (1-7): ").strip()

        if opc == "1":
            sys.registrar_alumno()
        elif opc == "2":
            sys.registrar_profesor()
        elif opc == "3":
            sys.registrar_nota()
        elif opc == "4":
            sys.deshacer_nota()
        elif opc == "5":
            sys.generar_certificados()
        elif opc == "6":
            sys.mostrar_reporte_general()
        elif opc == "7":
            print("\nFinalizando sesion del sistema...")
            break
        else:
            print("Opcion no valida.")


if __name__ == "__main__":
    iniciar_menu()